/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include "Buffer.h"
#include "OmpLock_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpLock_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

OmpLock_rep::OmpLock_rep(timestamp_t timestamp, ident_t lockid)
  : Event_rep(timestamp),
    m_lockid(lockid)
{
}


OmpLock_rep::OmpLock_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  m_lockid  = buffer.get_id();
}


//--- Event type information ------------------------------------------------

event_t OmpLock_rep::get_type() const
{
  return OMP_LOCK;
}


//--- Access event data -----------------------------------------------------

ident_t OmpLock_rep::get_lockid() const 
{
  return m_lockid;
}


bool OmpLock_rep::is_typeof(event_t type) const
{
  return type == OMP_LOCK || Event_rep::is_typeof(type);
}


//--- Serialize event data (protected) --------------------------------------

void OmpLock_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_id(m_lockid);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& OmpLock_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  lockid = " << m_lockid << endl;
}
