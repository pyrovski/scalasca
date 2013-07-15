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
#endif

#include <pearl_replay.h>
#include <MpiComm.h>
#include <MpiGroup.h>
#include <Operator.h>
#include <elg_error.h>
#include <Predicates.h>
#include <Synchronizer.h>
#include <OmpCommunication.h>
#include <Region.h>

using namespace std;
using namespace pearl;

Synchronizer::Synchronizer(int r, const pearl::LocalTrace& myTrace) 
  : trace(myTrace), firstEvtT(-DBL_MAX), max_error(0.01), m_max_slope(0.01), threshold(0.05),
    m_rank(r), num_sends(0), t_stat(NULL), i_stat(NULL) {
  init();

}
 
Synchronizer::Synchronizer(int r, const pearl::LocalTrace& myTrace, TimeStat *time_stat, IntervalStat *interval_stat) 
  : trace(myTrace), firstEvtT(-DBL_MAX), max_error(0.01), m_max_slope(0.01), threshold(0.05),
    m_rank(r), num_sends(0), t_stat(time_stat), i_stat(interval_stat) {
  
  init();

}
 
Synchronizer::~Synchronizer() {
  delete m_clc;
  delete r_stat;
  delete cbmanager_fwd;
  delete cbmanager_bkwd;
  delete []tsa;

  delete_requests();
  m_violations.clear();
#pragma omp master
  free_operators();
}

