/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include "MpiReceiveComplete_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiReceiveComplete_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiReceiveComplete_rep::MpiReceiveComplete_rep(timestamp_t timestamp,
                                               MpiComm*    communicator,
                                               uint32_t    source,
                                               uint32_t    tag,
                                               ident_t     reqid)
  : MpiReceive_rep(timestamp, communicator, source, tag),
    m_reqid(reqid),
    m_prev_reqoffs(0)
{
}


MpiReceiveComplete_rep::MpiReceiveComplete_rep(const GlobalDefs& defs,
                                               Buffer&           buffer)
  : MpiReceive_rep(defs, buffer),
    m_reqid(PEARL_NO_ID),
    m_prev_reqoffs(0)
{

}


//--- Event type information ------------------------------------------------

event_t MpiReceiveComplete_rep::get_type() const
{
  return MPI_RECV_COMPLETE;
}


bool MpiReceiveComplete_rep::is_typeof(event_t type) const
{
  return type == MPI_RECV_COMPLETE || MpiReceive_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

ident_t MpiReceiveComplete_rep::get_reqid() const
{
  return m_reqid;
}


//--- Modify event data -----------------------------------------------------

void MpiReceiveComplete_rep::set_reqid(uint32_t reqid)
{
  m_reqid = reqid;
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiReceiveComplete_rep::output(ostream& stream) const
{
  MpiReceive_rep::output(stream);

  return stream << "  reqid  = " << m_reqid << endl;
}


//--- Find next/prev request entries (protected) ----------------------------

uint32_t MpiReceiveComplete_rep::get_prev_reqoffs() const
{
  return m_prev_reqoffs;
}


void MpiReceiveComplete_rep::set_prev_reqoffs(uint32_t offs)
{
  m_prev_reqoffs = offs;
}
