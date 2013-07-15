/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPALOCK_REP_H
#define PEARL_OMPALOCK_REP_H


#include "OmpLock_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    OmpALock_rep.h
 * @ingroup PEARL_omp
 * @brief   Declaration of the class OmpALock_rep.
 *
 * This header file provides the declaration of the class OmpALock_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpALock_rep
 * @ingroup PEARL_omp
 * @brief   %Event representation for OMP_ALOCK events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR OmpALock_rep : public OmpLock_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    OmpALock_rep(timestamp_t timestamp, ident_t lockid);
    OmpALock_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPALOCK_REP_H */
