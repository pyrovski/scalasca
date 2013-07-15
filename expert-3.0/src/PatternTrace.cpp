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

#include "PatternTrace.h"
#include <string>
#include <iostream>
#include "RecordBuffer.h"
#include "EventRecord.h"
#include <iomanip>

using namespace std;
using namespace earl;

PatternTrace::PatternTrace(earl::EventTrace* trace, string filename, int flag):_trace(trace), _filename(filename), 
    _p_trace(ElgOut_open(_filename.c_str(), ELG_BYTE_ORDER, ELG_UNCOMPRESSED)), _buffer(new RecordBuffer(_trace)),
    _flag(flag)
{
}
 
PatternTrace::~PatternTrace(){
  // BUFFER FLUSH  
  // Close tracefile
   _buffer->WriteAll(_p_trace);
  delete _buffer;  
  WriteFooter();
  ElgOut_close(_p_trace);

}


// **************************************
// ******         WRITEHEADER        ****
// **************************************

void
PatternTrace::WriteHeader()
{
  long _string_id = 0;
  for(long i=0; i < GT_MAX;  ++i){
    ElgOut_write_STRING ( _p_trace, _string_id, 0, GroupList[i].name );
    GroupList[i].nid = _string_id++;
  }
  for(long i=0; i< RT_MAX; ++i){
    Region_t &reg = RegionList[i];
    ElgOut_write_STRING ( _p_trace, _string_id, 0, reg.name);
    reg.nid = _string_id++;
    reg.rid = i;
    ElgOut_write_REGION ( _p_trace, reg.rid, reg.nid, ELG_NO_ID, ELG_NO_LNO, ELG_NO_LNO, GroupList[reg.group].nid, reg.type);
  }
  // end regions
  // machine(s)
  long p_nmachs = _trace->get_nmachs();
  Machine* p_machine;
  
  for(long machine=0; machine < p_nmachs; ++machine){
    p_machine = _trace->get_mach(machine);
    ElgOut_write_STRING ( _p_trace, ++_string_id, 0, p_machine->get_name().c_str() );
    ElgOut_write_MACHINE( _p_trace, p_machine->get_id(), p_machine->get_nnodes(), _string_id);
    // node(s)
    long p_nnodes = p_machine->get_nnodes();
    Node* p_node;
    for(long node=0; node < p_nnodes; ++node){
      p_node = _trace->get_node(machine, node);
      ElgOut_write_STRING ( _p_trace, ++_string_id, 0, p_node->get_name().c_str() );
      ElgOut_write_NODE   ( _p_trace, p_node->get_node_id(), p_node->get_mach_id(), p_node->get_ncpus(), _string_id, p_node->get_clckrt() );
    } // end for node
  } // end for machine

  // location(s)
  long p_locs = _trace->get_nthrds();
  Location* p_loc;
  for(long location=0; location < p_locs; ++location){
    p_loc = _trace->get_loc(location);
    ElgOut_write_LOCATION ( _p_trace, location, p_loc->get_mach()->get_id(), p_loc->get_node()->get_node_id(), 
                            p_loc->get_proc()->get_id(), p_loc->get_thrd()->get_thrd_id() );
  } //end for locations
  // communicator(s)
  long p_comms = _trace->get_ncoms();
  Communicator* p_comm;
  long pid_temp, block, position;
  long byte_size = (p_locs + 7) / 8;
  unsigned char* grpv = new unsigned char[byte_size];

  for(long communicator=0; communicator < p_comms; ++communicator){
    p_comm = _trace->get_com(communicator);
    // set all bytes to zero
    for(long byte_pos=0; byte_pos < byte_size ; ++byte_pos)
      grpv[byte_pos]  &= 0;
    //set bits depending on processes
    for(long process=0; process< p_comm->get_nprocs(); ++process){
      pid_temp     = p_comm->get_proc(process)->get_id();
      block        = (pid_temp / 8);
      position     = 1 << (pid_temp % 8);  
      grpv[block] |= position;
    }
    ElgOut_write_MPI_COMM ( _p_trace, communicator, 0, byte_size, grpv );
  }// end for communicators

  delete[] grpv;

  // window(s)
  long p_wins = _trace->get_nwins();
  MPIWindow* p_win;
  for(long window=0; window < p_wins; ++window){
    p_win = _trace->get_win(window);
    ElgOut_write_MPI_WIN( _p_trace, p_win->get_id(), p_win->get_com()->get_id() );
  }// end for windows
  // LAST_DEF
  ElgOut_write_LAST_DEF( _p_trace);

  // OK area ENTER-STATEMENTS
  long OkRegion = RegionList[RT_OK].rid;
  for( long location = 0; location < p_locs; ++location)
    ElgOut_write_ENTER( _p_trace, location, 0.0, OkRegion, 0, NULL );
  
}
// **************************************
// ****** WRITEMPIBARRIERCOMPLETION *****
// **************************************

