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

#include "RegionStack.h"
#include "Grid.h"
#include "Error.h"

using namespace earl;
using namespace std;

RegionStack::RegionStack(const Grid* grid) : 
  stackv(grid->get_nlocs()),
  pid2fork(grid->get_nprocs()),
  grid(grid)
{}

void
RegionStack::push(Event enter) 
{
  stackv[enter.get_loc()->get_id()].push_back(enter);
}

void 
RegionStack::pop(Event exit) 
{  
  long loc_id = exit.get_loc()->get_id();

  if ( stackv[loc_id].empty() )
    throw FatalError("Error in RegionStack::pop(Event).");
  stackv[loc_id].pop_back();
}

void       
RegionStack::fork(Event fork)
{
  long proc_id = fork.get_loc()->get_proc()->get_id();

  pid2fork[proc_id] = fork;
}

void       
RegionStack::join(Event join)                           
{
  long proc_id = join.get_loc()->get_proc()->get_id();
  
  pid2fork[proc_id] = NULL;
}

void
RegionStack::stack(vector<Event>& out, Location* loc)  
{
  long loc_id = loc->get_id();

  if ( loc_id < 0 || long(stackv.size()) <= loc_id  )
    {
      throw RuntimeError("Error in RegionStack::stack(vector<Event>&, long).");;
    }
  else 
    {
      // the locations's own stack only
      for ( size_t i = 0; i < stackv[loc_id].size(); i++ )
	out.push_back(stackv[loc_id][i]);
    }
}

void
RegionStack::istack(vector<Event>& out, Location* loc) 
{
  long loc_id = loc->get_id();

  if ( loc_id < 0 || long(stackv.size()) <= loc_id )
    {
      throw RuntimeError("Error in RegionStack::istack(vector<Event>&, long).");;
    }
  else 
    {
      long proc_id = grid->get_loc(loc_id)->get_proc()->get_id();;
      long thrd_id = grid->get_loc(loc_id)->get_thrd()->get_thrd_id();;

      // slave?
      if ( !pid2fork[proc_id].null() && thrd_id != 0 )
	{
	  // slave inherits stack from master
	  long mstr_id = grid->get_loc(loc_id)->get_proc()->get_loc()->get_id();

	  for ( size_t i = 0; i < stackv[mstr_id].size(); i++ )
	    if ( stackv[mstr_id][i] < pid2fork[proc_id] ) 
	      out.push_back(stackv[mstr_id][i]);
	}

      // location's own stack
      for ( size_t i = 0; i < stackv[loc_id].size(); i++ )
	out.push_back(stackv[loc_id][i]);
    }
}


Event 
RegionStack::top(Location* loc) const 
{
  long loc_id = loc->get_id();

  if ( loc_id < 0 || long(stackv.size()) <= loc_id )
    {
      throw FatalError("Error in RegionStack::top(long).");
    }
  else if ( stackv[loc_id].size() >= 1 )
    {
      return stackv[loc_id].back();
    }
  else
    {
      long proc_id = grid->get_loc(loc_id)->get_proc()->get_id();;
      long thrd_id = grid->get_loc(loc_id)->get_thrd()->get_thrd_id();;

      // slave?
      if ( !pid2fork[proc_id].null() && thrd_id != 0  )
	{
	  // return element of master's stack before fork 
	  long mstr_id = grid->get_loc(loc_id)->get_proc()->get_loc()->get_id();
	  
	  for ( int i =  stackv[mstr_id].size() - 1; i >= 0; i-- )
	    if ( stackv[mstr_id][i] < pid2fork[proc_id] )
	      return stackv[mstr_id][i];
	}
      return NULL;
    }
}

Event 
RegionStack::last_fork(Location* loc) const 
{
  long proc_id = loc->get_proc()->get_id();

  if ( pid2fork[proc_id].null() )
    throw FatalError("Error in RegionStack::last_fork(Location*).");
  else
    return  pid2fork[proc_id];
}












