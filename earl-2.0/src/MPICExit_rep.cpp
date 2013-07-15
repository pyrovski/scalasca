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

#include "MPICExit_rep.h"

#include "State.h"

using namespace earl;

bool 
MPICExit_rep::is_type(etype type) const
{
  if ( Exit_rep::is_type(type) || type == MPICEXIT )
    return true;
  else
    return false;
}

void 
MPICExit_rep::trans(State& state)
{ 
  Exit_rep::trans(state);
  state.MPI_CollectiveQueue::queue_coll(this); 
}

