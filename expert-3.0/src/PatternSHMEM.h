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

#ifndef EXP_PATTERN_SHMEM_H
#define EXP_PATTERN_SHMEM_H

#include "Pattern.h"
#include "PatternTrace.h"
#include "PatternTraceTempl.h" 
#include "PatternHybrTempl.h"

#include "Callback.h"
#include "Severity.h"


/****************************************************************************
* Declaration of auxiliary functions                                                                                    
****************************************************************************/
void init_patternv_shmem(std::vector<Pattern*>& pv, earl::EventTrace* trace);

/****************************************************************************
* SHMEM                                                                                    
****************************************************************************/

class SHMEM : public Pattern
{
 public:

  SHMEM(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_SHMEM; } 
  long get_parent_id()           { return EXP_PAT_EXECUTION; }
  std::string get_name()         { return "SHMEM"; }
  std::string get_unique_name()  { return "shmem"; }
  std::string get_descr()        { return "Time spent in SHMEM calls"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "shmem"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 
  void end(Profile& profile, CallTree& ctree);

  cmp_event* collexit (earl::Event& event);
};


/****************************************************************************
* SHMEM Communication                                                                                    
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_COMMUNICATION>
{
  static long get_parent_id()           { return EXP_PAT_SHMEM;                      }
  static std::string get_name()         { return "Communication";                    }
  static std::string get_unique_name()  { return "shmem_communication"; }
  static std::string get_descr()        { return "Time spent in SHMEM communication calls"; }
  static std::string get_uom()          { return "sec";                              }
  static std::string get_url()          { return EXP_PATTERNS_URL "shmem_communication"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc);    }
  static bool isappropriate(const earl::Region* reg) { return is_shmem_comm(reg);    }
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_COMMUNICATION> SHMEMCommunication;


/****************************************************************************
* SHMEM collective communication 
****************************************************************************/

template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_COLLECTIVE>
{
  static long get_parent_id()           { return EXP_PAT_SHMEM_COMMUNICATION;        }
  static std::string get_name()         { return "Collective";                       }
  static std::string get_unique_name()  { return "shmem_collective"; }
  static std::string get_descr()        { return "Time spent in SHMEM collective communication calls"; }
  static std::string get_uom()          { return "sec";                              }
  static std::string get_url()          { return EXP_PATTERNS_URL "shmem_collective";}
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc);    }
  static bool isappropriate(const earl::Region* reg) { return is_shmem_coll(reg);    }
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_COLLECTIVE> SHMEMCollComm;


/****************************************************************************
* SHMEM late bcast                                                                             
****************************************************************************/

class SHMEMLateBroadcast : public Pattern
{
 public:

  SHMEMLateBroadcast(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_SHMEM_LATE_BROADCAST; }
  long get_parent_id()           { return EXP_PAT_SHMEM_COLLECTIVE; }
  std::string get_name()         { return "Late Broadcast"; }
  std::string get_unique_name()  { return "shmem_latebroadcast"; }
  std::string get_descr()        { return "Waiting time due to a late sender in SHMEM 1-to-n operations"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "shmem_late_bcast"; }

  bool isenabled();                                
  void regcb(Analyzer& analyzer); 

  cmp_event* cmpnd(const cmp_event* ce);  
};


/****************************************************************************
* SHMEM Wait at N x N                                                                             
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_SHMEM_WAIT_AT_NXN> 
{
  typedef shmem_coll CollT;
  static long get_parent_id()    { return EXP_PAT_SHMEM_COLLECTIVE;              }
  static std::string get_name()  { return "WaitAtNXNSHMEM"; }
  static std::string get_unique_name() { return "shmem_wait_nxn"; }
  static std::string get_descr() { return "Waiting time in front of SHMEM N x N operations"; }
  static std::string get_uom()   { return "sec";                                 }
  static std::string get_url()   { return EXP_PATTERNS_URL "shmem_wait_at_nxn";  }
  static cetype get_cmpnd()      { return SHMEM_COLL;                            }
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc);}
  static bool isappropriate(const earl::Region* reg) { return is_shmem_n2n(reg); }
};
typedef WaitAtPattern<EXP_PAT_SHMEM_WAIT_AT_NXN> SHMEMWaitAtNxN;


/****************************************************************************
* SHMEM RMA Communication                                                                                    
****************************************************************************/

