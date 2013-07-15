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

#include "PatternMPI.h"

#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <iostream>

// earl
#include "earl.h"

// expert
#include "Analyzer.h"
#include "CallTree.h"
#include "Profile.h"
#include "StatisticController.h"

using namespace earl;
using namespace std;

/****************************************************************************
*
* MPI Patterns
*
****************************************************************************/

void init_patternv_mpi(std::vector<Pattern*>& pv, earl::EventTrace* trace)
{
  pv.push_back(new MPI(trace));
  pv.push_back(new MPICommunication(trace));
  pv.push_back(new MPIIO(trace));
  pv.push_back(new MPIInit(trace));
  pv.push_back(new MPISynchronization(trace));
  pv.push_back(new MPIBarrier(trace));
  pv.push_back(new MPIBarrierCompletion(trace));
  pv.push_back(new MPIWaitAtBarrier(trace));
  pv.push_back(new Collective(trace));
  pv.push_back(new EarlyReduce(trace));
  pv.push_back(new LateBroadcast(trace));
  pv.push_back(new WaitAtN2N(trace));
  pv.push_back(new PointToPoint(trace));
  pv.push_back(new LateReceiver(trace));
  pv.push_back(new LateSender(trace));
  pv.push_back(new MsgsWrongOrderLS(trace));
  pv.push_back(new MsgsWrongOrderLR(trace));
  
  pv.push_back(new MPIRMAComm(trace));
  pv.push_back(new MPIRMASync(trace));
  pv.push_back(new MPIRMAWindow(trace));
  pv.push_back(new MPIRMAFenceSynchronization(trace));
  pv.push_back(new MPIRMAGATSyncronization(trace));
  pv.push_back(new MPIRMALockSyncronization(trace));
  pv.push_back(new MPIRMAWaitAtFence(trace));
  pv.push_back(new MPIRMAWaitAtWinCreate(trace));
  pv.push_back(new MPIRMAWaitAtWinFree(trace));
  pv.push_back(new MPIRMAEarlyWait(trace));
  pv.push_back(new MPIRMALatePost(trace));
  pv.push_back(new MPIRMAEarlyTransfer(trace));
  pv.push_back(new MPIRMALateComplete(trace));
}

bool
MPI::isenabled()
{
  return is_mpi_appl(trace);
}

void
MPI::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_event(MPICEXIT, this);
  analyzer.subscribe_event(MPIWCEXIT,this);
  analyzer.subscribe_event(MPIWEXIT, this);
}

void
MPI::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      if ( is_mpi_api(ctree.get_callee(cnode_id)) )
        for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
          severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}

cmp_event*
MPI::mpicexit(earl::Event& event)

{
  vector<Event> coll;

  trace->mpicoll(coll, event.get_pos());

  // if collective operation complete
  if ( coll.size() > 0 )
    {
      mpi_coll* mc = new mpi_coll();

      mc->region = event.get_reg();
      mc->exitv  = coll;
      for ( size_t i = 0; i < mc->exitv.size(); i++ )
        {
          mc->enterv.push_back(mc->exitv[i].get_enterptr());
          if ( event.get_root() == mc->exitv[i].get_loc() )
            mc->root_idx = i;
        }
      return mc;
    }
  return NULL;
}

cmp_event*
MPI::rmawcexit(earl::Event& event)
{
  vector<Event> coll;

  trace->get_mpisyncccoll(coll, event.get_pos());
 
  // if collective operation complete
  if ( coll.size() > 0 )
    {
      rma_coll* rc  = new rma_coll();
      rc->region = event.get_reg();
      rc->exitv  = coll;
      for ( size_t i = 0; i < rc->exitv.size(); i++ )
        rc->enterv.push_back(rc->exitv[i].get_enterptr());
      return rc;
    } 
  return NULL;
}

cmp_event*
MPI::rmawexit(earl::Event& event)
{
 
  vector<CountedPtr<MPIEpoch> > completed_epoches;
  trace->get_mpiglobalepoches(completed_epoches, event.get_pos());

  if ( completed_epoches.empty() )
    return NULL;
    
  return new rma_glb_epoch(completed_epoches); 
}