void Synchronizer::init() {
  
  // Map clocks to global zero
  timestamp_t  global_zero = DBL_MAX;
  timestamp_t  local_zero  = trace.begin()->get_time();
#pragma omp master
  MPI_Allreduce(&local_zero, &global_zero, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
#ifdef _OPENMP
  OMP_Bcast(global_zero, omp_global_min_timestamp);
  global_zero = omp_global_min_timestamp;
#endif

  LocalTrace::iterator it = trace.begin();
  for (it = trace.begin(); it != trace.end(); it++) {
    it->set_time(it->get_time() - global_zero);
  }
 
  // Get location
  Location *loc = trace.get_location();
  m_loc.machine     = loc->get_machine()->get_id();
  m_loc.node        = loc->get_node()->get_id();
  m_loc.process     = loc->get_process()->get_id();
  m_loc.thread      = loc->get_thread()->get_id();
 	
  // Create Latency
  m_latency = new Latency(0.0, 1.0e-6, 1.0e-6, 1.0e-6, 1.0e-6, 0.01e-6);

  // Create Network with Latency
  m_network = new Network(m_loc, m_latency);

  // Create clock
  m_clc     = new Clock(*m_network);

  // Create runtime statistics
  r_stat    = new RuntimeStat(1);

  // Create TimeStampArray (TSA)
  tsa         = NULL;
  get_ts();
 
  // Create callback manager
  cbmanager_fwd  = new CallbackManager();
  cbmanager_bkwd = new CallbackManager();
  
  // Register callbacks
  cbmanager_fwd->register_callback (ANY, PEARL_create_callback(this, &Synchronizer::amortize_forward ));
  cbmanager_bkwd->register_callback(ANY, PEARL_create_callback(this, &Synchronizer::amortize_backward));

  // Initialize operator and datatypes
#pragma omp master
  init_operators();
}

void Synchronizer::prepare_replay() {

  // Next pass
  r_stat->set_pass();

  // Reset clock
  m_clc->reset();

  // Apply controller to obtain controll variable for next pass
  m_clc->apply_controller(r_stat->get_pass());

  // Clear clock violation list
  m_violations.clear();
  
  // Reset number of send events for subsequent replay
  num_sends = 0;
  
  // Restore timestamps for subsequent replay
  if (r_stat->get_pass() != 1) set_ts();

}

/**
 ** Used to synchronize event stream
 **/

void Synchronizer::synchronize() {

  timestamp_t gl_error = -DBL_MAX;
  elg_cntl_msg("[CLC]: Forward amortization... ");
  
  if(t_stat != NULL && i_stat != NULL) {
    t_stat->get_org_timestamps();
    i_stat->get_org_timestamps();
  }

  // Perform the replay
  do {
    
    // Prepare Synchronizer for subsequent replay
    prepare_replay();

    // Replay the target application and synchronize timestamps...
#pragma omp barrier
    PEARL_forward_replay(trace, *cbmanager_fwd, NULL);
#pragma omp barrier

    // Until accuracy is sufficient
    gl_error    = r_stat->get_clock_error();
  } while (gl_error > threshold && r_stat->next_pass());

  // If clock condition violations occured -> apply backward amortization
  timestamp_t gl_violations = 0.0;
  timestamp_t violations    = (timestamp_t) m_clc->get_num_viol();
#ifdef _OPENMP
  OMP_Allreduce_sum(violations, omp_global_sum_timestamp);
  violations = omp_global_sum_timestamp;
#endif

#pragma omp master
  MPI_Allreduce(&violations, &gl_violations, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

#ifdef _OPENMP
  OMP_Bcast(gl_violations, omp_global_sum_timestamp);
  gl_violations = omp_global_sum_timestamp;
#endif

  if(t_stat != NULL && i_stat != NULL) {
    t_stat->get_fa_timestamps();
    i_stat->get_fa_timestamps();
  }

  if (gl_violations > 0.0) {
    elg_cntl_msg("[CLC]: Backward amortization... ");
#pragma omp barrier
    ba_amortize();
#pragma omp barrier
  } else {
    elg_cntl_msg("[CLC]: Skipping backward amortization... ");
  }
  
  if(t_stat != NULL && i_stat != NULL) {
    t_stat->get_ba_timestamps();
    i_stat->get_ba_timestamps();
  }
}

/**
 ** Callback function for timestamp amortization
 **/

void Synchronizer::amortize_forward(const CallbackManager& cbmanager,
				    int user_event, const Event& event, CallbackData* cdata) {
  switch(event->get_type())
    {
    case MPI_SEND:
    case MPI_SEND_REQUEST:
      fa_amortize_send(event); 
      break;
     
    case MPI_RECV:
    case MPI_RECV_COMPLETE:
      fa_amortize_recv(event);
      break;

    case MPI_COLLEXIT:
      if(isRecv(event, m_rank)) { 
	/* Amortize for each receiving collective exit event */
	fa_amortize_recv(event); 
      } else if(isSend(event.enterptr(), m_rank)) {
	/* Amortize missing collective exit events: all send only events */
	fa_amortize_send(event); 
      } else {
	fa_amortize_intern(event);
      }
      break;

#ifdef _OPENMP
    case OMP_FORK:
      fa_amortize_send(event); 
      break;
      
    case OMP_JOIN:
      fa_amortize_recv(event);
      break;
      
    case OMP_COLLEXIT:
      if (isRecv(event, m_rank))
	fa_amortize_recv(event);
      else if (isParallel(event))
	fa_amortize_send(event);
      else
	fa_amortize_intern(event);
      break;

    case ENTER:
      if (isParallel(event))
	fa_amortize_recv(event);
      else
	fa_amortize_intern(event);
      break;
#endif
    default:
      fa_amortize_intern(event);  /* local correction for all other events*/
      break;
  }
 }


/**
 ** Send event replay for forward amortization.
 ** Sends send-event time to the send-event's destination process.
 **/

void Synchronizer::fa_replay_send(const Event& sendEvt) {

  // Pack local data
  amortization_data local_object = pack_data(m_loc, sendEvt->get_time());

  if(sendEvt->is_typeof(MPI_SEND)) {

    // Send send-event timestamp
    MPI_Request  request;
    amortization_data *local = new amortization_data;
    *local = pack_data(m_loc, sendEvt->get_time());

    MPI_Isend(local, 1, AMORT_DATA, sendEvt->get_dest(),
	      sendEvt->get_tag(), sendEvt->get_comm()->get_comm(), &request);
    m_requests.push_back(request);
    m_data.push_back(local);
    update_requests();

  } else if(isNx1(sendEvt)) {

    // Send max ( all enter event timestamps )
    amortization_data ex_object;

    // If no data sent
    if (sendEvt.exitptr()->get_sent() == 0)
      local_object.timestamp = -DBL_MAX;

    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX,
	       sendEvt.exitptr()->get_root(),
	       sendEvt.exitptr()->get_comm()->get_comm());

  } else if(is1xN(sendEvt)) {
    
    // If no data sent
    if (sendEvt.exitptr()->get_sent() == 0)
      local_object.timestamp = -DBL_MAX;

    // Send root's send event timestamp
    MPI_Bcast(&local_object, 1, AMORT_DATA, 
	      sendEvt.exitptr()->get_root(),
	      sendEvt.exitptr()->get_comm()->get_comm());
    
  } else {

    cerr << "Non-send event passed to Synchronizer::fa_replay_send!"
         << endl
         << sendEvt << endl;
    exit(11);

  }
}

/**
 ** Receive event replay for forward amortization.
 ** Receives send-event time from the receive-event's source process,
 ** or gets equivalent timestamp for a collective operation.
 */

amortization_data Synchronizer::fa_replay_recv(const Event& recvEvt) {

  // Temporary buffer
  amortization_data ex_object;

  if(recvEvt->is_typeof(MPI_RECV)) {

    // Receive send event time 
    MPI_Status stat;
    MPI_Recv(&ex_object, 1, AMORT_DATA, recvEvt->get_source(),
	     recvEvt->get_tag(), recvEvt->get_comm()->get_comm(), &stat);
    cur_event_type = CLC_P2P;

  } else if (recvEvt->get_type() == OMP_JOIN) {

    // Receive latest send event timestamp
#ifdef _OPENMP
    ex_object = pack_data(m_loc, omp_global_max_timestamp);
    cur_event_type = CLC_OMP;
#endif
  } else if( recvEvt->get_type() == ENTER && isParallel(recvEvt)) {

    // Receive latest send event timestamp
#ifdef _OPENMP
#pragma omp barrier
    ex_object = pack_data(m_loc, omp_global_max_timestamp);
    cur_event_type = CLC_OMP;
#endif
  } else if( recvEvt->get_type() == OMP_COLLEXIT && isNxN(recvEvt)) {

    // Receive latest send event timestamp
#ifdef _OPENMP
    OMP_Allreduce_max(recvEvt.enterptr()->get_time(), omp_global_max_timestamp);
    ex_object = pack_data(m_loc, omp_global_max_timestamp);
    cur_event_type = CLC_OMP;
#endif
  } else if(isNxN(recvEvt)) {

    // Pack local data
    amortization_data local_object = pack_data(m_loc, recvEvt.enterptr()->get_time());

    // If no data sent
    string name = lowercase(recvEvt.enterptr()->get_region()->get_name());
    if (recvEvt->get_sent() == 0 && name != "mpi_barrier")
      local_object.timestamp = -DBL_MAX;
    
    // Receive max( all enter event timestamps )
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA, 
		  CLC_MAX, recvEvt->get_comm()->get_comm());
    
    // If no data received
    if (recvEvt->get_received() == 0 && name != "mpi_barrier")
      ex_object.timestamp = -DBL_MAX;

    cur_event_type = CLC_COLL;

  } else if(isNx1(recvEvt.enterptr())) {

    // Pack local data
    amortization_data local_object = pack_data(m_loc, recvEvt.enterptr()->get_time());

    // If no data sent
    if (recvEvt->get_sent() == 0)
      local_object.timestamp = -DBL_MAX;
    
    // Receive max ( all enter event times + latency )
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX,
	       recvEvt->get_root(),
    	       recvEvt->get_comm()->get_comm());
    cur_event_type = CLC_COLL;

    // If no data received
    if (recvEvt->get_received() == 0)
      ex_object.timestamp = -DBL_MAX;

  } else if(is1xN(recvEvt.enterptr())) {

    // Pack local data
    ex_object = pack_data(m_loc, recvEvt.enterptr()->get_time());

    // If no data sent
    if (recvEvt->get_sent() == 0)
      ex_object.timestamp = -DBL_MAX;
    
    // Receive latest send event timestamp
    MPI_Bcast(&ex_object, 1, AMORT_DATA, 
	      recvEvt->get_root(),
	      recvEvt->get_comm()->get_comm());

    cur_event_type = CLC_COLL;

    // If no data received
    if (recvEvt->get_received() == 0)
      ex_object.timestamp = -DBL_MAX;

  } else if (is_mpi_scan(recvEvt.enterptr()) || is_mpi_exscan(recvEvt.enterptr())) {

    // Pack local data
    amortization_data local_object = pack_data(m_loc, recvEvt.enterptr()->get_time());

    // If no data sent
    if (recvEvt->get_sent() == 0)
      local_object.timestamp = -DBL_MAX;

    // Receive latest send event timestamp
    MPI_Scan(&local_object, &ex_object, 1, AMORT_DATA, CLC_MAX, recvEvt->get_comm()->get_comm());

    // If no data received
    if (recvEvt->get_received() == 0)
      ex_object.timestamp = -DBL_MAX;

    cur_event_type = CLC_COLL;

  }

  return ex_object;
}

