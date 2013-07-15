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

#include "Enter_rep.h"

//#include <iostream>

#include "Callsite.h"
#include "State.h"

using namespace earl;
using namespace std;

Enter_rep::Enter_rep(State& state,
		     EventBuffer*  buffer,
		     Location*     loc, 
		     double        time,
		     Region*       region,
		     map<long, Metric*>&  metm,
		     vector<elg_ui8>& metv,
		     Callsite*     csite) :
  Flow_rep(state, buffer, loc, time, region, metm, metv), 
  csite(csite)
{
  if ( this->get_enterptr().null() )
    cedgeptr = NULL; 
  else
    cedgeptr = this->get_enterptr().get_cnodeptr();
  
  cnodeptr = state.get_cnodeptr(cedgeptr, region, csite);
  if ( cnodeptr.null() ) 
    cnodeptr = this;
}

bool 
Enter_rep::is_type(etype type) const
{
  if ( Flow_rep::is_type(type) || type == ENTER )
    return true;
  else
    return false;
}

void 
Enter_rep::trans(State& state)
{ 
  Flow_rep::trans(state); 
  state.push(this);
  state.add_node(this);
}

