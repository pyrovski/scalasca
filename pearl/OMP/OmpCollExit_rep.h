/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPCOLLEXIT_REP_H
#define PEARL_OMPCOLLEXIT_REP_H


#include "Exit_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    OmpCollExit_rep.h
 * @ingroup PEARL_omp
 * @brief   Declaration of the class OmpCollExit_rep.
 *
 * This header file provides the declaration of the class OmpCollExit_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpCollExit_rep
 * @ingroup PEARL_omp
 * @brief   %Event representation for OMP_COLLEXIT events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR OmpCollExit_rep : public Exit_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    OmpCollExit_rep(timestamp_t timestamp, uint64_t* metrics);
    OmpCollExit_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPCOLLEXIT_REP_H */