/****************************************************************************
* Barrier Completion (MPI)
****************************************************************************/

bool
MPIBarrierCompletion::isenabled()
{
  return is_mpi_appl(trace);
}

void
MPIBarrierCompletion::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(MPI_COLL, this);
}

cmp_event*
MPIBarrierCompletion::cmpnd(const cmp_event* ce)
{
  const mpi_coll* mc = (const mpi_coll*) ce;

  Event enter = mc->enterv[0];
  if (is_mpi_barrier(enter.get_reg()))
    {
      vector<double> end_timev;

      for ( size_t i = 0; i < mc->exitv.size(); i++ )
        end_timev.push_back(mc->exitv[i].get_time());
      double min_end_time = *min_element(end_timev.begin(), end_timev.end());
      double wastedTime = 0.0;
      for ( size_t i = 0; i < mc->exitv.size(); i++ )
        {
          long loc_id      = mc->enterv[i].get_loc()->get_id();
          long cnode_id    = mc->enterv[i].get_cnodeptr().get_pos();
          double comp_time = end_timev[i] - min_end_time;

          severity.add_val(cnode_id, loc_id, comp_time);
          wastedTime += comp_time;
        }
        if(StatisticController::theController->GetWaitAtBarrierWritten().second != enter.get_pos())
        {
            cerr << "EXPERT: Internal Error in line " << __LINE__
                 << " of file" << __FILE__ 
                 << ": the last written WaitAtBarrierPosition doesnt match "
                 << "the BarrierCompletion's position" << endl;
        }
        if(StatisticController::theController->IsActive(get_unique_name(), wastedTime))
        {
          Event earliestEnter = *min_element(mc->enterv.begin(),
                                         mc->enterv.end(), CompareEventsByTime);
          Event latestExit = *max_element(mc->exitv.begin(),
                                          mc->exitv.end(), CompareEventsByTime);
          //Use the cnode_id of the root process for the statistics
          StatisticController::severePatternInstance spi(
              earliestEnter.get_time(), latestExit.get_time(), wastedTime,
              mc->enterv[0].get_cnodeptr().get_pos(),
              latestExit.get_time(), latestExit.get_loc()->get_id());
          StatisticController::theController->AddSeverity(EXP_PAT_MPI_BARRIER_COMPLETION,
                               get_unique_name(), spi);
          if(!StatisticController::theController->GetWaitAtBarrierWritten().first)
          {
            if(PatternTrace::_pt_ptr)
              PatternTrace::_pt_ptr->WriteMPIBarrierCompletionBegin(mc);
          }
          if(PatternTrace::_pt_ptr)
            PatternTrace::_pt_ptr->WriteMPIBarrierCompletion(mc, min_end_time);
        }
        else if(StatisticController::theController->GetWaitAtBarrierWritten().first)
        {
          if(PatternTrace::_pt_ptr)
            PatternTrace::_pt_ptr->WriteMPIWaitAtBarrierEnd(mc);
        }
    }
  return NULL;
}

/****************************************************************************
* MPI Point-to-Point
****************************************************************************/

bool
PointToPoint::isenabled()
{
  return is_mpi_appl(trace);
}

void
PointToPoint::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_event(RECV, this);
}

void
PointToPoint::end(Profile& profile, CallTree& ctree)
{
  vector<long> cnode_idv;

  ctree.get_keyv(cnode_idv);

  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      // if callee P2P function
      if ( is_mpi_p2p(ctree.get_callee(cnode_id)) )
        for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
          severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
    }
}

cmp_event*
PointToPoint::recv(earl::Event& event)
{
  recv_op* ro = new recv_op();

  ro->recv = event;
  ro->enter_recv = event.get_enterptr();
  ro->region = ro->enter_recv.get_reg();
  return ro;
}

/****************************************************************************
* MPI Late Sender
****************************************************************************/

bool
LateSender::isenabled()
{
  return is_mpi_appl(trace);
}

void
LateSender::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(RECV_OP, this);
}

