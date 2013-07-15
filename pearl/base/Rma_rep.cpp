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
#include "RmaWindow.h"
#include "Rma_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RMA_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Event type information ------------------------------------------------

event_t Rma_rep::get_type() const
{
  return RMA;
}


bool Rma_rep::is_typeof(event_t type) const
{
  return type == RMA || Event_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

uint32_t Rma_rep::get_rma_id() const
{
  return m_rma_id;
}


//--- Constructors & destructor (protected) ---------------------------------

Rma_rep::Rma_rep(timestamp_t timestamp,
                 uint32_t    rma_id)
  : Event_rep(timestamp),
    m_rma_id(rma_id)
{
}


Rma_rep::Rma_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
    m_rma_id = buffer.get_uint32();    
}


//--- Serialize event data (protected) --------------------------------------

void Rma_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_uint32(m_rma_id);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& Rma_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  rma id   = " << m_rma_id << endl;
}