/**
 ** Amortization for receive events. Including both replays.
 */

void Synchronizer::fa_amortize_recv(const Event& event) {
  
  // Timestamp after internal forward amortization
  timestamp_t internT = 0.0;

  // Timestamp before any amortization
  timestamp_t curT = event->get_time();

  // Get corresponding timestamp, location, and event type via replay
  clc_location ex_loc;
  amortization_data ex_object = fa_replay_recv(event);
  timestamp_t sendEvtT = unpack_data(ex_loc, ex_object);

  // Amortize forward
  if(firstEvtT == -DBL_MAX) {
    // Store first event timestamp for use in backwards amortization
    firstEvtT = curT;
    
    // Set initial clock value
    m_clc->set_value(firstEvtT - m_clc->get_delta());
    
    // Set initial previous event 
    m_clc->set_prev(firstEvtT - m_clc->get_delta());
  } 
  
  internT = m_clc->amortize_forward_recv(curT, sendEvtT, ex_loc, cur_event_type);
  
  // Set new timestamp
  event->set_time(m_clc->get_value());
  
  // If violation detected store event
  if (internT < m_clc->get_value()) {
    m_violations[event] = internT;
  } 
  // Adjust statistics
   r_stat->set_statistics(curT, m_clc->get_value());
}

/**
 ** Amortization for internal events. Only Controlled logical clock is locally applied.
 */

