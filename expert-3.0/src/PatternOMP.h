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

#ifndef EXP_PATTERNOMP_H
#define EXP_PATTERNOMP_H

#include "Pattern.h" 
#include "Callback.h"
#include "PatternTrace.h"
#include "PatternTraceTempl.h"
#include "PatternHybrTempl.h"
#include "Severity.h"


/****************************************************************************
*
* OpenMP Patterns                                                                                 
*
****************************************************************************/


void init_patternv_omp(std::vector<Pattern*>& pv, earl::EventTrace* trace);
        
/****************************************************************************
* Idle Threads
****************************************************************************/

class IdleThreads : public Pattern
{
 public:

  IdleThreads(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_IDLE_THREADS; } 
  long get_parent_id()           { return EXP_PAT_TIME; }
  std::string get_name()         { return "Idle Threads"; }
  std::string get_unique_name()  { return "omp_idle_threads"; }
  std::string get_descr()        { return "Unused CPU reservation time during single-threaded execution"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "idle_threads"; }
  
  bool isenabled();                                
  void end(Profile& profile, CallTree& ctree);
};

/****************************************************************************
* OpenMP                                                                            
****************************************************************************/

class OMP : public Pattern
{
 public:

  OMP(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_OMP; } 
  long get_parent_id()           { return EXP_PAT_EXECUTION; }
  std::string get_name()         { return "OMP"; }
  std::string get_unique_name()  { return "omp"; }
  std::string get_descr()        { return "Time spent in the OpenMP run-time system and API"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "omp"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 
  void end(Profile& profile, CallTree& ctree);

  cmp_event* cmpnd(const cmp_event* ce);  // parameter type: team_create
};

/****************************************************************************
* OMP Flush                                                                          
****************************************************************************/
template <>
struct ProfilingPatternTraits<EXP_PAT_OMP_FLUSH>
{
  static long get_parent_id()           { return EXP_PAT_OMP; }
  static std::string get_name()         { return "Flush"; }
  static std::string get_unique_name()  { return "omp_flush"; }
  static std::string get_descr()        { return "Time spent in OpenMP flush directives"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "omp_flush"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_omp_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_omp_flush(reg); }
};
typedef ProfilingPatternT<EXP_PAT_OMP_FLUSH> OMPFlush;

/****************************************************************************
* OMP Fork                                                                           
****************************************************************************/

class OMPFork : public Pattern
{
 public:

  OMPFork(earl::EventTrace* trace) : Pattern(trace), forkv(trace->get_nprocs()) {}

  long get_id()                  { return EXP_PAT_OMP_FORK; } 
  long get_parent_id()           { return EXP_PAT_OMP; }
  std::string get_name()         { return "Fork"; }
  std::string get_unique_name()  { return "omp_fork"; }
  std::string get_descr()        { return "Time needed by the master thread to create a team"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "omp_fork"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* enter(earl::Event& event);
  cmp_event* fork(earl::Event& event);

 private:

  std::vector<earl::Event> forkv;
};

/****************************************************************************
* OMP Synchronization                                                                          
****************************************************************************/

template <>
struct ProfilingPatternTraits<EXP_PAT_OMP_SYNCHRONIZATION>
{
  static long get_parent_id()           { return EXP_PAT_OMP; }
  static std::string get_name()         { return "Synchronization"; }
  static std::string get_unique_name()  { return "omp_synchronization"; }
  static std::string get_descr()        { return "Time spent on OpenMP synchronization"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "omp_synchronization"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_omp_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_omp_sync(reg); }
};
typedef ProfilingPatternT<EXP_PAT_OMP_SYNCHRONIZATION> OMPSynchronization;

/****************************************************************************
* OMP Barrier                                                                         
****************************************************************************/

class OMPBarrier : public Pattern
{
 public:

