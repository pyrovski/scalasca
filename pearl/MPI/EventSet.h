/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_EVENTSET_H
#define PEARL_EVENTSET_H


#include <map>
#include <vector>

#include "Event.h"
#include "RemoteEvent.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    EventSet.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class EventSet.
 *
 * This header file provides the declaration of the class EventSet.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class GlobalDefs;
class MpiComm;
class MpiMessage;


/*-------------------------------------------------------------------------*/
/**
 * @class   EventSet
 * @ingroup PEARL_mpi
 * @brief   Container class for a set of local events with associated roles.
 */
/*-------------------------------------------------------------------------*/

class EventSet
{
  public:
    /// @name Adding events
    /// @{

    void add_event(const Event& event, uint32_t key);

    /// @}
    /// @name Retrieving events
    /// @{

    Event get_event(uint32_t key) const;

    /// @}
    /// @name Removing events
    /// @{

    void clear();

    /// @}
    /// @name Sending event sets
    /// @}

    void        send(const MpiComm& comm, int dest, int tag) const;
    MpiMessage* isend(const MpiComm& comm, int dest, int tag) const;

    void bcast(const MpiComm& comm, int root) const;

    /// @}
    /// @name Collective operations
    /// @{

    RemoteEvent allreduce_min(const GlobalDefs& defs, const MpiComm& comm) const;
    RemoteEvent allreduce_max(const GlobalDefs& defs, const MpiComm& comm) const;

    /// @}


  private:
    /// Container type for role |-@> index mapping
    typedef std::map<uint32_t,uint32_t> KeyMap;

    /// Container type for local events
    typedef std::vector<Event> EventList;


    /// Mapping table key (role ID) |-@> index
    KeyMap m_keys;

    /// Current set of events
    EventList m_elements;


    /* Private methods */
    void pack(MpiMessage& message) const;
};


}   /* namespace pearl */


#endif   /* !PEARL_EVENTSET_H */
