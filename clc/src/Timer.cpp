/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <mpi.h>
#ifdef _OPENMP
#  include <omp.h>
#  include <OmpData.h>
#  include <OmpCommunication.h>
#endif

#include <pearl.h>
#include <pearl_replay.h>
#include <Event.h>
#include <MpiComm.h>
#include <MpiGroup.h>
#include <MpiMessage.h>
#include <Location.h>
#include <Machine.h>
#include <Node.h>
#include <Process.h>
#include <Thread.h>
#include <Region.h>
#include <Operator.h>
#include <elg_error.h>
#include <Predicates.h>
#include <AmortData.h>

#include <Timer.h>

using namespace std;
using namespace pearl;

Timer::Timer(int r, const pearl::LocalTrace& myTrace) 
  : trace(myTrace), m_rank(r) {

  init();

}
  
Timer::~Timer() {

  delete cbmanager_fwd;
#pragma omp master
  free_operators ();

}

void Timer::init() {

  //Set up statistics
  direct_violation = 0;
  clock_violation  = 0;
  m_backward_error = 0.0;
  m_sum_error      = 0.0;
  m_messages       = 0;
  m_p2p_violation  = 0;
  m_coll_violation = 0;
  m_omp_violation  = 0;

  //Retrieve node id from trace
  Location *m_loc = trace.get_location();
  loc.machine     = m_loc->get_machine()->get_id();
  loc.node        = m_loc->get_node()->get_id();
  loc.process     = m_loc->get_process()->get_id();
  loc.thread      = m_loc->get_thread()->get_id();

  // Create Latency (default)
  latency = new Latency(0.0, 1.0e-6, 3.0e-6, 1.0e-6, 3.0e-6, 0.1e-6);
  // MareNostrum
  // latency = new Latency(0.0, 1.3e-6, 7.7e-6, 1.3e-6, 7.7e-6, 0.1e-6);
  // Jaguar
  // latency = new Latency(0.0, 0.6e-6, 8.6e-6, 0.6e-6, 8.6e-6, 0.1e-6);
  // Nicole
  // latency = new Latency(0.0, 1.5e-6, 4.5e-6, 1.5e-6, 4.5e-6, 0.1e-6);
  // NGS
  // latency = new Latency(1300.0e-6, 1.5e-6, 4.5e-6, 1.5e-6, 4.5e-6, 0.1e-6);
  
  // Create Network with Latency
  network = new Network(loc, latency);

  // Create callback manager
  cbmanager_fwd   = new CallbackManager();
  
  // Register callbacks
  cbmanager_fwd->register_callback(ANY, PEARL_create_callback(this, &Timer::check_trace));

  // Initialize operator and datatypes
#pragma omp master
  init_operators();

}

void Timer::check(){

  string msg    = "Checking the event stream... ";
  elg_cntl_msg(msg.c_str());
  
  // Replay the target application and check timestamps...
  PEARL_forward_replay(trace, *cbmanager_fwd, NULL);

}

void Timer::check_trace(const CallbackManager& cbmanager,
			int user_event, const Event& event, CallbackData* cdata){

  switch(event->get_type())
  {
    case MPI_SEND:
    case MPI_SEND_REQUEST:
      exchange_send(event); 
      break;

    case MPI_RECV:
    case MPI_RECV_COMPLETE:
      exchange_recv(event);
      break;

    case MPI_COLLEXIT:
      if(isRecv(event, m_rank)) 
      {
        exchange_recv(event); 
      }
      else if(isSend(event.enterptr(), m_rank))
      {
	exchange_send(event); 
      }
      break;

#ifdef _OPENMP
    case OMP_FORK:
      exchange_send(event); 
      break;
      
    case OMP_JOIN:
      exchange_recv(event);
      break;
      
    case OMP_COLLEXIT:
      if (isRecv(event, m_rank))
	exchange_recv(event);
      if (isParallel(event))
	exchange_send(event);
      break;

    case ENTER:
      if (isParallel(event))
	exchange_recv(event);
      break;
#endif

    default:
      break;
  }
}

