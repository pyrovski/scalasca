/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <algorithm>
#include <cfloat>
#include <functional>
#include <numeric>

#include <mpi.h>

#include "Error.h"
#include "LocalData.h"
#include "MpiComm.h"
#include "MpiMessage.h"

using namespace std;
using namespace pearl;

namespace
{
  /*
   *-------------------------------------------------------------------------
   * struct DeleteObject
   *-------------------------------------------------------------------------
   */

  struct DeleteObject {
    template<typename T>
    void operator () (T ptr) {
      delete ptr;
    }
  };


  /*
   *-------------------------------------------------------------------------
   * Accumulation-of-function-result adapter
   *-------------------------------------------------------------------------
   */

  template<typename BinOp, typename GetOp>
  class accu_fun_t {
    BinOp op;
    GetOp get;

  public:

    typedef typename BinOp::first_argument_type first_argument_type;
    typedef typename GetOp::argument_type       second_argument_type;
    typedef typename BinOp::result_type         result_type;

    accu_fun_t(const BinOp& o, const GetOp& g)
      : op(o), get(g) { }

    result_type operator()(const first_argument_type&  a, 
                           const second_argument_type& b) const {
      return op(a, get(b));
    }
  };

  template<typename BinOp, typename GetOp>
  accu_fun_t<BinOp, GetOp> accu_fun(const BinOp& op, const GetOp& get)
  {
    return accu_fun_t<BinOp, GetOp>(op, get);
  }

  template<typename GetOp>
  accu_fun_t<std::plus<typename GetOp::result_type>, GetOp> add_fun(const GetOp& get)
  {
    return accu_fun(std::plus<typename GetOp::result_type>(), get);
  }
}


/*
 *---------------------------------------------------------------------------
 *
 * class LocalData
 *
 *---------------------------------------------------------------------------
 */

//--- Destructor ------------------------------------------------------------

LocalData::~LocalData()
{
  clear();
}


//--- Adding data -----------------------------------------------------------

void LocalData::add_event(const Event& event, uint32_t key)
{
  EventList::iterator it = find(m_events.begin(),
                                m_events.end(),
                                event);

  uint32_t index = it - m_events.begin();
  if (it == m_events.end())
    m_events.push_back(event);

  m_event_keys.insert(KeyMap::value_type(key, index));
}

void LocalData::add_buffer(Buffer* buffer, uint32_t key)
{
  m_buffer_keys.insert(KeyMap::value_type(key, m_buffers.size()));
  m_buffers.push_back(buffer);
}


//--- Retrieving data-- -----------------------------------------------------

Event LocalData::get_event(uint32_t key) const
{
  KeyMap::const_iterator it = m_event_keys.find(key);
  if (it == m_event_keys.end())
    throw RuntimeError("LocalData::get_event(uint32_t) -- Unknown key.");

  return m_events[it->second];
}

Buffer* LocalData::get_buffer(uint32_t key) const
{
  KeyMap::const_iterator it = m_buffer_keys.find(key);
  if (it == m_buffer_keys.end())
    throw RuntimeError("LocalData::get_event(uint32_t) -- Unknown key.");

  return m_buffers[it->second];
}


//--- Removing events -------------------------------------------------------

void LocalData::clear()
{
  m_event_keys.clear();
  m_buffer_keys.clear();

  m_events.clear();
  
  for_each(m_buffers.begin(), m_buffers.end(),
           ::DeleteObject());

  m_buffers.clear();
}


//--- Sending event sets ----------------------------------------------------

void LocalData::send(const MpiComm& comm, int dest, int tag) const
{
  MpiMessage message(comm);

  // Construct message
  pack(message);

  // Send message
  message.send(dest, tag);
}


MpiMessage* LocalData::isend(const MpiComm& comm, int dest, int tag) const
{
  // determine message size in advance to avoid reallocations
  size_t size = 8 * m_event_keys.size()  +
                8 * m_buffer_keys.size() +
               24 * m_events.size();

  // add buffer sizes
  size = accumulate(m_buffers.begin(), m_buffers.end(), 
                    size,
                    ::add_fun(mem_fun(&Buffer::size)));

  MpiMessage* message = new MpiMessage(comm, std::max<size_t>(size, 64));

  // Construct message
  pack(*message);

  // Send message
  message->isend(dest, tag);

  return message;
}

MpiMessage* LocalData::issend(const MpiComm& comm, int dest, int tag) const
{
  // determine message size in advance to avoid reallocations
  size_t size = 8 * m_event_keys.size()  +
                8 * m_buffer_keys.size() +
               24 * m_events.size();

  // add buffer sizes
  size = accumulate(m_buffers.begin(), m_buffers.end(), 
                    size,
                    ::add_fun(mem_fun(&Buffer::size)));

  MpiMessage* message = new MpiMessage(comm, std::max<size_t>(size, 64));

  // Construct message
  pack(*message);

  // Send message
  message->issend(dest, tag);

  return message;
}

void LocalData::bcast(const MpiComm& comm, int root) const
{
  MpiMessage message(comm);

  // Construct message
  pack(message);

  // Send message
  message.bcast(root);
}


//--- Private methods -------------------------------------------------------

void LocalData::pack(MpiMessage& message) const
{
  // Pack events
  {
    uint32_t key_count = m_event_keys.size();
    uint32_t ev_count  = m_events.size();

    message.put_uint32(key_count);
    KeyMap::const_iterator kit = m_event_keys.begin();
    while (kit != m_event_keys.end()) {
      message.put_uint32(kit->first);
      message.put_uint32(kit->second);
      ++kit;
    }

    message.put_uint32(ev_count);
    EventList::const_iterator eit = m_events.begin();
    while (eit != m_events.end()) {
      message.put_event(*eit);
      ++eit;
    }
  }

  // Pack buffers
  {
    uint32_t key_count = m_buffer_keys.size();
    uint32_t buf_count = m_buffers.size();

    message.put_uint32(key_count);
    KeyMap::const_iterator kit = m_buffer_keys.begin();
    while (kit != m_buffer_keys.end()) {
      message.put_uint32(kit->first);
      message.put_uint32(kit->second);
      ++kit;
    }

    message.put_uint32(buf_count);
    BufferList::const_iterator eit = m_buffers.begin();
    while (eit != m_buffers.end()) {
      message.put_buffer(*(*eit));
      ++eit;
    }
  }
}
