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

#ifndef EXP_PATTERNTRACETEMPL_H
#define EXP_PATTERNTRACETEMPL_H

#include "Pattern.h"
#include "PatternTrace.h"

template <long id> 
struct PatternTraceTraits
{
  /*static long PatternRegion() { return ELG_NO_ID;}
  static long SeverityRegion() { return ELG_NO_ID;}
  typedef MPICollExitRecord RecordType;
  static long GetGroupID(earl::Event e) { return -1;}*/
};

template<>
struct PatternTraceTraits<EXP_PAT_MPI_WAIT_AT_BARRIER>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATBARRIER].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATBARRIER_SEVERITY].rid;}
  typedef MPICollExitRecord RecordType;
  static long GetGroupID(earl::Event e) { return e.get_com()->get_id(); }
};


template<>
struct PatternTraceTraits<EXP_PAT_MPI_WAIT_AT_N2N>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATN2N].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATN2N_SEVERITY].rid;}
  typedef MPICollExitRecord RecordType;
  static long GetGroupID(earl::Event e) { return e.get_com()->get_id(); }

};

template<>
struct PatternTraceTraits<EXP_PAT_MPI_RMA_WAIT_AT_FENCE>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATFENCE].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATFENCE_SEVERITY].rid;}
  typedef MPIRMACollExitRecord RecordType;
  static long GetGroupID(earl::Event e){ return e.get_win()->get_id(); }
};

template<>
struct PatternTraceTraits<EXP_PAT_MPI_RMA_WAIT_AT_WIN_CREATE>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATWINCREATE].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATWINCREATE_SEVERITY].rid;}
  typedef MPIRMACollExitRecord RecordType;
  static long GetGroupID(earl::Event e){ return e.get_win()->get_id(); }
};

template<>
struct PatternTraceTraits<EXP_PAT_MPI_RMA_WAIT_AT_WIN_FREE>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATWINFREE].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATWINFREE_SEVERITY].rid;}
  typedef MPIRMACollExitRecord RecordType;  
  static long GetGroupID(earl::Event e){ return e.get_win()->get_id(); }
};

template<>
struct PatternTraceTraits<EXP_PAT_OMP_WAIT_AT_EBARRIER>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATEBARRIER].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATEBARRIER_SEVERITY].rid;}
  typedef OMPCollExitRecord RecordType;
  static long GetGroupID(earl::Event e){ return ELG_NO_ID; }
};

template<>
struct PatternTraceTraits<EXP_PAT_OMP_WAIT_AT_IBARRIER>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATIBARRIER].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATIBARRIER_SEVERITY].rid;}
  typedef OMPCollExitRecord RecordType;
  static long GetGroupID(earl::Event e){ return ELG_NO_ID; }
};

template<>
struct PatternTraceTraits<EXP_PAT_SHMEM_WAIT_AT_NXN>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATNXNSHMEM].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATNXNSHMEM_SEVERITY].rid;}
  typedef CollExitRecord RecordType;
  static long GetGroupID(earl::Event e) { return e.get_com()->get_id(); }
};

template<>
struct PatternTraceTraits<EXP_PAT_SHMEM_WAIT_AT_BARRIER>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATBARRIERSHMEM].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_WAITATBARRIERSHMEM_SEVERITY].rid;}
  typedef CollExitRecord RecordType;
  static long GetGroupID(earl::Event e) { return e.get_com()->get_id(); }
};



