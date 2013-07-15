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

#ifndef __CLOCK_H__
#define __CLOCK_H__


#include <cfloat>

#include <pearl_types.h>
#include <Network.h>
#include <Controller.h>
#include <clc_types.h>

class Clock
{
  public:
  // Constructors
  Clock(Network &network);
  ~Clock();

  // Amortize methods
  pearl::timestamp_t amortize_forward_intern(const pearl::timestamp_t curEvtT); 
  pearl::timestamp_t amortize_forward_recv  (const pearl::timestamp_t curEvtT,
					     const pearl::timestamp_t sendEvtT,
					     clc_location &clc_loc,
					     clc_event_t  event_type);

  // Set new controll variable accordingly to respective pass
  void   apply_controller(int pass);

  // Set methods for members
  void   set_value(pearl::timestamp_t v) { value = v; }
  void   set_delta(pearl::timestamp_t d) { delta = d; }
  void   set_prev(pearl::timestamp_t d)  { prevEvtT  = d; }

  // Get methods for members
  pearl::timestamp_t get_delta() { return delta; }
  pearl::timestamp_t get_value() { return value; }
  long get_num_viol()  { return num_viol; }
  int  get_p2p_viol()  { return m_p2p_violation; }
  int  get_coll_viol() { return m_coll_violation; }
  int  get_omp_viol()  { return m_omp_violation; }

  // Reset Clock for new pass
  void   reset();

 protected:
  // The current value of this clock
  pearl::timestamp_t value;
  // The minimum time between events
  pearl::timestamp_t delta;

  // Event preceding the actual event in the event stream
  pearl::timestamp_t prevEvtT;               
 
 private:
  // Controller
  Controller *controller;

  // Network a clock is running on
  Network *network;
  
  // Number of violations
  long  num_viol;
  int m_p2p_violation;
  int m_coll_violation;
  int m_omp_violation;

  // Internal helper method
  void is_violation(const pearl::timestamp_t curEvtT, 
		    const pearl::timestamp_t sendEvtT, 
		    clc_event_t  event_type);

};

#endif

