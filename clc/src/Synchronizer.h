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

#ifndef __SYNCHRONIZER_H__
#define __SYNCHRONIZER_H__

#include <mpi.h>

#include <string> 
#include <map>

#include <pearl.h> 
#include <Event.h>
#include <LocalTrace.h>
#include <CallbackData.h>
#include <CallbackManager.h>
#include <Machine.h>
#include <Node.h>
#include <Process.h>
#include <Thread.h>
#include <Clock.h>
#include <Network.h>
#include <Latency.h> 
#include <Location.h>
#include <RuntimeStat.h>
#include <AmortData.h>
#include <TimeStat.h>
#include <IntervalStat.h>

/**
 ** Scalable timestamp synchronization class for
 ** postmortem performance analysis.
 ** 
 ** Uses the Extended Controlled Logical Clock
 **
 **/
 
class Synchronizer
{
 public:

  // Constructors and Destructor
  Synchronizer(int rank, const pearl::LocalTrace& myTrace);
  Synchronizer(int rank, const pearl::LocalTrace& myTrace, TimeStat *t_stat, IntervalStat *i_stat);

  ~Synchronizer();
  
  // Used to synchronize event stream
  void synchronize();
  
  // Statistic functions
  void print_statistics();

 private:

  // Extended controllel logical clock
  Clock *m_clc;                         

  // Network the clock is running on
  Network *m_network;

  // Latency of the network
  Latency *m_latency;

  // Location of the clock
  clc_location m_loc;

  // Statistics
  RuntimeStat *r_stat;

  // Local event trace
  const pearl::LocalTrace& trace; 

  // First event's timestamp
  pearl::timestamp_t firstEvtT; 

  // Initial slope for the backward amortization
  double max_error;         
  pearl::timestamp_t m_max_slope;
  // Threshold for synchronization error at synchronization end
  double threshold;   

  // My Rank during the replay
  int m_rank;                                            
  
  // Number of logical send events
  long num_sends;

  // Local time statistic
  TimeStat *t_stat; 

  // Local interval statistic
  IntervalStat *i_stat; 

  // Respective callback manager
  pearl::CallbackManager*  cbmanager_fwd; 
  pearl::CallbackManager*  cbmanager_bkwd; 


  // Map with the violating receive events
  std::map<pearl::Event, pearl::timestamp_t> m_violations;

  // Timestamp array
  pearl::timestamp_t* tsa; 
 

  // Additional information
  clc_event_t cur_event_type;

  // Asynchronous message handling
  std::vector<amortization_data*> m_data;
  std::vector<MPI_Request>        m_requests;
  std::vector<int>                m_indices;
  std::vector<MPI_Status>         m_statuses;
  void update_requests();
  void delete_requests();

  // Callback functions for amortization
  void amortize_forward  (const pearl::CallbackManager& cbmanager, int user_event,
			  const pearl::Event& event, pearl::CallbackData* cdata);
  void amortize_backward (const pearl::CallbackManager& cbmanager, int user_event,
			  const pearl::Event& event, pearl::CallbackData* cdata);

  // Forward replay
  void fa_replay_send(const pearl::Event& sendEvt);
  amortization_data fa_replay_recv(const pearl::Event& recvEvt);

  // Backward replay
  void ba_replay(const pearl::Event& event);
  
  // Amortization functions for forward amortization
  void fa_amortize_recv  (const pearl::Event& event);
  void fa_amortize_intern(const pearl::Event& event);
  void fa_amortize_send  (const pearl::Event& event);

  // Amortization functions for backward amortization
  void ba_amortize();

  void ba_piecewise_interpolation(const pearl::Event& event_rhs,
				  pearl::timestamp_t time_rhs,
				  pearl::timestamp_t delta_rhs,
				  pearl::timestamp_t slope_g1,
				  long send_idx);

  void ba_linear_amortization_event_to_event (pearl::Event& event_lhs,
					      const pearl::Event& event_rhs, 
					      pearl::timestamp_t slope_g, 
					      pearl::timestamp_t c_g);

  void ba_linear_amortization_time_to_event (pearl::timestamp_t time_lhs,
					     const pearl::Event& event_rhs, 
					     pearl::timestamp_t slope_g, 
					     pearl::timestamp_t c_g);
  
  // Useful helper functions
  void init();
  void prepare_replay();
  void get_ts();
  void set_ts();

};

#endif /* END OF SYNCHRONIZER_H */
