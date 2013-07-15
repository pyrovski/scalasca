/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include "MpiRequest_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRequest_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRequest_rep::MpiRequest_rep(timestamp_t timestamp, ident_t reqid)
  : Event_rep(timestamp),
    m_reqid(reqid)
{
}


MpiRequest_rep::MpiRequest_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer),
    m_reqid(PEARL_NO_ID) // request id is only local
{
}


//--- Event type information ------------------------------------------------

event_t MpiRequest_rep::get_type() const
{
  return MPI_REQUEST;
}


bool MpiRequest_rep::is_typeof(event_t type) const
{
  return type == MPI_REQUEST || Event_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

ident_t MpiRequest_rep::get_reqid() const
{
  return m_reqid;
}


//--- Modify event data -----------------------------------------------------

void MpiRequest_rep::set_reqid(uint32_t reqid)
{
  m_reqid = reqid;
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRequest_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  reqid  = " << m_reqid << endl;
}


//--- Find next/prev request entries (protected) ----------------------------

uint32_t MpiRequest_rep::get_prev_reqoffs() const
{
  return 0;
}


uint32_t MpiRequest_rep::get_next_reqoffs() const
{
  return 0;
}


void MpiRequest_rep::set_prev_reqoffs(uint32_t ptr)
{
}


void MpiRequest_rep::set_next_reqoffs(uint32_t ptr)
{
}
