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

#ifndef EXP_PATTERN_H
#define EXP_PATTERN_H

/*
 *----------------------------------------------------------------------------
 *
 * class Pattern and descendants
 *
 * classes representing execution patterns to be searched for
 *
 *---------------------------------------------------------------------------- 
 */


#define EXP_PATTERNS_VER "2.2"
#define EXP_PATTERNS_URL "@mirror@patterns-" EXP_PATTERNS_VER ".html#"

/*
 *----------------------------------------------------------------------------
 *
 * class Pattern and descendants
 *
 * classes representing execution patterns to be searched for
 *
 *---------------------------------------------------------------------------- 
 */

#define EXP_PAT_NONE                        -1
#define EXP_PAT_TIME                         0
#define EXP_PAT_EXECUTION                    1
#define EXP_PAT_OVERHEAD                     2
#define EXP_PAT_VISITS                       3
#define EXP_PAT_IDLE_THREADS                 4
#define EXP_PAT_FLOATING_POINT               5            
#define EXP_PAT_L1_DATA_CACHE                6
#define EXP_PAT_MPI                          7
#define EXP_PAT_MPI_COMMUNICATION            8
#define EXP_PAT_MPI_P2P                      9
#define EXP_PAT_MPI_LATE_SENDER             10     
#define EXP_PAT_MPI_WRONG_ORDER_LS          11     
#define EXP_PAT_MPI_LATE_RECEIVER           12     
#define EXP_PAT_MPI_WRONG_ORDER_LR          13
#define EXP_PAT_MPI_IO                      14
#define EXP_PAT_MPI_SYNCHRONIZATION         15
#define EXP_PAT_MPI_COLLECTIVE              16
#define EXP_PAT_MPI_WAIT_AT_N2N             17
#define EXP_PAT_MPI_LATE_BCAST              18
#define EXP_PAT_MPI_EARLY_REDUCE            19
#define EXP_PAT_MPI_BARRIER                 20 
#define EXP_PAT_MPI_WAIT_AT_BARRIER         21
#define EXP_PAT_MPI_BARRIER_COMPLETION      22
#define EXP_PAT_OMP                         23
#define EXP_PAT_OMP_FORK                    24
#define EXP_PAT_OMP_FLUSH                   25
#define EXP_PAT_OMP_SYNCHRONIZATION         26
#define EXP_PAT_OMP_BARRIER                 27
#define EXP_PAT_OMP_EXPLICIT_BARRIER        28
#define EXP_PAT_OMP_WAIT_AT_EBARRIER        29
#define EXP_PAT_OMP_IMPLICIT_BARRIER        30
#define EXP_PAT_OMP_WAIT_AT_IBARRIER        31
#define EXP_PAT_OMP_LOCK                    32
#define EXP_PAT_OMP_LOCK_API                33
#define EXP_PAT_OMP_LOCK_CRITICAL           34

#define EXP_PAT_MPI_RMA_COMMUNICATION       35
#define EXP_PAT_MPI_RMA_SYNCHRONIZATION     36
#define EXP_PAT_MPI_RMA_WINDOW              37
#define EXP_PAT_MPI_RMA_FENCE               38
#define EXP_PAT_MPI_RMA_LOCKS               39
#define EXP_PAT_MPI_RMA_GAT_SYNC            40
#define EXP_PAT_MPI_RMA_WAIT_AT_FENCE       41
#define EXP_PAT_MPI_RMA_WAIT_AT_WIN_CREATE  42
#define EXP_PAT_MPI_RMA_WAIT_AT_WIN_FREE    43
#define EXP_PAT_MPI_RMA_EARLY_WAIT          44
#define EXP_PAT_MPI_RMA_LATE_COMPLETE       45
#define EXP_PAT_MPI_RMA_LATE_POST           46
#define EXP_PAT_MPI_RMA_EARLY_TRANSFER      47

