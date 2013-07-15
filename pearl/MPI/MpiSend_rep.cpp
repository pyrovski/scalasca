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
#include "GlobalDefs.h"
#include "MpiSend_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiSend_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiSend_rep::MpiSend_rep(timestamp_t timestamp,
                         MpiComm*    communicator,
                         uint32_t    destination,
                         uint32_t    tag,
                         uint32_t    sent)
  : MpiP2P_rep(timestamp, communicator, tag),
    m_destination(destination),
    m_sent(sent)
{
}


MpiSend_rep::MpiSend_rep(const GlobalDefs& defs, Buffer& buffer)
  : MpiP2P_rep(defs, buffer)
{
  m_destination = buffer.get_uint32();
  m_sent        = buffer.get_uint32();
}


//--- Event type information ------------------------------------------------

event_t MpiSend_rep::get_type() const
{
  return MPI_SEND;
}


bool MpiSend_rep::is_typeof(event_t type) const
{
  return type == MPI_SEND || MpiP2P_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

uint32_t MpiSend_rep::get_sent() const
{
  return m_sent;
}


uint32_t MpiSend_rep::get_dest() const
{
  return m_destination;
}


//--- Serialize event data (protected) --------------------------------------

void MpiSend_rep::pack(Buffer& buffer) const
{
  MpiP2P_rep::pack(buffer);

  buffer.put_uint32(m_destination);
  buffer.put_uint32(m_sent);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiSend_rep::output(ostream& stream) const
{
  MpiP2P_rep::output(stream);

  return stream << "  dest   = " << m_destination << endl
                << "  sent   = " << m_sent << endl;
}