void Synchronizer::fa_amortize_intern(const Event& event) {

  // Timestamp before any amortization
  timestamp_t curT = event->get_time();
  if(firstEvtT == -DBL_MAX) {

      // Store first event timestamp for use in backwards amortization
      firstEvtT = curT;

      // Set initial clock value
      m_clc->set_value(curT);

      // Set initial previous event 
      m_clc->set_prev(curT);

  } else {

      // Amortize timestamp internally
      m_clc->amortize_forward_intern(curT);
  
  }

  // Set new timestamp
  event->set_time(m_clc->get_value());

  // Adjust statistics and timestamp
  r_stat->set_statistics(curT, m_clc->get_value());

}

/**
 ** Amortization for send events. 
 */

void Synchronizer::fa_amortize_send(const Event& event) {
  if(event->is_typeof(MPI_SEND)) {
    
    // Amortize timestamp internally
    fa_amortize_intern(event);

    // Exchange send event timestamp
    fa_replay_send(event);

  } else if (event->get_type() == MPI_COLLEXIT) {

    // Amortize timestamp internally
    fa_amortize_intern(event);

    // Exchange send event timestamp
    fa_replay_send(event.enterptr());

  } else if (event->get_type() == OMP_FORK) {

    // Amortize timestamp internally
    fa_amortize_intern(event);

    // Exchange send event timestamp
#ifdef _OPENMP
    omp_global_max_timestamp = event->get_time();
#endif
  } else if( event->get_type() == OMP_COLLEXIT && isParallel(event)) {

    // Amortize timestamp internally
    fa_amortize_intern(event);

    // Exchange send event timestamp
#ifdef _OPENMP
    OMP_Allreduce_max(event->get_time(), omp_global_max_timestamp);
#endif

  } else {

    cerr << "Non-send event passed to Synchronizer::amortize_send!"
         << endl
         << event << endl;
    exit(11);

  }
}

/**
 ** Callback function for timestamp amortization
 **/

void Synchronizer::amortize_backward(const CallbackManager& cbmanager,
				     int user_event, const Event& event, CallbackData* cdata) {
  switch(event->get_type())
    {
    case MPI_SEND:
    case MPI_SEND_REQUEST:
    case MPI_RECV:
    case MPI_RECV_COMPLETE:
    case MPI_COLLEXIT:
    case OMP_FORK:
    case OMP_JOIN:
    case OMP_COLLEXIT:
      ba_replay(event);
      break;
    default:
      break;
    }
}

