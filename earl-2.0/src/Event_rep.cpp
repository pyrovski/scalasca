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
 
#include "Event_rep.h"

#ifdef NO_INLINE
#  define inline
#  include "Event_rep.inl"
#endif

#include "Error.h"
#include "State.h"

using namespace earl;
using namespace std;

Event_rep::Event_rep(State&        state,
		     EventBuffer*  buffer,
		     Location*     loc, 
		     double        time) :
  pos(state.get_pos() + 1),
  loc(loc),
  time(time),
  enterptr(state.top(loc)),
  buffer(buffer),
  refc(0)
{}

bool           
Event_rep::is_type(etype type) const 
{ 
  if ( type == EVENT ) 
    return true; 
  else 
    return false; 
}

void 
Event_rep::trans(State& state) 
{
  if (state.get_pos() != pos - 1)
    throw FatalError("Error in Event_rep::trans(SystemState&).");
  state.inc_pos();
  state.MPI_CollectiveQueue::dequeue_coll();
  state.OMP_CollectiveQueue::dequeue_coll();
  state.CollectiveQueue::dequeue_coll();
  state.MPI_RmaCollQueue::dequeue_coll();
  // call order is significant 
  state.dequeue_epoch();
  //state.dequeue_gats_event();
}



















