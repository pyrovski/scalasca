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

#include "MPIGet1TO_rep.h"

#include "State.h"

using namespace earl;

void 
MPIGet1TO_rep::trans(State& state)
{ 
  MPIRmaComm_rep::trans(state);
  state.queue_mpiorigin(this);// loc and rma id requered
}




