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

#include "MPIGet1TE_rep.h"

#include "State.h"

using namespace earl;

MPIGet1TE_rep::MPIGet1TE_rep( State& state,
                              EventBuffer*  buffer,
                              Location*     loc, 
                              double        time,
                              Location*     src,
                              MPIWindow*    win,
                              long          rmaid) :
    MPIRmaComm_rep(state, buffer, loc, time, win, rmaid, 0), 
    src(src), 
    get1TSptr(state.MPI_TransferQueue::get_rmatransfer(loc,rmaid))
{
}
    
void 
MPIGet1TE_rep::trans(State& state)
{ 
  MPIRmaComm_rep::trans(state);
  // dequeue get1TS event from transfer queue
  state.MPI_TransferQueue::dequeue_rmatransfer(this); 
  // move exit event to epoches queue
  state.queue_gats_event(this); 
}




