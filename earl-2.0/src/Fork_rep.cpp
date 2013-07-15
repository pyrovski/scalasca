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

#include "Fork_rep.h"

#include "State.h"

using namespace earl;

bool 
Fork_rep::is_type(etype type) const
{
  if ( Team_rep::is_type(type) || type == FORK )
    return true;
  else
    return false;
}

void 
Fork_rep::trans(State& state)
{ 
  Event_rep::trans(state);
  state.fork(this); 
}




