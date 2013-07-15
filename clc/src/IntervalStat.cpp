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


#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cctype>
#include <cfloat>

#include <mpi.h>
#ifdef _OPENMP
#  include <omp.h>
#  include <OmpData.h>
#  include <OmpCommunication.h>
#endif

#include <IntervalStat.h>
#include <Operator.h>


using namespace std;
using namespace pearl;
/*
 *---------------------------------------------------------------------------
 *
 * class IntervalStat
 *
 *---------------------------------------------------------------------------
 */

IntervalStat::IntervalStat(const pearl::LocalTrace& myTrace, int r, int t): trace(myTrace), m_rank(r), m_tid(t) {

  num_events   = trace.num_events ();
  org_ts       = new timestamp_t [num_events]; 
  ba_ts        = new timestamp_t [num_events];
  fa_ts        = new timestamp_t [num_events];
  tsa          = new timestamp_t [num_events];
  t_last_org   = 0.0;
  t_last_sync  = 0.0;
  t_diff_org   = 0.0;
  t_diff_sync  = 0.0;
  t_sum_time   = 0.0;
  abs_sum      = 0.0;
  rel_sum      = 0.0;
}

IntervalStat::~IntervalStat() {

  delete[] org_ts;
  delete[] ba_ts;
  delete[] fa_ts;

}

void IntervalStat::print_timestamps() {
  
  if (m_rank == 0) {
    for (int i = 0; i < num_events; i ++) {
      printf("\n[%d %d] :: %.9f -> %.9f -> %.9f", m_rank, m_tid, org_ts[i], fa_ts[i], ba_ts[i]);
     }
  }
}

void IntervalStat::calculate() {

#pragma omp master
  if (m_rank == 0) cout << "\nStatistics after backward amortization:";

  prepare_calculation(ba_ts);

#pragma omp barrier
  calculate_data();

  /* Histograms */
#pragma omp master
  if (m_rank == 0) cout << "\nHistograms:";
  set_sort_att(STAT_REL);
  sort(data_vec.begin(), data_vec.end());
  calculate_histograms();

  /* Rel. statistic */
#pragma omp master
  if (m_rank == 0) cout << "\nRel. errors:";
  set_sort_att(STAT_REL);
  sort(data_vec.begin(), data_vec.end());
  calculate_relative_errors();
    
  /* Rel. statistic */
#pragma omp master
  if (m_rank == 0) cout << "\nAbs. errors:";
  set_sort_att(STAT_ABS);
  sort(data_vec.begin(), data_vec.end());
  calculate_absolute_errors();

  data_vec.erase(data_vec.begin(),data_vec.end());

#pragma omp master
  if (m_rank == 0) cout << "\nStatistics after forward amortization:";

  prepare_calculation(fa_ts);

#pragma omp barrier
  calculate_data();

  /* Rel. statistic */
#pragma omp master
  if (m_rank == 0) cout << "\nRel. errors:";
  set_sort_att(STAT_REL);
  sort(data_vec.begin(), data_vec.end());
  calculate_relative_errors();
    
  /* Abs. statistic */
#pragma omp master
  if (m_rank == 0) cout << "\nAbs. errors:";
  set_sort_att(STAT_ABS);
  sort(data_vec.begin(), data_vec.end());
  calculate_absolute_errors();

  data_vec.erase(data_vec.begin(),data_vec.end());

}

