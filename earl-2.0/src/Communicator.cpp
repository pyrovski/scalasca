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

#include "Communicator.h"

#include "Error.h"
#include "Location.h"
#include "Process.h"

using namespace earl;
using namespace std;

Communicator::Communicator(long id, std::vector<Process*>& procv) : id(id)
{
  for ( size_t i = 0; i < procv.size(); i++ )
    {
      Process* proc = procv[i]; 

      procm[i] = proc;
      pid2rank[proc->get_id()] = i;
    }
}
 
long 
Communicator::get_rank(const Location* loc) const
{
  Process* proc = loc->get_proc();

  map<long, long>::const_iterator it = pid2rank.find(proc->get_id());
  
  if (it == pid2rank.end())
    throw RuntimeError("Error in Communicator::get_rank(Location*).");
  else
    return it->second;
}

Process* 
Communicator::get_proc(long rank) const
{
  map<long, Process*>::const_iterator it = procm.find(rank);
  
  if (it == procm.end())
    throw RuntimeError("Error in Communicator::get_proc(long).");
  else
    return it->second;  
}
