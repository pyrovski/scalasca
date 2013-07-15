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

#include "MPIRmaComm_rep.h"

#include "State.h"

using namespace earl;

void 
MPIRmaComm_rep::trans(State& state)
{ 
  MPIRma_rep::trans(state);
  //state.queue_gats_event(this); 
}





