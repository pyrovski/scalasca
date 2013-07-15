/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "OmpALock_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpALock_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

OmpALock_rep::OmpALock_rep(timestamp_t timestamp, ident_t lockid)
  : OmpLock_rep(timestamp, lockid)
{
}


OmpALock_rep::OmpALock_rep(const GlobalDefs& defs, Buffer& buffer)
  : OmpLock_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t OmpALock_rep::get_type() const
{
  return OMP_ALOCK;
}


bool OmpALock_rep::is_typeof(event_t type) const
{
  return type == OMP_ALOCK || OmpLock_rep::is_typeof(type);
}