#define EXP_PAT_SHMEM                       60 
#define EXP_PAT_SHMEM_COMMUNICATION         61
#define EXP_PAT_SHMEM_COLLECTIVE            62
#define EXP_PAT_SHMEM_LATE_BROADCAST        63
#define EXP_PAT_SHMEM_WAIT_AT_NXN           64
#define EXP_PAT_SHMEM_RMA_COMMUNICATION     65
#define EXP_PAT_SHMEM_SYNCHRONIZATION       66 
#define EXP_PAT_SHMEM_BARRIER               67 
#define EXP_PAT_SHMEM_WAIT_AT_BARRIER       68 
#define EXP_PAT_SHMEM_SYNC_P2P              69 
#define EXP_PAT_SHMEM_LOCK_COMPL            70 
#define EXP_PAT_SHMEM_WAIT_UTIL             71 
#define EXP_PAT_SHMEM_INIT_EXIT             72 
#define EXP_PAT_SHMEM_MEM_MANGEMENT         73 
#define EXP_PAT_MPI_INIT                    74 

#define EXP_PAT_COUNTER_METRIC           100


#include <earl.h>

#include "Callback.h"
#include "Severity.h"

class Analyzer;
class CallTree;
class Pattern;
class Profile; 
class Region; 

/****************************************************************************
* Declaration of MPI-1 auxiliary functions                                                                                    
****************************************************************************/
void init_patternv_mpi(std::vector<Pattern*>& pv, earl::EventTrace* trace);

bool is_mpi_api(const earl::Region* region);    
bool is_mpi_appl(const earl::EventTrace* trace);
bool is_mpi_barrier(const earl::Region* region);
bool is_mpi_block_send(const earl::Region* region); // potentially blocking send
bool is_mpi_comm(const earl::Region* region);
bool is_mpi_collcom(const earl::Region* region);
bool is_mpi_io(const earl::Region* region);
bool is_mpi_init(const earl::Region* region);
bool is_mpi_12n(const earl::Region* region);
bool is_mpi_n21(const earl::Region* region);
bool is_mpi_n2n(const earl::Region* region);
bool is_mpi_p2p(const earl::Region* region);
bool is_mpi_recv(const earl::Region* region);
bool is_mpi_sync(const earl::Region* region);
bool is_mpi_testx(const earl::Region* region);
bool is_mpi_waitall(const earl::Region* region);
bool is_mpi_waitsome(const earl::Region* region);
bool is_mpi_win_api(const earl::Region* region);

/****************************************************************************
* Declaration of MPI-2 RMA auxiliary functions                                                                                    
****************************************************************************/
bool is_rma_comm(const earl::Region* region);
bool is_rma_GATSyncronization(const earl::Region* region);
bool is_rma_fence(const earl::Region* region);
bool is_rma_lock(const earl::Region* region);
bool is_rma_sync(const earl::Region* region);
bool is_rma_win_create(const earl::Region* region);
bool is_rma_win_free(const earl::Region* region);
bool is_rma_window_management(const earl::Region* region);
bool is_mpi_rma_appl(const earl::EventTrace* trace);

/****************************************************************************
* Declaration of SHMEM auxiliary functions                                                                                    
****************************************************************************/
bool is_shmem_api(const earl::Region* region);
bool is_shmem_appl(const earl::EventTrace* trace);
bool is_shmem_barrier(const earl::Region* region);
bool is_shmem_coll(const earl::Region* region);
bool is_shmem_comm(const earl::Region* region);
bool is_shmem_get(const earl::Region* region);
bool is_shmem_n2n(const earl::Region* region);
bool is_shmem_bcast(const earl::Region* region);
bool is_shmem_put(const earl::Region* region);
bool is_shmem_rma(const earl::Region* region);
bool is_shmem_sync(const earl::Region* region);
bool is_shmem_sync_p2p(const earl::Region* region);
bool is_shmem_atomic(const earl::Region* region);
bool is_shmem_sync_lock(const earl::Region* region);
bool is_shmem_sync_wait(const earl::Region* region);
bool is_shmem_init(const earl::Region* region);
bool is_shmem_mem_management(const earl::Region* region);