cmp_event*
LateSender::cmpnd(const cmp_event* ce)
{
  const recv_op* ro = (const recv_op*) ce;

  if ( is_mpi_api(ro->region) &&
       ( ! ( is_mpi_testx(ro->region) ||
             is_mpi_waitall(ro->region) ||
             is_mpi_waitsome(ro->region) )) )
    {
      late_send* ls = new late_send(ro);

      ls->send = ls->recv.get_sendptr();
      ls->enter_send = ls->send.get_enterptr();
      ls->idle_time = ls->enter_send.get_time() - ls->enter_recv.get_time();
      if ( ls->idle_time > 0 )
        {
          long loc_id = ls->enter_recv.get_loc()->get_id();
          long cnode_id = ls->enter_recv.get_cnodeptr().get_pos();

          severity.add_val(cnode_id, loc_id, ls->idle_time);
          // WriteMPILateSender(ls) handled by WrongOrder
          return ls;
        }
      delete ls;
    }
  return NULL;
}

/****************************************************************************
* Messages in Wrong Order (Late Sender)
****************************************************************************/

bool
MsgsWrongOrderLS::isenabled()
{
  return is_mpi_appl(trace);
}

void
MsgsWrongOrderLS::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(LATE_SEND, this);
}

cmp_event*
MsgsWrongOrderLS::cmpnd(const cmp_event* ce)
{
  const late_send* ls = (const late_send*) ce;

  long pos    = ls->recv.get_pos();
  long loc_id = ls->recv.get_loc()->get_id();

  vector<Event> queuev;
  trace->queue(queuev, pos, -1, loc_id);
  double enterTime = ls->enter_recv.get_time();
  //FIXME: Exit time
  double receiveTime = ls->recv.get_time();
  if ( ! queuev.empty() && queuev[0] < ls->send )
    {
      double sendEnterTime = queuev[0].get_enterptr().get_time();
      if(sendEnterTime < enterTime)
      {
          enterTime = sendEnterTime;
      }
      long locid   = ls->enter_recv.get_loc()->get_id();
      long cnode_id = ls->enter_recv.get_cnodeptr().get_pos();

      severity.add_val(cnode_id, locid, ls->idle_time);
      const string Name = "mpi_latesender_wo";
      if(StatisticController::theController->IsActive(Name, ls->idle_time))
      {
        StatisticController::severePatternInstance spi(
            enterTime, receiveTime, ls->idle_time, cnode_id, sendEnterTime,
            queuev[0].get_enterptr().get_loc()->get_id());
        StatisticController::theController->AddSeverity(EXP_PAT_MPI_WRONG_ORDER_LS, Name, spi);
        if(PatternTrace::_pt_ptr)
          PatternTrace::_pt_ptr->WriteMPILateSenderWO(ls, queuev[0]->get_pos());
      }
    }
    else
    {
      const string Name = "mpi_latesender";
      if(StatisticController::theController->IsActive(Name, ls->idle_time))
      {
        StatisticController::severePatternInstance spi(
            enterTime, receiveTime, ls->idle_time, 
            ls->enter_recv.get_cnodeptr().get_pos(),
            ls->send->get_time(), ls->send->get_loc()->get_id());
        StatisticController::theController->AddSeverity(EXP_PAT_MPI_LATE_SENDER, Name, spi);
        if(PatternTrace::_pt_ptr) PatternTrace::_pt_ptr->WriteMPILateSender(ls);
      }
    }
  return NULL;
}

/****************************************************************************
* MPI Late Receiver
****************************************************************************/

bool
LateReceiver::isenabled()
{
  return is_mpi_appl(trace);
}

void
LateReceiver::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(RECV_OP, this);
}

cmp_event*
LateReceiver::cmpnd(const cmp_event* ce)
{
  const recv_op* ro = (const recv_op*) ce;

  if ( is_mpi_recv(ro->region) )
    {
      late_recv* lr = new late_recv(ro);

      lr->send = lr->recv.get_sendptr();
      lr->enter_send = lr->send.get_enterptr();
      if ( is_mpi_block_send(lr->enter_send.get_reg()) )
        {
          vector<Event> stackv;

          lr->idle_time = lr->enter_recv.get_time() - lr->enter_send.get_time();
          trace->stack(stackv, lr->enter_recv.get_pos(),
                       lr->enter_send.get_loc()->get_id());

          if ( lr->idle_time > 0 && find(stackv.begin(), stackv.end(),
                                         lr->enter_send) != stackv.end() )
            {
              long loc_id = lr->enter_send.get_loc()->get_id();
              long cnode_id = lr->enter_send.get_cnodeptr().get_pos();

              severity.add_val(cnode_id, loc_id, lr->idle_time);
              //WriteMPILateReceiver(lr) handled by Wrong order
              return lr;
            }
          delete lr;
        }
    }
  return NULL;
}

