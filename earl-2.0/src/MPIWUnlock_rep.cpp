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

#include "MPIWUnlock_rep.h"

#include "State.h"
 
using namespace earl;

MPIWUnlock_rep::MPIWUnlock_rep(State& state,
                               EventBuffer*  buffer,
                               Location*     loc, 
                               double        time,
                               Location*     lloc,
                               MPIWindow*    win
                              ) :
  MPISync_rep(state, buffer, loc, time, lloc, win),
  lockptr(state.MPI_SyncQueue::last_sync(win->get_id(),win->get_id()))
{
}

void 
MPIWUnlock_rep::trans(State& state)
{ 
  Event_rep::trans(state);
  // dequeue isn't requered, because new lock replace the old one 
  // MPI locks the mpi window . 
  // Other locations must wait untl window lock will be released
  state.MPI_SyncQueue::queue_sync(this, get_win()->get_id()); 
}




