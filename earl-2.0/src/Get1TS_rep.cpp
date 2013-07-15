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

#include "Get1TS_rep.h"

#include "State.h"
 
using namespace earl;

void 
Get1TS_rep::trans(State& state)
{
  RmaComm_rep::trans(state);
  state.TransferQueue::queue_rmatransfer(this);
}

