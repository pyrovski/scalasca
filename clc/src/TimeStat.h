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


#ifndef __TIMESTAT_H
#define __TIMESTAT_H

#include <pearl.h>
#include <StatData.h>
#include <pearl_types.h>
#include <LocalTrace.h>

/*
 *---------------------------------------------------------------------------
 *
 * class TimeStat
 *
 *---------------------------------------------------------------------------
 */

class TimeStat
{
 public:
  /* Constructors & destructor */
  TimeStat(const pearl::LocalTrace& trace, int rank, int tid);

  ~TimeStat();

  // Get methods
  void get_org_timestamps();
  void get_fa_timestamps();
  void get_ba_timestamps();

  // Print methods
  void print_timestamps();

  void calculate();
  
 private:
  
  // Data vector
  std::vector<statistic_data> data_vec;
  
  // Local event trace
  const pearl::LocalTrace& trace; 

  // My rank and thread id
  int m_rank;
  int m_tid;

  // Timestamp buffers
  pearl::timestamp_t* org_ts; 
  pearl::timestamp_t* ba_ts; 
  pearl::timestamp_t* fa_ts;
  pearl::timestamp_t* tsa;

  // Number of events
  long                num_events; 
  pearl::timestamp_t  t_first_org;
  pearl::timestamp_t  t_first_sync;
  pearl::timestamp_t  t_diff_org;
  pearl::timestamp_t  t_diff_sync;
  pearl::timestamp_t  t_sum_time;
  pearl::timestamp_t  abs_sum;
  pearl::timestamp_t  rel_sum;
  
  // Calculate functions
  void calculate_data();
  void calculate_relative_errors();
  void calculate_absolute_errors();

  // Helper function
  void prepare_calculation(pearl::timestamp_t* t_array);
  
};

#endif   /* !__TIMESTAT_H */
