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
#include "Callsite.h"
#include "EnterCS_rep.h"
#include "GlobalDefs.h"
#include "Region.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class EnterCS_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

EnterCS_rep::EnterCS_rep(timestamp_t timestamp, Callsite* callsite,
                         uint64_t* metrics)
  : Enter_rep(timestamp, callsite->get_callee(), metrics),
    m_callsite(callsite)
{
}


EnterCS_rep::EnterCS_rep(const GlobalDefs& defs, Buffer& buffer)
  : Enter_rep(defs, buffer)
{
  m_callsite = defs.get_callsite(buffer.get_id());
}


//--- Event type information ------------------------------------------------

event_t EnterCS_rep::get_type() const
{
  return ENTER_CS;
}


bool EnterCS_rep::is_typeof(event_t type) const
{
  return type == ENTER_CS || Enter_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

Callsite* EnterCS_rep::get_callsite() const
{
  return m_callsite;
}


//--- Serialize event data (protected) --------------------------------------

void EnterCS_rep::pack(Buffer& buffer) const
{
  Enter_rep::pack(buffer);

  buffer.put_id(m_callsite->get_id());
}


//--- Generate human-readable output of event data (protected) --------------

ostream& EnterCS_rep::output(ostream& stream) const
{
  Enter_rep::output(stream);

  return stream << "  csite = " << *m_callsite
                << " {" << *m_callsite->get_callee() << "}" << endl;
}
