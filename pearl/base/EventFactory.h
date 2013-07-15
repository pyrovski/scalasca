/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_EVENTFACTORY_H
#define PEARL_EVENTFACTORY_H


#include <cassert>

#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    EventFactory.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class EventFactory.
 *
 * This header file provides the declaration of the class EventFactory.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Buffer;
class Event_rep;
class GlobalDefs;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   EventFactory
 * @ingroup PEARL_base
 * @brief   Abstract factory class for event representations.
 */
/*-------------------------------------------------------------------------*/

class EventFactory
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~EventFactory();

    /// @}
    /// @name Generic factory methods
    /// @{

    Event_rep* createEnter  (const GlobalDefs& defs,
                             timestamp_t       time,
                             ident_t           region,
                             uint64_t*         metrics) const;
    Event_rep* createEnterCS(const GlobalDefs& defs,
                             timestamp_t       time,
                             ident_t           csite,
                             uint64_t*         metrics) const;
    Event_rep* createExit   (const GlobalDefs& defs,
                             timestamp_t       time,
                             uint64_t*         metrics) const;

    /// @}
    /// @name MPI-1 specific factory methods
    /// @{

    virtual Event_rep* createMpiSend    (const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           comm,
                                         uint32_t          dest,
                                         uint32_t          tag,
                                         uint32_t          sent) const;
    virtual Event_rep* createMpiRecv    (const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           comm,
                                         uint32_t          source,
                                         uint32_t          tag) const;
    virtual Event_rep* createMpiCollExit(const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           comm,
                                         uint32_t          root,
                                         uint32_t          sent,
                                         uint32_t          recvd,
                                         uint64_t*         metrics) const;
    /// @}
    /// @name MPI-1 non-blocking specific factory methods
    /// @{

    virtual Event_rep* createMpiCancelled(
                                         const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           reqid) const;
    virtual Event_rep* createMpiSendRequest(
                                         const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           comm,
                                         uint32_t          dest,
                                         uint32_t          tag,
                                         uint32_t          sent,
                                         ident_t           reqid) const;
    virtual Event_rep* createMpiSendComplete(
                                         const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           reqid) const;
    virtual Event_rep* createMpiRecvComplete(
                                         const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           comm,
                                         uint32_t          source,
                                         uint32_t          tag,
                                         ident_t           reqid) const;
    virtual Event_rep* createMpiRecvRequest(
                                         const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           reqid) const;
    virtual Event_rep* createMpiRequestTested(
                                         const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           reqid) const;

    /// @}
    /// @name OpenMP specific factory methods
    /// @{

    virtual Event_rep* createOmpFork    (const GlobalDefs& defs,
                                         timestamp_t       time) const;
    virtual Event_rep* createOmpJoin    (const GlobalDefs& defs,
                                         timestamp_t       time) const;
    virtual Event_rep* createOmpCollExit(const GlobalDefs& defs,
                                         timestamp_t       time,
                                         uint64_t*         metrics) const;
    virtual Event_rep* createOmpALock   (const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           lkid) const;
    virtual Event_rep* createOmpRLock   (const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           lkid) const;

    /// @}
    /// @name RMA specific factory methods
    /// @{

    virtual Event_rep* createRmaPutStart(const GlobalDefs& defs,
                                         timestamp_t       time,
                                         uint32_t          rma_id,
                                         uint32_t          target,
                                         uint32_t          length) const;
    virtual Event_rep* createRmaPutEnd  (const GlobalDefs& defs,
                                         timestamp_t       time,
                                         uint32_t          rma_id) const;
    virtual Event_rep* createRmaGetStart(const GlobalDefs& defs,
                                         timestamp_t       time,
                                         uint32_t          rma_id,
                                         uint32_t          origin,
                                         uint32_t          length) const;
    virtual Event_rep* createRmaGetEnd  (const GlobalDefs& defs,
                                         timestamp_t       time,
                                         uint32_t          rma_id) const;

    /// @}
    /// @name MPI-2 RMA specific factory methods
    /// @{

    virtual Event_rep* createMpiRmaPutStart(const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          rma_id,
                                            uint32_t          target,
                                            uint32_t          length,
                                            ident_t           window) const;
    virtual Event_rep* createMpiRmaPutEnd  (const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          rma_id) const;
    virtual Event_rep* createMpiRmaGetStart(const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          rma_id,
                                            uint32_t          origin,
                                            uint32_t          length,
                                            ident_t           window) const;
    virtual Event_rep* createMpiRmaGetEnd  (const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          rma_id) const;
    virtual Event_rep* createMpiRmaExit    (const GlobalDefs& defs,
                                            timestamp_t       time,
                                            ident_t           window,
                                            ident_t           group,
                                            bool              sync,
                                            uint64_t*         metrics) const;
    virtual Event_rep* createMpiRmaCollExit(const GlobalDefs& defs,
                                            timestamp_t       time,
                                            ident_t           window,
                                            uint64_t*         metrics) const;
    virtual Event_rep* createMpiRmaLock    (const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          location,
                                            ident_t           window,
                                            bool              exclusive) const;
    virtual Event_rep* createMpiRmaUnlock  (const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          location,
                                            ident_t           window) const;

    /// @}
    /// @name Buffer-based factory methods
    /// @{

    virtual Event_rep* createEvent(event_t           type,
                                   const GlobalDefs& defs,
                                   Buffer&           buffer) const;

    /// @}
    /// @name Singleton interface
    /// @{

    static const EventFactory* instance();
    static void registerFactory(const EventFactory* factory);

    /// @}


  protected:
    /// @name Data copy methods
    /// @{

    uint64_t* copyMetrics(const GlobalDefs& defs, uint64_t* metrics) const;

    /// @}


  private:
    /// Single factory instance
    static const EventFactory* m_instance;
};


//--- Inline methods --------------------------------------------------------

inline const EventFactory* EventFactory::instance()
{
  assert(m_instance);
  return m_instance;
}


}   /* namespace pearl */


#endif   /* !PEARL_EVENTFACTORY_H */