/****************************************************************************
* Messages in Wrong Order (Late Receiver)
****************************************************************************/

bool MsgsWrongOrderLR::isenabled()
{
  return is_mpi_appl(trace);
}

void
MsgsWrongOrderLR::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(LATE_RECV, this);
}

cmp_event*
MsgsWrongOrderLR::cmpnd(const cmp_event* ce)
{
  const late_recv* lr = (const late_recv*) ce;

  long pos    = lr->recv.get_pos();
  long src_id = lr->recv.get_src()->get_id();

  vector<Event> queuev;
  trace->queue(queuev, pos, src_id, -1);
  double receiveTime = lr->recv.get_time();
  if(! queuev.empty() && queuev[0] < lr->send)
    {
      double enterTime = queuev[0].get_enterptr().get_time();
      long loc_id   = lr->enter_send.get_loc()->get_id();
      long cnode_id = lr->enter_send.get_cnodeptr().get_pos();

      severity.add_val(cnode_id, loc_id, lr->idle_time);
      const string Name = "mpi_latereceiver_wo";
      if(StatisticController::theController->IsActive(Name, lr->idle_time))
      {
        StatisticController::severePatternInstance spi(
            enterTime, receiveTime, lr->idle_time, cnode_id,
            lr->enter_recv.get_time(), lr->enter_recv.get_loc()->get_id());
        StatisticController::theController->AddSeverity(EXP_PAT_MPI_WRONG_ORDER_LR,
                                             Name, spi);
        if(PatternTrace::_pt_ptr)
          PatternTrace::_pt_ptr->WriteMPILateReceiverWO(lr, queuev[0]->get_pos() );
      }
    }
    else
    {
      double enterTime = lr->enter_send.get_time();
      const string Name = "mpi_latereceiver";
      if(StatisticController::theController->IsActive(Name, lr->idle_time))
      {
        StatisticController::severePatternInstance spi(
            enterTime, receiveTime, lr->idle_time,
            lr->enter_send.get_cnodeptr().get_pos(),
            lr->enter_recv.get_time(), lr->enter_recv.get_loc()->get_id());
        StatisticController::theController->
          AddSeverity(EXP_PAT_MPI_LATE_RECEIVER, Name, spi);
        if (PatternTrace::_pt_ptr)
          PatternTrace::_pt_ptr->WriteMPILateReceiver(lr);
      }
    }
  return NULL;
}

/****************************************************************************
* Early Reduce
****************************************************************************/

bool
EarlyReduce::isenabled()
{
  return is_mpi_appl(trace);
}

void
EarlyReduce::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(MPI_COLL, this);
}

cmp_event*
EarlyReduce::cmpnd(const cmp_event* ce)
{
  const mpi_coll* mc = (const mpi_coll*) ce;

  Event enter = mc->enterv[0];
  if ( is_mpi_n21(enter.get_reg()) )
    {
      vector<double> sender_startv;

      Event root_enter = mc->enterv[mc->root_idx];
      for ( size_t i = 0; i < mc->enterv.size(); i++ )
      {
        if ( mc->enterv[i] != root_enter )
          sender_startv.push_back(mc->enterv[i].get_time());
      }
      double idle_time = *min_element(sender_startv.begin(),
                                   sender_startv.end()) - root_enter.get_time();
      if ( idle_time > 0 )
        {
          long loc_id   = root_enter.get_loc()->get_id();
          long cnode_id = root_enter.get_cnodeptr().get_pos();
          severity.add_val(cnode_id, loc_id, idle_time);
          if(StatisticController::theController->IsActive(get_unique_name(),
                                                          idle_time))
          {
            Event earliestEnter = *min_element(mc->enterv.begin(),
                                         mc->enterv.end(), CompareEventsByTime);
            Event latestExit = *max_element(mc->exitv.begin(), mc->exitv.end(),
                                         CompareEventsByTime);
            StatisticController::severePatternInstance spi(
                earliestEnter.get_time(), latestExit.get_time(), idle_time,
                cnode_id,
                root_enter.get_time(), root_enter.get_loc()->get_id());
            StatisticController::theController->AddSeverity(EXP_PAT_MPI_EARLY_REDUCE,
                                         get_unique_name(), spi);
            if (PatternTrace::_pt_ptr)
              PatternTrace::_pt_ptr->WriteMPIEarlyReduce(mc, idle_time);
          }
        }
    }
  return NULL;
}

