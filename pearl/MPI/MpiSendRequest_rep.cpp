/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include "MpiSendRequest_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiSendRequest_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiSendRequest_rep::MpiSendRequest_rep(timestamp_t timestamp,
                                       MpiComm*    communicator,
                                       uint32_t    destination,
                                       uint32_t    tag,
                                       uint32_t    sent,
                                       ident_t     reqid)
  : MpiSend_rep(timestamp, communicator, destination, tag, sent),
    m_reqid(reqid),
    m_next_reqoffs(0)
{
}


MpiSendRequest_rep::MpiSendRequest_rep(const GlobalDefs& defs, Buffer& buffer)
  : MpiSend_rep(defs, buffer),
    m_reqid(PEARL_NO_ID),
    m_next_reqoffs(0)
{
}


//--- Event type information ------------------------------------------------

event_t MpiSendRequest_rep::get_type() const
{
  return MPI_SEND_REQUEST;
}


bool MpiSendRequest_rep::is_typeof(event_t type) const
{
  return type == MPI_SEND_REQUEST || MpiSend_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

ident_t MpiSendRequest_rep::get_reqid() const
{
  return m_reqid;
}


//--- Modify event data -----------------------------------------------------

void MpiSendRequest_rep::set_reqid(uint32_t reqid)
{
  m_reqid = reqid;
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiSendRequest_rep::output(ostream& stream) const
{
  MpiSend_rep::output(stream);

  return stream << "  reqid  = " << m_reqid << endl;
}


//--- Find next/prev request entries (protected) ----------------------------

uint32_t MpiSendRequest_rep::get_next_reqoffs() const
{
  return m_next_reqoffs;
}


void MpiSendRequest_rep::set_next_reqoffs(uint32_t offs)
{
  m_next_reqoffs = offs;
}