void
PatternTrace::WriteMPIBarrierCompletion(const mpi_coll* mc, double min_end_time){
  double ptime;
  long   ploc;
  long   severityregion = RegionList[RT_BARRIERCOMPLETION_SEVERITY].rid; 

  /***** ENTER BARRIER *****/
  // ENTER BARRIER is created in the Wait At Barrier Pattern (see PatternTraceTempl)
 
  /***** ENTER SEVERITY *****/ 
  if(_flag&ExpertOpt::PT_SEVERITY){ 
    for(size_t count=0; count < mc->exitv.size(); ++count){
      if(mc->exitv[count].get_time() > min_end_time){
        ploc  = mc->exitv[count].get_loc()->get_id();
        _buffer->AddEvent(ploc, new EnterRecord(min_end_time, severityregion) );
      }
    }
  }
   
  /***** EXIT SEVERITY / MPICOLLEXIT BARRIER *****/
  long pcomm = mc->exitv[1].get_com()->get_id();
  for(size_t count=0; count < mc->exitv.size(); ++count){
    ptime = mc->exitv[count].get_time();
    ploc  = mc->exitv[count].get_loc()->get_id();
    switch(_flag){
      case ExpertOpt::PT_SEVERITY:  
        if(mc->exitv[count].get_time() > min_end_time)
          _buffer->AddEvent(ploc, new ExitRecord(ptime) );
        break;
      case ExpertOpt::PT_PATTERN: 
        _buffer->AddEvent(ploc, new MPICollExitRecord(ptime, pcomm) );
        break;
      case ExpertOpt::PT_ALL: 
        _buffer->AddEvent(ploc, new MPICollExitRecord(ptime, pcomm) );
        if(mc->exitv[count].get_time() > min_end_time)
          _buffer->PrependEvent(ploc, new ExitRecord(ptime) );
        break;
      default:
        break;
    }
  }
}


// **************************************
// **** WRITEMPIBARRIERCOMPLETIONBEGIN***
// **************************************

void
PatternTrace::WriteMPIBarrierCompletionBegin(const mpi_coll* mc)
{
  /*** ENTER PATTERN ***/
  for(size_t count=0; count < mc->enterv.size(); ++count){
    double ptime = mc->enterv[count].get_time();
    long ploc  = mc->enterv[count].get_loc()->get_id();
    switch( _flag ){
      case ExpertOpt::PT_ALL:
      case ExpertOpt::PT_PATTERN:
        _buffer->AddEvent(ploc, new EnterRecord(ptime, RegionList[RT_BARRIERCOMPLETION].rid) );
        break;
    }
  }
}

// **************************************
// ******  WRITEMPIWAITATBARRIEREND *****
// **************************************

void
PatternTrace::WriteMPIWaitAtBarrierEnd (const mpi_coll* mc)
{
  /***** MPICOLLEXIT BARRIER *****/
  long pcomm = mc->exitv[1].get_com()->get_id();
  for(size_t count=0; count < mc->exitv.size(); ++count){
    double ptime = mc->exitv[count].get_time();
    long   ploc  = mc->exitv[count].get_loc()->get_id();
    switch(_flag){
      case ExpertOpt::PT_PATTERN: 
      case ExpertOpt::PT_ALL: 
        _buffer->AddEvent(ploc, new MPICollExitRecord(ptime, pcomm) );
        break;
    }
  }
}

// **************************************
// ******   WRITEMPIEARLYREDUCE     *****
// **************************************

