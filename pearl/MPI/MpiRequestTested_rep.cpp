/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiRequestTested_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRequestTested_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRequestTested_rep::MpiRequestTested_rep(timestamp_t timestamp,
                                           ident_t     reqid)
  : MpiRequest_rep(timestamp, reqid),
    m_next_reqoffs(0),
    m_prev_reqoffs(0)
{
}


MpiRequestTested_rep::MpiRequestTested_rep(const GlobalDefs& defs,
                                           Buffer&           buffer)
  : MpiRequest_rep(defs, buffer),
    m_next_reqoffs(0),
    m_prev_reqoffs(0)
{
}


//--- Event type information ------------------------------------------------

event_t MpiRequestTested_rep::get_type() const
{
  return MPI_REQUEST_TESTED;
}


bool MpiRequestTested_rep::is_typeof(event_t type) const
{
  return type == MPI_REQUEST_TESTED || MpiRequest_rep::is_typeof(type);
}


//--- Find next/prev request entries (protected) ----------------------------

uint32_t MpiRequestTested_rep::get_prev_reqoffs() const
{
  return m_prev_reqoffs;
}


uint32_t MpiRequestTested_rep::get_next_reqoffs() const
{
  return m_next_reqoffs;
}


void MpiRequestTested_rep::set_prev_reqoffs(uint32_t offs)
{
  m_prev_reqoffs = offs;
}


void MpiRequestTested_rep::set_next_reqoffs(uint32_t offs)
{
  m_next_reqoffs = offs;
}
