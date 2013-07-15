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
#include "Location.h"
#include "RmaPutStart_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RmaPutStart_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

RmaPutStart_rep::RmaPutStart_rep(timestamp_t timestamp,
                                 uint32_t    rma_id,
                                 uint32_t    remote,
                                 uint32_t    sent)
  : Rma_rep(timestamp, rma_id),
    m_remote(remote),
    m_sent(sent)
{
}


RmaPutStart_rep::RmaPutStart_rep(const GlobalDefs& defs, Buffer& buffer)
  : Rma_rep(defs, buffer)
{
    m_remote = buffer.get_uint32();
    m_sent   = buffer.get_uint32();
}


//--- Event type information ------------------------------------------------

event_t RmaPutStart_rep::get_type() const
{
  return RMA_PUT_START;
}


bool RmaPutStart_rep::is_typeof(event_t type) const
{
  return type == RMA_PUT_START || Rma_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

uint32_t RmaPutStart_rep::get_remote() const
{
  return m_remote;
}

uint32_t RmaPutStart_rep::get_sent() const
{
  return m_sent;
}


//--- Serialize event data (protected) --------------------------------------

void RmaPutStart_rep::pack(Buffer& buffer) const
{
  Rma_rep::pack(buffer);

  buffer.put_uint32(m_remote);
  buffer.put_uint32(m_sent);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& RmaPutStart_rep::output(ostream& stream) const
{
  Rma_rep::output(stream);

  return stream << "  remote   = " << m_remote << endl
                << "  sent     = " << m_sent << endl;
}
