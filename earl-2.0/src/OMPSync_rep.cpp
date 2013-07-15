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

#include "OMPSync_rep.h"

#include "State.h"

using namespace earl;

OMPSync_rep::OMPSync_rep(State&        state,
		   EventBuffer*  buffer,
		   Location*     loc, 
		   double        time,
		   long          lkid) :
  Event_rep(state, buffer, loc, time), 
  lkid(lkid),
  lockptr(state.OMP_SyncQueue::last_sync(lkid, loc->get_proc()->get_id()))
{}

bool 
OMPSync_rep::is_type(etype type) const
{
  if ( Event_rep::is_type(type) || type == OMPSYNC )
    return true;
  else
    return false;
}

void 
OMPSync_rep::trans(State& state)
{ 
  Event_rep::trans(state);
  // OMP locks are process global. 
  // Other threads must wait untl lock will be released
  state.OMP_SyncQueue::queue_sync(this, get_loc()->get_proc()->get_id());
}

