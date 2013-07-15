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

#include "MPIGet1TS_rep.h"


#include "MPIGet1TO_rep.h"

#include "State.h"
 
using namespace earl;

void 
MPIGet1TS_rep::trans(State& state)
{
  MPIRmaComm_rep::trans(state);
  state.MPI_TransferQueue::queue_rmatransfer(this);
  state.MPI_OriginQueue::dequeue_mpiorigin(dst,get_rmaid());
}




