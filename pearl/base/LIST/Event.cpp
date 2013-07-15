/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2006-2013                                                **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include "Buffer.h"
#include "CNode.h"
#include "Enter_rep.h"
#include "Error.h"
#include "Event.h"
#include "Event_rep.h"
#include "GlobalDefs.h"
#include "LocalTrace.h"
#include "Location.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Event
 *
 *---------------------------------------------------------------------------
 */

#define ITEM   (*m_trace)[m_index]

//--- Default constructor ---------------------------------------------------

Event::Event()
  : m_trace(0), m_index(0)
{
}

//--- Comparison operators --------------------------------------------------

bool Event::operator==(const Event& rhs) const
{
  return m_trace == rhs.m_trace &&
         m_index == rhs.m_index;
}


bool Event::operator!=(const Event& rhs) const
{
  return m_trace != rhs.m_trace ||
         m_index != rhs.m_index;
}


bool Event::operator<(const Event& rhs) const
{
  // Compare timestamps
  return ITEM->get_time() < (*rhs.m_trace)[rhs.m_index]->get_time();
}


bool Event::operator>(const Event& rhs) const
{
  // Compare timestamps
  return rhs < *this;
}


//--- Forward iterator interface --------------------------------------------

Event& Event::operator++()
{
  ++m_index;
  return *this;
}


Event Event::operator++(int)
{
  Event result(*this);
  ++m_index;
  return result;
}


//--- Backward iterator interface -------------------------------------------

Event& Event::operator--()
{
  --m_index;
  return *this;
}


Event Event::operator--(int)
{
  Event result(*this);
  --m_index;
  return result;
}


//--- Check iterator validity -----------------------------------------------

bool Event::is_valid() const
{
  return m_index < m_trace->size();
}


//--- Access related events -------------------------------------------------

Event Event::enterptr() const
{
  // Search for associated ENTER event while keeping track of nested
  // ENTERs/EXITs (necessary because of possible buffer flushing)
  int depth = 0;
  Event result(*this);
  --result;
  while (!(result->is_typeof(ENTER) && 0 == depth)) {
    if (result->is_typeof(EXIT))
      ++depth;
    if (result->is_typeof(ENTER))
      --depth;

    --result;
  }

  return result;
}


Event Event::exitptr() const
{
  // Search for associated EXIT event while keeping track of nested
  // ENTERs/EXITs (necessary because of possible buffer flushing)
  int depth = 0;
  Event result(*this);
  ++result;
  while (!(result->is_typeof(EXIT) && 0 == depth)) {
    if (result->is_typeof(ENTER))
      ++depth;
    if (result->is_typeof(EXIT))
      --depth;

    ++result;
  }

  return result;
}


Event Event::request() const
{
  // Search for a corresponding request event.
  // For blocking events, reqoffs is 0, hence *this is returned.

  uint32_t i = m_index;

  for (uint32_t offs = (*m_trace)[i]->get_prev_reqoffs(); offs; i -= offs)
    offs = (*m_trace)[i]->get_prev_reqoffs();

  return Event(m_trace, i);
}


Event Event::completion() const
{
  // Search for a corresponding completion event

  uint32_t i = m_index;

  for (uint32_t offs = (*m_trace)[i]->get_next_reqoffs(); offs; i += offs)
    offs = (*m_trace)[i]->get_next_reqoffs();
  
  return Event(m_trace, i);
}


Event Event::next_request_event() const
{
  // Search for next corresponding test or completion event
  return Event(m_trace, m_index + ITEM->get_next_reqoffs());
}


Event Event::prev_request_event() const
{
  // Search for previous corresponding test or request event
  return Event(m_trace, m_index - ITEM->get_prev_reqoffs());
}


//--- Access local event information ----------------------------------------

CNode* Event::get_cnode() const
{
  if (const Enter_rep* event = dynamic_cast<const Enter_rep*>(ITEM))
    return event->get_cnode();

  return enterptr().get_cnode();
}


Location* Event::get_location() const
{
  return m_trace->get_location();
}


//--- Access event representation -------------------------------------------

Event_rep& Event::operator*() const
{
  return *ITEM;
}


Event_rep* Event::operator->() const
{
  return ITEM;
}


//--- Private methods -------------------------------------------------------

Event::Event(const LocalTrace* trace, bool isEnd)
  : m_trace(trace),
    m_index(isEnd ? trace->size() : 0)
{
}


Event::Event(const LocalTrace* trace, uint32_t index)
  : m_trace(trace),
    m_index(index)
{
}


void Event::pack(Buffer& buffer) const
{
  // Store type & location
  buffer.put_uint32(static_cast<uint32_t>(ITEM->get_type()));
  buffer.put_id(m_trace->get_location()->get_id());
  buffer.put_id(get_cnode()->get_id());

  // Pack event data
  ITEM->pack(buffer);
}


ostream& Event::output(ostream& stream) const
{
  stream << "EVENT {" << endl
         << "  type   = " << event_typestr(ITEM->get_type()) << endl
         << "  loc    = " << *m_trace->get_location() << endl;
  ITEM->output(stream);
  return stream << "}" << endl;
}


/*
 *---------------------------------------------------------------------------
 *
 * Related functions
 *
 *---------------------------------------------------------------------------
 */

ostream& pearl::operator<<(ostream& stream, const Event& event)
{
  return event.output(stream);
}


string pearl::event_typestr(event_t type)
{
  static const char* const type_strings[] = {
                             "ANY",
                             "FLOW",
                             "ENTER",
                             "ENTER_CS",
                             "EXIT",
                             "MPI_COLLEXIT",
                             "REQUEST",
                             "MPI_P2P",
                             "MPI_SEND",
                             "MPI_SEND_REQUEST",
                             "MPI_SEND_COMPLETE",
                             "MPI_RECV",
                             "MPI_RECV_REQUEST",
                             "MPI_RECV_COMPLETE",
                             "MPI_REQUEST_TESTED",
                             "MPI_CANCELLED",
                             "OMP_FORK",
                             "OMP_JOIN",
                             "OMP_COLLEXIT",
                             "OMP_LOCK",
                             "OMP_ALOCK",
                             "OMP_RLOCK",
                             "RMA",
                             "RMA_PUT_START",
                             "RMA_PUT_END",
                             "RMA_GET_START",
                             "RMA_GET_END",
                             "MPI_RMA",
                             "MPI_RMA_PUT_START",
                             "MPI_RMA_PUT_END",
                             "MPI_RMA_GET_START",
                             "MPI_RMA_GET_END",
                             "MPI_RMAEXIT",
                             "MPI_RMACOLLEXIT",
                             "MPI_RMA_LOCK",
                             "MPI_RMA_UNLOCK"
                           };

  return type_strings[type];
}
