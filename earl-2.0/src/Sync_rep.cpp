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

#include "Sync_rep.h"

#include "State.h"

using namespace earl;

Sync_rep::Sync_rep(State&        state,
                   EventBuffer*  buffer,
                   Location*     loc, 
                   double        time,
                   long          lkid) :
      Event_rep(state, buffer, loc, time), 
      lkid(lkid),
      lockptr(state.SyncQueue::last_sync(lkid, 0))
{}

void 
Sync_rep::trans(State& state)
{ 
  Event_rep::trans(state);
  // location ID==0 means the lock is system global. 
  // all other locations must wait untl lock will be released 
  // by this location
  state.SyncQueue::queue_sync(this, 0); 
}

