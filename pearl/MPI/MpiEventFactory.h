/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIEVENTFACTORY_H
#define PEARL_MPIEVENTFACTORY_H


#include "EventFactory.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiEventFactory.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiEventFactory.
 *
 * This header file provides the declaration of the class MpiEventFactory.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiEventFactory
 * @ingroup PEARL_mpi
 * @brief   Abstract factory class for MPI-specific event representations.
 */
/*-------------------------------------------------------------------------*/


class MpiEventFactory : virtual public EventFactory
{
  public:
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
    /// @name MPI-1 Non-blocking specific factory methods
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
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIEVENTFACTORY_H */
