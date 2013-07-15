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

#include "Pattern.h"
#include "PatternHWC.h"
#include "PatternMPI.h"
#include "PatternOMP.h"
#include "PatternSHMEM.h"

#include "earl.h"

#include <algorithm>
#include <cstdlib>
#include <string>

#include <stdarg.h>
#include <unistd.h>

#include "Analyzer.h"
#include "CallTree.h"
#include "Profile.h"
#include "EventTrace.h"
#include "ExpertOpt.h"

using namespace earl;
using namespace std;

/****************************************************************************
* Pattern creation and deletion
****************************************************************************/

void get_patternv(std::vector<Pattern*>& pv, earl::EventTrace* trace, ExpertOpt* opt)
{
  pv.push_back(new Time(trace));
  pv.push_back(new Execution(trace));
  pv.push_back(new Overhead(trace));
  pv.push_back(new NumberOfVisits(trace));
  
  init_patternv_mpi(pv, trace);
  init_patternv_omp(pv, trace);
  init_patternv_hwc(pv, trace, opt);
  init_patternv_shmem(pv, trace);
  
}

/****************************************************************************
* Declaration of auxiliary functions
****************************************************************************/

static bool is_overhead(const earl::Region* region);

/****************************************************************************
* Time
****************************************************************************/

void   
Time::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    { 
      long cnode_id =  cnode_idv[i];
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        {
          const Location* loc = trace->get_loc(loc_id);

          // if slave thread outside a parallel region 
          if ( profile.is_multhrd(cnode_id, loc_id) == false && loc->get_thrd()->get_thrd_id() != 0 )
            {
              long   master_id  = loc->get_proc()->get_thrd(0)->get_loc()->get_id();
              double master_val = profile.get_time(cnode_id, master_id);

              severity.set_val(cnode_id, loc_id, master_val);
            }
          else
            {
              severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
            }
        }
    }
}

/****************************************************************************
* Execution
****************************************************************************/

void   
Execution::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    { 
      long cnode_id =  cnode_idv[i];
      if ( ! is_overhead(ctree.get_callee(cnode_id)) )
	for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
	  severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}

/****************************************************************************
* Overhead
****************************************************************************/

void   
Overhead::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    { 
      long cnode_id =  cnode_idv[i];
      if ( is_overhead(ctree.get_callee(cnode_id)) )
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}

/****************************************************************************
* Number of Visits
****************************************************************************/

void 
NumberOfVisits::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    { 
      long cnode_id =  cnode_idv[i];
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        severity.set_val(cnode_id, loc_id, 0. + profile.get_visits(cnode_id, loc_id));
    }
}

/****************************************************************************
*
* Auxiliary functions                                                                                       
*
****************************************************************************/

string lower(string str)
{
  string res=str;

  // resolve warnings on some compilers
  //transform (str.begin(), str.end(), res.begin(), ::tolower);

  for (size_t i = 0; i < res.length(); i++ )
    res[i] = tolower(res[i]);
  return res;
}

bool is_overhead(const earl::Region* region)
{
  return ((region->get_file() == EARL_REGION_GENERATOR) &&
          (region->get_name() == EARL_REGION_TRACING));
}
