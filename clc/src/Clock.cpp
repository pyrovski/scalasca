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

/*
 *---------------------------------------------------------------------------
 *
 * class Clock
 *
 *---------------------------------------------------------------------------
 */

#include <algorithm>
#include <Clock.h>
#include <cmath>

using namespace pearl;
using namespace std;

Clock::Clock(Network &net): value(-DBL_MAX), delta(1.0e-9), prevEvtT(0.5*DBL_MAX), 
			    num_viol(0), m_p2p_violation(0), m_coll_violation(0), m_omp_violation(0) {

  controller = new Controller();
  network    = new Network();
  *network    = net;

}

Clock::~Clock() {

  delete network;

}

/**
 ** Query control variable
 **/
void Clock::apply_controller(int pass) { 

  controller->set_gamma(pass); 

}

/**
 ** Resets the clock
 **/
void Clock::reset() {

  num_viol    = 0; 
  prevEvtT    = DBL_MAX;

}

/**
 ** Amortizes clock forward as in equation (3).
 ** Returns timestamp of internal event after amortization.
 **/
timestamp_t Clock::amortize_forward_intern(const timestamp_t curEvtT) {

  // Calculate simple controlled logical clock value
  value = max( max( value + delta, value + (controller->get_gamma() * (curEvtT - prevEvtT))), curEvtT);

  // Store preceding event for further processing
  prevEvtT = curEvtT;
  
  return value;

}

/**
 ** Amortizes clock forward as in equation (4).
 ** Returns timestamp of -->local<-- amortization,
 ** that is, the timestamp of receive event after amortization,
 ** but before clock condidion violation is corrected.
 */
timestamp_t Clock::amortize_forward_recv  (const timestamp_t curEvtT,
					   const timestamp_t sendEvtT,
					   clc_location &clc_loc,
					   clc_event_t  event_type) {
  // Retrieve correct latency
  timestamp_t lat = network->get_latency(clc_loc, event_type);

  // Apply internal amortization
  timestamp_t internT = amortize_forward_intern(curEvtT);

  // Apply clock correction
  value = max(sendEvtT + lat, internT);

  // Clock condition violation?
  is_violation(internT, sendEvtT+lat, event_type);
   
  return internT;

}

/**
 ** Compares receive and send event timestamp
 ** Returns true in case of a clock condition violation
 ** Increments internal violation counter
 **/
void Clock::is_violation(const timestamp_t curEvtT, const timestamp_t sendEvtT, clc_event_t  event_type) {

  if (curEvtT < sendEvtT) {
    num_viol++;
    
    if (event_type == CLC_P2P) {
      m_p2p_violation++;
    } else if (event_type == CLC_COLL) {
      m_coll_violation++;
    } else if (event_type == CLC_OMP) {
      m_omp_violation++;
    }
  }
}
