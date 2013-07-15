/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiReceiveRequest_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiReceiveRequest_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiReceiveRequest_rep::MpiReceiveRequest_rep(timestamp_t timestamp,
                                             ident_t     reqid)
  : MpiRequest_rep(timestamp, reqid),
    m_next_reqoffs(0)
{
}


MpiReceiveRequest_rep::MpiReceiveRequest_rep(const GlobalDefs& defs,
                                             Buffer&           buffer)
  : MpiRequest_rep(defs, buffer),
    m_next_reqoffs(0)
{
}


//--- Event type information ------------------------------------------------

event_t MpiReceiveRequest_rep::get_type() const
{
  return MPI_RECV_REQUEST;
}


bool MpiReceiveRequest_rep::is_typeof(event_t type) const
{
  return type == MPI_RECV_REQUEST || MpiRequest_rep::is_typeof(type);
}


//--- Find next/prev request entries (protected) ----------------------------

uint32_t MpiReceiveRequest_rep::get_next_reqoffs() const
{
  return m_next_reqoffs;
}


void MpiReceiveRequest_rep::set_next_reqoffs(uint32_t offs)
{
  m_next_reqoffs = offs;
}
