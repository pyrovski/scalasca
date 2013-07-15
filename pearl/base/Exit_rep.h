/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_EXIT_REP_H
#define PEARL_EXIT_REP_H


#include "Flow_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    Exit_rep.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Exit_rep.
 *
 * This header file provides the declaration of the class Exit_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   Exit_rep
 * @ingroup PEARL_base
 * @brief   %Event representation for leaving a source code region.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR Exit_rep : public Flow_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    Exit_rep(timestamp_t timestamp, uint64_t* metrics);
    Exit_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_EXIT_REP_H */