template<long id, typename traits>
void WriteWaitAt(const cmp_event* ce, double maxtime){

  RecordBuffer* _buffer = PatternTrace::_pt_ptr->GetBuffer();
 
  const typename traits::CollT* col = (const typename traits::CollT*) ce;
        
  double ptime;
  long   ploc;
  long   patternregion  = PatternTraceTraits<id>::PatternRegion();
  long   severityregion = PatternTraceTraits<id>::SeverityRegion();
  int    _flag          = PatternTrace::_pt_ptr->GetFlag();

  /*** ENTER PATTERN / ENTER SEVERITY / EXIT SEVERITY ***/
  for(size_t count=0; count < col->enterv.size(); ++count){
    ptime = col->enterv[count].get_time();
    ploc  = col->enterv[count].get_loc()->get_id();
    switch( _flag ){
      case ExpertOpt::PT_SEVERITY:  
        if(ptime < maxtime){
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
          _buffer->AddEvent(ploc, new ExitRecord(maxtime) );
        }
        break;
      case ExpertOpt::PT_PATTERN: 
        _buffer->AddEvent(ploc, new EnterRecord(ptime, patternregion) );
        break;
      case ExpertOpt::PT_ALL: 
        if(ptime < maxtime){
          _buffer->AddEvent(ploc, new ExitRecord(maxtime) );
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
          _buffer->PrependEvent(ploc, new EnterRecord(ptime, patternregion) );
        }else{
           _buffer->AddEvent(ploc, new EnterRecord(ptime, patternregion) );
        }
        break;
      default:
        break;
    }
  }
  /*** EXIT PATTERN ***/
  if(_flag&ExpertOpt::PT_PATTERN && (id != EXP_PAT_MPI_WAIT_AT_BARRIER) ){
    long psent;
    long precvd;
    long proot=0;
    
    //proot = col->enterv[0].get_root()->get_id();
    long pcomm = PatternTraceTraits<id>::GetGroupID(col->exitv[1]);

    for(size_t count=0; count < col->exitv.size(); ++count){
      ptime = col->exitv[count].get_time();
      ploc  = col->exitv[count].get_loc()->get_id();
      psent = col->exitv[count].get_sent();
      precvd= col->exitv[count].get_recvd();
      _buffer->AddEvent(ploc, new typename PatternTraceTraits<id>::RecordType (ptime, pcomm, proot, psent, precvd) );
    }
  }

  
}

// ************************************************************
// ****    LATEBROADCAST
// ************************************************************

template <long id>
struct LateBroadCastTraits
{
};

template <>
struct LateBroadCastTraits<EXP_PAT_SHMEM_LATE_BROADCAST>{
  static std::string get_name()         { return "LateBroadcastSHMEM"; }
  static std::string get_unique_name()  { return "shmem_latebroadcast"; }
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_LATEBROADCASTSHMEM].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_LATEBROADCASTSHMEM_SEVERITY].rid;}
  typedef CollExitRecord RecordType;
  typedef shmem_coll CollT;
};

template <>
struct LateBroadCastTraits<EXP_PAT_MPI_LATE_BCAST>{
  static std::string get_name()         { return "LateBroadcast"; }
  static std::string get_unique_name()  { return "mpi_latebroadcast"; }
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_LATEBROADCASTMPI].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_LATEBROADCASTMPI_SEVERITY].rid;}
  typedef MPICollExitRecord RecordType;
  typedef mpi_coll CollT;
};

template<long id>
void WriteLateBroadCast(const cmp_event* ce){

  RecordBuffer* _buffer = PatternTrace::_pt_ptr->GetBuffer();

  const typename LateBroadCastTraits<id>::CollT* coll = (const typename LateBroadCastTraits<id>::CollT*) ce;

  long   ploc;
  long   broadcastregion  = LateBroadCastTraits<id>::PatternRegion(); 
  long   severityregion = LateBroadCastTraits<id>::SeverityRegion(); 
  int    _flag          = PatternTrace::_pt_ptr->GetFlag();

  /***** ENTER BROADCAST / ENTER SEVERITY *****/
  double ptime;
  double rootstarttime = coll->enterv[coll->root_idx].get_time(); 
  for(size_t count=0; count < coll->enterv.size(); ++count){
    ptime = coll->enterv[count].get_time();
    ploc  = coll->enterv[count].get_loc()->get_id();
    switch(_flag){
      case ExpertOpt::PT_SEVERITY:  
        if(ptime < rootstarttime)
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
        break;
      case ExpertOpt::PT_PATTERN: 
        _buffer->AddEvent(ploc, new EnterRecord(ptime, broadcastregion) );
        break;
      case ExpertOpt::PT_ALL: 
        if(ptime < rootstarttime){
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
          _buffer->PrependEvent(ploc, new EnterRecord(ptime, broadcastregion) );
        }else{
          _buffer->AddEvent(ploc, new EnterRecord(ptime, broadcastregion) );
        }
        break;
      default:
        break;
    }
  }

  /*****  EXIT SEVERITY *****/

  if(_flag&ExpertOpt::PT_SEVERITY){
    for(size_t count=0; count < coll->enterv.size(); ++count){
      ptime = coll->enterv[count].get_time();
      ploc  = coll->enterv[count].get_loc()->get_id();
      if(ptime < rootstarttime)
          _buffer->AddEvent(ploc, new ExitRecord(rootstarttime) );
    }
  }

  /*****  EXIT BROADCAST *****/

  if(_flag&ExpertOpt::PT_PATTERN){
    long pcomm = coll->exitv[1].get_com()->get_id();
    long proot = (coll->enterv[coll->root_idx]).get_loc()->get_id();
    long psent;
    long precvd;
    for(size_t count=0; count < coll->exitv.size(); ++count){
      ptime = coll->exitv[count].get_time();
      ploc  = coll->exitv[count].get_loc()->get_id();
      psent = coll->exitv[count].get_sent();
      precvd= coll->exitv[count].get_recvd();
      _buffer->AddEvent(ploc, new typename LateBroadCastTraits<id>::RecordType(ptime, pcomm, proot, psent, precvd) );
    }
  } 
}
 