void IntervalStat::calculate_data() {

  timestamp_t val;   
  
  t_last_org   = org_ts[0];
  t_last_sync  = tsa[0];
  t_sum_time   = org_ts[0];
  t_diff_org   = 0.0;
  t_diff_sync  = 0.0;
  abs_sum      = 0.0;
  rel_sum      = 0.0;
  t_sum_time   = 0.0;
    
  /* Error measurement times */
  for (long i = 1; i < num_events; i++) {
    t_diff_org = org_ts[i] - t_last_org;
    t_diff_sync = tsa[i] - t_last_sync;
    t_last_org = org_ts[i];
    t_last_sync = tsa[i];

    if ((t_diff_org - t_diff_sync) != 0 && t_diff_org != 0)
      val = fabs((t_diff_org - t_diff_sync)/t_diff_org);
    else
      val = 0.0;
    abs_sum += fabs(t_diff_org - t_diff_sync);
    rel_sum += val;
    t_sum_time += t_diff_org;
    
    statistic_data mine;
    mine.rel = val;
    mine.abs = fabs(t_diff_org - t_diff_sync);
    mine.org = t_diff_org;
    mine.syn = t_diff_sync;
    data_vec.push_back(mine);

#if 0
    if (m_rank == 0) {
      printf("\n[%d %d] :: %.9f -> %.9f -> %.9f (abs=%.9f rel=%.9f org=%.9f sync=%.9f)", 
	     m_rank, m_tid, org_ts[i], fa_ts[i], ba_ts[i], mine.abs, mine.rel, mine.org, mine.syn); 
    }
#endif
  }
}

