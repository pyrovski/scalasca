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

#include "Node.h"

#include "Error.h"
#include "Process.h"

using namespace earl;
using namespace std;

void 
Node::add_proc(Process* proc) 
{ 
  procm[proc->get_id()] = proc; 
}

