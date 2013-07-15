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

#include <TimeStat.h>
#include <Operator.h>

using namespace std;
using namespace pearl;
/*
 *---------------------------------------------------------------------------
 *
 * class TimeStat
 *
 *---------------------------------------------------------------------------
 */

TimeStat::TimeStat(const pearl::LocalTrace& myTrace, int r, int tid): trace(myTrace), m_rank(r), m_tid(tid) {

  num_events   = trace.num_events ();
  org_ts       = new timestamp_t [num_events]; 
  ba_ts        = new timestamp_t [num_events];
  fa_ts        = new timestamp_t [num_events];
  tsa          = new timestamp_t [num_events];
  t_first_org  = 0.0;
  t_first_sync = 0.0;
  t_diff_org   = 0.0;
  t_diff_sync  = 0.0;
  t_sum_time   = 0.0;
  abs_sum      = 0.0;
  rel_sum      = 0.0;
}

TimeStat::~TimeStat() {

  delete[] org_ts;
  delete[] ba_ts;
  delete[] fa_ts;

}

void TimeStat::print_timestamps() {
  
  if (m_rank == 0 && m_tid == 1) {
    for (int i = 0; i < num_events; i ++) {
      printf("%.9f -> %.9f -> %.9f\n", org_ts[i], fa_ts[i], ba_ts[i]);
     }
  }
}

void TimeStat::calculate() {


  if (m_rank == 0 && m_tid == 0) cout << "\nStatistics after backward amortization:";

#pragma omp barrier
  prepare_calculation(ba_ts);
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

void TimeStat::calculate_data() {

  timestamp_t val;   
  
  t_first_org  = org_ts[0];
  t_first_sync = tsa[0];
  t_sum_time   = org_ts[0];
  t_diff_org   = 0.0;
  t_diff_sync  = 0.0;
  abs_sum      = 0.0;
  rel_sum      = 0.0;
  t_sum_time   = 0.0;
    
  /* Error measurement times */
  for (long i = 1; i < num_events; i++) {
    t_diff_org = org_ts[i] - t_first_org;
    t_diff_sync = tsa[i] - t_first_sync;
    
    val = fabs((t_diff_org - t_diff_sync)/t_diff_org);
    abs_sum += fabs(t_diff_org - t_diff_sync);
    rel_sum += val;
    t_sum_time += org_ts[i];
    
    statistic_data mine;
    mine.rel = val;
    mine.abs = fabs(t_diff_org - t_diff_sync);
    mine.org = t_diff_org;
    mine.syn = t_diff_sync;
    data_vec.push_back(mine);
  }
}

void TimeStat::calculate_relative_errors() {
  
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

  MPI_Allreduce(&data_vec[((long)vec_size)-1], &ex_data, 1, STAT_DATA, STAT_REL_MAX, MPI_COMM_WORLD);
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

void TimeStat::calculate_absolute_errors() {

  timestamp_t t_org         = -DBL_MAX; 
  timestamp_t t_syn         = -DBL_MAX; 
  timestamp_t sum_abs       = 0.0;
  timestamp_t num_intervals = 0.0;
  timestamp_t vec_size      = (timestamp_t) data_vec.size();
#ifdef _OPENMP
  timestamp_t ex_data;

  timestamp_t local_data = data_vec[((long)vec_size)-1].abs;
  OMP_Allreduce_max(local_data, omp_global_max_timestamp);
  local_data = omp_global_max_timestamp;
#pragma omp master
 {
   MPI_Allreduce(&local_data, &ex_data, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);   
   if (m_rank == 0) printf("\t\nGlobal maximum absolute error\n"); 
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
   if (m_rank == 0) printf("\tGlobal average absolute error,\n\t%.9f\n", (sum_abs/num_intervals));
 }

 /* Overall statistics */  
 timestamp_t omp_sum_intervals_org = 0.0;
 OMP_Allreduce_sum(org_ts[num_events-1], omp_global_sum_timestamp);
 omp_sum_intervals_org  = omp_global_sum_timestamp;
 timestamp_t omp_sum_intervals_sync = 0.0;
 OMP_Allreduce_sum(tsa[num_events-1], omp_global_sum_timestamp);
 omp_sum_intervals_sync  = omp_global_sum_timestamp;
#pragma omp master
 {
   MPI_Allreduce(&omp_sum_intervals_org, &t_org, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
   MPI_Allreduce(&omp_sum_intervals_sync, &t_syn, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
   if (m_rank == 0) printf("\tRelative error of overall execution time\n\t%.9f\n", (fabs(t_org-t_syn)/t_org));
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
   
   /* Overall statistics */  
  MPI_Allreduce(&org_ts[num_events-1], &t_org, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&tsa[num_events-1], &t_syn, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
  if (m_rank == 0) printf("\tRelative error of overall execution time\n\t%.9f\n", (fabs(t_org-t_syn)/t_org));
#endif
}

int num_events_tmp_t = 0;
void TimeStat::prepare_calculation(pearl::timestamp_t* t_array) {

#if 1
  for (long i = 0; i < num_events; i++)
    tsa[i] = t_array[i];
#endif
#if 0
  if (num_events_tmp_t != 0) 
    num_events = num_events_tmp_t;
  int j = 0;
  for (long i = 0; i < num_events; i++) {
    if (t_array[i] > 100.00) {
      if (t_array[i] < 1000.00) {
	tsa[j] = t_array[i];

	if (num_events_tmp_t == 0) {
	    org_ts[j] = org_ts[i];
	}
	j++;
      }
    }
  }
  num_events = j;
#endif
}

void TimeStat::get_org_timestamps() {

  long i = 0;
  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    org_ts[i] = it->get_time();
    i++;
    it++;
  }
}

void TimeStat::get_fa_timestamps() {

  long i = 0;
  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    fa_ts[i] = it->get_time();
    i++;
    it++;
  }
}

void TimeStat::get_ba_timestamps() {

  long i = 0;
  LocalTrace::iterator it = trace.begin();
  while (it != trace.end()) {
    ba_ts[i] = it->get_time();
    i++;
    it++;
  }
}

