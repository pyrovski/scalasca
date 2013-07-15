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

#include "MPIWCExit_rep.h"

#include "State.h"

using namespace earl;
 

void 
MPIWCExit_rep::trans(State& state)
{ 
  Exit_rep::trans(state);
  //queue envent into RMA WCExit- queue
  state.MPI_RmaCollQueue::queue_coll(this); 
}


