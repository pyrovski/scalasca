/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiRmaGetEnd_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRmaGetEnd_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRmaGetEnd_rep::MpiRmaGetEnd_rep(timestamp_t timestamp,
                         uint32_t    rma_id)
  : RmaGetEnd_rep(timestamp, rma_id)
{
}


MpiRmaGetEnd_rep::MpiRmaGetEnd_rep(const GlobalDefs& defs, Buffer& buffer)
  : RmaGetEnd_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t MpiRmaGetEnd_rep::get_type() const
{
  return RMA_GET_END;
}


bool MpiRmaGetEnd_rep::is_typeof(event_t type) const
{
  return type == MPI_RMA_GET_END || RmaGetEnd_rep::is_typeof(type);
}