void IntervalStat::calculate_relative_errors() {
  timestamp_t sum_intervals = 0.0;
  timestamp_t sum_abs       = 0.0;
  timestamp_t sum_rel       = 0.0;
  timestamp_t num_intervals = 0.0;
  timestamp_t vec_size      = (timestamp_t) data_vec.size();

#ifdef _OPENMP
  timestamp_t ex_data;

  timestamp_t local_data = data_vec[((long)vec_size)-1].rel;
  OMP_Allreduce_max(local_data, omp_global_max_timestamp);
  local_data = omp_global_max_timestamp;
#pragma omp master
 {
   MPI_Allreduce(&local_data, &ex_data, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
   if (m_rank == 0) printf("\n\tGlobal maximum relative error\n"); 
   if (m_rank == 0) printf("\t%.9f\n", ex_data); 
 }

 local_data = data_vec[0].rel;
 OMP_Allreduce_min(local_data, omp_global_min_timestamp);
 local_data = omp_global_min_timestamp;
#pragma omp master  
 {
   MPI_Allreduce(&local_data, &ex_data, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
   if (m_rank == 0) printf("\tGlobal minimum relative error\n"); 
   if (m_rank == 0) printf("\t%.9f\n", ex_data); 
 }

   timestamp_t omp_sum_abs       = 0.0;   
   OMP_Allreduce_sum(abs_sum, omp_global_sum_timestamp);
   omp_sum_abs = omp_global_sum_timestamp;
#pragma omp master 
   MPI_Allreduce(&omp_sum_abs, &sum_abs, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

   timestamp_t omp_sum_rel       = 0.0;
   OMP_Allreduce_sum(rel_sum, omp_global_sum_timestamp);
   omp_sum_rel = omp_global_sum_timestamp;
#pragma omp master 
   MPI_Allreduce(&omp_sum_rel, &sum_rel, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
   
   timestamp_t omp_sum_intervals = 0.0;
   OMP_Allreduce_sum(t_sum_time, omp_global_sum_timestamp);
   omp_sum_intervals = omp_global_sum_timestamp;
#pragma omp master 
   MPI_Allreduce(&omp_sum_intervals, &sum_intervals, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

   timestamp_t omp_num_intervals = 0.0;
   OMP_Allreduce_sum(vec_size, omp_global_sum_timestamp);
   omp_num_intervals = omp_global_sum_timestamp;
   
#pragma omp master 
   MPI_Allreduce(&omp_num_intervals, &num_intervals, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

#pragma omp master 
 {   
   if (m_rank == 0) printf("\tGlobal average relative error (naive);\n\t%.9f\n", sum_rel/num_intervals); 
   if (m_rank == 0) printf("\tGlobal average relative error (weighted);\n\t%.9f", (sum_abs/sum_intervals)); 
 }
#else
  statistic_data ex_data;

  MPI_Allreduce(&data_vec[((long) vec_size) -1 ], &ex_data, 1, STAT_DATA, STAT_REL_MAX, MPI_COMM_WORLD);
  if (m_rank == 0) printf("\n\tGlobal maximum relative error; abs. error; org. difference; synchronized difference\n"); 
  if (m_rank == 0) printf("\t%.9f; %.9f; %.9f; %.9f\n", ex_data.rel, ex_data.abs, ex_data.org, ex_data.syn); 
  
  MPI_Allreduce(&data_vec[0], &ex_data, 1, STAT_DATA, STAT_REL_MIN, MPI_COMM_WORLD);
  if (m_rank == 0) printf("\tGlobal minimum relative error; abs. error; org. difference; synchronized difference\n"); 
  if (m_rank == 0) printf("\t%.9f; %.9f; %.9f; %.9f\n", ex_data.rel, ex_data.abs, ex_data.org, ex_data.syn); 


  MPI_Allreduce(&abs_sum, &sum_abs, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
   
  MPI_Allreduce(&rel_sum, &sum_rel, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  
  MPI_Allreduce(&t_sum_time, &sum_intervals, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  
  MPI_Allreduce(&vec_size, &num_intervals, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
   
  if (m_rank == 0) printf("\tGlobal average relative error (naive);\n\t%.9f\n", sum_rel/num_intervals); 
  if (m_rank == 0) printf("\tGlobal average relative error (weighted);\n\t%.9f", (sum_abs/sum_intervals)); 
#endif  
}

void IntervalStat::calculate_absolute_errors() {

  timestamp_t sum_abs       = 0.0;
  timestamp_t num_intervals = 0.0;
  timestamp_t vec_size      = (timestamp_t) data_vec.size();
#ifdef _OPENMP
  timestamp_t ex_data;
  
  timestamp_t omp_sum_abs       = 0.0;   
  OMP_Allreduce_sum(abs_sum, omp_global_sum_timestamp);
  omp_sum_abs = omp_global_sum_timestamp;
  
  timestamp_t omp_num_intervals = 0.0;
  OMP_Allreduce_sum(vec_size, omp_global_sum_timestamp);
  omp_num_intervals = omp_global_sum_timestamp;
  
#pragma omp master
  { 
    MPI_Allreduce(&omp_sum_abs, &sum_abs, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&omp_num_intervals, &num_intervals, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    if (m_rank == 0) printf("\n\tGlobal average absolute error,\n\t%.9f\n", (sum_abs/num_intervals));
  }
  
  timestamp_t local_data = data_vec[((long)vec_size)-1].abs;
  OMP_Allreduce_max(local_data, omp_global_max_timestamp);
  local_data = omp_global_max_timestamp;
#pragma omp master
  {
   MPI_Allreduce(&local_data, &ex_data, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);   
   if (m_rank == 0) printf("\tGlobal maximum absolute error\n"); 
   if (m_rank == 0) printf("\t%.9f\n", ex_data); 
 }

 local_data = data_vec[0].abs;
 OMP_Allreduce_min(local_data, omp_global_min_timestamp);
 local_data = omp_global_min_timestamp;
#pragma omp master
 { 
   MPI_Allreduce(&local_data, &ex_data, 1, STAT_DATA, STAT_ABS_MIN, MPI_COMM_WORLD);
   if (m_rank == 0) printf("\tGlobal minimum absolute error\n"); 
   if (m_rank == 0) printf("\t%.9f\n", ex_data); 
 }
#else
  statistic_data ex_data;
    
  MPI_Allreduce(&data_vec[((long)vec_size)-1], &ex_data, 1, STAT_DATA, STAT_ABS_MAX, MPI_COMM_WORLD);   
  if (m_rank == 0) printf("\t\nGlobal maximum absolute error; rel. error; org. difference; synchronized difference\n"); 
  if (m_rank == 0) printf("\t%.9f; %.9f; %.9f; %.9f\n", ex_data.abs, ex_data.rel, ex_data.org, ex_data.syn); 
   
  MPI_Allreduce(&data_vec[0], &ex_data, 1, STAT_DATA, STAT_ABS_MIN, MPI_COMM_WORLD);
  if (m_rank == 0) printf("\tGlobal minimum absolute error; rel. error; org. difference; synchronized difference\n"); 
  if (m_rank == 0) printf("\t%.9f; %.9f; %.9f; %.9f\n", ex_data.abs, ex_data.rel, ex_data.org, ex_data.syn); 

  MPI_Allreduce(&abs_sum, &sum_abs, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allreduce(&vec_size, &num_intervals, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  if (m_rank == 0) printf("\tGlobal average absolute error,\n\t%.9f\n", (sum_abs/num_intervals));
   
#endif
}

void IntervalStat::calculate_histogram_class(statistic_data data, 
					     timestamp_t num_data,
					     timestamp_t num_interval, 
					     timestamp_t g_org_exe_time, 
					     timestamp_t g_sync_exe_time) {

  timestamp_t abs_org_error = 0.0;
  timestamp_t num_g         = 0.0;
  timestamp_t sum_abso      = 0.0; 
  timestamp_t sum_sync      = 0.0; 
  timestamp_t sum_orig      = 0.0; 

#ifdef _OPENMP
  OMP_Allreduce_sum(data.abs, omp_global_sum_timestamp);
  data.abs = omp_global_sum_timestamp;
#endif
#pragma omp master
    MPI_Allreduce(&data.abs, &sum_abso, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

#ifdef _OPENMP
  OMP_Allreduce_sum(data.syn, omp_global_sum_timestamp);
  data.syn = omp_global_sum_timestamp;
#endif
#pragma omp master
    MPI_Allreduce(&data.syn, &sum_sync, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

#ifdef _OPENMP
  OMP_Allreduce_sum(data.org, omp_global_sum_timestamp);
  data.org = omp_global_sum_timestamp;
#endif
#pragma omp master
    MPI_Allreduce(&data.org, &sum_orig, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

#ifdef _OPENMP
  OMP_Allreduce_sum(num_data, omp_global_sum_timestamp);
  num_data = omp_global_sum_timestamp;
#pragma omp barrier
#endif

#pragma omp master 
 {
    MPI_Allreduce(&num_data, &num_g, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
 }

#pragma omp master 
 {
   if (m_rank == 0) printf("%.9f ; %.9f; %.9f; %.9f; %.0f; %.9f; %.9f; %.9f; %.9f; %.9f\n", num_g/num_interval, 
			   sum_orig/g_org_exe_time, sum_sync/g_sync_exe_time,
			   abs_org_error, num_g, sum_orig, sum_sync, sum_abso, g_org_exe_time, g_sync_exe_time);
 }
}


void IntervalStat::calculate_histograms() {

  timestamp_t num_interval = 0.0;
  timestamp_t vec_size     = (timestamp_t) data_vec.size();
  
  timestamp_t org_exe_time  = org_ts[num_events-1] - org_ts[0];
  timestamp_t sync_exe_time = tsa[num_events-1] - tsa[0];
  
  timestamp_t local_vec_size = vec_size;
#ifdef _OPENMP
  OMP_Allreduce_sum(vec_size, omp_global_sum_timestamp);
  local_vec_size = omp_global_sum_timestamp;
#endif

#pragma omp master
  MPI_Allreduce(&local_vec_size, &num_interval, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

#ifdef _OPENMP
  OMP_Bcast(num_interval, omp_global_sum_timestamp);
  num_interval = omp_global_sum_timestamp;
#endif
  
  long i = 0;
  statistic_data m_tmp;
  m_tmp.abs = 0.0;
  m_tmp.rel = 0.0;
  m_tmp.org = 0.0;
  m_tmp.syn = 0.0;

  statistic_data max_0 = m_tmp;
  statistic_data max_1 = m_tmp;
  statistic_data max_2 = m_tmp;
  statistic_data max_3 = m_tmp;
  statistic_data max_4 = m_tmp;
  statistic_data max_5 = m_tmp;
  statistic_data max_6 = m_tmp;

  timestamp_t num_0 = 0.0;
  timestamp_t num_1 = 0.0;
  timestamp_t num_2 = 0.0;
  timestamp_t num_3 = 0.0;
  timestamp_t num_4 = 0.0;
  timestamp_t num_5 = 0.0;
  timestamp_t num_6 = 0.0;


  for ( i = 0; i < vec_size; i++ ) {
    if (data_vec[i].abs == 0.0) {
      num_0++; 
      max_0.abs += data_vec[i].abs; 
      max_0.syn += data_vec[i].syn;
      max_0.org += data_vec[i].org;
    }
    if (data_vec[i].rel > 0.0 && data_vec[i].rel <= 0.0001) {
      num_1++;
      max_1.abs += data_vec[i].abs; 
      max_1.syn += data_vec[i].syn;
      max_1.org += data_vec[i].org;
    }
    if (data_vec[i].rel > 0.0001 && data_vec[i].rel <= 0.001) {
      num_2++;
      max_2.abs += data_vec[i].abs; 
      max_2.syn += data_vec[i].syn;
      max_2.org += data_vec[i].org;
    }
    if (data_vec[i].rel > 0.001 && data_vec[i].rel <= 0.01) {
      num_3++;
      max_3.abs += data_vec[i].abs; 
      max_3.syn += data_vec[i].syn;
      max_3.org += data_vec[i].org;
    }
    if (data_vec[i].rel > 0.01 && data_vec[i].rel <= 0.1) {
      num_4++;
      max_4.abs += data_vec[i].abs; 
      max_4.syn += data_vec[i].syn;
      max_4.org += data_vec[i].org;
    }
    if (data_vec[i].rel > 0.1 && data_vec[i].rel <= 1.0) {
      num_5++;
      max_5.abs += data_vec[i].abs; 
      max_5.syn += data_vec[i].syn;
      max_5.org += data_vec[i].org;
    }
     if (data_vec[i].rel > 1.00 ) {
       num_6++;
       max_6.abs += data_vec[i].abs; 
       max_6.syn += data_vec[i].syn;
       max_6.org += data_vec[i].org;
     }
  } 

#pragma omp master
  if (m_rank == 0) {
    printf("\n\tFraction A; Fraction of #intervals " 
	   "in error class compared to total #intervals\n");
    printf("\tFraction B; Fraction of accumulated duration of "
	   "orig. intervals in error class compared to accumulated "
	   "duration of all intervals in orig. trace\n");
    printf("\tFraction C; Fraction of accumulated duration of sync. "
	   "intervals in error class compared to accumulated duration" 
	   "of all intervals in sync. trace\n");
    printf("\tFraction D; Fraction of accumulated abs. error of intervals"
	   " in error class compared to accumulated duration of intervals "
	   "in error class in org. trace\n");
    printf("\tE; Number of #intervals in error class \n");
    printf("\tF; Seconds of accumulated duration of orig. intervals in error class\n");
    printf("\tG; Seconds of accumulated duration of sync. intervals in error class \n");
    printf("\tH; Seconds of accumulated abs. error of intervals in error class\n");
    printf("\tI; Accumulated duration of all intervals in orig. trace\n");
    printf("\tJ; Accumulated duration of all intervals in sync. trace\n");

    printf("\n\tInterval; Fraction A;");
    printf("\tFraction B;");
    printf("\tFraction C;");
    printf("\tFraction D;");
    printf("\tE;");
    printf("\tF;");
    printf("\tG;");
    printf("\tH;");
    printf("\tI;");
    printf("\tJ;\n");

  }
  timestamp_t g_org_exe_time = 0.0;
  timestamp_t g_sync_exe_time = 0.0;

  timestamp_t local_org_exe_time = org_exe_time;
#ifdef _OPENMP
  OMP_Allreduce_sum(local_org_exe_time, omp_global_sum_timestamp);
  local_org_exe_time = omp_global_sum_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&local_org_exe_time, &g_org_exe_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#ifdef _OPENMP
  OMP_Bcast(g_org_exe_time, omp_global_sum_timestamp);
  g_org_exe_time = omp_global_sum_timestamp;
#endif

  timestamp_t local_sync_exe_time = sync_exe_time;
#ifdef _OPENMP
  OMP_Allreduce_sum(local_sync_exe_time, omp_global_sum_timestamp);
  local_sync_exe_time = omp_global_sum_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&local_sync_exe_time, &g_sync_exe_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#ifdef _OPENMP
  OMP_Bcast(g_sync_exe_time, omp_global_sum_timestamp);
  g_sync_exe_time = omp_global_sum_timestamp;
#endif
#pragma omp master
  if (m_rank == 0) printf("\t'=0.0%%; ");
  calculate_histogram_class(max_0, num_0, num_interval, g_org_exe_time, g_sync_exe_time);

#pragma omp master
  if (m_rank == 0) printf("\t>0.0%% und <= 0.01%%; ");
  calculate_histogram_class(max_1, num_1, num_interval, g_org_exe_time, g_sync_exe_time);

  max_1.abs +=max_0.abs;
  max_1.syn +=max_0.syn;
  max_1.org +=max_0.org;
  num_1 += num_0;
#pragma omp master
  if (m_rank == 0) printf("\t<= 0.01; ");
  calculate_histogram_class(max_1, num_1, num_interval, g_org_exe_time, g_sync_exe_time);

#pragma omp master
  if (m_rank == 0) printf("\t> 0.01%% and <= 0.1%%; ");
  calculate_histogram_class(max_2, num_2, num_interval, g_org_exe_time, g_sync_exe_time);

#pragma omp master
  if (m_rank == 0) printf("\t>0.1%% and <= 1%%; ");
  calculate_histogram_class(max_3, num_3, num_interval, g_org_exe_time, g_sync_exe_time);

#pragma omp master
  if (m_rank == 0) printf("\t> 1%% and <= 10%%; ");
  calculate_histogram_class(max_4, num_4, num_interval, g_org_exe_time, g_sync_exe_time);

#pragma omp master
  if (m_rank == 0) printf("\t> 10%% and <= 100%%; ");
  calculate_histogram_class(max_5, num_5, num_interval, g_org_exe_time, g_sync_exe_time);

#pragma omp master
  if (m_rank == 0) printf("\t> 100%%; ");
  calculate_histogram_class(max_6, num_6, num_interval, g_org_exe_time, g_sync_exe_time);
}

int num_events_tmp = 0;
void IntervalStat::prepare_calculation(pearl::timestamp_t* t_array) {

#if 1
  for (long i = 0; i < num_events; i++)
    tsa[i] = t_array[i];
#endif 
#if 0
  if (num_events_tmp != 0) 
    num_events = num_events_tmp;
  int j = 0;
  for (long i = 0; i < num_events; i++) {
    if (t_array[i] > 100.00) {
      if (t_array[i] < 1000.00) {
	tsa[j] = t_array[i];

	if (num_events_tmp == 0) {
	    org_ts[j] = org_ts[i];
	}
	j++;
      }
    }
  }
  num_events = j;
#endif

}


void IntervalStat::get_org_timestamps() {

  long i = 0;
  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    org_ts[i] = it->get_time();
    i++;
    it++;
  }
}

void IntervalStat::get_fa_timestamps() {

  long i = 0;
  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    fa_ts[i] = it->get_time();
    i++;
    it++;
  }
}

void IntervalStat::get_ba_timestamps() {

  long i = 0;
  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    ba_ts[i] = it->get_time();
    i++;
    it++;
  }
}

