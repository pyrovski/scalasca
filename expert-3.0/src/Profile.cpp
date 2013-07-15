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

#include "Profile.h"

#include <vector>

#include "Analyzer.h"
#include "CallTree.h"

using namespace earl;
using namespace std;

Profile::Profile(earl::EventTrace* trace, Analyzer& analyzer) :
  trace(trace),
  time_sev(trace->get_nlocs()),
  visits_sev(trace->get_nlocs()),
  multhrd_sev(trace->get_nlocs()),
  ismulthrd(trace->get_nprocs(), false)
{ 
  for ( long i = 0; i < trace->get_nmets(); i++ )
    met_sevv.push_back(Severity<long long>(trace->get_nlocs()));

  analyzer.subscribe_event(FORK, this);
  analyzer.subscribe_event(JOIN, this);
  analyzer.subscribe_event(EXIT, this);
}

struct cmp_event* 
Profile::fork(earl::Event& event) 
{ 
  ismulthrd[event.get_loc()->get_proc()->get_id()] = true;
  return NULL;
}

struct cmp_event* 
Profile::join(earl::Event& event) 
{ 
  ismulthrd[event.get_loc()->get_proc()->get_id()] = false;
  return NULL;
}

struct cmp_event* 
Profile::exit(earl::Event& event) 
{ 
  long loc_id   = event.get_loc()->get_id();
  long proc_id  = event.get_loc()->get_proc()->get_id();
  Event enter   = event.get_enterptr();
  long cnode_id = enter.get_cnodeptr().get_pos();
  
  // determine time difference
  double duration = event.get_time() - enter.get_time();
  time_sev.add_val(cnode_id, loc_id, duration);

  // count number of visits
  visits_sev.add_val(cnode_id, loc_id, 1);

  // determine counter difference
  for ( int met_id = 0; met_id < event.get_nmets(); met_id++ )
    {
      long long difference  = event.get_metval(met_id) - enter.get_metval(met_id); 
      met_sevv[met_id].add_val(cnode_id, loc_id, difference);
    }

  // identify multithreaded tuples (call path, location)
  if ( ismulthrd[proc_id] == false )
    {
      // !! no EXIT event for slave threads outside parallel regions
      for ( int thrd_id = 0; thrd_id < event.get_loc()->get_proc()->get_nthrds(); thrd_id++ )
	{
	  long lid = event.get_loc()->get_proc()->get_thrd(thrd_id)->get_loc()->get_id();
	  multhrd_sev.set_val(cnode_id, lid, 0);
	}
    }
  else
    {
      multhrd_sev.set_val(cnode_id, loc_id, 1);
    }
  return NULL;
}

void 
Profile::exclusify(CallTree& ctree) 
{ 
  // exclusify
  time_sev.exclusify(ctree);
  for ( int met_id = 0; met_id < trace->get_nmets(); met_id++ )
    met_sevv[met_id].exclusify(ctree);
            
  // outside parallel regions set profile of slave threads to zero
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    { 
      long cnode_id =  cnode_idv[i];
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
	{
	  const Location* loc = trace->get_loc(loc_id);
	  
	  // if slave thread outside a parallel region 
	  if ( !is_multhrd(cnode_id, loc_id) && loc->get_thrd()->get_thrd_id() != 0 )
	    {
	      time_sev.set_val(cnode_id, loc_id, 0);
	      for ( int met_id = 0; met_id < trace->get_nmets(); met_id++ )
		met_sevv[met_id].set_val(cnode_id, loc_id, 0);
	    }
	}
    }
  // note: no need to exclusify visits
}

double    
Profile::get_time(long cnode_id, long loc_id)   
{ 
  return time_sev.get(cnode_id, loc_id);
}

long long 
Profile::get_visits(long cnode_id, long loc_id)
{
  return visits_sev.get(cnode_id, loc_id);
}

long long
Profile::get_metval(long met_id, long cnode_id, long loc_id)  
{ 
  return met_sevv[met_id].get(cnode_id, loc_id);
}

bool      
Profile::is_multhrd(long cnode_id, long loc_id)  
{ 
  if ( multhrd_sev.get(cnode_id, loc_id) > 0 )
    return true;
  else
    return false;
}
