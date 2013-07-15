/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiSendComplete_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiSendComplete_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiSendComplete_rep::MpiSendComplete_rep(timestamp_t timestamp,
                                         ident_t     reqid)
  : MpiRequest_rep(timestamp, reqid),
    m_prev_reqoffs(0)
{
}


MpiSendComplete_rep::MpiSendComplete_rep(const GlobalDefs& defs, Buffer& buffer)
  : MpiRequest_rep(defs, buffer),
    m_prev_reqoffs(0)
{
}


//--- Event type information ------------------------------------------------

event_t MpiSendComplete_rep::get_type() const
{
  return MPI_SEND_COMPLETE;
}


bool MpiSendComplete_rep::is_typeof(event_t type) const
{
  return type == MPI_SEND_COMPLETE || MpiRequest_rep::is_typeof(type);
}


//--- Find next/prev request entries ----------------------------------------

uint32_t MpiSendComplete_rep::get_prev_reqoffs() const
{
  return m_prev_reqoffs;
}


void MpiSendComplete_rep::set_prev_reqoffs(uint32_t offs)
{
  m_prev_reqoffs = offs;
}