void
PatternTrace::WriteMPIEarlyReduce(const mpi_coll* mc, double idle_time)
{
  double ptime;
  long   ploc;
  long   reduceregion  = RegionList[RT_EARLYREDUCE].rid;
  long   severityregion = RegionList[RT_EARLYREDUCE_SEVERITY].rid; 
  
  /***** ENTER REDUCE / ENTER SEVERITY *****/
  for(unsigned count=0; count < mc->enterv.size(); ++count){
    ptime = mc->enterv[count].get_time();
    ploc  = mc->enterv[count].get_loc()->get_id();
    switch(_flag){
      case ExpertOpt::PT_SEVERITY:  
        if(count == (unsigned) mc->root_idx)
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
        break;
      case ExpertOpt::PT_PATTERN: 
        _buffer->AddEvent(ploc, new EnterRecord(ptime, reduceregion) );
        break;
      case ExpertOpt::PT_ALL: 
        _buffer->AddEvent(ploc, new EnterRecord(ptime, reduceregion) );
        if(count == (unsigned) mc->root_idx)
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
        break;
      default:
        break;
    }
  }

    /***** MPICOLLEXIT REDUCE *****/
  if(_flag&ExpertOpt::PT_PATTERN){
    long pcomm = mc->exitv[1].get_com()->get_id();
    long proot = (mc->enterv[mc->root_idx]).get_loc()->get_id();
    long psent;
    long precvd;
    for(size_t count=0; count < mc->exitv.size(); ++count){
      ptime = mc->exitv[count].get_time();
      ploc  = mc->exitv[count].get_loc()->get_id();
      psent = mc->exitv[count].get_sent();
      precvd= mc->exitv[count].get_recvd();
      _buffer->AddEvent(ploc, new MPICollExitRecord(ptime, pcomm, proot, psent, precvd) );
    }
  } 

  /***** EXIT SEVERITY *****/ 
  if(_flag&ExpertOpt::PT_SEVERITY){ 
    ptime = mc->enterv[mc->root_idx].get_time();
    ploc  = mc->enterv[mc->root_idx].get_loc()->get_id();
    _buffer->AddEvent(ploc, new ExitRecord(ptime+idle_time) );
  }
}

// **************************************
// ******   WRITEMPILATEBROADCAST  *****
// **************************************
/*
void
PatternTrace::WriteMPILateBroadcast     (const mpi_coll* mc){
  double ptime;
  long   ploc;
  long   broadcastregion  = RegionList[RT_LATEBROADCASTMPI].rid;
  long   severityregion = RegionList[RT_LATEBROADCASTMPI_SEVERITY].rid; 

  // ***** ENTER BROADCAST / ENTER SEVERITY *****
  double rootstarttime = mc->enterv[mc->root_idx].get_time(); 
  for(long count=0; count < mc->enterv.size(); ++count){
    ptime = mc->enterv[count].get_time();
    ploc  = mc->enterv[count].get_loc()->get_id();
    switch(_flag){
      case ExpertOpt::PT_SEVERITY:  
        if(ptime < rootstarttime)
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
        break;
      case ExpertOpt::PT_PATTERN: 
        _buffer->AddEvent(ploc, new EnterRecord(ptime, broadcastregion) );
        break;
      case ExpertOpt::PT_ALL: 
        _buffer->AddEvent(ploc, new EnterRecord(ptime, broadcastregion) );
        if(ptime < rootstarttime)
          _buffer->AddEvent(ploc, new EnterRecord(ptime, severityregion) );
        break;
      default:
        break;
    }
  }

  // *****  EXIT SEVERITY *****

  if(_flag&ExpertOpt::PT_SEVERITY){
    for(long count=0; count < mc->enterv.size(); ++count){
      ptime = mc->enterv[count].get_time();
      ploc  = mc->enterv[count].get_loc()->get_id();
      if(ptime < rootstarttime)
          _buffer->AddEvent(ploc, new ExitRecord(rootstarttime) );
    }
  }

  // *****  EXIT BROADCAST *****

  if(_flag&ExpertOpt::PT_PATTERN){
    long pcomm = mc->exitv[1].get_com()->get_id();
    long proot = (mc->enterv[mc->root_idx]).get_loc()->get_id();
    long psent;
    long precvd;
    for(long count=0; count < mc->exitv.size(); ++count){
      ptime = mc->exitv[count].get_time();
      ploc  = mc->exitv[count].get_loc()->get_id();
      psent = mc->exitv[count].get_sent();
      precvd= mc->exitv[count].get_recvd();
      _buffer->AddEvent(ploc, new MPICollExitRecord(ptime, pcomm, proot, psent, precvd) );
    }
  } 
}
*/

