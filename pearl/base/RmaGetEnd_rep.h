/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_RMAGETEND_REP_H
#define PEARL_RMAGETEND_REP_H


#include "Rma_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    RmaGetEnd_rep.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class RmaGetEnd_rep.
 *
 * This header file provides the declaration of the class RmaGetEnd_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   RmaGetEnd_rep
 * @ingroup PEARL_base
 * @brief   %Event representation for RMA_GET_END events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR RmaGetEnd_rep : public Rma_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    RmaGetEnd_rep(timestamp_t timestamp,
                  uint32_t    rma_id);
    RmaGetEnd_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

};


}   /* namespace pearl */


#endif   /* !PEARL_RMAGETEND_REP_H */