/****************************************************************************
* Declaration of OpenMP auxiliary functions                                                                                    
****************************************************************************/
bool is_omp_appl(const earl::EventTrace* trace);
bool is_omp_api(const earl::Region* region); 
bool is_omp_lock(const earl::Region* region); 
bool is_omp_barrier(const earl::Region* region); 
bool is_omp_ibarrier(const earl::Region* region);
bool is_omp_ebarrier(const earl::Region* region); 
bool is_omp_flush(const earl::Region* region); 
bool is_omp_critical(const earl::Region* region); 
bool is_omp_parallel(const earl::Region* region); 
bool is_omp_sync(const earl::Region* region); 
bool is_function(const earl::Region* region);

/****************************************************************************
* Pattern creation
****************************************************************************/
class ExpertOpt;
void get_patternv(std::vector<Pattern*>& pv, earl::EventTrace* trace, ExpertOpt* opt);

/****************************************************************************
*
* Auxiliary functions
*
****************************************************************************/

std::string lower(std::string str);
        
/****************************************************************************
*
* Pattern interface (abstract class)
*
****************************************************************************/

class Pattern : public Callback
{
 public:

  Pattern(earl::EventTrace* trace) : 
    trace(trace),
    severity(trace->get_nlocs()) {}
  
  virtual ~Pattern() {}

  virtual long get_id() =0;
  virtual long get_parent_id() =0;
  virtual std::string get_name() =0;
  virtual std::string get_unique_name() = 0;
  virtual std::string get_descr() =0;
  virtual std::string get_uom() =0;
  virtual std::string get_url() =0;
  virtual std::string get_val()                           { return ""; }
  
  virtual bool isenabled()                                { return true; }
  virtual void regcb(Analyzer&)                           { return; }
  virtual void end(Profile&, CallTree&)                   { return; }

  double get_sev(long cnode_id, long loc_id)              { return severity.get(cnode_id, loc_id); }   

 protected:
  
  earl::EventTrace* trace;
  Severity<double> severity;
};

/****************************************************************************
*
* Derived pattern classes (concrete)
*
****************************************************************************/


/****************************************************************************
* Time
****************************************************************************/

class Time : public Pattern
{
 public:

  Time(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_TIME; }
  long get_parent_id()           { return EXP_PAT_NONE; }
  std::string get_name()         { return "Time"; }
  std::string get_unique_name()  { return "time"; }
  std::string get_descr()        { return "Total CPU allocation time (includes time allocated for idle threads)"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "time"; }
  
  void end(Profile& profile, CallTree& ctree);
};

/****************************************************************************
* Execution
****************************************************************************/

class Execution : public Pattern
{
 public:

  Execution(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_EXECUTION; } 
  long get_parent_id()           { return EXP_PAT_TIME; }
  std::string get_name()         { return "Execution"; }
  std::string get_unique_name()  { return "execution"; }
  std::string get_descr()        { return "Execution time (does not includes time allocated for idle threads)"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "execution"; }
  
  void end(Profile& profile, CallTree& ctree);
};

/****************************************************************************
* Overhead
****************************************************************************/

class Overhead : public Pattern
{
 public:

  Overhead(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_OVERHEAD; } 
  long get_parent_id()           { return EXP_PAT_TIME; }
  std::string get_name()         { return "Overhead"; }
  std::string get_unique_name()  { return "overhead"; }
  std::string get_descr()        { return "Time spent performing tasks related to trace generation"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "overhead"; }
  
  void end(Profile& profile, CallTree& ctree);
};

/****************************************************************************
* Number of Visits
****************************************************************************/

class NumberOfVisits : public Pattern
{
 public:

  NumberOfVisits(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_VISITS; } 
  long get_parent_id()           { return EXP_PAT_NONE; }
  std::string get_name()         { return "Visits"; }
  std::string get_unique_name()  { return "visits"; }
  std::string get_descr()        { return "Number of visits"; }
  std::string get_uom()          { return "occ"; }
  std::string get_url()          { return EXP_PATTERNS_URL "visits"; }
  
  void end(Profile& profile, CallTree& ctree);
};

#endif




