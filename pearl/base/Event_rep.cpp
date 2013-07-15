/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>
#include <iomanip>
#include <iostream>

#include "Buffer.h"
#include "Error.h"
#include "Event_rep.h"

using namespace std;
using namespace pearl;


// Some compilers (e.g., Oracle/Sun) issue bogus warnings if a non-void
// function returns via an unconditional throw statement. This macro is
// used to make those buggy compilers happy.
#if defined(__SUNPRO_CC)
  #define DUMMY_RETURN(return_type)   return (return_type)0
#else
  #define DUMMY_RETURN(return_type)
#endif


/*
 *---------------------------------------------------------------------------
 *
 * class Event_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Event type information ------------------------------------------------

event_t Event_rep::get_type() const
{
  throw FatalError("EventRep::get_type() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(event_t);
}


bool Event_rep::is_typeof(event_t type) const
{
  return type == ANY;
}


//--- Access event data -----------------------------------------------------

pearl::timestamp_t Event_rep::get_time() const
{
  return m_timestamp;
}


Region* Event_rep::get_region() const
{
  throw FatalError("EventRep::get_region() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(Region*);
}


Callsite* Event_rep::get_callsite() const
{
  throw FatalError("EventRep::get_callsite() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(Callsite*);
}


MpiComm* Event_rep::get_comm() const
{
  throw FatalError("EventRep::get_comm() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(MpiComm*);
}


uint32_t Event_rep::get_root() const
{
  throw FatalError("EventRep::get_root() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


uint32_t Event_rep::get_sent() const
{
  throw FatalError("EventRep::get_sent() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


uint32_t Event_rep::get_received() const
{
  throw FatalError("EventRep::get_received() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


uint32_t Event_rep::get_source() const
{
  throw FatalError("EventRep::get_source() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


uint32_t Event_rep::get_dest() const
{
  throw FatalError("EventRep::get_dest() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


uint32_t Event_rep::get_tag() const
{
  throw FatalError("EventRep::get_tag() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


ident_t Event_rep::get_reqid() const
{
  throw FatalError("EventRep::get_reqid() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(ident_t);
}


uint32_t Event_rep::get_lockid() const
{
  throw FatalError("EventRep::get_lockid() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


MpiWindow* Event_rep::get_window() const
{
  throw FatalError("EventRep::get_window() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(MpiWindow*);
}


uint32_t Event_rep::get_remote() const
{
  throw FatalError("EventRep::get_remote() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


uint32_t Event_rep::get_rma_id() const
{
  throw FatalError("EventRep::get_rma_id() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


MpiGroup* Event_rep::get_group() const
{
  throw FatalError("EventRep::get_group() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(MpiGroup*);
}


bool Event_rep::is_sync() const
{
  throw FatalError("EventRep::is_sync() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(bool);
}


bool Event_rep::is_exclusive() const
{
  throw FatalError("EventRep::is_exclusive() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(bool);
}


#ifdef PEARL_ENABLE_METRICS
uint64_t Event_rep::get_metric(int index) const
{
  throw FatalError("EventRep::get_metric(int) -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint64_t);
}
#endif


//--- Modify event data -----------------------------------------------------

void Event_rep::set_time(timestamp_t timestamp)
{
  m_timestamp = timestamp;
}

void Event_rep::set_reqid(uint32_t)
{
  throw FatalError("EventRep::set_reqid(int) -- "
                   "Oops! This method should not be called.");
}


//--- Constructors & destructor (protected) ---------------------------------

Event_rep::Event_rep(pearl::timestamp_t timestamp)
  : m_timestamp(timestamp)
{
}


Event_rep::Event_rep(const GlobalDefs& defs, Buffer& buffer)
{
  m_timestamp = buffer.get_time();
}


//--- Serialize event data (protected) --------------------------------------

void Event_rep::pack(Buffer& buffer) const
{
  buffer.put_time(m_timestamp);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& Event_rep::output(ostream& stream) const
{
  return stream << "  time   = " << fixed << m_timestamp << endl;
}


//--- Find previous / next request event in trace (protected) ---------------

uint32_t Event_rep::get_next_reqoffs() const
{
  throw FatalError("EventRep::get_next_reqoffs() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


uint32_t Event_rep::get_prev_reqoffs() const
{
  throw FatalError("EventRep::get_prev_reqoffs() -- "
                   "Oops! This method should not be called.");

  DUMMY_RETURN(uint32_t);
}


void Event_rep::set_next_reqoffs(uint32_t) 
{
  throw FatalError("EventRep::set_next_reqoffs(uint32_t) -- "
                   "Oops! This method should not be called.");
}


void Event_rep::set_prev_reqoffs(uint32_t)
{
  throw FatalError("EventRep::set_prev_reqoffs(uint32_t) -- "
                   "Oops! This method should not be called.");
}
