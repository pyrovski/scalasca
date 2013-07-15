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
#include "MpiCollExit_rep.h"
#include "MpiComm.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiCollExit_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiCollExit_rep::MpiCollExit_rep(timestamp_t timestamp,
                                 MpiComm*    communicator,
                                 uint32_t    root,
                                 uint32_t    sent,
                                 uint32_t    received,
                                 uint64_t*   metrics)
  : Exit_rep(timestamp, metrics),
    m_communicator(communicator),
    m_root(root),
    m_sent(sent),
    m_received(received)
{
}


MpiCollExit_rep::MpiCollExit_rep(const GlobalDefs& defs, Buffer& buffer)
  : Exit_rep(defs, buffer)
{
  m_communicator = dynamic_cast<MpiComm*>(defs.get_comm(buffer.get_id()));
  m_root         = buffer.get_uint32();
  m_sent         = buffer.get_uint32();
  m_received     = buffer.get_uint32();
}


//--- Event type information ------------------------------------------------

event_t MpiCollExit_rep::get_type() const
{
  return MPI_COLLEXIT;
}


bool MpiCollExit_rep::is_typeof(event_t type) const
{
  return type == MPI_COLLEXIT || Exit_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

MpiComm* MpiCollExit_rep::get_comm() const
{
  return m_communicator;
}


uint32_t MpiCollExit_rep::get_root() const
{
  return m_root;
}


uint32_t MpiCollExit_rep::get_sent() const
{
  return m_sent;
}


uint32_t MpiCollExit_rep::get_received() const
{
  return m_received;
}


//--- Serialize event data (protected) --------------------------------------

void MpiCollExit_rep::pack(Buffer& buffer) const
{
  Exit_rep::pack(buffer);

  buffer.put_id(m_communicator->get_id());
  buffer.put_uint32(m_root);
  buffer.put_uint32(m_sent);
  buffer.put_uint32(m_received);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiCollExit_rep::output(ostream& stream) const
{
  Exit_rep::output(stream);

  stream << "  comm   = " << *m_communicator << endl
         << "  root   = ";
  if (PEARL_NO_ID != m_root)
    stream << m_root;
  else
    stream << "-";
  return stream << endl << "  sent   = " << m_sent << endl
                        << "  rcvd   = " << m_received << endl;
}
