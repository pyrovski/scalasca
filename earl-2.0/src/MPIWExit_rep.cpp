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

#include "MPIWExit_rep.h"

#include "State.h"

using namespace earl;


void
MPIWExit_rep::trans(State& state)
{
  Exit_rep::trans(state);
  //state.queue_gats_event(this); 
}


