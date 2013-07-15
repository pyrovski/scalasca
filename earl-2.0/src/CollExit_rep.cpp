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

#include "CollExit_rep.h"

#include "State.h"

using namespace earl;

bool 
CollExit_rep::is_type(etype type) const
{
  if ( Exit_rep::is_type(type) || type == COLLEXIT )
    return true;
  else
    return false;
}

void 
CollExit_rep::trans(State& state)
{ 
  Exit_rep::trans(state);
  state.CollectiveQueue::queue_coll(this); 
}


