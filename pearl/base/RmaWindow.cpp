/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>

#include "RmaWindow.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class RmaWindow
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes its identifier with the given
 * value @a id and @a communicator, respectively.
 *
 * @param id           Window identifier
 * @param communicator Associated communicator
 */
RmaWindow::RmaWindow(ident_t id, Comm* communicator)
  : IdObject(id),
    m_communicator(communicator)
{
  assert(communicator);
}


//--- Get window information ------------------------------------------------

/**
 * Returns the associated communicator object.
 *
 * @return Communicator
 */
Comm* RmaWindow::get_comm() const
{
  return m_communicator;
}