/****************************************************************************
* Late Broadcast
****************************************************************************/

bool
LateBroadcast::isenabled()
{
  return is_mpi_appl(trace);
}

void
LateBroadcast::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(MPI_COLL, this);
}

cmp_event*
LateBroadcast::cmpnd(const cmp_event* ce)
{
  const mpi_coll* mc = (const mpi_coll*) ce;

  Event enter = mc->enterv[0];
  if ( is_mpi_12n(enter.get_reg()) )
    {
      double root_start = mc->enterv[mc->root_idx].get_time();
      double wastedTime = 0.0;
      for ( size_t i = 0; i < mc->enterv.size(); i++ )
        {
          double idle_time = root_start - mc->enterv[i].get_time();
          if ( idle_time > 0 )
            {
              long loc_id   = mc->enterv[i].get_loc()->get_id();
              long cnode_id = mc->enterv[i].get_cnodeptr().get_pos();

              severity.add_val(cnode_id, loc_id, idle_time);
              wastedTime += idle_time;
            }
        }
          const string Name =
                 LateBroadCastTraits<EXP_PAT_MPI_LATE_BCAST>::get_unique_name();
          if(StatisticController::theController->IsActive(Name, wastedTime))
          {
            Event earliestEnter = *min_element(mc->enterv.begin(),
                                         mc->enterv.end(), CompareEventsByTime);
            Event latestExit = *max_element(mc->exitv.begin(), mc->exitv.end(),
                                         CompareEventsByTime);
            //Use root process to abtain the cnode_id
            Event root_enter = mc->enterv[mc->root_idx];
            StatisticController::severePatternInstance spi(
                earliestEnter.get_time(), latestExit.get_time(), wastedTime, 
                root_enter.get_cnodeptr().get_pos(),
                root_start, root_enter.get_loc()->get_id());
            StatisticController::theController->AddSeverity(EXP_PAT_MPI_LATE_BCAST,
                                Name, spi);
            if (PatternTrace::_pt_ptr)
              WriteLateBroadCast<EXP_PAT_MPI_LATE_BCAST>(mc);
          }
    }
  return NULL;
}


/****************************************************************************
* MPI-2 RMA Early Wait
****************************************************************************/

bool
MPIRMAEarlyWait::isenabled()
{
  return is_mpi_rma_appl(trace);
}
 
void
MPIRMAEarlyWait::regcb(Analyzer& analyzer)
{

  analyzer.subscribe_cmpnd(RMA_GLB_EPOCH, this);
}

cmp_event*
MPIRMAEarlyWait::cmpnd(const cmp_event* ce)
{
  if (!ce)
    return NULL;
 
  const vector<CountedPtr<MPIEpoch> >& eps = ((const rma_glb_epoch*) ce)->completed_epoches;

  for (size_t i=0; i<eps.size(); i++)
    {
      const MPIEpoch& ep = *eps[i];
  
      if ( !ep.is_complete() )
        return NULL; 
    
      // get exposure epoch and related access epoches
      CountedPtr<MPILocEpoch> exp_ep =  ep.get_exp_epoch();
      vector<CountedPtr<MPILocEpoch> > acc_eps;
      ep.get_acc_epoches(acc_eps);
    
      // find time of MPI_Win_complete enter event
      double max_time = 0.0;
      CountedPtr<MPILocEpoch> acc_ep;
      for (size_t j=0; j < acc_eps.size(); j++ )
        {
          Event acc_end = acc_eps[j]->get_epoch_end();
          double time = acc_end.get_enterptr().get_time();
          if ( time > max_time) {
            max_time = time;
            acc_ep=acc_eps[j];
          }
        }
    
      Event enter = exp_ep->get_epoch_end().get_enterptr();
      double idle_time = max_time - enter.get_time();
     
      if ( idle_time > 0.0 )
        {
          long loc_id = enter.get_loc()->get_id();
          long cnode_id = enter.get_cnodeptr().get_pos();
          severity.add_val(cnode_id, loc_id, idle_time);
    
          if ( exp_ep ==  acc_ep )
            throw RuntimeError("MPIRMAEarlyWait::cmpnd(): invalid epoches");
          return new early_wait(idle_time, exp_ep, acc_ep);
        }
    }

  return NULL;
}

