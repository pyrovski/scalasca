/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiComm.h"
#include "MpiWindow.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiWindow
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes its identifier and associated
 * communicator with the given values @a id and @a communicator,
 * respectively.
 *
 * @param id           Window identifier
 * @param communicator Associated MPI communicator
 */
MpiWindow::MpiWindow(ident_t id, MpiComm* communicator)
  : RmaWindow(id, communicator)
{
}
