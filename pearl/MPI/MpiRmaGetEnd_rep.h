/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRMAGETEND_REP_H
#define PEARL_MPIRMAGETEND_REP_H


#include "RmaGetEnd_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRmaGetEnd_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRmaGetEnd_rep.
 *
 * This header file provides the declaration of the class MpiRmaGetEnd_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRmaGetEnd_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for RMA_GET_END events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaGetEnd_rep : public RmaGetEnd_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiRmaGetEnd_rep(timestamp_t timestamp,
                     uint32_t    rma_id);
    MpiRmaGetEnd_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

};


}   /* namespace pearl */


#endif   /* !PEARL_RMAGETEND_REP_H */
