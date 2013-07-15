/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <algorithm>

#include <mpi.h>

#include "Error.h"
#include "MpiComm.h"
#include "MpiMessage.h"
#include "RemoteData.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 * struct DeleteObject
 *---------------------------------------------------------------------------
 */

namespace
{
  struct DeleteObject {
    template<typename T>
    void operator () (T ptr) {
      delete ptr;
    }
  };
}


/*
 *---------------------------------------------------------------------------
 *
 * class RemoteData
 *
 *---------------------------------------------------------------------------
 */

//--- Destructor ------------------------------------------------------------

RemoteData::~RemoteData()
{
  clear();
}


//--- Adding data -----------------------------------------------------------

void RemoteData::add_event(const RemoteEvent& event, uint32_t key)
{
  m_event_keys.insert(KeyMap::value_type(key, m_events.size()));
  m_events.push_back(event);
}

void RemoteData::add_buffer(Buffer* buffer, uint32_t key)
{
  m_buffer_keys.insert(KeyMap::value_type(key, m_buffers.size()));
  m_buffers.push_back(buffer);
}


//--- Retrieving data -------------------------------------------------------

RemoteEvent RemoteData::get_event(uint32_t key) const
{
  KeyMap::const_iterator it = m_event_keys.find(key);
  if (it == m_event_keys.end())
    throw RuntimeError("RemoteData::get_event(uint32_t) -- Unknown key.");

  return m_events[it->second];
}

Buffer* RemoteData::get_buffer(uint32_t key) const
{
  KeyMap::const_iterator it = m_buffer_keys.find(key);
  if (it == m_buffer_keys.end())
    throw RuntimeError("RemoteData::get_event(uint32_t) -- Unknown key.");

  return m_buffers[it->second];
}


//--- Removing events -------------------------------------------------------

void RemoteData::clear()
{
  m_event_keys.clear();
  m_buffer_keys.clear();

  m_events.clear();
  
  for_each(m_buffers.begin(), m_buffers.end(),
           ::DeleteObject());

  m_buffers.clear();
}


//--- Receiving event sets --------------------------------------------------

void RemoteData::recv(const GlobalDefs& defs,
                      const MpiComm&    comm,
                      int               source,
                      int               tag)
{
  // Receive message
  MpiMessage message(comm);
  message.recv(source, tag);

  // Unpack events
  {
    uint32_t key_count = message.get_uint32();
    for (uint32_t i = 0; i < key_count; ++i) {
      uint32_t key   = message.get_uint32();
      uint32_t index = message.get_uint32();

      m_event_keys.insert(KeyMap::value_type(key, index));
    }

    uint32_t ev_count = message.get_uint32();
    for (uint32_t i = 0; i < ev_count; ++i) {
      RemoteEvent event = message.get_event(defs);

      m_events.push_back(event);
    }
  }

  // Unpack buffers
  {
    uint32_t key_count = message.get_uint32();
    for (uint32_t i = 0; i < key_count; ++i) {
      uint32_t key   = message.get_uint32();
      uint32_t index = message.get_uint32();

      m_buffer_keys.insert(KeyMap::value_type(key, index));
    }

    uint32_t ev_count = message.get_uint32();
    for (uint32_t i = 0; i < ev_count; ++i) {
      Buffer* buffer = message.get_buffer();

      m_buffers.push_back(buffer);
    }
  }
}


void RemoteData::bcast(const GlobalDefs& defs,
                       const MpiComm&    comm,
                       int               root)
{
  // Receive message
  MpiMessage message(comm);
  message.bcast(root);

  // Unpack events
  {
    uint32_t key_count = message.get_uint32();
    for (uint32_t i = 0; i < key_count; ++i) {
      uint32_t key   = message.get_uint32();
      uint32_t index = message.get_uint32();

      m_event_keys.insert(KeyMap::value_type(key, index));
    }

    uint32_t ev_count = message.get_uint32();
    for (uint32_t i = 0; i < ev_count; ++i) {
      RemoteEvent event = message.get_event(defs);

      m_events.push_back(event);
    }
  }

  // Unpack buffers
  {
    uint32_t key_count = message.get_uint32();
    for (uint32_t i = 0; i < key_count; ++i) {
      uint32_t key   = message.get_uint32();
      uint32_t index = message.get_uint32();

      m_buffer_keys.insert(KeyMap::value_type(key, index));
    }

    uint32_t ev_count = message.get_uint32();
    for (uint32_t i = 0; i < ev_count; ++i) {
      Buffer* buffer = message.get_buffer();

      m_buffers.push_back(buffer);
    }
  }
}
