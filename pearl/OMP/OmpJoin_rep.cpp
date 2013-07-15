/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "OmpJoin_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpJoin_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

OmpJoin_rep::OmpJoin_rep(timestamp_t timestamp)
  : Event_rep(timestamp)
{
}


OmpJoin_rep::OmpJoin_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t OmpJoin_rep::get_type() const
{
  return OMP_JOIN;
}


bool OmpJoin_rep::is_typeof(event_t type) const
{
  return type == OMP_JOIN || Event_rep::is_typeof(type);
}
