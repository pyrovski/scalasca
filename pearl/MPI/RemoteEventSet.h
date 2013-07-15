/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_REMOTEEVENTSET_H
#define PEARL_REMOTEEVENTSET_H


#include <map>
#include <vector>

#include "RemoteEvent.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    RemoteEventSet.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class RemoteEventSet.
 *
 * This header file provides the declaration of the class RemoteEventSet.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class GlobalDefs;
class MpiComm;


/*-------------------------------------------------------------------------*/
/**
 * @class   RemoteEventSet
 * @ingroup PEARL_mpi
 * @brief   Container class for a set of remote events with associated roles.
 */
/*-------------------------------------------------------------------------*/

class RemoteEventSet
{
  public:
    /// @name Adding events
    /// @{

    void add_event(const RemoteEvent& event, uint32_t key);

    /// @}
    /// @name Retrieving eventsconst MpiComm&
    /// @{

    RemoteEvent get_event(uint32_t key) const;

    /// @}
    /// @name Removing events
    /// @{

    void clear();

    /// @}
    /// @name Receiving event sets
    /// @{

    void recv(const GlobalDefs& defs, const MpiComm& comm, int source, int tag);
    void bcast(const GlobalDefs& defs, const MpiComm& comm, int root);

    /// @}


  private:
    /// Container type for role |-@> index mapping
    typedef std::map<uint32_t,uint32_t> KeyMap;

    /// Container type for local events
    typedef std::vector<RemoteEvent> EventList;


    /// Mapping table key (role ID) |-@> index
    KeyMap m_keys;

    /// Current set of events
    EventList m_elements;
};


}   /* namespace pearl */


#endif   /* !PEARL_REMOTEEVENTSET_H */
