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

#include "Put1TE_rep.h"

#include "State.h"

using namespace earl;
 
Put1TE_rep::Put1TE_rep(State& state,
                       EventBuffer*  buffer,
                       Location*     loc, 
                       double        time,
                       Location*     src,
                       long          rmaid) :
    RmaComm_rep(state, buffer, loc, time, rmaid), 
    src(src), 
    put1TSptr( state.TransferQueue::get_rmatransfer(loc, rmaid))
{
}

void 
Put1TE_rep::trans(State& state)
{ 
  RmaComm_rep::trans(state);
  state.TransferQueue::dequeue_rmatransfer(this);
}

