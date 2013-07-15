/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "OmpCollExit_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpCollExit_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

OmpCollExit_rep::OmpCollExit_rep(timestamp_t timestamp, uint64_t* metrics)
  : Exit_rep(timestamp, metrics)
{
}


OmpCollExit_rep::OmpCollExit_rep(const GlobalDefs& defs, Buffer& buffer)
  : Exit_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t OmpCollExit_rep::get_type() const
{
  return OMP_COLLEXIT;
}


bool OmpCollExit_rep::is_typeof(event_t type) const
{
  return type == OMP_COLLEXIT || Exit_rep::is_typeof(type);
}
