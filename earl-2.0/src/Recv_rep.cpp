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

#include "Recv_rep.h"

#include "State.h"

using namespace earl;

Recv_rep::Recv_rep(State&        state,
		   EventBuffer*  buffer,
		   Location*     loc, 
		   double        time,
		   Communicator* com,
		   long          tag,
		   Location*     src) : 
  P2P_rep(state, buffer, loc, time, com, tag, state.get_match_msg(src, loc, tag, com).get_length()), 
  src(src),
  sendptr(state.get_match_msg(src, loc, tag, com)) 
{}

bool 
Recv_rep::is_type(etype type) const
{
  if ( P2P_rep::is_type(type) || type == RECV )
    return true;
  else
    return false;
}

void 
Recv_rep::trans(State& state)
{ 
  P2P_rep::trans(state); 
  state.dequeue_msg(this->sendptr.get_pos()); 
}


