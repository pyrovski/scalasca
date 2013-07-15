/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "OmpRLock_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpRLock_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

OmpRLock_rep::OmpRLock_rep(timestamp_t timestamp, ident_t lockid)
  : OmpLock_rep(timestamp, lockid)
{
}


OmpRLock_rep::OmpRLock_rep(const GlobalDefs& defs, Buffer& buffer)
  : OmpLock_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t OmpRLock_rep::get_type() const
{
  return OMP_RLOCK;
}


bool OmpRLock_rep::is_typeof(event_t type) const
{
  return type == OMP_RLOCK || OmpLock_rep::is_typeof(type);
}
