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
#include "MpiGroup.h"
#include "MpiRmaExit_rep.h"
#include "MpiWindow.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRmaExit_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRmaExit_rep::MpiRmaExit_rep(timestamp_t timestamp,
                               MpiWindow*  window,
                               MpiGroup*   group,
                               bool        sync,
                               uint64_t*   metrics)
  : Exit_rep(timestamp, metrics), 
    m_window(window),
    m_group(group),
    m_sync(sync)
{
}


MpiRmaExit_rep::MpiRmaExit_rep(const GlobalDefs& defs, Buffer& buffer)
  : Exit_rep(defs, buffer)
{
  m_window = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
  m_group  = dynamic_cast<MpiGroup*>(defs.get_group(buffer.get_id()));
  m_sync   = (bool)buffer.get_uint8();
}


//--- Event type information ------------------------------------------------

event_t MpiRmaExit_rep::get_type() const
{
  return MPI_RMAEXIT;
}


bool MpiRmaExit_rep::is_typeof(event_t type) const
{
  return type == MPI_RMAEXIT || Exit_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaExit_rep::get_window() const
{
  return m_window;
}


MpiGroup* MpiRmaExit_rep::get_group() const
{
  return m_group;
}


bool MpiRmaExit_rep::is_sync() const
{
    return m_sync;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaExit_rep::pack(Buffer& buffer) const
{
  Exit_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
  buffer.put_id(m_group->get_id());
  buffer.put_uint8((uint8_t)(m_sync ? 1 : 0));
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaExit_rep::output(ostream& stream) const
{
  Exit_rep::output(stream);

  return stream << "  win    = " << *m_window << endl
                << "  group  = " << *m_group  << endl
                << "  sync   = " <<  m_sync   << endl;
}
