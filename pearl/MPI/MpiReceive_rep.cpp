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
#include "MpiReceive_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiReceive_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiReceive_rep::MpiReceive_rep(timestamp_t timestamp,
                               MpiComm*    communicator,
                               uint32_t    source,
                               uint32_t    tag)
  : MpiP2P_rep(timestamp, communicator, tag),
    m_source(source)
{
}


MpiReceive_rep::MpiReceive_rep(const GlobalDefs& defs, Buffer& buffer)
  : MpiP2P_rep(defs, buffer)
{
  m_source = buffer.get_uint32();
}


//--- Event type information ------------------------------------------------

event_t MpiReceive_rep::get_type() const
{
  return MPI_RECV;
}


bool MpiReceive_rep::is_typeof(event_t type) const
{
  return type == MPI_RECV || MpiP2P_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

uint32_t MpiReceive_rep::get_source() const
{
  return m_source;
}


//--- Serialize event data (protected) --------------------------------------

void MpiReceive_rep::pack(Buffer& buffer) const
{
  MpiP2P_rep::pack(buffer);

  buffer.put_uint32(m_source);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiReceive_rep::output(ostream& stream) const
{
  MpiP2P_rep::output(stream);

  return stream << "  source = " << m_source << endl;
}
