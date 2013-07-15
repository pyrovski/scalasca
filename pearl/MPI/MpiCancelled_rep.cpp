/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiCancelled_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiCancelled_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiCancelled_rep::MpiCancelled_rep(timestamp_t timestamp,
                                   ident_t     reqid)
  : MpiRequest_rep(timestamp, reqid),
    m_prev_reqoffs(0)
{
}


MpiCancelled_rep::MpiCancelled_rep(const GlobalDefs& defs, Buffer& buffer)
  : MpiRequest_rep(defs, buffer),
    m_prev_reqoffs(0)
{
}


//--- Event type information ------------------------------------------------

event_t MpiCancelled_rep::get_type() const
{
  return MPI_CANCELLED;
}


bool MpiCancelled_rep::is_typeof(event_t type) const
{
  return type == MPI_CANCELLED || MpiRequest_rep::is_typeof(type);
}


//--- Find next/prev request entries ----------------------------------------

uint32_t MpiCancelled_rep::get_prev_reqoffs() const
{
  return m_prev_reqoffs;
}


void MpiCancelled_rep::set_prev_reqoffs(uint32_t offs)
{
  m_prev_reqoffs = offs;
}
