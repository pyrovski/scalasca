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

#include "Join_rep.h"

#include "State.h"

using namespace earl;

Join_rep::Join_rep(State&        state,
		   EventBuffer*  buffer,
		   Location*     loc, 
		   double        time) :
  Team_rep(state, buffer, loc, time),
  forkptr(state.last_fork(loc))  
{}

bool 
Join_rep::is_type(etype type) const
{
  if ( Team_rep::is_type(type) || type == JOIN )
    return true;
  else
    return false;
}

void 
Join_rep::trans(State& state)
{ 
  Event_rep::trans(state);
  state.join(this); 
  state.OMP_CollectiveQueue::check_dynamic(this);
}



