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

#include "MPIPut1TE_rep.h"

#include "State.h"

using namespace earl;
 
MPIPut1TE_rep::MPIPut1TE_rep(State& state,
                             EventBuffer*  buffer,
                             Location*     loc, 
                             double        time,
                             Location*     src,
                             MPIWindow*    win,
                             long          rmaid) :
    MPIRmaComm_rep(state, buffer, loc, time, win, rmaid, 0), 
    src(src), 
    put1TSptr(state.MPI_TransferQueue::get_rmatransfer(src,rmaid))
{
}

void 
MPIPut1TE_rep::trans(State& state)
{

  MPIRmaComm_rep::trans(state);
  // dequeue put1TS event
  state.MPI_TransferQueue::dequeue_rmatransfer(this);
  // move exit event to epoches queue
  state.queue_gats_event(this); 
}





