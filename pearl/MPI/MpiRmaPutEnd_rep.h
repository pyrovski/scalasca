/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRMAPUTEND_REP_H
#define PEARL_MPIRMAPUTEND_REP_H


#include "RmaPutEnd_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRmaPutEnd_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRmaPutEnd_rep.
 *
 * This header file provides the declaration of the class MpiRmaPutEnd_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRmaPutEnd_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for RMA_PUT_END events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaPutEnd_rep : public RmaPutEnd_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiRmaPutEnd_rep(timestamp_t timestamp,
                     uint32_t    rma_id);
    MpiRmaPutEnd_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIRMAPUTEND_REP_H */
