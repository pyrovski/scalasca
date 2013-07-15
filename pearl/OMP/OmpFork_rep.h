/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPFORK_REP_H
#define PEARL_OMPFORK_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    OmpFork_rep.h
 * @ingroup PEARL_omp
 * @brief   Declaration of the class OmpFork_rep.
 *
 * This header file provides the declaration of the class OmpFork_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpFork_rep
 * @ingroup PEARL_omp
 * @brief   %Event representation for OMP_FORK events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR OmpFork_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    OmpFork_rep(timestamp_t timestamp);
    OmpFork_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPFORK_REP_H */
