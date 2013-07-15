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

#include <cfloat>

#include <pearl.h>
#include <RuntimeStat.h>
#ifdef _OPENMP
#include <OmpData.h>
#include <OmpCommunication.h>
#endif

#include <mpi.h>
#ifdef _OPENMP
#include <omp.h>
#endif
using namespace std;
using namespace pearl;

RuntimeStat::RuntimeStat() {

  num_passes = 3;
  m_init();

}

RuntimeStat::RuntimeStat(int passes) {

  num_passes = passes;
  m_init();

}

RuntimeStat::~RuntimeStat() {};

void RuntimeStat::m_init() {

  pass = 0;
  clock_diff     = 0.0;
  sum_clock_diff = 0.0;
  max_clock_diff = 0.0;
  num_clock_corr = 0;
  s_time = DBL_MAX;
  e_time = -DBL_MAX;

}

int RuntimeStat::get_num_passes(){

  return num_passes;

}

timestamp_t RuntimeStat::get_num_clock_corr() { 

  timestamp_t gl_corr = 0.0;
  timestamp_t corr    = (timestamp_t) num_clock_corr;
#ifdef _OPENMP
  OMP_Allreduce_sum(corr, omp_global_sum_timestamp);
  corr = omp_global_sum_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&corr, &gl_corr, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#ifdef _OPENMP
  OMP_Bcast(gl_corr, omp_global_sum_timestamp);
  gl_corr = omp_global_sum_timestamp;
#endif
  return gl_corr; 
}

timestamp_t  RuntimeStat::get_max_clock_error() { 

  double gl_max_error = 0.0;
  double max_error    = max_clock_diff;
#ifdef _OPENMP
  OMP_Allreduce_max(max_error, omp_global_max_timestamp);
  max_error = omp_global_max_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&max_error, &gl_max_error, 1, MPI_DOUBLE, MPI_MAX,MPI_COMM_WORLD);
#ifdef _OPENMP
  OMP_Bcast(gl_max_error, omp_global_max_timestamp);
  gl_max_error = omp_global_max_timestamp;
#endif
  return gl_max_error; 

}

timestamp_t RuntimeStat::get_sum_clock_error() { 

  double gl_sum_error = 0.0;
  double sum_error    = sum_clock_diff;
#ifdef _OPENMP
  OMP_Allreduce_sum(sum_error, omp_global_sum_timestamp);
  sum_error = omp_global_sum_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&sum_error, &gl_sum_error, 1, MPI_DOUBLE, MPI_SUM,MPI_COMM_WORLD);
#ifdef _OPENMP
  OMP_Bcast(gl_sum_error, omp_global_sum_timestamp);
  gl_sum_error = omp_global_sum_timestamp;
#endif
  return gl_sum_error; 

}

timestamp_t RuntimeStat::get_clock_error() { 

  double gl_error = 0.0;
  double error    = clock_diff/get_runtime();
#ifdef _OPENMP
  OMP_Allreduce_max(error, omp_global_max_timestamp);
  error = omp_global_max_timestamp;
#endif
#pragma omp master
  MPI_Allreduce(&error, &gl_error, 1, MPI_DOUBLE, MPI_MAX,MPI_COMM_WORLD);
#ifdef _OPENMP
  OMP_Bcast(gl_error, omp_global_max_timestamp);
  gl_error = omp_global_max_timestamp;
#endif
  return gl_error; 

}
  
int RuntimeStat::get_pass() {

  return pass;

}

void RuntimeStat::set_num_passes(int num) {

  num_passes = num;

}

void RuntimeStat::set_pass() {

  pass++;

}

bool  RuntimeStat::next_pass() {

  if (pass < num_passes)
    return true;

  return false;
}


/**
 ** Calculates clock's statistics
 **/

void RuntimeStat::set_statistics(const timestamp_t curEvtT, const timestamp_t value) {

  clock_diff = value - curEvtT;
  
  sum_clock_diff += clock_diff;

  if (clock_diff > 0.0) num_clock_corr++;

  if (max_clock_diff < clock_diff) max_clock_diff = clock_diff;

  if (value < s_time) s_time = value;
  if (value > e_time) e_time = value;
}


