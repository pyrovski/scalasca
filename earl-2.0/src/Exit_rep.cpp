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

#include "Exit_rep.h"

#include "Error.h"
#include "State.h"

using namespace earl;
using namespace std;

Exit_rep::Exit_rep(State& state,
		   EventBuffer*  buffer,
		   Location*     loc, 
		   double        time,
		   std::map<long, Metric*>&  metm,
		   std::vector<elg_ui8>& metv) :
  Flow_rep(state, buffer, loc, time, NULL, metm, metv)
{
    if ( state.top(loc).null() )
	throw RuntimeError("Error in Exit_rep::Exit_rep(...): stack underflow");
    Flow_rep::set_reg(state.top(loc).get_reg());
}

bool 
Exit_rep::is_type(etype type) const
{
  if ( Flow_rep::is_type(type) || type == EXIT )
    return true;
  else
    return false;
}

void 
Exit_rep::trans(State& state)    
{ 
  Flow_rep::trans(state);
  state.pop(this); 
  state.queue_gats_event(this); 
}

