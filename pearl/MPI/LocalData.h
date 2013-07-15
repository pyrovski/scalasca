/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_LOCALDATA_H
#define PEARL_LOCALDATA_H


#include <map>
#include <vector>

#include "Event.h"
#include "RemoteEvent.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    LocalData.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class LocalData.
 *
 * This header file provides the declaration of the class LocalData.
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
 * @class   LocalData
 * @ingroup PEARL_mpi
 * @brief   Container class for a set of local events with associated roles.
 */
/*-------------------------------------------------------------------------*/

class LocalData
{
  public:
    /// @name Destructor
    /// @{

    ~LocalData();

    /// @}
    /// @name Adding data
    /// @{

    void add_event(const Event& event,  uint32_t key);
    void add_buffer(Buffer*     buffer, uint32_t key);

    /// @}
    /// @name Retrieving data
    /// @{

    Event   get_event (uint32_t key) const;
    Buffer* get_buffer(uint32_t key) const;

    /// @}
    /// @name Removing events
    /// @{

    void clear();

    /// @}
    /// @name Sending event sets
    /// @}

    void        send  (const MpiComm& comm, int dest, int tag) const;
    MpiMessage* isend (const MpiComm& comm, int dest, int tag) const;
    MpiMessage* issend(const MpiComm& comm, int dest, int tag) const;

    void bcast(const MpiComm& comm, int root) const;

    /// @}


  private:
    /// Container type for role |-@> index mapping
    typedef std::map<uint32_t, uint32_t> KeyMap;

    /// Container type for local events
    typedef std::vector<Event>   EventList;
    typedef std::vector<Buffer*> BufferList;


    /// Mapping table key (role ID) |-@> index
    KeyMap     m_event_keys;
    /// Mapping table key (role ID) |-@> index
    KeyMap     m_buffer_keys;

    /// Current set of events
    EventList  m_events;
    /// Current set of buffers
    BufferList m_buffers;


    /* Private methods */
    void pack(MpiMessage& message) const;
};


}   /* namespace pearl */


#endif   /* !PEARL_LOCALDATA_H */