/****************************************************************************
* MPI-2 RMA Late Post 
****************************************************************************/

bool
MPIRMALatePost::isenabled()
{
  return is_mpi_rma_appl(trace);
}
 
void
MPIRMALatePost::regcb(Analyzer& analyzer)
{

  analyzer.subscribe_cmpnd(RMA_GLB_EPOCH, this);
}

cmp_event*
MPIRMALatePost::cmpnd(const cmp_event* ce)
{
  if (!ce)
    return NULL;
  
  const vector<CountedPtr<MPIEpoch> >& eps = ((const rma_glb_epoch*) ce)->completed_epoches;
  
  for (size_t i=0; i<eps.size(); i++)
    {
      const MPIEpoch& ep = *eps[i];
      
      if ( ep.is_complete() )
        {
    
          // get exposure epoch and related access epoches
          CountedPtr<MPILocEpoch> exp_ep =  ep.get_exp_epoch();
          vector<CountedPtr<MPILocEpoch> > acc_eps;
          ep.get_acc_epoches(acc_eps);
    
          const double enter_time = exp_ep->get_epoch_begin().get_enterptr().get_time();
          
          // find time of MPI_Win_complete enter event
          for (size_t j=0; j < acc_eps.size(); j++ )
            {
              Event exit  = acc_eps[j]->get_reg_exit(enter_time);
              if ( !exit.null() && exit.is_type(MPIWEXIT) )
                {
                  Event enter = exit.get_enterptr();
                  const double idle_time = enter_time - enter.get_time();
                  if ( idle_time > 0.0 )
                    {
                      long loc_id = enter.get_loc()->get_id();
                      long cnode_id = enter.get_cnodeptr().get_pos();
                      severity.add_val(cnode_id, loc_id, idle_time);
                    }
                }
            }
        }
    }
  return NULL;
}

/****************************************************************************
* MPI-2 RMA Early Transfer 
****************************************************************************/

bool
MPIRMAEarlyTransfer::isenabled()
{
  return is_mpi_rma_appl(trace);
}
 
void
MPIRMAEarlyTransfer::regcb(Analyzer& analyzer)
{

  analyzer.subscribe_cmpnd(RMA_GLB_EPOCH, this);
}

cmp_event*
MPIRMAEarlyTransfer::cmpnd(const cmp_event* ce)
{
  if (!ce)
    return NULL;
  
  const vector<CountedPtr<MPIEpoch> >& eps = ((const rma_glb_epoch*) ce)->completed_epoches;
  for (size_t i=0; i<eps.size(); i++)
    {
      const MPIEpoch& ep = *eps[i];
  
      if ( !ep.is_complete() ) 
        return NULL;

      // get exposure epoch and related access epoches
      CountedPtr<MPILocEpoch> exp_ep =  ep.get_exp_epoch();
      vector<CountedPtr<MPILocEpoch> > acc_eps;
      ep.get_acc_epoches(acc_eps);

      const double enter_time = exp_ep->get_epoch_begin().get_enterptr().get_time();
     
      // find time of MPI_Win_complete enter event
      for (size_t j=0; j < acc_eps.size(); j++ )
        {
          Event exit  = acc_eps[j]->get_reg_exit(enter_time);
          if ( !exit.null() && exit.get_type() == EXIT )
            {
              Event enter = exit.get_enterptr();
              const double idle_time = enter_time - enter.get_time();
              if ( idle_time > 0.0 )
                {
                  long loc_id = enter.get_loc()->get_id();
                  long cnode_id = enter.get_cnodeptr().get_pos();
                  severity.add_val(cnode_id, loc_id, idle_time);
                }
            }
        }
    }
  return NULL;
}

