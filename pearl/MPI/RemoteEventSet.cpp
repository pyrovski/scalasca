/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <mpi.h>

#include "Error.h"
#include "MpiComm.h"
#include "MpiMessage.h"
#include "RemoteEventSet.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RemoteEventSet
 *
 *---------------------------------------------------------------------------
 */

//--- Adding events ---------------------------------------------------------

void RemoteEventSet::add_event(const RemoteEvent& event, uint32_t key)
{
  m_keys.insert(KeyMap::value_type(key, m_elements.size()));
  m_elements.push_back(event);
}


//--- Retrieving events -----------------------------------------------------

RemoteEvent RemoteEventSet::get_event(uint32_t key) const
{
  KeyMap::const_iterator it = m_keys.find(key);
  if (it == m_keys.end())
    throw RuntimeError("RemoteEventSet::get_event(uint32_t) -- Unknown key.");

  return m_elements[it->second];
}


//--- Removing events -------------------------------------------------------

void RemoteEventSet::clear()
{
  m_keys.clear();
  m_elements.clear();
}


//--- Receiving event sets --------------------------------------------------

void RemoteEventSet::recv(const GlobalDefs& defs,
                          const MpiComm&    comm,
                          int               source,
                          int               tag)
{
  // Receive message
  MpiMessage message(comm);
  message.recv(source, tag);

  // Unpack message
  uint32_t key_count = message.get_uint32();
  for (uint32_t i = 0; i < key_count; ++i) {
    uint32_t key   = message.get_uint32();
    uint32_t index = message.get_uint32();

    m_keys.insert(KeyMap::value_type(key, index));
  }

  uint32_t ev_count = message.get_uint32();
  for (uint32_t i = 0; i < ev_count; ++i) {
    RemoteEvent event = message.get_event(defs);

    m_elements.push_back(event);
  }
}


void RemoteEventSet::bcast(const GlobalDefs& defs,
                           const MpiComm&    comm,
                           int               root)
{
  // Receive message
  MpiMessage message(comm);
  message.bcast(root);

  // Unpack message
  uint32_t key_count = message.get_uint32();
  for (uint32_t i = 0; i < key_count; ++i) {
    uint32_t key   = message.get_uint32();
    uint32_t index = message.get_uint32();

    m_keys.insert(KeyMap::value_type(key, index));
  }

  uint32_t ev_count = message.get_uint32();
  for (uint32_t i = 0; i < ev_count; ++i) {
    RemoteEvent event = message.get_event(defs);

    m_elements.push_back(event);
  }
}
