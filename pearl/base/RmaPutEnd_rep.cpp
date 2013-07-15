/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "RmaPutEnd_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RmaPutEnd_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

RmaPutEnd_rep::RmaPutEnd_rep(timestamp_t timestamp,
                             uint32_t    rma_id)
  : Rma_rep(timestamp, rma_id)
{
}


RmaPutEnd_rep::RmaPutEnd_rep(const GlobalDefs& defs, Buffer& buffer)
  : Rma_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t RmaPutEnd_rep::get_type() const
{
  return RMA_PUT_END;
}


bool RmaPutEnd_rep::is_typeof(event_t type) const
{
  return type == RMA_PUT_END || Rma_rep::is_typeof(type);
}