// **************************************
// ******   WRITEMPILATESENDER  *****
// **************************************

void
PatternTrace::WriteMPILateSender(const late_send* ls){
  long severityregion = RegionList[RT_LATESENDER_SEVERITY].rid; 
  long latesenderregion = RegionList[RT_LATESENDER].rid; 
  long recvloc = ls->enter_recv.get_loc()->get_id();
  long sendloc = ls->enter_send.get_loc()->get_id();

  // ENTER Receive / ENTER Send / ENTER Severity 
  double rtime = ls->enter_recv.get_time();
  double stime = ls->enter_send.get_time(); 
  switch(_flag){
    case ExpertOpt::PT_SEVERITY:
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, severityregion) );
      break;       
    case ExpertOpt::PT_PATTERN:
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, latesenderregion) );
      _buffer->AddEvent(sendloc, new EnterRecord(stime, latesenderregion) );
      break;
    case ExpertOpt::PT_ALL:
      _buffer->AddEvent(sendloc, new EnterRecord(stime, latesenderregion) );
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, severityregion) );
      _buffer->PrependEvent(recvloc, new EnterRecord(rtime, latesenderregion) ); 
      break;
    default:
      break;
  }
 
  // EXIT Severity
  if(_flag&ExpertOpt::PT_SEVERITY)
    _buffer->AddEvent(recvloc, new ExitRecord(stime) );

  // EXIT Send / EXIT Receive  
  if(_flag&ExpertOpt::PT_PATTERN){
    stime = ls->send.get_time();
    rtime = ls->recv.get_time();
    if(HandleSendOrReleaseExit(ls->send.get_pos(), ls->recv.get_pos(), ls->enter_send.get_pos() ) )
      _buffer->PushOpenEnter(ls->enter_send.get_pos(), sendloc);
    _buffer->PushOpenEnter(ls->enter_recv.get_pos(), recvloc);
    //MESSAGE SEND/RECV
    long pcomm = ls->send.get_com()->get_id();
    long ptag  = ls->send.get_tag();
    _buffer->AddEvent( sendloc, new SendRecord(stime, recvloc, pcomm, ptag, ls->send.get_length() ) );
    _buffer->AddEvent( recvloc, new RecvRecord(rtime, sendloc, pcomm, ptag) );
  }
}

// **********************************************
// ******   WRITEMPILATESENDER WRONG ORDER  *****
// **********************************************

void
PatternTrace::WriteMPILateSenderWO(const late_send* ls, long sendpos){
  long severityregion = RegionList[RT_LATESENDER_WO_SEVERITY].rid; 
  long latesenderregion = RegionList[RT_LATESENDER_WO].rid; 
  long recvloc = ls->enter_recv.get_loc()->get_id();
  long sendloc = ls->enter_send.get_loc()->get_id();
  Event singlesend = _trace->event(sendpos);
  long send2loc= singlesend.get_loc()->get_id();

  // ENTER Receive / ENTER Send / ENTER Severity 
  double rtime = ls->enter_recv.get_time();
  double stime = ls->enter_send.get_time(); 
  double s2time= singlesend.get_enterptr()->get_time();
  switch(_flag){
    case ExpertOpt::PT_SEVERITY:
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, severityregion) );
      break;       
    case ExpertOpt::PT_PATTERN:
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, latesenderregion) );
      _buffer->AddEvent(sendloc, new EnterRecord(stime, latesenderregion) );
      _buffer->AddEvent(send2loc, new EnterRecord(s2time, latesenderregion) );
      break;
    case ExpertOpt::PT_ALL:
      _buffer->AddEvent(sendloc, new EnterRecord(stime, latesenderregion) );
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, severityregion) );
      _buffer->PrependEvent(recvloc, new EnterRecord(rtime, latesenderregion) ); 
      _buffer->AddEvent(send2loc, new EnterRecord(s2time, latesenderregion) );
      break;
    default:
      break;
  }
 
  // EXIT Severity
  if(_flag&ExpertOpt::PT_SEVERITY)
    _buffer->AddEvent(recvloc, new ExitRecord(stime) );

  // EXIT Send / EXIT Receive  
  if(_flag&ExpertOpt::PT_PATTERN){
    stime = ls->send.get_time();
    rtime = ls->recv.get_time();
    s2time = singlesend.get_time();
    if(HandleSendOrReleaseExit(ls->send.get_pos(), ls->recv.get_pos(), ls->enter_send.get_pos() ) )
      _buffer->PushOpenEnter(ls->enter_send.get_pos(), sendloc);
    _buffer->PushOpenEnter(ls->enter_recv.get_pos(), recvloc);
    if(HandleSendOrReleaseExit(sendpos, ls->recv.get_pos(), singlesend.get_enterptr().get_pos() ) )
      _buffer->PushOpenEnter(singlesend.get_enterptr().get_pos(), send2loc );
    //MESSAGE SEND/RECV
    long pcomm = ls->send.get_com()->get_id();
    long ptag  = ls->send.get_tag();
    _buffer->AddEvent( sendloc, new SendRecord(stime, recvloc, pcomm, ptag, ls->send.get_length() ) );
    _buffer->AddEvent( recvloc, new RecvRecord(rtime, sendloc, pcomm, ptag) );
    _buffer->AddEvent( send2loc, new SendRecord(s2time, recvloc, pcomm, 4711, 0 ) );
    _buffer->AddEvent( recvloc, new RecvRecord(rtime, send2loc, pcomm, 4711) );
  }
    // TEST
    //double time = send->get_time();