/****************************************************************************
* MPI-2 RMA Late Complete 
****************************************************************************/

bool
MPIRMALateComplete::isenabled()
{
  return is_mpi_rma_appl(trace);
}

void
MPIRMALateComplete::regcb(Analyzer& analyzer)
{
  analyzer.subscribe_cmpnd(EARLY_WAIT, this);
}

cmp_event*
MPIRMALateComplete::cmpnd(const cmp_event* ce)
{
  // get epoches  
  const early_wait* ew = (const early_wait*) ce;
  const CountedPtr<MPILocEpoch>& acc_ep = ew->acc_ep;
  const CountedPtr<MPILocEpoch>& exp_ep = ew->exp_ep;

  // get events  
  Event tr_exit  = acc_ep->get_last_transfer();
  Event cmpl_enter = acc_ep->get_epoch_end().get_enterptr();
  if ( tr_exit.null() )
    return NULL;
  
  // calculate idle_time 
  double idle_time = cmpl_enter.get_time() - tr_exit.get_time();
  idle_time =  min( idle_time, ew->idle_time);

  if ( idle_time > 0.0 ) 
    { 
      // save severity 
      long loc_id   = exp_ep->get_loc()->get_id();
      long cnode_id = exp_ep->get_epoch_end().get_enterptr().get_cnodeptr().get_pos();
      severity.add_val(cnode_id, loc_id, idle_time);
    }

  return NULL;
}




/****************************************************************************
*
* Auxiliary functions
*
****************************************************************************/

bool is_mpi_appl(const earl::EventTrace* trace)
{
  return trace->get_nprocs() > 1;
}

bool is_mpi_api(const earl::Region* region)
{
  if ( region->get_name().length() < 4 )
    return false;

  string prefix(region->get_name(), 0, 3);

  return lower(prefix) == "mpi";
}

bool is_mpi_barrier(const earl::Region* region)
{
  if ( region->get_name().length() < 11 )
    return false;

  string prefix(region->get_name(), 0, 11);

  return lower(prefix) == "mpi_barrier";
}

bool is_mpi_sync(const earl::Region* region)
{
  return is_mpi_barrier(region) || is_rma_sync(region);
}

bool is_mpi_block_send(const earl::Region* region)
{
   return lower(region->get_name()) == "mpi_send" ||  lower(region->get_name()) == "mpi_ssend";
}

bool is_mpi_comm(const earl::Region* region)
{
   return is_mpi_collcom(region) || is_rma_comm(region) || is_mpi_p2p(region);
}

bool is_mpi_collcom(const earl::Region* region)
{
  if ( ! is_mpi_api(region) )
    return false;

  string suffix = lower(region->get_name().substr(3));

  if ( suffix.find("_all") != string::npos ||
       suffix.find("_op") != string::npos ||
       suffix.find("bcast") != string::npos ||
       suffix.find("gather") != string::npos ||
       suffix.find("reduce") != string::npos ||
       suffix.find("scan") != string::npos ||
       suffix.find("scatter") != string::npos )
    return true;
  else
    return false;
}

bool is_mpi_io(const earl::Region* region)
{
  if ( region->get_name().length() < 8 )
    return false;

  string prefix(region->get_name(), 0, 8);

  return lower(prefix) == "mpi_file";
}

bool is_mpi_init(const earl::Region* region)
{
   if ( ! is_mpi_api(region) )
    return false;

  string suffix = lower(region->get_name().substr(4));
  return suffix == "init" || suffix == "init_thread" || suffix == "finalize";
}

bool is_mpi_12n(const earl::Region* region)
{
   if ( ! is_mpi_api(region) )
    return false;

  string suffix = lower(region->get_name().substr(4));

  if ( suffix == "bcast" ||
       suffix == "scatter" ||
       suffix == "scatterv" )
    return true;
  else
    return false;
}

