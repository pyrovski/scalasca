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
#include "RmaGetStart_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RmaGetStart_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

RmaGetStart_rep::RmaGetStart_rep(timestamp_t timestamp,
                                 uint32_t    rma_id,
                                 uint32_t    remote,
                                 uint32_t    received)
  : Rma_rep(timestamp, rma_id),
    m_remote(remote),
    m_received(received)
{
}


RmaGetStart_rep::RmaGetStart_rep(const GlobalDefs& defs, Buffer& buffer)
  : Rma_rep(defs, buffer)
{
    m_remote   = buffer.get_uint32();
    m_received = buffer.get_uint32();
}


//--- Event type information ------------------------------------------------

event_t RmaGetStart_rep::get_type() const
{
  return RMA_GET_START;
}


bool RmaGetStart_rep::is_typeof(event_t type) const
{
  return type == RMA_GET_START || Rma_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

uint32_t RmaGetStart_rep::get_remote() const
{
  return m_remote;
}

uint32_t RmaGetStart_rep::get_received() const
{
  return m_received;
}


//--- Serialize event data (protected) --------------------------------------

void RmaGetStart_rep::pack(Buffer& buffer) const
{
  Rma_rep::pack(buffer);

  buffer.put_uint32(m_remote);
  buffer.put_uint32(m_received);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& RmaGetStart_rep::output(ostream& stream) const
{
  Rma_rep::output(stream);

  return stream << "  remote     = " << m_remote << endl
                << "  received   = " << m_received << endl;
}
