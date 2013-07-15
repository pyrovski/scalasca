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
#include "GlobalDefs.h"
#include "MpiRmaUnlock_rep.h"
#include "MpiWindow.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRmaUnlock_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRmaUnlock_rep::MpiRmaUnlock_rep(timestamp_t timestamp,
                                   uint32_t    location,
                                   MpiWindow*  window)
  : Event_rep(timestamp),
    m_window(window),
    m_location(location)
{
}


MpiRmaUnlock_rep::MpiRmaUnlock_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  m_window    = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
  m_location  = buffer.get_uint32();
}


//--- Event type information ------------------------------------------------

event_t MpiRmaUnlock_rep::get_type() const
{
    return MPI_RMA_UNLOCK;
}


bool MpiRmaUnlock_rep::is_typeof(event_t type) const
{
  return type == MPI_RMA_UNLOCK || Event_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaUnlock_rep::get_window() const
{
  return m_window;
}


uint32_t MpiRmaUnlock_rep::get_remote() const
{
  return m_location;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaUnlock_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
  buffer.put_uint32(m_location);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaUnlock_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  location  = " <<  m_location << endl
                << "  win       = " << *m_window << endl;
}