/*
 *  Backward replay handling.
 *
 */

void Synchronizer::ba_replay(const Event& event) {

  // Pack local data
  amortization_data local_object = pack_data(m_loc, event->get_time());

  // Temporary buffer
  amortization_data ex_object;
  clc_location ex_loc;
  timestamp_t ex_time;

  if(event->is_typeof(MPI_RECV)) {

    // Send receive-event time 
    MPI_Request request;
    amortization_data *local = new amortization_data;
    *local = pack_data(m_loc, event->get_time());

    MPI_Isend(local, 1, AMORT_DATA, event->get_source(),
	      event->get_tag(), event->get_comm()->get_comm(), &request);
    m_requests.push_back(request);
    m_data.push_back(local);
    update_requests();
    
  } else if(event->is_typeof(MPI_SEND)) {

    // Receive receive-event timestamp
    MPI_Status stat;
    MPI_Recv(&ex_object, 1, AMORT_DATA, event->get_dest(),
	     event->get_tag(), event->get_comm()->get_comm(), &stat); 
    
    // Unpack data received
    ex_time = unpack_data(ex_loc, ex_object);

    // Adjust data for local backward amortization
    tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_P2P);
    num_sends++;
  } else if(event->get_type() == OMP_JOIN) {
    
    // Send receive-event time - latency
#ifdef _OPENMP
    omp_global_max_timestamp = event->get_time() - m_network->get_latency(ex_loc, CLC_OMP);
#endif
  } else if(event->get_type() == OMP_FORK) {
    
    // Send receive-event time - latency
    // Adjust data for local backward amortization
#ifdef _OPENMP
    tsa[num_sends] = omp_global_max_timestamp - m_network->get_latency(ex_loc, CLC_OMP);
    num_sends++;
#endif
  } else if(event->get_type() == OMP_COLLEXIT) {
    if (isParallel(event)) {

    // Adjust data for local backward amortization
#ifdef _OPENMP
#pragma omp barrier
      tsa[num_sends] = omp_global_max_timestamp - m_network->get_latency(ex_loc, CLC_OMP);
      num_sends++;
#endif
    } else if(isRecv(event, m_rank)) {
      
      // Receive and send earliest receive event timestamp
#ifdef _OPENMP
      OMP_Allreduce_min(event->get_time(), omp_global_min_timestamp);
#endif      
      // Adjust data for local backward amortization
#ifdef _OPENMP
      tsa[num_sends] = omp_global_min_timestamp - m_network->get_latency(ex_loc, CLC_OMP);
      num_sends++;
#endif
    }
  } else if(isNxN(event)) {

    // If no data received
    string name = lowercase(event.enterptr()->get_region()->get_name());
    if (event->get_received() == 0 && name != "mpi_barrier")
      local_object.timestamp = DBL_MAX;
    
    // Send/recv min (all exit event times)
    MPI_Allreduce(&local_object, &ex_object, 1, AMORT_DATA,
		  CLC_MIN, event->get_comm()->get_comm());
    
    // Unpack data received
    ex_time = unpack_data(ex_loc, ex_object);
    
    // Adjust data for local backward amortization
    tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

    // If no data sent
    if (event->get_sent() == 0 && name != "mpi_barrier")
      tsa[num_sends] = DBL_MAX;

    num_sends++;
    
  } else if(is1xN(event.enterptr())) {

    // If no data transfered
    if (event->get_received() == 0)
      local_object.timestamp = DBL_MAX;
    
    // Send/recv root exit time 
    MPI_Reduce(&local_object, &ex_object, 1, AMORT_DATA, CLC_MIN, 
	       event->get_root(),
	       event->get_comm()->get_comm());

    // Adjust data for local backward amortization (only on root)
    if(isOnRoot(event, m_rank)) {
      ex_time        = unpack_data(ex_loc, ex_object);
      tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

      if (event->get_sent() == 0)
	tsa[num_sends] = DBL_MAX;

      num_sends++;
    }

  } else if(isNx1(event.enterptr())) {

    // If no data received
    if (event->get_received() == 0)
      local_object.timestamp = DBL_MAX;

    // Send/recv min (all exit event times)
    MPI_Bcast(&local_object, 1, AMORT_DATA, 
	      event->get_root(),
	      event->get_comm()->get_comm());

    // Adjust data for local backward amortization
    if (!isOnRoot(event, m_rank)) {
      ex_time        = unpack_data(ex_loc, local_object);
      tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

      // If no data sent
      if (event->get_sent() == 0)
        tsa[num_sends] = DBL_MAX;

      num_sends++;
    }

  } else if(is_mpi_scan(event.enterptr()) || is_mpi_exscan(event.enterptr())) {

    MPI_Comm reversed_comm;
    
    MPI_Comm_split (event->get_comm()->get_comm(), 42, 
		    (event->get_comm()->get_group()->num_ranks() - event->get_comm()->get_group()->get_local_rank(event.get_location()->get_process()->get_id()) -1),
		    &reversed_comm);

    // If no data received
    if (event->get_received() == 0)
      local_object.timestamp = DBL_MAX;
    
    // Receive earliest receive event timestamp
    MPI_Scan(&local_object, &ex_object, 1, AMORT_DATA, CLC_MIN, reversed_comm);
    MPI_Comm_free(&reversed_comm);

    // Unpack data received
    ex_time = unpack_data(ex_loc, ex_object);

    // Adjust data for local backward amortization
    tsa[num_sends] = ex_time - m_network->get_latency(ex_loc, CLC_COLL);

    // If no data sent
    if (event->get_sent() == 0)
      tsa[num_sends] = DBL_MAX;

    num_sends++;

  }
}