void Timer::replay_send(const Event& sendEvt) {

  amortization_data local_object = pack_data(loc, sendEvt->get_time());
  
  if(sendEvt->is_typeof(MPI_SEND)) {

    // Send send-event timestamp
    MPI_Send(&local_object, 1, AMORT_DATA, sendEvt->get_dest(),
	     sendEvt->get_tag(), sendEvt->get_comm()->get_comm());
    
  } else if (sendEvt->get_type() == OMP_FORK) {
#ifdef _OPENMP
    // Exchange send event timestamp
    omp_global_max_timestamp = sendEvt->get_time();
    m_messages+=omp_get_num_threads();
#endif  
  } else if(sendEvt->get_type() == OMP_COLLEXIT && isParallel(sendEvt)) {
#ifdef _OPENMP
    // Exchange send event timestamp
    OMP_Allreduce_max(sendEvt->get_time(), omp_global_max_timestamp);
#endif
  } else if(isNx1(sendEvt)) {
   
    // Send max ( all enter event timestamps )
    amortization_data ex_object;
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX, 
	       sendEvt.exitptr()->get_root(),
	       sendEvt.exitptr()->get_comm()->get_comm());

  } else if(is1xN(sendEvt)) {

      MPI_Bcast(&local_object, 1, AMORT_DATA, 
		sendEvt.exitptr()->get_root(),
		sendEvt.exitptr()->get_comm()->get_comm());

  } else {
    
    cerr << "Non-send event passed to Timer::replay_send!"
         << endl
         << sendEvt << endl;
    exit(11);

  }
}

amortization_data Timer::replay_recv(const Event& recvEvt) {
  
  amortization_data ex_object;

  if(recvEvt->is_typeof(MPI_RECV)) {
      // Receive send event time 
      MPI_Status m_stat;
      MPI_Recv(&ex_object, 1, AMORT_DATA, recvEvt->get_source(),
               recvEvt->get_tag(), recvEvt->get_comm()->get_comm(), &m_stat);
      cur_event_type = CLC_P2P;
      m_messages++;

  } else if (recvEvt->get_type() == OMP_JOIN) {
#ifdef _OPENMP
    // Receive latest send event timestamp
    ex_object = pack_data(loc, omp_global_max_timestamp);
    cur_event_type = CLC_OMP;
    m_messages+=omp_get_num_threads();
#endif    
  } else if( recvEvt->get_type() == ENTER && isParallel(recvEvt)) {
#ifdef _OPENMP    
    // Receive latest send event timestamp
#pragma omp barrier
    ex_object = pack_data(loc, omp_global_max_timestamp);
    cur_event_type = CLC_OMP;
    
  } else if( recvEvt->get_type() == OMP_COLLEXIT && isNxN(recvEvt)) {
    
    // Receive latest send event timestamp
    OMP_Allreduce_max(recvEvt.enterptr()->get_time(), omp_global_max_timestamp);
    ex_object = pack_data(loc, omp_global_max_timestamp);
    cur_event_type = CLC_OMP;
    m_messages+=omp_get_num_threads();
#endif
  } else if(isNxN(recvEvt)) {

    // Pack local data
    amortization_data local_object = pack_data(loc, recvEvt.enterptr()->get_time());

    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA, 
		  CLC_MAX, recvEvt->get_comm()->get_comm());
    
    cur_event_type = CLC_COLL;
    m_messages+=recvEvt->get_comm()->get_group()->num_ranks();
    
  } else if(isNx1(recvEvt.enterptr())) {

    // Pack local data
    amortization_data local_object = pack_data(loc, recvEvt.enterptr()->get_time());
    
    // Receive max ( all enter event times + latency )
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX,
	       recvEvt->get_root(),
	       recvEvt->get_comm()->get_comm());
    cur_event_type = CLC_COLL;
    m_messages+=recvEvt->get_comm()->get_group()->num_ranks();
  } else if(is1xN(recvEvt.enterptr())) {
    
    ex_object = pack_data(loc, recvEvt.enterptr()->get_time());
    MPI_Bcast(&ex_object, 1, AMORT_DATA, 
	      recvEvt->get_root(),
	      recvEvt->get_comm()->get_comm());
    cur_event_type = CLC_COLL;
    m_messages++;
  } else {
    
    cerr << "Non-receive event passed to Timer::replay_recv!"
	 << endl
	 << recvEvt << endl;
    exit(11);
  }

  return ex_object;
}

