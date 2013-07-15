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

#include <algorithm>
#include <Operator.h>
#include <AmortData.h>
#include <StatData.h>

using namespace std;
using namespace pearl;

MPI_Datatype AMORT_DATA, STAT_DATA;
MPI_Op       CLC_MAX, CLC_MIN, STAT_REL_MAX, STAT_ABS_MAX, STAT_REL_MIN, STAT_ABS_MIN;


void clc_max ( amortization_data *, amortization_data *, int *, MPI_Datatype * );

void clc_min ( amortization_data *, amortization_data *, int *, MPI_Datatype * );

void stat_rel_max ( statistic_data *, statistic_data *, int *, MPI_Datatype * );

void stat_rel_min ( statistic_data *, statistic_data *, int *, MPI_Datatype * );

void stat_abs_max ( statistic_data *, statistic_data *, int *, MPI_Datatype * );

void stat_abs_min ( statistic_data *, statistic_data *, int *, MPI_Datatype * );


	 
void clc_max(amortization_data *in, amortization_data *inout, int *len, MPI_Datatype *)
{
  for (int i = 0; i < *len; ++i) {
    inout[i].timestamp = max(inout[i].timestamp, in[i].timestamp);
    if (inout[i].timestamp == in[i].timestamp) {
      inout[i].machine_id = in[i].machine_id;
      inout[i].node_id    = in[i].node_id;
      inout[i].process_id = in[i].process_id;
      inout[i].thread_id  = in[i].thread_id;
    }
  }
}

void clc_min(amortization_data *in, amortization_data *inout, int *len, MPI_Datatype *)
{
  for (int i = 0; i < *len; ++i) {
    inout[i].timestamp = min(inout[i].timestamp, in[i].timestamp);
    if (inout[i].timestamp == in[i].timestamp) {
      inout[i].machine_id = in[i].machine_id;
      inout[i].node_id    = in[i].node_id;
      inout[i].process_id = in[i].process_id;
      inout[i].thread_id  = in[i].thread_id;
    }
  }
}

void stat_rel_max(statistic_data *in, statistic_data *inout, int *len, MPI_Datatype *)
{
  for (int i = 0; i < *len; ++i) {
    inout[i].rel = max(inout[i].rel, in[i].rel);
    if (inout[i].rel == in[i].rel) {
      inout[i].abs = in[i].abs;
      inout[i].org = in[i].org;
      inout[i].syn = in[i].syn;
    }
  }
}

void stat_rel_min(statistic_data *in, statistic_data *inout, int *len, MPI_Datatype *)
{
  for (int i = 0; i < *len; ++i) {
    inout[i].rel = min(inout[i].rel, in[i].rel);
    if (inout[i].rel == in[i].rel) {
      inout[i].abs = in[i].abs;
      inout[i].org = in[i].org;
      inout[i].syn = in[i].syn;
    }
  }
}

void stat_abs_max(statistic_data *in, statistic_data *inout, int *len, MPI_Datatype *)
{
  for (int i = 0; i < *len; ++i) {
    inout[i].abs = max(inout[i].abs, in[i].abs);
    if (inout[i].abs == in[i].abs) {
      inout[i].rel = in[i].rel;
      inout[i].org = in[i].org;
      inout[i].syn = in[i].syn;
    }
  }
}

void stat_abs_min(statistic_data *in, statistic_data *inout, int *len, MPI_Datatype *)
{
  for (int i = 0; i < *len; ++i) {
    inout[i].abs = min(inout[i].abs, in[i].abs);
    if (inout[i].abs == in[i].abs) {
      inout[i].rel = in[i].rel;
      inout[i].org = in[i].org;
      inout[i].syn = in[i].syn;
    }
  }
}

void init_operators() {
  
  int          blocklens[2];
  MPI_Aint     indices[2];
  MPI_Datatype old_types[2];
  
  /* One value of each type */
  blocklens[0] = 4;
  blocklens[1] = 1;

  /* The base types */
  old_types[0] = MPI_INT;
  old_types[1] = MPI_DOUBLE;

  /* The locations of each element */
  amortization_data m_val;
  MPI_Address( &m_val.machine_id, &indices[0] );
  MPI_Address( &m_val.timestamp,  &indices[1] );

  /* Make relative */
  indices[1] = indices[1] - indices[0];
  indices[0] = 0;
  
  /* Commit it */
  MPI_Type_struct( 2, blocklens, indices, old_types, &AMORT_DATA );
  MPI_Type_commit( &AMORT_DATA );
	  
  MPI_Type_contiguous(4, MPI_DOUBLE, &STAT_DATA);
  MPI_Type_commit(&STAT_DATA);
 
  /* Make user defined operators */
  MPI_Op_create( (MPI_User_function *)clc_max, 1, &CLC_MAX );
  MPI_Op_create( (MPI_User_function *)clc_min, 1, &CLC_MIN );
  
  MPI_Op_create( (MPI_User_function *)stat_rel_max, 1, &STAT_REL_MAX );
  MPI_Op_create( (MPI_User_function *)stat_abs_max, 1, &STAT_ABS_MAX );
  MPI_Op_create( (MPI_User_function *)stat_rel_min, 1, &STAT_REL_MIN );
  MPI_Op_create( (MPI_User_function *)stat_abs_min, 1, &STAT_ABS_MIN );

}

void free_operators() {
  
  /* Clean up types */
  MPI_Type_free( &AMORT_DATA );
  MPI_Type_free( &STAT_DATA );

  /* Clean up operators */
  MPI_Op_free( &CLC_MAX );
  MPI_Op_free( &CLC_MIN );
  MPI_Op_free( &STAT_REL_MAX );
  MPI_Op_free( &STAT_ABS_MAX );
  MPI_Op_free( &STAT_REL_MIN );
  MPI_Op_free( &STAT_ABS_MIN );

}