/**
 ** Store original timestamps in buffer.
 */

void Synchronizer::get_ts(){
  long i = 0;
  uint32_t num =  trace.num_events ();
  tsa = new timestamp_t [num];

  LocalTrace::iterator it = trace.begin();

  while (it != trace.end()) {
    tsa[i] = it->get_time();
    i++;
    it++;
  }
}

/**
 ** Restore original timestamps stored by get_ts() before.
 */

void Synchronizer::set_ts(){
  int i = 0;
  LocalTrace::iterator it = trace.begin();

  while (it != trace.end()) {
    it->set_time(tsa[i]);
    i++;
    it++;
  }
}

/**
 ** Print some fancy runtime statistics :-).
 */

void Synchronizer::print_statistics() {
  
  timestamp_t gl_corr       = r_stat->get_num_clock_corr();
  
  timestamp_t gl_max_error  = r_stat->get_max_clock_error();
    
  timestamp_t gl_error      = r_stat->get_clock_error();
  
  timestamp_t gl_num        = 0.0;
  timestamp_t num           = (timestamp_t) trace.num_events ();
  
  timestamp_t gl_violations = 0.0;
  timestamp_t violations    = (timestamp_t) m_clc->get_num_viol();

  timestamp_t gl_max_slope     = 0.0;

  int sum_p2p_violation     = 0;
  int p2p_violation         = m_clc->get_p2p_viol();
  int sum_coll_violation    = 0;
  int coll_violation        = m_clc->get_coll_viol();
  int sum_omp_violation     = 0;
  int omp_violation         =m_clc->get_omp_viol();


#ifdef _OPENMP
  OMP_Allreduce_sum(num, omp_global_sum_timestamp);
  num = omp_global_sum_timestamp;
  OMP_Allreduce_sum(violations, omp_global_sum_timestamp);
  violations = omp_global_sum_timestamp;
  OMP_Allreduce_max(m_max_slope, omp_global_max_timestamp);
  m_max_slope = omp_global_max_timestamp;
  OMP_Allreduce_sum(p2p_violation, omp_global_sum_count);
  p2p_violation = omp_global_sum_count;
  OMP_Allreduce_sum(coll_violation, omp_global_sum_count);
  coll_violation = omp_global_sum_count;
  OMP_Allreduce_sum(omp_violation, omp_global_sum_count);
  omp_violation = omp_global_sum_count;
#endif

#pragma omp master
  {
    MPI_Allreduce(&num, &gl_num, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
 
    MPI_Allreduce(&violations, &gl_violations, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    
    MPI_Allreduce(&m_max_slope, &gl_max_slope, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    MPI_Allreduce(&p2p_violation, &sum_p2p_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    
    MPI_Allreduce(&coll_violation, &sum_coll_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
 
    MPI_Allreduce(&omp_violation, &sum_omp_violation, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  
    if (isclcRoot(m_loc)) {
      printf("\n\t# passes        : %d\n", r_stat->get_pass());
      printf("\t# violated      : %.0f\n", gl_violations);
      if (gl_violations > 0.0) {
        printf("\t# corrected     : %.0f\n", gl_corr);
        printf("\t# reversed-p2p  : %d\n", sum_p2p_violation);
        printf("\t# reversed-coll : %d\n", sum_coll_violation);
        printf("\t# reversed-omp  : %d\n", sum_omp_violation);
        printf("\t# events        : %.0f\n", gl_num);
        printf("\tmax. error      : %.6f [s]\n", gl_max_error);
        printf("\terror at final. : %.6f [%%] \n", gl_error*100);
        printf("\tMax slope       : %.9f  \n", gl_max_slope);
      }
    }
  } 
}

/**
 ** Applies backward amortization
 ** First : Replay
 ** Second: Local Correction
 **/

void Synchronizer::ba_amortize() {

  LocalTrace::iterator it = trace.begin();
  map<Event,timestamp_t>::iterator iter;

  // Perform backward replay to obtain (earliest) receive timestamp
  PEARL_backward_replay(trace, *cbmanager_bkwd, NULL);

  // Apply local correction
  // Index of next send event
  int mySend = num_sends - 1;

  // Traverse the trace from begin to end. 
  while (it != trace.end()) {
    // For every receive event R encountered along the way with d_r > 0 
    iter = m_violations.find(it);
    if (iter != m_violations.end()) {

      // Calculate delta_r
      timestamp_t       recv_s    = iter->first->get_time();
      timestamp_t       recv_o    = iter->second;
      timestamp_t       delta_r   = recv_s - recv_o;
      
      // Start the piece-wise linear amortization
      // with Er := R = iter->first
      //      tr := r* = r - d_r = recv_o
      //      delta_er := delta_r
      //      slope m := const = max_error = 0.01
      ba_piecewise_interpolation(iter->first, recv_o, delta_r, max_error, mySend);
    }

    if (it->is_typeof(MPI_SEND)                            ||
 	(it->get_type() == OMP_FORK)                       ||
 	(it->get_type() == OMP_COLLEXIT && isParallel(it)) ||
 	(it->get_type() == ENTER && isSend(it, m_rank))) {
      mySend--;
    }
    ++it;
  }
}

/**
 ** Backward amortization for receive events.
 ** Computes an interval, L_A, and amortizes events on this interval
 ** using a piecewise linear interpolation function 
 **/

void Synchronizer::ba_piecewise_interpolation (const Event& event_rhs,
					       timestamp_t time_rhs,
					       timestamp_t delta_rhs,
					       timestamp_t slope_g1,
					       long send_idx) {

  // Index of the preceding send event 
  // Used to obtain correcponding receive event timestamp in timestamp array
  int prec_send_idx  = send_idx + 1;

  // Left corner tl := tr - (delta_rhs/m). 
  // Calculate left corner of the amortization interval 
  timestamp_t m_left = time_rhs - (delta_rhs / slope_g1);

  // If tl < begin of the trace e_b then tl := e_b and m := delta_rhs/(tr - e_b).
  // with tr := r* = r - d_r = time_rhs  
  // Set or (if left_t < firstEvtT) calculate m_slope_g1
  timestamp_t m_slope = slope_g1;

  if (m_left < firstEvtT) {
    m_left  = firstEvtT;
    m_slope = delta_rhs / (time_rhs - m_left);
  }

  // Linear function g1(t) := m * t - (m * tl) = m_slope * t + m_const
  // Calculate offset of linear equation
  timestamp_t m_const = -1.0 * m_slope * m_left;

  // Search for the send event Si in [tl,Er) 
  // with slope mi := (delta_rhs - delta_i)/(tr - si) maximal 
  // and mi > m.

  // Up til now we have not found such a send event
  bool found_send = false;

  // Declare individuelle offsets and slopes at distinct send events
  timestamp_t delta_i, slope_i;

  // Temporary variables to store max. values during the search phase
  Event       event_send = event_rhs;
  timestamp_t slope_send = 0.0;
  timestamp_t delta_send = 0.0;
  timestamp_t const_send = 0.0;
  int         index_send = 0; 

  for (Event it = event_rhs.prev(); it.is_valid() && m_left < it->get_time() ; --it) {
    // Send event found?
    if(it->is_typeof(MPI_SEND)                           ||
      (it->get_type() == OMP_FORK)                       ||
      (it->get_type() == OMP_COLLEXIT && isParallel(it)) ||
      (it->get_type() == ENTER && isSend(it, m_rank))) {
      
      // Determine offset between send and receive (-l_min)
      delta_i = tsa[prec_send_idx] - it->get_time();
      //if (it->get_time() > tsa[prec_send_idx] ) 
      // Calculate individuelle slope
      slope_i = (delta_rhs - delta_i)/(time_rhs - it->get_time());
      
      // If individual slope is larger than initial slope
      if (slope_i > m_slope) {

	// Found conflicting send event
	found_send = true;
	// Store new maximum slope
	m_slope    = slope_i;
	
	// Store event data of send event
	event_send = it;
	delta_send = delta_i;
	slope_send = slope_i;
	const_send = delta_send - (slope_send * event_send->get_time());
	index_send = prec_send_idx;
      } 
      // Increment pointer to preceding send
      prec_send_idx++;
    }
  }
  
  // If there is no such Si, apply the linear amortization to the interval [tl,Er) 
  // and continue the forward traversal 

  if (!found_send) {

    ba_linear_amortization_time_to_event(m_left, event_rhs , m_slope, m_const);

  } else {
    // Apply the linear amortization to the interval [si,Er) using gi
    ba_linear_amortization_event_to_event(event_send, event_rhs, slope_send, const_send);
 
    // If delta_i > 0 start the piece-wise linear amortization again 
    // with Er := Si = event_send
    // tr := si
    // delta_rhs := delat_s
    // m = delta_i/(si - tl).
    time_rhs   = event_send->get_time() - delta_send;
    slope_send = delta_send / (time_rhs - m_left);
    if (delta_send > 0.0) 
      ba_piecewise_interpolation(event_send, time_rhs, delta_send, slope_send, index_send);
  }
}

void Synchronizer::ba_linear_amortization_event_to_event(Event& event_lhs, const Event& event_rhs, 
							 timestamp_t slope_g, timestamp_t c_g)
{
  for(Event it = event_rhs.prev(); it.is_valid() && event_lhs.prev() != it ; --it) {
    it->set_time( it->get_time() + (slope_g * it->get_time()) + c_g);
  }
  if (m_max_slope < slope_g) m_max_slope = slope_g;
}

void Synchronizer::ba_linear_amortization_time_to_event(timestamp_t time_lhs, const Event& event_rhs, 
							timestamp_t slope_g, timestamp_t c_g)
{
  for(Event it = event_rhs.prev(); it.is_valid() && time_lhs < it->get_time() ; --it) {
    it->set_time(it->get_time() + (slope_g * it->get_time()) + c_g);
  }
}

void Synchronizer::update_requests() {

  // No pending requests? ==> continue
  if (m_requests.empty())
    return;
  
  // Check for completed messages
  int completed;
  int count = m_requests.size();
  m_indices.resize(count);
  m_statuses.resize(count);

  MPI_Testsome(count, &m_requests[0], &completed, &m_indices[0], &m_statuses[0]);

  m_requests.erase(remove(m_requests.begin(), m_requests.end(),
			  static_cast<MPI_Request>(MPI_REQUEST_NULL)),
		   m_requests.end());

  // Update array of messages
  for (int i = 0; i < completed; ++i) {

    int index = m_indices[i];

    delete m_data[index];
    m_data[index] = NULL;
  }
  m_data.erase(remove(m_data.begin(), m_data.end(),
		      static_cast<amortization_data*>(NULL)),
	       m_data.end());

}

void Synchronizer::delete_requests() {
  
  MPI_Status status;

  update_requests();

  // Handle remaining messages
  if (!m_requests.empty()) {
    int count = m_requests.size();

    // Print warning
    elg_warning("Encountered %d unreceived send operations!", count);

    // Cancel pending messages
    for (int i = 0; i < count; ++i) {
      MPI_Cancel(&m_requests[i]);
      MPI_Wait(&m_requests[i], &status);
      delete m_data[i];
    }

    m_requests.clear();
    m_data.clear();
  }
}