template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_RMA_COMMUNICATION>
{
  static long get_parent_id()    { return EXP_PAT_SHMEM_COMMUNICATION;               }
  static std::string get_name()  { return "RMA";                                     }
  static std::string get_unique_name() {return "shmem_rma_communication"; }
  static std::string get_descr() { return "Time spent in SHMEM one-sided (RMA) communication calls."; }
  static std::string get_uom()   { return "sec";                                     }
  static std::string get_url()   { return EXP_PATTERNS_URL "shmem_rma_communication";}
  static bool isenabled(const earl::EventTrace* trc)  { return is_shmem_appl(trc);   }
  static bool isappropriate(const earl::Region* reg ) { return is_shmem_rma(reg);    }
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_RMA_COMMUNICATION> SHMEMRMAComm;


/****************************************************************************
* SHMEM Synchronization                                                                                    
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_SYNCHRONIZATION>
{
  static long get_parent_id()           { return EXP_PAT_SHMEM;                      }
  static std::string get_name()         { return "Synchronization";                  }
  static std::string get_unique_name()  { return "shmem_synchronization"; }
  static std::string get_descr()        { return "Time spent in SHMEM barrier calls";}
  static std::string get_uom()          { return "sec";                              }
  static std::string get_url()          { return EXP_PATTERNS_URL "shmem_synchronization"; }
  static bool isenabled(const earl::EventTrace* trc)  { return is_shmem_appl(trc);   }
  static bool isappropriate(const earl::Region* reg ) { return is_shmem_sync(reg);   }
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_SYNCHRONIZATION> SHMEMSynchronization;


/****************************************************************************
* Barrier (SHMEM)                                                                             
****************************************************************************/

template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_BARRIER>
{
  static long get_parent_id()           { return EXP_PAT_SHMEM_SYNCHRONIZATION;       }
  static std::string get_name()         { return "Barrier";                           }
  static std::string get_unique_name()  { return "shmem_sync_collective";}
  static std::string get_descr()        { return "Time spent in SHMEM barriers";      }
  static std::string get_uom()          { return "sec";                               }
  static std::string get_url()          { return EXP_PATTERNS_URL "shmem_barrier";    }
  static bool isenabled(const earl::EventTrace* trc)  { return is_shmem_appl(trc);    }
  static bool isappropriate(const earl::Region* reg ) { return is_shmem_barrier(reg); }
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_BARRIER> SHMEMBarrier;

 
/****************************************************************************
* Wait at Barrier (SHMEM)                                                                             
****************************************************************************/
template <> 
struct WaitAtPatternTraits<EXP_PAT_SHMEM_WAIT_AT_BARRIER>
{
  typedef shmem_coll CollT;
  static long get_parent_id()    { return EXP_PAT_SHMEM_BARRIER;                     }
  static std::string get_name()  { return "WaitAtBarrierSHMEM"; }
  static std::string get_unique_name() { return "shmem_barrier_wait"; }
  static std::string get_descr() { return "Waiting time in front of SHMEM barriers"; }
  static std::string get_uom()   { return "sec";                                     }
  static std::string get_url()   { return EXP_PATTERNS_URL "shmem_wait_at_barrier";  }
  static cetype get_cmpnd()      { return SHMEM_COLL;                                }
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc);    }
  static bool isappropriate(const earl::Region* reg) { return is_shmem_barrier(reg); }
};
typedef WaitAtPattern<EXP_PAT_SHMEM_WAIT_AT_BARRIER> SHMEMWaitAtBarrier;


