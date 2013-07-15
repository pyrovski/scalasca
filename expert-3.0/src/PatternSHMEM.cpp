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

#include "PatternSHMEM.h"


#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>

// earl
#include "earl.h"

// expert
#include "Analyzer.h"
#include "CallTree.h"
#include "PatternHybrTempl.h"
#include "PatternTraceTempl.h"
#include "Profile.h"
#include "StatisticController.h"

using namespace earl;
using namespace std;

/****************************************************************************
*
* SHMEM Patterns
*
****************************************************************************/


void init_patternv_shmem(std::vector<Pattern*>& pv, earl::EventTrace* trace)
{
  pv.push_back(new SHMEM(trace));
  pv.push_back(new SHMEMCommunication(trace));
  pv.push_back(new SHMEMCollComm(trace));
  pv.push_back(new SHMEMLateBroadcast(trace));
  pv.push_back(new SHMEMWaitAtNxN(trace));
  pv.push_back(new SHMEMRMAComm(trace));
  pv.push_back(new SHMEMSynchronization(trace));
  pv.push_back(new SHMEMBarrier(trace));
  pv.push_back(new SHMEMWaitAtBarrier(trace));
  pv.push_back(new SHMEMSyncP2P(trace));
  pv.push_back(new SHMEMLockCompletion(trace));
  pv.push_back(new SHMEMWaitUntil(trace));
  pv.push_back(new SHMEMInitExit(trace));
  pv.push_back(new SHMEMMemoryManagament(trace));
}

bool
SHMEM::isenabled()
{
  return is_shmem_appl(trace);
}

void
SHMEM::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_event(COLLEXIT, this);
}

void
SHMEM::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      if ( is_shmem_api(ctree.get_callee(cnode_id)) )
        for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
          severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}

cmp_event*
SHMEM::collexit(earl::Event& event)

{
  vector<Event> coll;

  trace->get_rmacolls(coll, event.get_pos());

  // if collective operation complete
  if ( coll.size() > 0 )
    {
      shmem_coll* smc = new shmem_coll;

      smc->region = event.get_reg();
      smc->exitv  = coll;
      smc->enterv.reserve(coll.size());
      for ( size_t i = 0; i < smc->exitv.size(); i++ ) {
          smc->enterv.push_back(smc->exitv[i].get_enterptr());
          if ( event.get_root() == smc->exitv[i].get_loc() )
            smc->root_idx = i;
      }
      
      return smc;
    }
  return NULL;
}

/****************************************************************************
* SHMEM late bcast 
****************************************************************************/

bool
SHMEMLateBroadcast::isenabled()
{
  return is_shmem_appl(trace);
}

void
SHMEMLateBroadcast::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(SHMEM_COLL, this);
}

cmp_event*
SHMEMLateBroadcast::cmpnd(const cmp_event* ce)
{
  const shmem_coll* smc = (const shmem_coll*) ce;
  Event enter = smc->enterv[0];

  if ( is_shmem_bcast(enter.get_reg()) )
    {
      double root_start = smc->enterv[smc->root_idx].get_time();
      double wastedTime = 0.0;
      for ( size_t i = 0; i < smc->enterv.size(); i++ )
        {
          double idle_time = root_start - smc->enterv[i].get_time();
          if ( idle_time > 0 )
            {
              long loc_id       = smc->enterv[i].get_loc()->get_id();
              long cnode_id     = smc->enterv[i].get_cnodeptr().get_pos();
              wastedTime += idle_time;
              severity.add_val(cnode_id, loc_id, idle_time);
            }
        }
      if(StatisticController::theController->IsActive(get_unique_name(), wastedTime))
      {
        Event earliestEnter = *min_element(smc->enterv.begin(),
                                           smc->enterv.end(), CompareEventsByTime);
        Event latestExit = *max_element(smc->exitv.begin(), smc->exitv.end(),
                                        CompareEventsByTime);
        //Use root process to determine the cnode_id
        Event root_enter = smc->enterv[smc->root_idx];
        StatisticController::severePatternInstance spi(
            earliestEnter.get_time(), latestExit.get_time(), wastedTime,
            root_enter.get_cnodeptr().get_pos(),
            root_start, root_enter.get_loc()->get_id());
        StatisticController::theController->AddSeverity(EXP_PAT_SHMEM_LATE_BROADCAST,
                                get_unique_name(), spi);
        if(PatternTrace::_pt_ptr)
          WriteLateBroadCast<EXP_PAT_SHMEM_LATE_BROADCAST>(smc);
      }
    }

  return NULL;
}

/****************************************************************************
* SHMEM lock completion //FIXME: use LockPtr
****************************************************************************/

bool
SHMEMLockCompletion::isenabled()
{
  return is_shmem_appl(trace);
}

void
SHMEMLockCompletion::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      if ( is_shmem_sync_lock(ctree.get_callee(cnode_id)) )
        for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
          severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}