//    cout << "Zeit: " << s2time << " Location: "<<send2loc << endl;
}

// **************************************
// ******   WRITEMPILATERECEIVER    *****
// **************************************


void
PatternTrace::WriteMPILateReceiver(const late_recv* ls){

  long severityregion = RegionList[RT_LATERECEIVER_SEVERITY].rid; 
  long latereceiverregion = RegionList[RT_LATERECEIVER].rid; 
  long recvloc = ls->enter_recv.get_loc()->get_id();
  long sendloc = ls->enter_send.get_loc()->get_id();

  // ENTER Send / ENTER Receive / ENTER Severity 
  double stime = ls->enter_send.get_time();
  double rtime = ls->enter_recv.get_time();
  switch(_flag){
    case ExpertOpt::PT_SEVERITY:
      _buffer->AddEvent(sendloc, new EnterRecord(stime, severityregion) );
      break;       
    case ExpertOpt::PT_PATTERN:
      _buffer->AddEvent(sendloc, new EnterRecord(stime, latereceiverregion) );
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, latereceiverregion) );
      break;
    case ExpertOpt::PT_ALL:
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, latereceiverregion) );
      _buffer->AddEvent(sendloc, new EnterRecord(stime, severityregion) );
      _buffer->PrependEvent(sendloc, new EnterRecord(stime, latereceiverregion) ); 
      break;
    default:
      break;
  }

    // EXIT Severity
  if(_flag&ExpertOpt::PT_SEVERITY)
    _buffer->AddEvent(sendloc, new ExitRecord(rtime) );
  // EXIT Send / EXIT Recv
  if(_flag&ExpertOpt::PT_PATTERN){
    stime = ls->send.get_time();
    rtime = ls->recv.get_time();
    if(HandleSendOrReleaseExit(ls->enter_recv.get_pos(), ls->recv.get_pos(), ls->enter_send.get_pos() ) )
      _buffer->PushOpenEnter(ls->enter_send.get_pos(), sendloc);
    _buffer->PushOpenEnter(ls->enter_recv.get_pos(), recvloc);
    long pcomm = ls->send.get_com()->get_id();
    long ptag  = ls->send.get_tag();
    _buffer->AddEvent( sendloc, new SendRecord(stime, recvloc, pcomm, ptag, ls->send.get_length() ) );
    _buffer->AddEvent( recvloc, new RecvRecord(rtime, sendloc, pcomm, ptag) );
  }
}

// **************************************************
// ******   WRITEMPILATERECEIVER WRONG ORDER    *****
// **************************************************


