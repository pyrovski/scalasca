/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPJOIN_REP_H
#define PEARL_OMPJOIN_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    OmpJoin_rep.h
 * @ingroup PEARL_omp
 * @brief   Declaration of the class OmpJoin_rep.
 *
 * This header file provides the declaration of the class OmpJoin_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpJoin_rep
 * @ingroup PEARL_omp
 * @brief   %Event representation for OMP_JOIN events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR OmpJoin_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    OmpJoin_rep(timestamp_t timestamp);
    OmpJoin_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPJOIN_REP_H */
