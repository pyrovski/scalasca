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

#ifndef EXP_PATTERNMPI_H
#define EXP_PATTERNMPI_H

#include "Pattern.h" 

#include "Callback.h"
#include "Severity.h"
#include "PatternTrace.h"
#include "PatternTraceTempl.h"
#include "PatternHybrTempl.h"

/****************************************************************************
* MPI                                                                                    
****************************************************************************/

class MPI : public Pattern
{
 public:

  MPI(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI; } 
  long get_parent_id()           { return EXP_PAT_EXECUTION; }
  std::string get_name()         { return "MPI"; }
  std::string get_unique_name()  { return "mpi"; }
  std::string get_descr()        { return "Time spent in MPI calls"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 
  void end(Profile& profile, CallTree& ctree);

  cmp_event* mpicexit (earl::Event& event);
  cmp_event* rmawcexit(earl::Event& event);
  cmp_event* rmawexit (earl::Event& event);
};

/****************************************************************************
* MPI Communication                                                                                    
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_COMMUNICATION>
{
  static long get_parent_id()           { return EXP_PAT_MPI;                   }
  static std::string get_name()         { return "Communication";               }
  static std::string get_unique_name()  { return "mpi_communication";           }
  static std::string get_descr()        { return "Time spent in MPI communication calls"; }
  static std::string get_uom()          { return "sec";                         }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_communication"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_mpi_comm(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_COMMUNICATION> MPICommunication;


/****************************************************************************
* MPI IO                                                                                    
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_IO>
{
  static long get_parent_id()           { return EXP_PAT_MPI; }
  static std::string get_name()         { return "IO"; }
  static std::string get_unique_name()  { return "mpi_io"; }
  static std::string get_descr()        { return "Time spent in MPI IO calls"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_io"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_mpi_io(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_IO> MPIIO;

/****************************************************************************
* MPI Init/Finalyze                                                                                    
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_INIT>
{
  static long get_parent_id()           { return EXP_PAT_MPI; }
  static std::string get_name()         { return "Init/Exit"; }
  static std::string get_unique_name()  { return "mpi_init_exit"; }
  static std::string get_descr()        { return "Time spent in MPI initialization calls"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_init_exit"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_mpi_init(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_INIT> MPIInit;

/****************************************************************************
* MPI Synchronization                                                                                    
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_SYNCHRONIZATION>
{
  static long get_parent_id()           { return EXP_PAT_MPI; }
  static std::string get_name()         { return "Synchronization"; }
  static std::string get_unique_name()  { return "mpi_synchronization"; }
  static std::string get_descr()        { return "Time spent in MPI barrier calls"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_synchronization"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_mpi_sync(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_SYNCHRONIZATION> MPISynchronization;

/****************************************************************************
* Barrier (MPI)                                                                             
****************************************************************************/

template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_BARRIER>
{
  static long get_parent_id()           { return EXP_PAT_MPI_SYNCHRONIZATION; }
  static std::string get_name()         { return "Barrier"; }
  static std::string get_unique_name()  { return "mpi_sync_collective"; }
  static std::string get_descr()        { return "Time spent in MPI barriers"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_barrier"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_mpi_barrier(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_BARRIER> MPIBarrier;

/****************************************************************************
* Wait at Barrier (MPI)                                                                             
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_MPI_WAIT_AT_BARRIER>
{
  typedef mpi_coll CollT;
  static long get_parent_id()           { return EXP_PAT_MPI_BARRIER; }
  static std::string get_name()         { return "WaitAtBarrier"; }
  static std::string get_unique_name()  { return "mpi_barrier_wait"; }
  static std::string get_descr()        { return "Waiting time in front of MPI barriers"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_wait_at_barrier"; }
  static cetype get_cmpnd()             { return MPI_COLL;}
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc);};
  static bool isappropriate(const earl::Region* reg) { return is_mpi_barrier(reg);};
};
typedef WaitAtPattern<EXP_PAT_MPI_WAIT_AT_BARRIER> MPIWaitAtBarrier;


/****************************************************************************
* Barrier Completion (MPI)                                                                             
****************************************************************************/

class MPIBarrierCompletion : public Pattern
{
 public:

  MPIBarrierCompletion(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_BARRIER_COMPLETION; } 
  long get_parent_id()           { return EXP_PAT_MPI_BARRIER; }
  std::string get_name()         { return "Barrier Completion"; }
  std::string get_unique_name()  { return "mpi_barrier_completion";}
  std::string get_descr()        { return "Time needed to finish an MPI barrier"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_barrier_completion"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type: mpi_coll
};

/****************************************************************************
* MPI Point-to-Point                                                                                   
****************************************************************************/
class PointToPoint : public Pattern
{
 public:

  PointToPoint(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_P2P; }
  long get_parent_id()           { return EXP_PAT_MPI_COMMUNICATION; }
  std::string get_name()         { return "P2P"; }
  std::string get_unique_name()  { return "mpi_point2point"; }
  std::string get_descr()        { return "MPI point-to-point communication"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_p2p"; }

  bool isenabled();
  void regcb(Analyzer& analyzer);
  void end(Profile& profile, CallTree& ctree);

  cmp_event* recv(earl::Event& event);
};


/****************************************************************************
* MPI Late Sender                                                                                   
****************************************************************************/

class LateSender : public Pattern
{
 public:

  LateSender(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_LATE_SENDER; } 
  long get_parent_id()           { return EXP_PAT_MPI_P2P; }
  std::string get_name()         { return "Late Sender"; }
  std::string get_unique_name()  { return "mpi_latesender"; }
  std::string get_descr()        { return "Time a receiving process is waiting for a message"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_late_sender"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type: recv_op
};

/****************************************************************************
* Messages in Wrong Order (Late Sender)                                                                                 
****************************************************************************/

class MsgsWrongOrderLS : public Pattern
{
 public:

  MsgsWrongOrderLS(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_WRONG_ORDER_LS; } 
  long get_parent_id()           { return EXP_PAT_MPI_LATE_SENDER; }
  std::string get_name()         { return "Messages in Wrong Order"; }
  std::string get_unique_name()  { return "mpi_latesender_wo"; }
  std::string get_descr()        { return "Late Sender situation due to messages received in the wrong oder"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_wrong_order_ls"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type: late_send
};

/****************************************************************************
* MPI Late Receiver                                                                                   
****************************************************************************/

class LateReceiver : public Pattern
{
 public:

  LateReceiver(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_LATE_RECEIVER; } 
  long get_parent_id()           { return EXP_PAT_MPI_P2P; }
  std::string get_name()         { return "Late Receiver"; }
  std::string get_unique_name()  { return "mpi_latereceiver"; }
  std::string get_descr()        { return "Time a sending process is waiting for the receiver to become ready"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_late_receiver"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type: recv_op
};

/****************************************************************************
* Messages in Wrong Order (Late Receiver)                                                                                 
****************************************************************************/

class MsgsWrongOrderLR : public Pattern
{
 public:

  MsgsWrongOrderLR(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_WRONG_ORDER_LR; } 
  long get_parent_id()           { return EXP_PAT_MPI_LATE_RECEIVER; }
  std::string get_name()         { return "Messages in Wrong Order"; }
  std::string get_unique_name()  { return "mpi_latereceiver_wo"; }
  std::string get_descr()        { return "Late Receiver situation due to messages sent in the wrong oder"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_wrong_order_lr"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type: late_recv
};

/****************************************************************************
* Collective                                                                                
****************************************************************************/
template<>
struct ProfilingPatternTraits<EXP_PAT_MPI_COLLECTIVE>
{
  static long get_parent_id()           { return EXP_PAT_MPI_COMMUNICATION; }
  static std::string get_name()         { return "Collective"; }
  static std::string get_unique_name()  { return "mpi_collective"; }
  static std::string get_descr()        { return "MPI collective communication"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_collective"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc);} 
  static bool isappropriate(const earl::Region* reg) { return is_mpi_collcom(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_COLLECTIVE> Collective;

/****************************************************************************
* Early Reduce                                                                               
****************************************************************************/

class EarlyReduce : public Pattern
{
 public:

  EarlyReduce(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_EARLY_REDUCE; } 
  long get_parent_id()           { return EXP_PAT_MPI_COLLECTIVE; }
  std::string get_name()         { return "Early Reduce"; }
  std::string get_unique_name()  { return "mpi_earlyreduce"; }
  std::string get_descr()        { return "Waiting time due to an early receiver in MPI n-to-1 operations"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_early_reduce"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type: mpi_coll
};

/****************************************************************************
* Late Broadcast                                                                              
****************************************************************************/

class LateBroadcast : public Pattern
{
 public:

  LateBroadcast(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_MPI_LATE_BCAST; } 
  long get_parent_id()           { return EXP_PAT_MPI_COLLECTIVE; }
  std::string get_name()         { return "Late Broadcast"; }
  std::string get_unique_name()  { return "mpi_latebroadcast"; }
  std::string get_descr()        { return "Waiting time due to a late sender in MPI 1-to-n operations"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "mpi_late_bcast"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type: mpi_coll
};

/****************************************************************************
* Wait at N to N                                                                            
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_MPI_WAIT_AT_N2N>
{
  typedef mpi_coll CollT;
  static long get_parent_id()           { return EXP_PAT_MPI_COLLECTIVE; }
  static std::string get_name()         { return "WaitAtNxN"; }
  static std::string get_unique_name()  { return "mpi_wait_nxn"; }
  static std::string get_descr()        { return "Waiting time due to inherent synchronization in MPI n-to-n operations"; }
  static std::string get_uom()          { return "sec"; }
  static std::string get_url()          { return EXP_PATTERNS_URL "mpi_wait_at_n2n"; }
  static cetype get_cmpnd()             { return MPI_COLL;}
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_appl(trc);};
  static bool isappropriate(const earl::Region* reg) { return is_mpi_n2n(reg);};
};
typedef WaitAtPattern<EXP_PAT_MPI_WAIT_AT_N2N> WaitAtN2N;


/****************************************************************************
* MPI-2 RMA
****************************************************************************/


/****************************************************************************
* Profiling patterns
****************************************************************************/


/****************************************************************************
* MPI-2 RMA Communication
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_RMA_COMMUNICATION>
{
  static long get_parent_id()    { return EXP_PAT_MPI_COMMUNICATION; }
  static std::string get_name()  { return "RMA"; }
  static std::string get_unique_name() {return "mpi_rma_communication";}
  static std::string get_descr() { return "Time spent for one-sided (RMA) communication."; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_rma_communication"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_rma_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_rma_comm(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_RMA_COMMUNICATION> MPIRMAComm;

/****************************************************************************
* MPI-2 RMA Synchronization
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_RMA_SYNCHRONIZATION>
{
  static long get_parent_id()    { return EXP_PAT_MPI_SYNCHRONIZATION; }
  static std::string get_name()  { return "RMA"; }
  static std::string get_unique_name() { return "mpi_rma_synchronization"; }
  static std::string get_descr() { return "Time spent in RMA synchronization calls"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_rma_synchronization"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_rma_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_rma_sync(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_RMA_SYNCHRONIZATION> MPIRMASync;

/****************************************************************************
* MPI-2 Window Management
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_RMA_WINDOW>
{
  static long get_parent_id()    { return EXP_PAT_MPI_RMA_SYNCHRONIZATION; }
  static std::string get_name()  { return "Window Management"; }
  static std::string get_unique_name() { return "mpi_rma_window_management"; }
  static std::string get_descr() { return "Time spent in RMA window construction and destruction calls"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_window_management"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_rma_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_rma_window_management(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_RMA_WINDOW> MPIRMAWindow;

/****************************************************************************
* MPI-2 Fence Synchronisation
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_RMA_FENCE>
{
  static long get_parent_id()    { return EXP_PAT_MPI_RMA_SYNCHRONIZATION; }
  static std::string get_name()  { return "Fence"; }
  static std::string get_unique_name() { return "mpi_rma_fence"; }
  static std::string get_descr() { return "Time spent in RMA fence calls"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_fence"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_rma_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_rma_fence(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_RMA_FENCE> MPIRMAFenceSynchronization;

/****************************************************************************
* MPI-2 Lock Synchronisation
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_RMA_LOCKS>
{
  static long get_parent_id()    { return EXP_PAT_MPI_RMA_SYNCHRONIZATION; }
  static std::string get_name()  { return "Lock Synchronization"; }
  static std::string get_unique_name() {return "mpi_rma_lock_synchronization";};
  static std::string get_descr() { return "Time spent in RMA lock calls"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_locks"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_rma_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_rma_lock(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_RMA_LOCKS> MPIRMALockSyncronization;

/****************************************************************************
* MPI-2 GATS Synchronisation
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_MPI_RMA_GAT_SYNC>
{
  static long get_parent_id()    { return EXP_PAT_MPI_RMA_SYNCHRONIZATION; }
  static std::string get_name()  { return "GAT Synchronization"; }
  static std::string get_unique_name() { return "mpi_rma_gat_synchronization"; }
  static std::string get_descr() { return "Time spent in RMA 'Post' 'Start' 'Complete' 'Wait' and 'Test' calls"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_gats"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_mpi_rma_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_rma_GATSyncronization(reg); }
};
typedef ProfilingPatternT<EXP_PAT_MPI_RMA_GAT_SYNC> MPIRMAGATSyncronization;

/****************************************************************************
*  Complex patterns
****************************************************************************/

/****************************************************************************
* Wait at Fence (RMA)                                                                             
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_MPI_RMA_WAIT_AT_FENCE>
{
  typedef rma_coll CollT;
  static long get_parent_id()    { return EXP_PAT_MPI_RMA_FENCE; }
  static std::string get_name()  { return "WaitAtFence"; }
  static std::string get_unique_name() { return "mpi_rma_fence_wait"; }
  static std::string get_descr() { return "Waiting time in front of MPI fence"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_wait_at_fence"; }
  static cetype get_cmpnd()      { return RMA_COLL;}
  static bool isenabled(earl::EventTrace* trace) { return is_mpi_rma_appl(trace);}
  static bool isappropriate(const earl::Region* reg) { return is_rma_fence(reg);}
};
typedef WaitAtPattern<EXP_PAT_MPI_RMA_WAIT_AT_FENCE> MPIRMAWaitAtFence;


/****************************************************************************
* Wait at Window Create (RMA)                                                                             
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_MPI_RMA_WAIT_AT_WIN_CREATE>
{
  typedef rma_coll CollT;
  static long get_parent_id()    { return EXP_PAT_MPI_RMA_WINDOW; }
  static std::string get_name()  { return "WaitAtWinCreate"; }
  static std::string get_unique_name() { return "mpi_rma_wait_window_create"; }
  static std::string get_descr() { return "Waiting time in front of RMA window creation"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_wait_at_win_create"; }
  static cetype get_cmpnd()      { return RMA_COLL;}
  static bool isenabled(const earl::EventTrace* trace) { return is_mpi_rma_appl(trace);};
  static bool isappropriate(const earl::Region* reg) { return is_rma_win_create(reg);};
};
typedef WaitAtPattern<EXP_PAT_MPI_RMA_WAIT_AT_WIN_CREATE> MPIRMAWaitAtWinCreate;


/****************************************************************************
* Wait at Window Free (RMA)                                                                             
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_MPI_RMA_WAIT_AT_WIN_FREE>
{
  typedef rma_coll CollT;
  static long get_parent_id()    { return EXP_PAT_MPI_RMA_WINDOW; }
  static std::string get_name()  { return "WaitAtWinFree"; }
  static std::string get_unique_name() { return "mpi_rma_wait_window_free"; }
  static std::string get_descr() { return "Waiting time in front of RMA window destruktion"; }
  static std::string get_uom()   { return "sec"; }
  static std::string get_url()   { return EXP_PATTERNS_URL "mpi_wait_at_win_free"; }
  static cetype get_cmpnd()      { return RMA_COLL;}
  static bool isenabled(const earl::EventTrace* trace) { return is_mpi_rma_appl(trace);};
  static bool isappropriate(const earl::Region* reg) { return is_rma_win_free(reg);};
};
typedef WaitAtPattern<EXP_PAT_MPI_RMA_WAIT_AT_WIN_FREE> MPIRMAWaitAtWinFree;


/****************************************************************************
* Late Post (RMA)                                                                             
****************************************************************************/

class MPIRMALatePost : public Pattern
{
 public:

  MPIRMALatePost(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()           { return EXP_PAT_MPI_RMA_LATE_POST; } 
  long get_parent_id()    { return EXP_PAT_MPI_RMA_GAT_SYNC; }
  std::string get_name()  { return "Late Post"; }
  std::string get_unique_name() { return "mpi_rma_latepost"; }
  std::string get_descr() { return "The access to the target window is delayed until the window is exposed"; }
  std::string get_uom()   { return "sec"; }
  std::string get_url()   { return EXP_PATTERNS_URL "mpi_late_post"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);
};

/****************************************************************************
* Early Transfer (RMA)                                                                             
****************************************************************************/

class MPIRMAEarlyTransfer : public Pattern
{
 public:

  MPIRMAEarlyTransfer(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()           { return EXP_PAT_MPI_RMA_EARLY_TRANSFER; }
  long get_parent_id()    { return EXP_PAT_MPI_RMA_COMMUNICATION; }
  std::string get_name()  { return "Early Transfer"; }
  std::string get_unique_name() { return "mpi_rma_earlytransfer"; }
  std::string get_descr() { return "MPI_Put()/MPI_Get() call is delayed until the window is exposed on target"; }
  std::string get_uom()   { return "sec"; }
  std::string get_url()   { return EXP_PATTERNS_URL "mpi_early_transfer"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);
};

/****************************************************************************
* Early Wait (RMA)                                                                             
****************************************************************************/

class MPIRMAEarlyWait : public Pattern
{
 public:

  MPIRMAEarlyWait(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()           { return EXP_PAT_MPI_RMA_EARLY_WAIT; } 
  long get_parent_id()    { return EXP_PAT_MPI_RMA_GAT_SYNC; }
  std::string get_name()  { return "Early Wait"; }
  std::string get_unique_name() { return "mpi_rma_earlywait"; }
  std::string get_descr() { return "Early call to MPI_Win_wait() is delayed on target, until all matching calls to MPI_Win_complete() have occurred"; }
  std::string get_uom()   { return "sec"; }
  std::string get_url()   { return EXP_PATTERNS_URL "mpi_early_wait"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);
};

/****************************************************************************
* Late Complete (RMA)                                                                             
****************************************************************************/

class MPIRMALateComplete : public Pattern
{
 public:

  MPIRMALateComplete(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()           { return EXP_PAT_MPI_RMA_LATE_COMPLETE; } 
  long get_parent_id()    { return EXP_PAT_MPI_RMA_EARLY_WAIT; }
  std::string get_name()  { return "Late Complete"; }
  std::string get_unique_name() { return "mpi_rma_latecomplete"; }
  std::string get_descr() { return "Time between last transfer operation and MPI_Win_Complete(), which causes dalaying of MPI_Win_wait() on target"; }
  std::string get_uom()   { return "sec"; }
  std::string get_url()   { return EXP_PATTERNS_URL "mpi_late_complete"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);   // parameter type:
};

#endif




