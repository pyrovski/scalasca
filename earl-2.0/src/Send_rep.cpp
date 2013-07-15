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

#include "Send_rep.h"

#include "State.h"

using namespace earl;

bool 
Send_rep::is_type(etype type) const
{
  if ( P2P_rep::is_type(type) || type == SEND )
    return true;
  else
    return false;
}

void 
Send_rep::trans(State& state)
{ 
  P2P_rep::trans(state); 
  state.queue_msg(this); 
}
