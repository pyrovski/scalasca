/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiRmaPutEnd_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRmaPutEnd_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRmaPutEnd_rep::MpiRmaPutEnd_rep(timestamp_t timestamp,
                                   uint32_t    rma_id)
  : RmaPutEnd_rep(timestamp, rma_id)
{
}


MpiRmaPutEnd_rep::MpiRmaPutEnd_rep(const GlobalDefs& defs, Buffer& buffer)
  : RmaPutEnd_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t MpiRmaPutEnd_rep::get_type() const
{
  return MPI_RMA_PUT_END;
}


bool MpiRmaPutEnd_rep::is_typeof(event_t type) const
{
  return type == MPI_RMA_PUT_END || RmaPutEnd_rep::is_typeof(type);
}