// *************************************
// ***  OMP_LOCK
// *************************************

template <long id>
struct OMPLockTraits
{
};

template<>
struct OMPLockTraits<EXP_PAT_OMP_LOCK_API>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_LOCK_API_OMP].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_LOCK_API_OMP_SEVERITY].rid;}

};

template<>
struct OMPLockTraits<EXP_PAT_OMP_LOCK_CRITICAL>{
  static long PatternRegion() { return PatternTrace::RegionList[PatternTrace::RT_LOCK_CRITICAL_OMP].rid;}
  static long SeverityRegion() { return PatternTrace::RegionList[PatternTrace::RT_LOCK_CRITICAL_OMP_SEVERITY].rid;}

};

template <long id>
void WriteLockCompletion(const lock_comp* lc){
  RecordBuffer* _buffer = PatternTrace::_pt_ptr->GetBuffer();
  int    _flag          = PatternTrace::_pt_ptr->GetFlag();
  long   patternregion  = OMPLockTraits<id>::PatternRegion(); 
  long   severityregion = OMPLockTraits<id>::SeverityRegion(); 

  double ptime = lc->enter_alock.get_time();
  long aloc  = lc->enter_alock.get_loc()->get_id();
  long rloc  = lc->omprlock.get_loc()->get_id();
   
  // ENTER SEVERITY / ENTER ALOCK
  switch(_flag){
    case ExpertOpt::PT_SEVERITY:  
      _buffer->AddEvent(aloc, new EnterRecord(ptime, severityregion) );
      break;
    case ExpertOpt::PT_PATTERN: 
      _buffer->AddEvent(aloc, new EnterRecord(ptime, patternregion) );
      break;
    case ExpertOpt::PT_ALL: 
      _buffer->AddEvent(aloc, new EnterRecord(ptime, severityregion) );
      _buffer->PrependEvent(aloc, new EnterRecord(ptime, patternregion) );
      break;
    default:
      break;
  }
  // EXIT SEVERITY
  if(_flag&ExpertOpt::PT_SEVERITY)
     _buffer->AddEvent(aloc, new ExitRecord(lc->omprlock.get_time() ) );

  if(_flag&ExpertOpt::PT_PATTERN){
    // RELEASE LOCK AND AQUIRE LOCK
    _buffer->AddEvent(rloc, new RLockRecord(lc->omprlock.get_time(), lc->omprlock.get_lock_id() ) );
    _buffer->AddEvent(aloc, new ALockRecord(lc->ompalock.get_time(), lc->omprlock.get_lock_id() ) );
    // Additional Send Recv Record for illustrating Lock Events
    _buffer->AddEvent( rloc, new SendRecord(lc->omprlock.get_time(), aloc, 0, 424242, 0 ) );
    _buffer->AddEvent( aloc, new RecvRecord(lc->ompalock.get_time(), rloc, 0, 424242) );
        // EXIT RLOCK
    if(PatternTrace::_pt_ptr->HandleSendOrReleaseExit(lc->omprlock.get_pos(), lc->ompalock.get_pos(), lc->omprlock.get_enterptr().get_pos() ) )
      _buffer->PushOpenEnter(lc->omprlock.get_pos(), rloc);
    //EXIT  ALOCK CRITICAL
    _buffer->PushOpenEnter(lc->ompalock.get_enterptr().get_pos(), aloc);  

    if(id == EXP_PAT_OMP_LOCK_CRITICAL){
      //ENTER RLOCK CRITICAL 
      PatternTrace::_pt_ptr->HandleCriticalSblockExit(lc->omprlock.get_enterptr().get_pos(), lc->omprlock.get_pos(), patternregion); 
    }else{
      ptime = lc->omprlock.get_enterptr().get_time();
      // ENTER RLOCK
      _buffer->AddEvent(rloc, new EnterRecord(ptime, patternregion) );
    }
  }
}

#endif

