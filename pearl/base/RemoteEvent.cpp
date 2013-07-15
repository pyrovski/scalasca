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
#include "Event.h"
#include "EventFactory.h"
#include "GlobalDefs.h"
#include "Location.h"
#include "RemoteEvent.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RemoteEvent
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

RemoteEvent::RemoteEvent(const GlobalDefs& defs, Buffer& buffer)
{
  // Unpack data
  event_t type = static_cast<event_t>(buffer.get_uint32());
  m_location   = defs.get_location(buffer.get_id());
  m_cnode      = defs.get_cnode(buffer.get_id());
  m_event      = CountedPtr<Event_rep>(
                   EventFactory::instance()->createEvent(type, defs, buffer));
}


//--- Access remote event information ---------------------------------------

CNode* RemoteEvent::get_cnode() const
{
  return m_cnode;
}


Location* RemoteEvent::get_location() const
{
  return m_location;
}


//--- Access event representation -------------------------------------------

Event_rep& RemoteEvent::operator*() const
{
  return *m_event.get();
}


Event_rep* RemoteEvent::operator->() const
{
  return m_event.get();
}


//--- Private methods -------------------------------------------------------

ostream& RemoteEvent::output(ostream& stream) const
{
  return stream << "REMOTE EVENT {" << endl
                << "  type   = " << event_typestr(m_event->get_type()) << endl
                << "  loc    = " << *m_location << endl
                << m_event->output(stream)
                << "}" << endl;
}


/*
 *---------------------------------------------------------------------------
 *
 * Related functions
 *
 *---------------------------------------------------------------------------
 */

ostream& pearl::operator<<(ostream& stream, const RemoteEvent& event)
{
  return event.output(stream);
}
