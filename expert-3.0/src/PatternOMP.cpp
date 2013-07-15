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

#include "PatternOMP.h"


#include <algorithm>
#include <cstdlib>
#include <string>

#include <stdarg.h>
#include <unistd.h>

// earl
#include "earl.h"

// expert
#include "Analyzer.h"
#include "CallTree.h"
#include "Profile.h"
#include "StatisticController.h"


using namespace earl;
using namespace std;

void init_patternv_omp(std::vector<Pattern*>& pv, earl::EventTrace* trace)
{
  pv.push_back(new OMP(trace));
  pv.push_back(new OMPFlush(trace));
  pv.push_back(new OMPFork(trace));
  pv.push_back(new OMPSynchronization(trace));
  pv.push_back(new OMPBarrier(trace));
  pv.push_back(new OMPEBarrier(trace));
  pv.push_back(new OMPWaitAtEBarrier(trace));
  pv.push_back(new OMPIBarrier(trace));
  pv.push_back(new OMPWaitAtIBarrier(trace));
  pv.push_back(new OMPLock(trace));
  pv.push_back(new OMPAPILock(trace));
  pv.push_back(new OMPCriticalLock(trace));
  pv.push_back(new IdleThreads(trace));
}

/****************************************************************************
* Idle Threads
****************************************************************************/

bool 
IdleThreads::isenabled() 
{ 
  return is_omp_appl(trace);
}                               

void 
IdleThreads::end(Profile& profile, CallTree& ctree)
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
        }
    }
}

/****************************************************************************
* OpenMP                                                                            
****************************************************************************/

bool 
OMP::isenabled() 
{ 
  return is_omp_appl(trace);
}                               

void 
OMP::regcb(Analyzer& analyzer) 
{ 
  analyzer.subscribe_cmpnd(TEAM_CREATE, this);
}

void 
OMP::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;
  
  ctree.get_keyv(cnode_idv);
  
  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    { 
      long cnode_id =  cnode_idv[i];
      if ( is_omp_api(ctree.get_callee(cnode_id)) ||
           is_omp_barrier(ctree.get_callee(cnode_id)) ||
           is_omp_flush(ctree.get_callee(cnode_id)) ||
           is_omp_critical(ctree.get_callee(cnode_id)) )
        for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
          severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}

cmp_event* 
OMP::cmpnd(const cmp_event* ce) 
{
  const team_create* tc = (const team_create*) ce;
 
  long loc_id   = tc->enter.get_loc()->get_id();
  long cnode_id = tc->enter.get_cedgeptr().get_pos();

  severity.add_val(cnode_id, loc_id, tc->time);
  return NULL;
} 

/****************************************************************************
* OMP Fork                                                                           
****************************************************************************/

bool 
OMPFork::isenabled() 
{ 
  return is_omp_appl(trace);
}
                               
void 
OMPFork::regcb(Analyzer& analyzer) 
{ 
  analyzer.subscribe_event(ENTER, this);
  analyzer.subscribe_event(FORK, this);
}

cmp_event* 
OMPFork::enter(earl::Event& event)
{
  if ( is_omp_parallel(event.get_reg()) )
    {
      long loc_id   = event.get_loc()->get_id();
      long proc_id  = event.get_loc()->get_proc()->get_id();
      long thrd_id  = event.get_loc()->get_thrd()->get_thrd_id();
      if ( thrd_id != 0 )
        return NULL;
      if ( ! event.get_cedgeptr().null() )
        {
          long cnode_id = event.get_cedgeptr().get_pos();
          team_create* tc = new team_create();
          tc->enter = event;
          if ( forkv[proc_id].null() )
            {
              cerr << "EXPERT: Error in OMPFork::enter(earl::Event& event)." << endl;
              std::exit(EXIT_FAILURE);
          }
          tc->time  = event.get_time() - forkv[proc_id].get_time();
          severity.add_val(cnode_id, loc_id, tc->time);
          return tc;
          
        }
    }
  return NULL;
}

cmp_event* 
OMPFork::fork(earl::Event& event)
{
  long proc_id  = event.get_loc()->get_proc()->get_id();
  forkv[proc_id] = event;
  return NULL;
}


/****************************************************************************
* OMP Barrier                                                                         
****************************************************************************/
bool 
OMPBarrier::isenabled() 
{ 
  return is_omp_appl(trace);
}                               

void 
OMPBarrier::regcb(Analyzer& analyzer) 
{ 
  analyzer.subscribe_event(OMPCEXIT, this);
}

void 
OMPBarrier::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;
  
  ctree.get_keyv(cnode_idv);
  
  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    { 
      long cnode_id =  cnode_idv[i];
      if ( is_omp_barrier(ctree.get_callee(cnode_id)) )
        for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
          severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}

cmp_event* 
OMPBarrier::ompcexit(earl::Event& event)
{
  vector<Event> coll;

  trace->ompcoll(coll, event.get_pos());

  // if collective operation complete
  if ( coll.size() > 0 )
    {
      omp_coll* oc = new omp_coll();
      
      oc->region = event.get_reg();
      oc->exitv  = coll;
      for ( size_t i = 0; i < oc->exitv.size(); i++ )
        oc->enterv.push_back(oc->exitv[i].get_enterptr());
      return oc;
    }
  return NULL;
} 


/****************************************************************************
* OMP Lock                                                                     
****************************************************************************/

bool 
OMPLock::isenabled() 
{ 
  return is_omp_appl(trace);
}                               

