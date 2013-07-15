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

#ifndef EXP_PATTERNTRACE_H
#define EXP_PATTERNTRACE_H

#include <earl.h>
#include <string>
#include <elg_rw.h>
#include "CompoundEvents.h"
#include "RecordBuffer.h"
#include "ExpertOpt.h"


/*
 *----------------------------------------------------------------------------
 *
 * class PatternTrace
 *
 * Pattern Analysis process
 *
 *---------------------------------------------------------------------------- 
 */

class PatternTrace{
public:
  PatternTrace(earl::EventTrace* trace, string filename, int flag);
  ~PatternTrace();
  void WriteHeader();
  void WriteMPIBarrierCompletion(const mpi_coll* mc, double min_end_time);
  void WriteMPIEarlyReduce      (const mpi_coll* mc, double idle_time);
//  void WriteMPILateBroadcast    (const mpi_coll* mc);
  void WriteMPILateSender       (const late_send* ls);
  void WriteMPILateSenderWO     (const late_send* ls, long sendpos);
  void WriteMPILateReceiver     (const late_recv* lr);
  void WriteMPILateReceiverWO   (const late_recv* lr, long sendpos);
  void WriteMPIWaitAtBarrierEnd (const mpi_coll* mc);
  void WriteMPIBarrierCompletionBegin(const mpi_coll* mc);
  //returns false if event is in given region
  bool HandleSendOrReleaseExit  (long startpos, long endpos, long enterpos );
  void HandleCriticalSblockExit ( long startpos, long endpos, long patternregion); 

  void WriteFooter();
  int GetFlag(){ return _flag;}
  RecordBuffer* GetBuffer() const { return _buffer;}

  static PatternTrace* _pt_ptr;
  
  // public datatypes
  enum GroupType{GT_OK,
                 GT_MPIP2PPATTERN, GT_MPIP2PSEVERITY,
                 GT_MPICOLLPATTERN, GT_MPICOLLSEVERITY,
                 GT_MPIRMAPATTERN, GT_MPIRMASEVERITY,
                 GT_OMPPATTERN, GT_OMPSEVERITY,
                 GT_SHMEMPATTERN, GT_SHMEMSEVERITY,
                 GT_MAX
  };
  struct Group_t{
    long nid;
    const char* name;
  };

  enum RegionType{RT_OK,
                  RT_BARRIERCOMPLETION, RT_BARRIERCOMPLETION_SEVERITY,
                  RT_EARLYREDUCE, RT_EARLYREDUCE_SEVERITY,
                  RT_LATEBROADCASTMPI, RT_LATEBROADCASTMPI_SEVERITY,
                  RT_LATEBROADCASTSHMEM, RT_LATEBROADCASTSHMEM_SEVERITY,
                  RT_LATESENDER, RT_LATESENDER_SEVERITY,
                  RT_LATESENDER_WO, RT_LATESENDER_WO_SEVERITY,
                  RT_LATERECEIVER, RT_LATERECEIVER_SEVERITY,
                  RT_LATERECEIVER_WO, RT_LATERECEIVER_WO_SEVERITY,
                  RT_WAITATBARRIER, RT_WAITATBARRIER_SEVERITY,
                  RT_WAITATN2N, RT_WAITATN2N_SEVERITY,
                  RT_WAITATFENCE, RT_WAITATFENCE_SEVERITY,
                  RT_WAITATWINCREATE, RT_WAITATWINCREATE_SEVERITY,
                  RT_WAITATWINFREE, RT_WAITATWINFREE_SEVERITY,
                  RT_WAITATEBARRIER, RT_WAITATEBARRIER_SEVERITY,
                  RT_WAITATIBARRIER, RT_WAITATIBARRIER_SEVERITY,
                  RT_WAITATNXNSHMEM, RT_WAITATNXNSHMEM_SEVERITY,
                  RT_WAITATBARRIERSHMEM, RT_WAITATBARRIERSHMEM_SEVERITY,
                  RT_LOCK_API_OMP, RT_LOCK_API_OMP_SEVERITY,
                  RT_LOCK_CRITICAL_OMP, RT_LOCK_CRITICAL_OMP_SEVERITY,
                  RT_MAX
  } ;
  struct Region_t{
    long nid;
    long rid;
    const char* name;
    enum GroupType group;
    int type;
  };
  
  // static data members
  static Group_t GroupList[GT_MAX];
  static Region_t RegionList[RT_MAX];

private:
  // private data members
  earl::EventTrace * _trace;
  string _filename;
  ElgOut* _p_trace;
  RecordBuffer* _buffer;
  int _flag;// severity, pattern or both 

  };

#endif


