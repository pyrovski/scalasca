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
#include "MpiRmaPutStart_rep.h"
#include "MpiWindow.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiRmaPutStart_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MpiRmaPutStart_rep::MpiRmaPutStart_rep(timestamp_t timestamp,
                                       uint32_t    rma_id,
                                       uint32_t    remote,
                                       uint32_t    length,
                                       MpiWindow*  window)
  : RmaPutStart_rep(timestamp, rma_id, remote, length),
    m_window(window)
{
}


MpiRmaPutStart_rep::MpiRmaPutStart_rep(const GlobalDefs& defs, Buffer& buffer)
  : RmaPutStart_rep(defs, buffer)
{
    m_window = dynamic_cast<MpiWindow*>(defs.get_window(buffer.get_id()));
}


//--- Event type information ------------------------------------------------

event_t MpiRmaPutStart_rep::get_type() const
{
  return MPI_RMA_PUT_START;
}


bool MpiRmaPutStart_rep::is_typeof(event_t type) const
{
  return type == MPI_RMA_PUT_START || RmaPutStart_rep::is_typeof(type);
}


//--- Access event data -----------------------------------------------------

MpiWindow* MpiRmaPutStart_rep::get_window() const
{
  return m_window;
}


//--- Serialize event data (protected) --------------------------------------

void MpiRmaPutStart_rep::pack(Buffer& buffer) const
{
  RmaPutStart_rep::pack(buffer);

  buffer.put_id(m_window->get_id());
}


//--- Generate human-readable output of event data (protected) --------------

ostream& MpiRmaPutStart_rep::output(ostream& stream) const
{
  RmaPutStart_rep::output(stream);

  return stream << "  win      = " << *m_window << endl;
}
