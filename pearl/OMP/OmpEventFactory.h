/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPEVENTFACTORY_H
#define PEARL_OMPEVENTFACTORY_H


#include "EventFactory.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    OmpEventFactory.h
 * @ingroup PEARL_omp
 * @brief   Declaration of the class OmpEventFactory.
 *
 * This header file provides the declaration of the class OmpEventFactory.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpEventFactory
 * @ingroup PEARL_omp
 * @brief   Abstract factory class for OMP-specific event representations.
 */
/*-------------------------------------------------------------------------*/


class OmpEventFactory : virtual public EventFactory
{
  public:
    /// @name OMP specific factory methods
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
                                         ident_t           lockid) const;
    virtual Event_rep* createOmpRLock   (const GlobalDefs& defs,
                                         timestamp_t       time,
                                         ident_t           lockid) const;

    /// @}
    /// @name Buffer-based factory methods
    /// @{

    virtual Event_rep* createEvent(event_t           type,
                                   const GlobalDefs& defs,
                                   Buffer&           buffer) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPEVENTFACTORY_H */
