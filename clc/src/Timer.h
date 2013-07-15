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

#ifndef __TIMER_H__
#define __TIMER_H__

#include <string>
#include <map>

#include <pearl.h>
#include <Event.h>
#include <LocalTrace.h>
#include <CallbackData.h>
#include <CallbackManager.h>
#include <clc_types.h>
#include <AmortData.h>
#include <Network.h>
#include <Latency.h>

class Timer
{
 public:

  // Constructors and Destructor
  Timer(int r, const pearl::LocalTrace& myTrace);
    
  ~Timer();
  
  // Used to sanity check the event stream
  void check();
  
  // Statistic functions
  void print_statistics();

 private:

  // Each Timer has a trace
  const pearl::LocalTrace& trace;                        // Local event trace

  // Network the clock is running on
  Network *network;

  // Latency of the network
  Latency *latency;
  
  // Callback manager
  pearl::CallbackManager*  cbmanager_fwd;                // Associated callback manager
  
  // Some statistics
  int direct_violation;
  int clock_violation;
  int m_messages;
  int m_p2p_violation;
  int m_coll_violation;
  int m_omp_violation;
  pearl::timestamp_t m_backward_error;
  pearl::timestamp_t m_sum_error;

  // Additional information
  clc_location loc;
  int m_rank;                                            // My Rank during the replay

  // Additional information
  clc_event_t cur_event_type;

  // Callback functions for amortization
  void check_trace  (const pearl::CallbackManager& cbmanager, int user_event,
		     const pearl::Event& event, pearl::CallbackData* cdata);

  // Forward replay
  void replay_send(const pearl::Event& sendEvt);
  amortization_data replay_recv(const pearl::Event& recvEvt);

  // Data exchange functions for logical events
  void exchange_recv(const pearl::Event& event);
  void exchange_send(const pearl::Event& event);

  // Useful helper functions
  void init();
		     
};

#endif /* END OF TIMER_H */