void 
OMPLock::regcb(Analyzer& analyzer) 
{
  analyzer.subscribe_event(OMPALOCK, this);
}

cmp_event* 
OMPLock::ompalock(earl::Event& event) 
{
 
  if ( ! event.get_lockptr().null() )
    {
      lock_comp* lc = new lock_comp();
  
      lc->ompalock = event;
      lc->enter_alock = event.get_enterptr();
      lc->omprlock = event.get_lockptr();
      lc->idle_time = lc->omprlock.get_time() - lc->enter_alock.get_time();
      if ( lc->idle_time > 0)
        {
          long loc_id   = lc->enter_alock.get_loc()->get_id();
          long cnode_id = lc->enter_alock.get_cnodeptr().get_pos();
          
          severity.add_val(cnode_id, loc_id, lc->idle_time);          
          return lc;
        }
      delete lc;
    }
  return NULL;
}


/****************************************************************************
* OMP Lock (API)                                                                     
****************************************************************************/

bool 
OMPAPILock::isenabled() 
{ 
  return is_omp_appl(trace);
}                               

void 
OMPAPILock::regcb(Analyzer& analyzer) 
{ 
  analyzer.subscribe_cmpnd(LOCK_COMP, this);
}

cmp_event* 
OMPAPILock::cmpnd(const cmp_event* ce)
{
  const lock_comp* lc = (const lock_comp*) ce;

  if ( is_function(lc->enter_alock.get_reg()) )
    {
      long loc_id   = lc->enter_alock.get_loc()->get_id();
      long cnode_id = lc->enter_alock.get_cnodeptr().get_pos();
      
      severity.add_val(cnode_id, loc_id, lc->idle_time);
      if(StatisticController::theController->IsActive(get_unique_name(), lc->idle_time))
      {
         StatisticController::severePatternInstance spi(
             lc->enter_alock.get_time(), lc->ompalock.get_time(), lc->idle_time,
             cnode_id, lc->omprlock.get_time(),lc->omprlock.get_loc()->get_id());
         StatisticController::theController->AddSeverity(EXP_PAT_OMP_LOCK_API,
                                             get_unique_name(), spi);
         if(PatternTrace::_pt_ptr)
           WriteLockCompletion<EXP_PAT_OMP_LOCK_API>(lc);
      }
    }
  return NULL;
}

/****************************************************************************
* OMP Lock (Critical Section)                                                                     
****************************************************************************/

bool 
OMPCriticalLock::isenabled() 
{ 
  return is_omp_appl(trace);
}                               

void 
OMPCriticalLock::regcb(Analyzer& analyzer) 
{ 
  analyzer.subscribe_cmpnd(LOCK_COMP, this);
}

cmp_event* 
OMPCriticalLock::cmpnd(const cmp_event* ce) 
{ 
   const lock_comp* lc = (const lock_comp*) ce;

  if ( is_omp_critical(lc->enter_alock.get_reg()) )
    {
      long loc_id   = lc->enter_alock.get_loc()->get_id();
      long cnode_id = lc->enter_alock.get_cnodeptr().get_pos();
      
      severity.add_val(cnode_id, loc_id, lc->idle_time);
      if(StatisticController::theController->IsActive(get_unique_name(),
                                                      lc->idle_time))
      {
        StatisticController::severePatternInstance spi(
            lc->enter_alock.get_time(), lc->ompalock.get_time(), lc->idle_time,
            cnode_id, lc->omprlock.get_time(),lc->omprlock.get_loc()->get_id());
        StatisticController::theController->AddSeverity(EXP_PAT_OMP_LOCK_CRITICAL,
                                            get_unique_name(), spi);
        if(PatternTrace::_pt_ptr)
          WriteLockCompletion<EXP_PAT_OMP_LOCK_CRITICAL>(lc);
      }
    }
  return NULL;
}


/****************************************************************************
*
* Auxiliary functions                                                                                       
*
****************************************************************************/

bool is_omp_appl(const earl::EventTrace* trace)
{
  return trace->get_nthrds() > trace->get_nprocs();
}

bool is_omp_api(const earl::Region* region) 
{
  if ( region->get_name().length() < 4 )
    return false;
  
  string prefix(region->get_name(), 0, 3);

  return lower(prefix) == "omp";
}

bool is_omp_lock(const earl::Region* region) 
{
  if ( ! is_omp_api(region) )
    return false;

  string suffix = region->get_name().substr(region->get_name().length() - 4);
  
  return suffix == "lock";
}

bool is_omp_barrier(const earl::Region* region) 
{
  return region->get_rtype() == "OMP_BARRIER" || region->get_rtype() == "OMP_IBARRIER";
}

bool is_omp_ibarrier(const earl::Region* region)
{
  return region->get_rtype() == "OMP_IBARRIER"; 
}

bool is_omp_ebarrier(const earl::Region* region) 
{
  return region->get_rtype() == "OMP_BARRIER";
} 

bool is_omp_flush(const earl::Region* region) 
{
  return region->get_rtype() == "OMP_FLUSH"; 
}

bool is_omp_critical(const earl::Region* region) 
{
  return region->get_rtype() == "OMP_CRITICAL"; 
}

bool is_omp_sync(const earl::Region* region) 
{
  return is_omp_lock(region) || is_omp_barrier(region) || is_omp_critical(region); 
}

bool is_omp_parallel(const earl::Region* region) 
{
  return region->get_rtype() == "OMP_PARALLEL"; 
}

bool is_function(const earl::Region* region) 
{
  return region->get_rtype() == "FUNCTION"; 
}



