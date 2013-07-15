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
#include "MpiRmaCollExit_rep.h"
#include "MpiWindow.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRmaCollExit_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRmaCollExit_rep::MpiRmaCollExit_rep(timestamp_t timestamp,
                                       MpiWindow*  window,
                                       uint64_t*   metrics)
  : Exit_rep(timestamp, metrics), m_window(window)
{
}


MpiRmaCollExit_rep::MpiRmaCollExit_rep(const GlobalDefs& defs, Buffer& buffer)
  : Exit_rep(defs, buffer)
{
  m_window = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
}


//--- Event type information ------------------------------------------------

event_t MpiRmaCollExit_rep::get_type() const
{
  return MPI_RMACOLLEXIT;
}


bool MpiRmaCollExit_rep::is_typeof(event_t type) const
{
  return type == MPI_RMACOLLEXIT || Exit_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaCollExit_rep::get_window() const
{
  return m_window;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaCollExit_rep::pack(Buffer& buffer) const
{
  Exit_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaCollExit_rep::output(ostream& stream) const
{
  Exit_rep::output(stream);

  return stream << "  win    = " << *m_window << endl;
}