bool is_mpi_n21(const earl::Region* region)
{
  if ( ! is_mpi_api(region) )
    return false;

  string suffix = lower(region->get_name().substr(4));

  if ( suffix == "gather" ||
       suffix == "gatherv" ||
       suffix == "reduce" )
    return true;
  else
    return false;
}

bool is_mpi_n2n(const earl::Region* region)
{
  if ( ! is_mpi_api(region) )
    return false;

  string suffix = lower(region->get_name().substr(3));

  if ( suffix.find("_all") != string::npos || suffix == "_reduce_scatter" )
    return true;
  else
    return false;
}


bool is_mpi_p2p(const earl::Region* region)
{
  if ( ! is_mpi_api(region) )
    return false;

  if ( is_mpi_win_api(region) )  // MPI_Win_wait MPI_Win_start
    return false;

  string suffix = lower(region->get_name().substr(4));

  if ( suffix.find("buffer") != string::npos ||
       suffix.find("cancel") != string::npos ||
       suffix.find("get_count") != string::npos ||
       suffix.find("probe") != string::npos ||
       suffix.find("recv") != string::npos ||
       suffix.find("request") != string::npos ||
       suffix.find("send") != string::npos ||
       suffix.find("start") != string::npos ||
       suffix.compare(0, 4, "test") == 0 ||
       suffix.find("wait") != string::npos 
       )
    return true;
  else
    return false;
}

bool is_mpi_recv(const earl::Region* region)
{
  return lower(region->get_name()) == "mpi_recv";
}

bool is_mpi_rma_appl(const earl::EventTrace* trace)
{
  return trace->get_nwins() > 0;
}

bool is_mpi_win_api(const earl::Region* region)
{
  if ( region->get_name().length() < 8 )
    return false;

  string prefix(region->get_name(), 0, 7);

  return lower(prefix) == "mpi_win";
}


bool is_mpi_testx(const earl::Region* region)
{
  if ( region->get_name().length() < 8 )
    return false;

  string prefix(region->get_name(), 0, 8);

  return prefix == "mpi_test";
}

bool is_mpi_waitall(const earl::Region* region)
{
  if ( region->get_name().length() != 11 )
    return false;

  return lower(region->get_name()) == "mpi_waitall";
}

bool is_mpi_waitsome(const earl::Region* region)
{
  if ( region->get_name().length() != 12 )
    return false;

  return lower(region->get_name()) == "mpi_waitsome";
}

/****************************************************************************
* MPI-2 RMA
****************************************************************************/

bool is_rma_comm(const earl::Region* region)
{
  string name = lower(region->get_name());
  return name == "mpi_get" || name == "mpi_put";
}

bool is_rma_sync(const earl::Region* r)
{
  return is_rma_fence(r) || is_rma_lock(r) ||
         is_rma_window_management(r) || is_rma_GATSyncronization(r);
}

bool is_rma_fence(const earl::Region* region)
{
  if (!region)
    throw RuntimeError("is_rma_fence(): Invalid input");
  string name = lower(region->get_name());
  return name=="mpi_win_fence";
}

bool is_rma_lock(const earl::Region* region)
{
  string name = lower(region->get_name());
  return name == "mpi_win_lock" || name == "mpi_win_unlock";
}

bool is_rma_window_management(const earl::Region* region)
{
  string name = lower(region->get_name());
  return name=="mpi_win_create" || name=="mpi_win_free";
}

bool is_rma_win_create(const earl::Region* region)
{
  return lower(region->get_name()) =="mpi_win_create";
}

bool is_rma_win_free(const earl::Region* region)
{
  return lower(region->get_name()) =="mpi_win_free";
}

bool is_rma_GATSyncronization(const earl::Region* region)
{
  string prefix(region->get_name(), 0, 7);
  
  //check MPI_Win prefix
  if (lower(prefix) != "mpi_win")
    return false;

  //check suffix
  try {
    string suffix(region->get_name().substr(8));
    return suffix=="post" || suffix=="start" || suffix=="complete" ||
           suffix=="wait" || suffix=="test";
  } catch (out_of_range&){}
  return false;
}