  OMPBarrier(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_OMP_BARRIER; } 
  long get_parent_id()           { return EXP_PAT_OMP_SYNCHRONIZATION; }
  std::string get_name()         { return "Barrier"; }
  std::string get_unique_name()  { return "omp_barrier"; }
  std::string get_descr()        { return "OpenMP barrier synchronization"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "omp_barrier"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 
  void end(Profile& profile, CallTree& ctree);

  cmp_event* ompcexit(earl::Event& event);
};

/****************************************************************************
* OMP Explicit Barrier                                                                         
****************************************************************************/

template <> 
struct ProfilingPatternTraits<EXP_PAT_OMP_EXPLICIT_BARRIER>
{
  static long get_parent_id()           { return EXP_PAT_OMP_BARRIER; }
  static std::string get_name()         { return "Explicit"; }
  static std::string get_unique_name()  { return "omp_explicit_barrier"; }
  static std::string get_descr()        { return "Time spent in explicit OpenMP barriers"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "omp_explicit_barrier"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_omp_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_omp_ebarrier(reg); }
};
typedef ProfilingPatternT<EXP_PAT_OMP_EXPLICIT_BARRIER> OMPEBarrier;

/****************************************************************************
* Wait at Barrier (OMP, explicit)                                                                         
****************************************************************************/
template<>
struct WaitAtPatternTraits<EXP_PAT_OMP_WAIT_AT_EBARRIER>
{
 public:
  typedef omp_coll CollT;
  static long get_parent_id()           { return EXP_PAT_OMP_EXPLICIT_BARRIER; }
  static std::string get_name()         { return "WaitAtEBarrier"; }
  static std::string get_unique_name()  { return "omp_ebarrier_wait"; }
  static std::string get_descr()        { return "Waiting time in front of explicit OpenMP barriers"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "omp_wait_at_ebarrier"; }
  static cetype get_cmpnd()             { return OMP_COLL;}
  static bool isenabled(const earl::EventTrace* trace) { return is_omp_appl(trace);}
  static bool isappropriate(const earl::Region* reg) { return is_omp_ebarrier(reg);}
};
typedef WaitAtPattern<EXP_PAT_OMP_WAIT_AT_EBARRIER> OMPWaitAtEBarrier;


/****************************************************************************
* OMP Implicit Barrier                                                                         
****************************************************************************/

template <> 
struct ProfilingPatternTraits<EXP_PAT_OMP_IMPLICIT_BARRIER>
{
  static long get_parent_id()           { return EXP_PAT_OMP_BARRIER; }
  static std::string get_name()         { return "Implict"; }
  static std::string get_unique_name()  { return "omp_implicit_barrier"; }
  static std::string get_descr()        { return "Time spent in implicit OpenMP barriers"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "omp_implicit_barrier"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_omp_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_omp_ibarrier(reg); }
};
typedef ProfilingPatternT<EXP_PAT_OMP_IMPLICIT_BARRIER> OMPIBarrier;


/****************************************************************************
* Wait at Barrier (OMP, implicit)                                                                         
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_OMP_WAIT_AT_IBARRIER>
{
 public:
  typedef omp_coll CollT;
  static long get_parent_id()           { return EXP_PAT_OMP_IMPLICIT_BARRIER; }
  static std::string get_name()         { return "WaitAtIBarrier"; }
  static std::string get_unique_name()  { return "omp_ibarrier_wait"; }
  static std::string get_descr()        { return "Waiting time in front of implicit OpenMP barriers"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "omp_wait_at_ibarrier"; }
  static cetype get_cmpnd()             { return OMP_COLL;}
  static bool isenabled(const earl::EventTrace* trace) { return is_omp_appl(trace);}
  static bool isappropriate(const earl::Region* reg) { return is_omp_ibarrier(reg);}
};
typedef WaitAtPattern<EXP_PAT_OMP_WAIT_AT_IBARRIER> OMPWaitAtIBarrier;


/****************************************************************************
* OMP Lock                                                                     
****************************************************************************/

class OMPLock : public Pattern
{
 public:

  OMPLock(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_OMP_LOCK; } 
  long get_parent_id()           { return EXP_PAT_OMP_SYNCHRONIZATION; }
  std::string get_name()         { return "Lock Competition"; }
  std::string get_unique_name()  { return "omp_lock_competition"; }
  std::string get_descr()        { return "Time spent waiting for a lock which is in possesion of another thread"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "omp_lock"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* ompalock(earl::Event& event);
};

/****************************************************************************
* OMP Lock (API)                                                                     
****************************************************************************/

class OMPAPILock : public Pattern
{
 public:

  OMPAPILock(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_OMP_LOCK_API; } 
  long get_parent_id()           { return EXP_PAT_OMP_LOCK; }
  std::string get_name()         { return "API"; }
  std::string get_unique_name()  { return "omp_lock_api"; }
  std::string get_descr()        { return "Time spent in OpenMP API calls waiting for a lock"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "omp_lock_api"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce); // paremter type: lock_comp
};

/****************************************************************************
* OMP Lock (Critical Section)                                                                     
****************************************************************************/

class OMPCriticalLock : public Pattern
{
 public:

  OMPCriticalLock(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_OMP_LOCK_CRITICAL; } 
  long get_parent_id()           { return EXP_PAT_OMP_LOCK; }
  std::string get_name()         { return "Critical"; }
  std::string get_unique_name()  { return "omp_lock_critical"; }
  std::string get_descr()        { return "Time spent in front of a critical section waiting for a lock"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "omp_lock_critical"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce); // paremter type: lock_comp
};


#endif




