/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "OmpFork_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpFork_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

OmpFork_rep::OmpFork_rep(timestamp_t timestamp)
  : Event_rep(timestamp)
{
}


OmpFork_rep::OmpFork_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t OmpFork_rep::get_type() const
{
  return OMP_FORK;
}


bool OmpFork_rep::is_typeof(event_t type) const
{
  return type == OMP_FORK || Event_rep::is_typeof(type);
}
