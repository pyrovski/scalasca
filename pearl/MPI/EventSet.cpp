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
#include "EventSet.h"
#include "MpiComm.h"
#include "MpiMessage.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class EventSet
 *
 *---------------------------------------------------------------------------
 */

//--- Adding events ---------------------------------------------------------

void EventSet::add_event(const Event& event, uint32_t key)
{
  EventList::iterator it = find(m_elements.begin(),
                                m_elements.end(),
                                event);

  uint32_t index = it - m_elements.begin();
  if (it == m_elements.end())
    m_elements.push_back(event);

  m_keys.insert(KeyMap::value_type(key, index));
}


//--- Retrieving events -----------------------------------------------------

Event EventSet::get_event(uint32_t key) const
{
  KeyMap::const_iterator it = m_keys.find(key);
  if (it == m_keys.end())
    throw RuntimeError("EventSet::get_event(uint32_t) -- Unknown key.");

  return m_elements[it->second];
}


//--- Removing events -------------------------------------------------------

void EventSet::clear()
{
  m_keys.clear();
  m_elements.clear();
}


//--- Sending event sets ----------------------------------------------------

void EventSet::send(const MpiComm& comm, int dest, int tag) const
{
  MpiMessage message(comm);

  // Construct message
  pack(message);

  // Send message
  message.send(dest, tag);
}


MpiMessage* EventSet::isend(const MpiComm& comm, int dest, int tag) const
{
  MpiMessage* message = new MpiMessage(comm);

  // Construct message
  pack(*message);

  // Send message
  message->isend(dest, tag);

  return message;
}


void EventSet::bcast(const MpiComm& comm, int root) const
{
  MpiMessage message(comm);

  // Construct message
  pack(message);

  // Send message
  message.bcast(root);
}


//--- Collective operations -------------------------------------------------

RemoteEvent EventSet::allreduce_min(const GlobalDefs& defs,
                                    const MpiComm&    comm) const
{
  struct {
    double time;
    int    rank;
  } local, global;

  MPI_Comm mpiComm = comm.get_comm();

  // Find local event with minimal timestamp
  EventList::const_iterator it = min_element(m_elements.begin(),
                                             m_elements.end());
  local.time = (*it)->get_time();
  MPI_Comm_rank(mpiComm, &local.rank);

  // Determine global minimal timestamp
  MPI_Allreduce(&local, &global, 1, MPI_DOUBLE_INT, MPI_MINLOC, mpiComm);

  // Distribute event
  MpiMessage eventmsg(mpiComm);
  if (global.rank == local.rank && global.time == local.time)
    eventmsg.put_event(*it);
  eventmsg.bcast(global.rank);

  return RemoteEvent(defs, eventmsg);
}


RemoteEvent EventSet::allreduce_max(const GlobalDefs& defs,
                                    const MpiComm&    comm) const
{
  struct {
    double time;
    int    rank;
  } local, global;

  MPI_Comm mpiComm = comm.get_comm();

  // Find local event with maximal timestamp
  EventList::const_iterator it = max_element(m_elements.begin(),
                                             m_elements.end());
  local.time = (*it)->get_time();
  MPI_Comm_rank(mpiComm, &local.rank);

  // Determine global minimal timestamp
  MPI_Allreduce(&local, &global, 1, MPI_DOUBLE_INT, MPI_MAXLOC, mpiComm);

  // Distribute event
  MpiMessage eventmsg(mpiComm);
  if (global.rank == local.rank && global.time == local.time)
    eventmsg.put_event(*it);
  eventmsg.bcast(global.rank);

  return RemoteEvent(defs, eventmsg);
}


//--- Private methods -------------------------------------------------------

void EventSet::pack(MpiMessage& message) const
{
  uint32_t key_count = m_keys.size();
  uint32_t ev_count  = m_elements.size();

  // Construct message
  message.put_uint32(key_count);
  KeyMap::const_iterator kit = m_keys.begin();
  while (kit != m_keys.end()) {
    message.put_uint32(kit->first);
    message.put_uint32(kit->second);
    ++kit;
  }

  message.put_uint32(ev_count);
  EventList::const_iterator eit = m_elements.begin();
  while (eit != m_elements.end()) {
    message.put_event(*eit);
    ++eit;
  }
}