void
PatternTrace::WriteMPILateReceiverWO(const late_recv* ls, long sendpos){

  long severityregion = RegionList[RT_LATERECEIVER_WO_SEVERITY].rid; 
  long latereceiverregion = RegionList[RT_LATERECEIVER_WO].rid; 
  long recvloc = ls->enter_recv.get_loc()->get_id();
  long sendloc = ls->enter_send.get_loc()->get_id();
  Event singlesend = _trace->event(sendpos);

  // ENTER Send / ENTER Receive / ENTER Severity 
  double stime = ls->enter_send.get_time();
  double rtime = ls->enter_recv.get_time();
  double s1time =  singlesend.get_enterptr()->get_time();
  switch(_flag){
    case ExpertOpt::PT_SEVERITY:
      _buffer->AddEvent(sendloc, new EnterRecord(stime, severityregion) );
      break;       
    case ExpertOpt::PT_PATTERN:
      _buffer->AddEvent(sendloc, new EnterRecord(stime, latereceiverregion) );
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, latereceiverregion) );
      _buffer->AddEvent(sendloc, new EnterRecord(s1time, latereceiverregion) );
      break;
    case ExpertOpt::PT_ALL:
      _buffer->AddEvent(recvloc, new EnterRecord(rtime, latereceiverregion) );
      _buffer->AddEvent(sendloc, new EnterRecord(stime, severityregion) );
      _buffer->PrependEvent(sendloc, new EnterRecord(stime, latereceiverregion) ); 
      _buffer->AddEvent(sendloc, new EnterRecord(s1time, latereceiverregion) );
      break;
    default:
      break;
  }

    // EXIT Severity
  if(_flag&ExpertOpt::PT_SEVERITY)
    _buffer->AddEvent(sendloc, new ExitRecord(rtime) );
  // EXIT Send / EXIT Recv
  if(_flag&ExpertOpt::PT_PATTERN){
    stime = ls->send.get_time();
    rtime = ls->recv.get_time();
    s1time = singlesend.get_time();
    double s2time = ls->send.get_enterptr()->get_time();
    if(HandleSendOrReleaseExit(ls->enter_recv.get_pos(), ls->recv.get_pos(), ls->enter_send.get_pos() ) )
      _buffer->PushOpenEnter(ls->enter_send.get_pos(), sendloc);
    _buffer->PushOpenEnter(ls->enter_recv.get_pos(), recvloc);
    long pcomm = ls->send.get_com()->get_id();
    long ptag  = ls->send.get_tag();
    _buffer->AddEvent( sendloc, new SendRecord(stime, recvloc, pcomm, ptag, ls->send.get_length() ) );
    _buffer->AddEvent( recvloc, new RecvRecord(rtime, sendloc, pcomm, ptag) );
    _buffer->AddEvent( sendloc, new SendRecord(s1time, sendloc, pcomm, 4711, 0) );
    _buffer->AddEvent( sendloc, new RecvRecord(s2time, sendloc, pcomm, 4711) );
    HandleSendOrReleaseExit(sendpos, ls->enter_send.get_pos(), singlesend.get_enterptr()->get_pos() );
  }
}


bool PatternTrace::HandleSendOrReleaseExit  (long startpos, long endpos, long enterpos ){
  //returns false if event is in given region
  for ( long pos = startpos; pos <= endpos; ++pos){
    Event curr = _trace->event(pos);
    if ( (curr.get_type() == EXIT) && (curr.get_enterptr().get_pos() == enterpos) ){
      _buffer->AddEvent(curr.get_loc()->get_id(), new ExitRecord(curr.get_time() ) );
      return false;
    } 
  }
  // Exit is not in given region
  return true;

}

void PatternTrace::HandleCriticalSblockExit ( long startpos, long endpos, long patternregion){
  // Generates the Enter Event for the Structured Block Pattern Region  
  for ( long pos = startpos; pos <= endpos; pos++){
    Event curr = _trace->event(pos);
    if((curr.get_type() == EXIT) && ( curr.get_enterptr().get_reg()->get_rtype()=="OMP_CRITICAL_SBLOCK") /*&&  (curr.get_enterptr().get_enterptr().get_pos() == startpos)*/ ){
      _buffer->AddEvent(curr.get_loc()->get_id(), new EnterRecord(curr.get_time(), patternregion) ) ;
      return;
    }
  }
  cerr << " CRITICAL ENTER RLOCK NOT FOUND!! " << endl;
}

// **************************************
// ******       WRITEFOOTER         *****
// **************************************

