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

#include "Process.h"

#include "Error.h"
#include "Grid.h"
#include "Location.h"
#include "Node.h"
#include "Thread.h"

using namespace earl;
using namespace std;

Thread*     
Process::get_thrd(long thrd_id) const
{
  map<long, Thread*>::const_iterator it = thrdm.find(thrd_id);
  
  if (it == thrdm.end())
    throw RuntimeError("Error in Process::get_thrd(long).");
  else
    return it->second;
}

Location*   
Process::get_loc() const
{
  return this->get_thrd(0)->get_loc();
}     

void 
Process::add_thrd(Thread* thrd) 
{ 
  thrdm[thrd->get_thrd_id()] = thrd; 
}
