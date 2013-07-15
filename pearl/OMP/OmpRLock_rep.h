/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPRLOCK_REP_H
#define PEARL_OMPRLOCK_REP_H


#include "OmpLock_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    OmpRLock_rep.h
 * @ingroup PEARL_omp
 * @brief   Declaration of the class OmpRLock_rep.
 *
 * This header file provides the declaration of the class OmpRLock_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpRLock_rep
 * @ingroup PEARL_omp
 * @brief   %Event representation for OMP_RLOCK events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR OmpRLock_rep : public OmpLock_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    OmpRLock_rep(timestamp_t timestamp, ident_t lockid);
    OmpRLock_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPRLOCK_REP_H */