void
PatternTrace::WriteFooter(){
  double t_end = _trace->event( (_trace->get_nevents() ) )->get_time();

  // Close all traces
  long p_locs = _trace->get_nthrds();
  for(long location = 0; location < p_locs ; ++location){
    ElgOut_write_EXIT ( _p_trace, location, t_end, 0, NULL);
  } 
  
}

  PatternTrace::Group_t PatternTrace::GroupList[GT_MAX]={
    /* GT_OK       */        {-1, "OK"},
    /* GT_MPIP2PPATTERN*/    {-1, "MPI P2P PATTERN"},
    /* GT_MPIP2PSEVERITY */  {-1, "MPI P2P SEVERITY"},
    /* GT_MPICOLLPATTERN*/   {-1, "MPI COLL PATTERN"},
    /* GT_MPICOLLSEVERITY */ {-1, "MPI COLL SEVERITY"},
    /* GT_MPIRMAPATTERN */   {-1, "MPI RMA PATTERN"},
    /* GT_MPIRMASEVERITY */  {-1, "MPI RMA SEVERITY"},
    /* GT_OMPPATTERN */      {-1, "OMP PATTERN"},
    /* GT_OMPSEVERITY */     {-1, "OMP SEVERITY"},
    /* GT_SHMEMPATTERN */    {-1, "SHMEM PATTERN"},
    /* GT_SHMEM_SEVERITY */  {-1, "SHMEM SEVERITY"}
    }
  ;
  PatternTrace::Region_t PatternTrace::RegionList[RT_MAX]={
    /* RT_OK       */                   {-1, -1, "OK", GT_OK, ELG_FUNCTION},
    /* RT_BARRIERCOMPLETION  */         {-1, -1, "BarrierCompletion", GT_MPICOLLPATTERN, ELG_FUNCTION_COLL_BARRIER},
    /* RT_BARRIERCOMPLETION_SEVERITY */ {-1, -1, "Severity (BarrierCompletion)", GT_MPICOLLSEVERITY, ELG_FUNCTION},
    /* RT_EARLYREDUCE */                {-1, -1, "EarlyReduce", GT_MPICOLLPATTERN, ELG_FUNCTION_COLL_ALL2ONE},
    /* RT_EARLYREDUCE_SEVERITY */       {-1, -1, "Severity (EarlyReduce)", GT_MPICOLLSEVERITY, ELG_FUNCTION},
    /* RT_LATEBROADCAST */              {-1, -1, "LateBroadcast", GT_MPICOLLPATTERN, ELG_FUNCTION_COLL_ONE2ALL},
    /* RT_LATEBROADCAST_SEVERITY */     {-1, -1, "Severity (LateBroadcast)", GT_MPICOLLSEVERITY, ELG_FUNCTION},
    /* RT_LATEBROADCASTSHMEM */         {-1, -1, "LateBroadcast SHMEM", GT_SHMEMPATTERN, ELG_FUNCTION_COLL_ONE2ALL},
    /* RT_LATEBROADCASTSHMEM_SEVERITY */{-1, -1, "Severity(LateBroadcast SHMEM)", GT_SHMEMSEVERITY, ELG_FUNCTION},
    /* RT_LATESENDER */                 {-1, -1, "LateSender", GT_MPIP2PPATTERN, ELG_FUNCTION}, 
    /* RT_LATESENDER_SEVERTIY*/         {-1, -1, "Severity (LateSender)", GT_MPIP2PSEVERITY, ELG_FUNCTION},
    /* RT_LATESENDER_WO*/               {-1, -1, "LateSender Wrong Order", GT_MPIP2PPATTERN, ELG_FUNCTION}, 
    /* RT_LATESENDER_WO_SEVERTIY*/      {-1, -1, "Severity (LateSender Wrong Order)", GT_MPIP2PSEVERITY, ELG_FUNCTION},
    /* RT_LATERECEIVER */               {-1, -1, "LateReceiver", GT_MPIP2PPATTERN, ELG_FUNCTION}, 
    /* RT_LATERECEIVER_SEVERTIY*/       {-1, -1, "Severity (LateReceiver)", GT_MPIP2PSEVERITY, ELG_FUNCTION},
    /* RT_LATERECEIVER_WO */            {-1, -1, "LateReceiver Wrong Order", GT_MPIP2PPATTERN, ELG_FUNCTION}, 
    /* RT_LATERECEIVER_WO_SEVERTIY */   {-1, -1, "Severity (LateReceiver Wrong Order)", GT_MPIP2PSEVERITY, ELG_FUNCTION},
    /* RT_WAITATBARRIER */              {-1, -1, "WaitAtBarrier", GT_MPICOLLPATTERN, ELG_FUNCTION_COLL_BARRIER}, 
    /* RT_WAITATBARRIER_SEVERITY */     {-1, -1, "Severity (WaitAtBarrier)", GT_MPICOLLSEVERITY, ELG_FUNCTION},
    /* RT_WAITATN2N */                  {-1, -1, "WaitAtNxN", GT_MPICOLLPATTERN, ELG_FUNCTION_COLL_ALL2ALL}, 
    /* RT_WAITATN2N_SEVERITY */         {-1, -1, "Severity (WaitAtNxN)", GT_MPICOLLSEVERITY, ELG_FUNCTION},
    /* RT_WAITATFENCE */                {-1, -1, "WaitAtFence", GT_MPIRMAPATTERN, ELG_FUNCTION_COLL_BARRIER}, 
    /* RT_WAITATFENCE_SEVERITY */       {-1, -1, "Severity (WaitAtFence)", GT_MPIRMASEVERITY, ELG_FUNCTION},
    /* RT_WAITATWINCREATE */            {-1, -1, "WaitAtWinCreate", GT_MPIRMAPATTERN, ELG_FUNCTION_COLL_BARRIER},
    /* RT_WAITATWINCREATE_SEVERITY */   {-1, -1, "Severity (WaitAtWinCreate)", GT_MPIRMASEVERITY, ELG_FUNCTION},
    /* RT_WAITATWINFREE */              {-1, -1, "WaitAtWinFree", GT_MPIRMAPATTERN, ELG_FUNCTION_COLL_BARRIER},
    /* RT_WAITATWINFREE_SEVERITY */     {-1, -1, "Severity (WaitAtWinFree)", GT_MPIRMASEVERITY, ELG_FUNCTION},
    /* RT_WAITATEBARRIER */             {-1, -1, "WaitAtEBarrier", GT_OMPPATTERN, ELG_OMP_BARRIER},
    /* RT_WAITATEBARRIER_SEVERITY */    {-1, -1, "Severity (WaitAtEBarrier)", GT_OMPSEVERITY, ELG_FUNCTION},
    /* RT_WAITATIBARRIER */             {-1, -1, "WaitAtIBarrier", GT_OMPPATTERN, ELG_OMP_IBARRIER},
    /* RT_WAITATIBARRIER_SEVERITY */    {-1, -1, "Severity (WaitAtIBarrier)", GT_OMPSEVERITY, ELG_FUNCTION},
    /* RT_WAITATNXNSHMEM */             {-1, -1, "WaitAtNXN SHMEM", GT_SHMEMPATTERN, ELG_FUNCTION_COLL_ALL2ALL},
    /* RT_WAITATENXNSHMEM_SEVERITY */   {-1, -1, "Severity (WaitAtNXN SHMEM)", GT_SHMEMSEVERITY, ELG_FUNCTION},
    /* RT_WAITATBARRIERSHMEM */         {-1, -1, "WaitAtBarrier SHMEM", GT_SHMEMPATTERN, ELG_FUNCTION_COLL_BARRIER},
    /* RT_WAITATBARRIERSHMEM_SEVERITY */{-1, -1, "Severity (WaitAtBarrier SHMEM)", GT_SHMEMSEVERITY, ELG_FUNCTION},
    /* RT_LOCK_API_OMP */               {-1, -1, "Lock API OMP", GT_OMPPATTERN, ELG_FUNCTION},
    /* RT_LOCK_API_OMP_SEVERITY */      {-1, -1, "Severity (Lock API OMP)", GT_OMPSEVERITY, ELG_FUNCTION},
    /* RT_LOCK_API_OMP */               {-1, -1, "Lock Critical OMP", GT_OMPPATTERN, ELG_FUNCTION},
    /* RT_LOCK_API_OMP_SEVERITY */      {-1, -1, "Severity (Lock Critical OMP)", GT_OMPSEVERITY, ELG_FUNCTION}
    }
  ;

  PatternTrace* PatternTrace::_pt_ptr = 0;
