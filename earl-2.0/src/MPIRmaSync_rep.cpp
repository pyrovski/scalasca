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

#include "MPIRmaSync_rep.h"

#include "State.h"

using namespace earl;

void 
MPISync_rep::trans(State& state)
{ 
  Event_rep::trans(state);
  // TODO create model for MPI-2 Lock/Unlock synchronization
  // and implement it
  //state.MPI_SyncQueue::queue_sync(this, get_win()->get_id()); 
}

