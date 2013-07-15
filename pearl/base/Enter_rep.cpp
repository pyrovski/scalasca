/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>
#include <iostream>

#include "Buffer.h"
#include "CNode.h"
#include "Enter_rep.h"
#include "GlobalDefs.h"
#include "Region.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Enter_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

Enter_rep::Enter_rep(timestamp_t timestamp, Region* region, uint64_t* metrics)
  : Flow_rep(timestamp, metrics),
    m_region(region)
{
}


Enter_rep::Enter_rep(const GlobalDefs& defs, Buffer& buffer)
  : Flow_rep(defs, buffer)
{
  m_cnode = defs.get_cnode(buffer.get_id());
}


//--- Event type information ------------------------------------------------

event_t Enter_rep::get_type() const
{
  return ENTER;
}


bool Enter_rep::is_typeof(event_t type) const
{
  return type == ENTER || Flow_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

Region* Enter_rep::get_region() const
{
  return m_cnode->get_region();
}


Callsite* Enter_rep::get_callsite() const
{
  return NULL;
}


//--- Serialize event data (protected) --------------------------------------

void Enter_rep::pack(Buffer& buffer) const
{
  Flow_rep::pack(buffer);

  buffer.put_id(m_cnode->get_id());
}


//--- Generate human-readable output of event data (protected) --------------

ostream& Enter_rep::output(ostream& stream) const
{
  Flow_rep::output(stream);

  // WARNING: This assumes that the trace has already been preprocessed!
  return stream << "  region = " << *m_cnode->get_region() << endl;
}


//--- Private methods -------------------------------------------------------

Region* Enter_rep::get_regionEntered() const
{
  return m_region;
}


CNode* Enter_rep::get_cnode() const
{
  return m_cnode;
}


void Enter_rep::set_cnode(CNode* cnode)
{
  m_cnode = cnode;
}
