/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Flow_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Flow_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

Flow_rep::~Flow_rep()
{
#ifdef PEARL_ENABLE_METRICS
  delete[] m_metrics;
#endif
}


//--- Event type information ------------------------------------------------

bool Flow_rep::is_typeof(event_t type) const
{
  return type == FLOW || Event_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

#ifdef PEARL_ENABLE_METRICS
uint64_t Flow_rep::get_metric(int index) const
{
  // FIXME: Validation of arguments missing.
  return m_metrics[index];
}
#endif


//--- Constructors & destructor (protected) ---------------------------------

Flow_rep::Flow_rep(timestamp_t timestamp, uint64_t* metrics)
  : Event_rep(timestamp)
{
#ifdef PEARL_ENABLE_METRICS
  m_metrics = metrics;
#endif
}


Flow_rep::Flow_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
#ifdef PEARL_ENABLE_METRICS
  m_metrics = NULL;
#endif
}
