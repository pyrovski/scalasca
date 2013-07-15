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
#include "MpiRmaLock_rep.h"
#include "MpiWindow.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRmaLock_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRmaLock_rep::MpiRmaLock_rep(timestamp_t timestamp,
                               uint32_t    location,
                               MpiWindow*  window,
                               bool        exclusive)
  : Event_rep(timestamp),
    m_window(window),
    m_location(location),
    m_exclusive(exclusive)
{
}


MpiRmaLock_rep::MpiRmaLock_rep(const GlobalDefs& defs, Buffer& buffer)
  : Event_rep(defs, buffer)
{
  m_window    = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
  m_location  = buffer.get_uint32();
  m_exclusive = buffer.get_uint8();
}


//--- Event type information ------------------------------------------------

event_t MpiRmaLock_rep::get_type() const
{
    return MPI_RMA_LOCK;
}


bool MpiRmaLock_rep::is_typeof(event_t type) const
{
  return type == MPI_RMA_LOCK || Event_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaLock_rep::get_window() const
{
  return m_window;
}


uint32_t MpiRmaLock_rep::get_remote() const
{
  return m_location;
}


bool MpiRmaLock_rep::is_exclusive() const
{
  return m_exclusive;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaLock_rep::pack(Buffer& buffer) const
{
  Event_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
  buffer.put_uint32(m_location);
  buffer.put_uint8(m_exclusive ? 1 : 0);
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaLock_rep::output(ostream& stream) const
{
  Event_rep::output(stream);

  return stream << "  location  = " <<  m_location << endl
                << "  win       = " << *m_window << endl
                << "  exclusive = " <<  m_exclusive << endl;
}