void Timer::exchange_recv(const Event& event) {
  
  // Receive event timestamp
  timestamp_t recvEvtT = event->get_time();

  // Get corresponding timestamp, location, and event type via replay
  clc_location ex_loc;
  amortization_data ex_object = replay_recv(event);
  timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);

  if (recvEvtT < sendEvtT) {
    direct_violation++;
    m_backward_error  = max (m_backward_error, (sendEvtT - recvEvtT));
    m_sum_error      += (sendEvtT - recvEvtT);
    
    if (cur_event_type == CLC_P2P) {
      m_p2p_violation++;
    } else if (cur_event_type == CLC_COLL) {
      m_coll_violation++;
    } else if (cur_event_type == CLC_OMP) {
      m_omp_violation++;
    }
  }
  
  sendEvtT += network->get_latency(ex_loc, cur_event_type);
  if (recvEvtT < sendEvtT) {
    clock_violation++;

  }
}

void Timer::exchange_send(const Event& event) {

  if(event->is_typeof(MPI_SEND)) {

    replay_send(event);

  } else if (event->get_type() == MPI_COLLEXIT) {

    replay_send(event.enterptr());

  } else if (event->get_type() == OMP_FORK) {

    replay_send(event);
    
  } else if(event->get_type() == OMP_COLLEXIT && isParallel(event)) {

    replay_send(event);
    
  } else {

    cerr << "Non-send event passed to Timer::exchange_send!"
         << endl
         << event << endl;
    exit(11);

  }
}

void Timer::print_statistics() {

  int sum_direct_violation = 0;
#ifdef _OPENMP
  OMP_Allreduce_sum(direct_violation, omp_global_sum_count);
  direct_violation = omp_global_sum_count;
#endif
#pragma omp master
  MPI_Allreduce(&direct_violation, &sum_direct_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  int sum_clock_violation = 0;
#ifdef _OPENMP
  OMP_Allreduce_sum(clock_violation, omp_global_sum_count);
  clock_violation = omp_global_sum_count;
#endif
#pragma omp master
  MPI_Allreduce(&clock_violation, &sum_clock_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  int sum_messages = 0;
#ifdef _OPENMP
  OMP_Allreduce_sum(m_messages, omp_global_sum_count);
  m_messages = omp_global_sum_count;
#endif
#pragma omp master
  MPI_Allreduce(&m_messages, &sum_messages, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  timestamp_t max_error = 0.0;
#ifdef _OPENMP
  OMP_Allreduce_max(m_backward_error, omp_global_max_timestamp);
  m_backward_error = omp_global_max_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&m_backward_error, &max_error, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

  timestamp_t sum_error = 0.0;
#ifdef _OPENMP
  OMP_Allreduce_max(m_sum_error, omp_global_max_timestamp);
  m_sum_error = omp_global_max_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&m_sum_error, &sum_error, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

  int sum_p2p_violation = 0;
#ifdef _OPENMP
  OMP_Allreduce_sum(m_p2p_violation, omp_global_sum_count);
  m_p2p_violation = omp_global_sum_count;
#endif
#pragma omp master
  MPI_Allreduce(&m_p2p_violation, &sum_p2p_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  int sum_coll_violation = 0;
#ifdef _OPENMP
  OMP_Allreduce_sum(m_coll_violation, omp_global_sum_count);
  m_coll_violation = omp_global_sum_count;
#endif
#pragma omp master
  MPI_Allreduce(&m_coll_violation, &sum_coll_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  int sum_omp_violation = 0;
#ifdef _OPENMP
  OMP_Allreduce_sum(m_omp_violation, omp_global_sum_count);
  m_omp_violation = omp_global_sum_count;
#endif
#pragma omp master
  MPI_Allreduce(&m_omp_violation, &sum_omp_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

 {
#pragma omp barrier
 }

#pragma omp master
  {
    if (m_rank == 0) printf("\n\t# messages       : %d\n", sum_messages);
    if (m_rank == 0) printf("\t# reversed       : %d\n", sum_direct_violation);
    if (m_rank == 0) printf("\t# reversed-p2p   : %d\n", sum_p2p_violation);
    if (m_rank == 0) printf("\t# reversed-coll  : %d\n", sum_coll_violation);
    if (m_rank == 0) printf("\t# reversed-omp   : %d\n", sum_omp_violation);
    if (m_rank == 0) printf("\t# reversed-mu    : %d\n", sum_clock_violation);
    if (m_rank == 0) printf("\t# max. error [s] : %.9f\n", max_error);
    if (m_rank == 0 && sum_direct_violation != 0) printf("\t# avg. error [s]: %.9f\n", (sum_error/sum_direct_violation));
  }

}


