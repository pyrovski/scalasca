/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_HYBRIDEVENTFACTORY_H
#define PEARL_HYBRIDEVENTFACTORY_H


#include "MpiEventFactory.h"
#include "OmpEventFactory.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    HybridEventFactory.h
 * @ingroup PEARL_hybrid
 * @brief   Declaration of the class HybridEventFactory.
 *
 * This header file provides the declaration of the class HybridEventFactory.
 */
/*-------------------------------------------------------------------------*/

namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   HybridEventFactory
 * @ingroup PEARL_hybrid
 * @brief   Abstract factory class for MPI and OpenMP event representations.
 */
/*-------------------------------------------------------------------------*/

class HybridEventFactory : public MpiEventFactory, public OmpEventFactory
{
 public:
    /// @}
    /// @name Buffer-based factory methods
    /// @{

    virtual Event_rep* createEvent(event_t           type,
                                   const GlobalDefs& defs,
                                   Buffer&           buffer) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_HYBRIDEVENTFACTORY_H */