/****************************************************************************
*
* Auxiliary functions
*
****************************************************************************/

bool is_shmem_appl(const earl::EventTrace* trace)
{
  //return true;
  return trace->has_shmem();
}

bool is_shmem_api(const earl::Region* region)
{
  if ( !region || region->get_name().length() < 6 )
    return false;
  
  if ( region->get_name().length() == 7 && region->get_name() == "barrier" )
    return true;

  if ( is_shmem_mem_management(region) )
    return true;

  
  string prefix(region->get_name(), 0, 5);
 
  return prefix == "shmem";
}

bool is_shmem_sync(const earl::Region* region)
{
  if (!is_shmem_api(region)) 
      return false;
  
  string name = region->get_name();
 
  if ( name.find("barrier") != string::npos ||
       name.find("wait")    != string::npos ||
       name.find("fence")   != string::npos ||
       name.find("quiet")   != string::npos ||
       name.find("poll")    != string::npos )
    return true;

  return is_shmem_sync_p2p(region)       ||
         is_shmem_mem_management(region) ||
         is_shmem_init(region);
}

bool is_shmem_barrier(const earl::Region* region)
{
  if ( !is_shmem_api(region) )
    return false;
 
  bool res = region->get_name().find("barrier") != string::npos;
  return res;
}

bool is_shmem_sync_p2p(const earl::Region* region)
{
  return is_shmem_sync_lock(region) || is_shmem_sync_wait(region);
}

bool is_shmem_comm(const earl::Region* region)
{
  return is_shmem_coll(region) || is_shmem_rma(region);
}

bool is_shmem_coll(const earl::Region* region)
{
    return is_shmem_n2n(region) || is_shmem_bcast(region);
}

bool is_shmem_n2n(const earl::Region* region)
{
  if (!is_shmem_api(region)) 
    return false;
    
  string suffix = region->get_name().substr(6);
  if ( suffix.find("collect") != string::npos )
    return true;

  suffix = region->get_name().substr(region->get_name().length()-6);
  return suffix == "to_all";
}

bool is_shmem_bcast(const earl::Region* region)
{
  if (!is_shmem_api(region)) 
    return false;
    
  string name = region->get_name();
  if ( name.find("broadcast") != string::npos )
    return true;

  return false;
}

bool is_shmem_rma(const earl::Region* region)
{
  return is_shmem_get(region) || is_shmem_put(region) || is_shmem_atomic(region);
}

bool is_shmem_get(const earl::Region* region)
{
  if ( ! is_shmem_api(region) )
    return false;
    
  // shmem_*get*
  string suffix = region->get_name().substr(6);
  if ( suffix.find("get") != string::npos )
    return true;
  
  // shmem_*_g
  if ( suffix.length() < 2 ) return false;
  suffix = suffix.substr(suffix.length()-2); 
  return suffix == "_g";
}


bool is_shmem_put(const earl::Region* region)
{
  if ( ! is_shmem_api(region) )
    return false;
  
  // shmem_*put*
  string suffix = region->get_name().substr(6);
  if ( suffix.find("put") != string::npos )
    return true;

  // shmem_*_p
  if ( suffix.length() < 2 ) return false;
  suffix = suffix.substr( suffix.length() );
  return suffix == "_p";
}


bool is_shmem_atomic(const earl::Region* region)
{
  if ( ! is_shmem_api(region) )
    return false;
    
  // shmem_*swap, shmem_*fadd shmem_*add shmem_*inc
  if ( region->get_name().length() < 3 ) return false;
  string suffix = region->get_name().substr( region->get_name().length()-3 );
  return suffix=="wap" || suffix=="add" || suffix=="inc";
}

bool is_shmem_sync_lock(const earl::Region* region)
{
  if ( ! is_shmem_api(region) )
    return false;
    
  // shmem_*swap, shmem_*fadd shmem_*add shmem_*inc
  if ( region->get_name().length() < 5 ) return false;
  string suffix = region->get_name().substr( region->get_name().length()-5 );
  return suffix=="_lock";
}

bool is_shmem_sync_wait(const earl::Region* region)
{
  if ( ! is_shmem_api(region) )
    return false;
    
  // shmem_*wait, shmem_*wait_until
  if ( region->get_name().length() < 5 ) return false;
  string suffix = region->get_name().substr( region->get_name().length()-5 );
  return suffix=="_wait" || suffix=="until";
}

bool is_shmem_init(const earl::Region* region)
{
  string name = region->get_name();
  return name=="shmem_init" || name=="shmem_finalize";
}

bool is_shmem_mem_management(const earl::Region* region)
{
  // shmalloc, shmalloc_nb, shfree, shrealloc and shmemalign
  if ( region->get_name().length() < 6 ) return false;
  string suffix = region->get_name().substr(0,6);
  return suffix == "shmall" || suffix == "shfree" || 
         suffix == "shreal" || suffix == "shmema";
}

