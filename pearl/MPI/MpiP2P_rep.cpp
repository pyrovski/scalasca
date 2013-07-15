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
#include "MpiComm.h"
#include "MpiP2P_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiP2P_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Event type information ------------------------------------------------

bool MpiP2P_rep::is_typeof(event_t type) const
{
  return type == MPI_P2P || Event_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

MpiComm* MpiP2P_rep::get_comm() const
{
  return m_communicator;
}


uint32_t MpiP2P_rep::get_tag() const
{
  return m_tag;
}


ident_t MpiP2P_rep::get_reqid() const
{
  return PEARL_NO_ID;
}


//--- Constructors & destructor (protected) ---------------------------------

MpiP2P_rep::MpiP2P_rep(timestamp_t timestamp,
                       MpiComm*    communicator,
                       uint32_t    tag)
  : Event_rep(timestamp),
    m_communicator(communicator),
    m_tag(tag)
{
}


MpiP2P_rep::MpiP2P_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  m_communicator = dynamic_cast<MpiComm*>(defs.get_comm(buffer.get_id()));
  m_tag          = buffer.get_uint32();
}


//--- Serialize event data (protected) --------------------------------------

void MpiP2P_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_id(m_communicator->get_id());
  buffer.put_uint32(m_tag);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiP2P_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  comm   = " << *m_communicator << endl
                << "  tag    = " << m_tag << endl;
}


//--- Find previous / next communication request entries (protected) --------

uint32_t MpiP2P_rep::get_prev_reqoffs() const
{
  return 0;
}


uint32_t MpiP2P_rep::get_next_reqoffs() const
{
  return 0;
}


void MpiP2P_rep::set_prev_reqoffs(uint32_t ptr)
{
}


void MpiP2P_rep::set_next_reqoffs(uint32_t ptr)
{
}
