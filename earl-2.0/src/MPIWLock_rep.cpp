/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "MPIWLock_rep.h"

#include "State.h"
 
using namespace earl;

void 
MPIWLock_rep::trans(State& state)
{ 
  Event_rep::trans(state);
  // TODO state.queue_sync(this); 
}