/****************************************************************************
* SHMEM Point2Point synchronization                                                                             
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_SYNC_P2P>
{
  static long get_parent_id()    { return EXP_PAT_SHMEM_SYNCHRONIZATION;             }
  static std::string get_name()  { return "P2P Synchronization";                     }
  static std::string get_unique_name() { return "shmem_point2point_synchronization"; }
  static std::string get_descr() { return "Time spent in SHMEM point-to-point synchronization"; }
  static std::string get_uom()   { return "sec";                                     }
  static std::string get_url()   { return EXP_PATTERNS_URL "shmem_synchronization";  }
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc);    }
  static bool isappropriate(const earl::Region* reg) { return is_shmem_sync_p2p(reg);}
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_SYNC_P2P> SHMEMSyncP2P;


/****************************************************************************
* SHMEM lock completion                                                                             
****************************************************************************/
class SHMEMLockCompletion : public Pattern
{
 public:

  SHMEMLockCompletion(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return EXP_PAT_SHMEM_LOCK_COMPL; } 
  long get_parent_id()           { return EXP_PAT_SHMEM_SYNC_P2P; }
  std::string get_name()         { return "lock completion"; }
  std::string get_unique_name()  { return "shmem_lock_completion"; }
  std::string get_descr()        { return "Time spent waiting for a lock which is in possesion of another PE"; }
  std::string get_uom()          { return "sec"; }
  std::string get_url()          { return EXP_PATTERNS_URL "shmem_sync_p2p"; }

  bool isenabled();                                
  void end(Profile& profile, CallTree& ctree);
};


/****************************************************************************
* SHMEM wait until                                                                             
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_WAIT_UTIL>
{
  static long get_parent_id()           { return EXP_PAT_SHMEM_SYNC_P2P;              }
  static std::string get_name()         { return "wait until";                        }
  static std::string get_unique_name()  { return "shmem_wait_until"; }
  static std::string get_descr()        { return "Time  spent waiting for a shared variable to be changed by a remote write or atomic swap issued by a different PE."; } 
  static std::string get_uom()          { return "sec";                               }
  static std::string get_url()          { return EXP_PATTERNS_URL "shmem_sync_p2p";   }
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc);     }
  static bool isappropriate(const earl::Region* reg) { return is_shmem_sync_wait(reg);}
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_WAIT_UTIL> SHMEMWaitUntil;


/****************************************************************************
* SHMEM Init/Exit 
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_INIT_EXIT>
{
  static long get_parent_id()           { return EXP_PAT_SHMEM_SYNCHRONIZATION;      }
  static std::string get_name()         { return "Init/Exit";                        }
  static std::string get_unique_name()  { return "shmem_init_exit"; }
  static std::string get_descr()        { return "Time spent in SHMEM initialization calls."; } 
  static std::string get_uom()          { return "sec";                              }
  static std::string get_url()          { return EXP_PATTERNS_URL "shmem_init_exit"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc);    }
  static bool isappropriate(const earl::Region* reg) { return is_shmem_init(reg);    }
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_INIT_EXIT> SHMEMInitExit;


/****************************************************************************
* SHMEM Memory Management
****************************************************************************/
template <> 
struct ProfilingPatternTraits<EXP_PAT_SHMEM_MEM_MANGEMENT>
{
  static long get_parent_id()           { return EXP_PAT_SHMEM_SYNCHRONIZATION;   }
  static std::string get_name()         { return "Memory Management";             }
  static std::string get_unique_name()  { return "shmem_memory_management"; }
  static std::string get_descr()        { return "Time  spent in  memory management SHMEM calls."; } 
  static std::string get_uom()          { return "sec";                           }
  static std::string get_url()          { return EXP_PATTERNS_URL "shmem_memory_management"; }
  static bool isenabled(const earl::EventTrace* trc) { return is_shmem_appl(trc); }
  static bool isappropriate(const earl::Region* reg) { return is_shmem_mem_management(reg);}
};
typedef ProfilingPatternT<EXP_PAT_SHMEM_MEM_MANGEMENT> SHMEMMemoryManagament;

#endif




