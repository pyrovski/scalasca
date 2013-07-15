/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "RmaGetEnd_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RmaGetEnd_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

RmaGetEnd_rep::RmaGetEnd_rep(timestamp_t timestamp,
                             uint32_t    rma_id)
  : Rma_rep(timestamp, rma_id)
{
}


RmaGetEnd_rep::RmaGetEnd_rep(const GlobalDefs& defs, Buffer& buffer)
  : Rma_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t RmaGetEnd_rep::get_type() const
{
  return RMA_GET_END;
}


bool RmaGetEnd_rep::is_typeof(event_t type) const
{
  return type == RMA_GET_END || Rma_rep::is_typeof(type);
}
