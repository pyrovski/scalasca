/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _SLOG2_MPITAB_H
#define _SLOG2_MPITAB_H

/* Define States */

#define ST_MESSAGE   0
#define ST_USER      1
#define ST_EPIK      2
#define ST_MPIRECV   3
#define ST_MPISEND   4
#define ST_MPITEST   5
#define ST_MPIWAIT   6
#define ST_MPIISEND  7
#define ST_MPIIRECV  8
#define ST_MPICOLL   9
#define ST_MPIOTHER 10
#define ST_MPISDRV  11
#define ST_MPIRMA   12
#define ST_OMP      13
#define ST_OMPCOLL  14

#define MPI_FUNCTIONS 181

typedef struct {
  const char* name;
  int   state;
} slog2_mpi_tab_t;

slog2_mpi_tab_t slog2_mpi_tab[] = {
  { "MPI_Abort",                 ST_MPIOTHER },
  { "MPI_Accumulate",            ST_MPIRMA   },
  { "MPI_Address",               ST_MPIOTHER },
  { "MPI_Allgather",             ST_MPICOLL  },
  { "MPI_Allgatherv",            ST_MPICOLL  },
  { "MPI_Allreduce",             ST_MPICOLL  },
  { "MPI_Alltoall",              ST_MPICOLL  },
  { "MPI_Alltoallv",             ST_MPICOLL  },
  { "MPI_Attr_delete",           ST_MPIOTHER },
  { "MPI_Attr_get",              ST_MPIOTHER },
  { "MPI_Attr_put",              ST_MPIOTHER },
  { "MPI_Barrier",               ST_MPICOLL  },
  { "MPI_Bcast",                 ST_MPICOLL  },
  { "MPI_Bsend",                 ST_MPISEND  },
  { "MPI_Bsend_init",            ST_MPISEND  },
  { "MPI_Buffer_attach",         ST_MPIOTHER },
  { "MPI_Buffer_detach",         ST_MPIOTHER },
  { "MPI_Cancel",                ST_MPIOTHER },
  { "MPI_Cart_coords",           ST_MPIOTHER },
  { "MPI_Cart_create",           ST_MPIOTHER },
  { "MPI_Cart_get",              ST_MPIOTHER },
  { "MPI_Cart_map",              ST_MPIOTHER },
  { "MPI_Cart_rank",             ST_MPIOTHER },
  { "MPI_Cart_shift",            ST_MPIOTHER },
  { "MPI_Cart_sub",              ST_MPIOTHER },
  { "MPI_Cartdim_get",           ST_MPIOTHER },
  { "MPI_Comm_compare",          ST_MPIOTHER },
  { "MPI_Comm_create",           ST_MPIOTHER },
  { "MPI_Comm_dup",              ST_MPIOTHER },
  { "MPI_Comm_free",             ST_MPIOTHER },
  { "MPI_Comm_group",            ST_MPIOTHER },
  { "MPI_Comm_rank",             ST_MPIOTHER },
  { "MPI_Comm_remote_group",     ST_MPIOTHER },
  { "MPI_Comm_remote_size",      ST_MPIOTHER },
  { "MPI_Comm_size",             ST_MPIOTHER },
  { "MPI_Comm_split",            ST_MPIOTHER },
  { "MPI_Comm_test_inter",       ST_MPIOTHER },
  { "MPI_Dims_create",           ST_MPIOTHER },
  { "MPI_Errhandler_create",     ST_MPIOTHER },
  { "MPI_Errhandler_free",       ST_MPIOTHER },
  { "MPI_Errhandler_get",        ST_MPIOTHER },
  { "MPI_Errhandler_set",        ST_MPIOTHER },
  { "MPI_Error_class",           ST_MPIOTHER },
  { "MPI_Error_string",          ST_MPIOTHER },
  { "MPI_Finalize",              ST_MPIOTHER },
  { "MPI_Gather",                ST_MPICOLL  },
  { "MPI_Gatherv",               ST_MPICOLL  },
  { "MPI_Get",                   ST_MPIRMA   },
  { "MPI_Get_count",             ST_MPIOTHER },
  { "MPI_Get_elements",          ST_MPIOTHER },
  { "MPI_Get_processor_name",    ST_MPIOTHER },
  { "MPI_Graph_create",          ST_MPIOTHER },
  { "MPI_Graph_get",             ST_MPIOTHER },
  { "MPI_Graph_map",             ST_MPIOTHER },
  { "MPI_Graph_neighbors",       ST_MPIOTHER },
  { "MPI_Graph_neighbors_count", ST_MPIOTHER },
  { "MPI_Graphdims_get",         ST_MPIOTHER },
  { "MPI_Group_compare",         ST_MPIOTHER },
  { "MPI_Group_difference",      ST_MPIOTHER },
  { "MPI_Group_excl",            ST_MPIOTHER },
  { "MPI_Group_free",            ST_MPIOTHER },
  { "MPI_Group_incl",            ST_MPIOTHER },
  { "MPI_Group_intersection",    ST_MPIOTHER },
  { "MPI_Group_range_excl",      ST_MPIOTHER },
  { "MPI_Group_range_incl",      ST_MPIOTHER },
  { "MPI_Group_rank",            ST_MPIOTHER },
  { "MPI_Group_size",            ST_MPIOTHER },
  { "MPI_Group_translate_ranks", ST_MPIOTHER },
  { "MPI_Group_union",           ST_MPIOTHER },
  { "MPI_Ibsend",                ST_MPIISEND },
  { "MPI_Init",                  ST_MPIOTHER },
  { "MPI_Init_thread",           ST_MPIOTHER },
  { "MPI_Initialized",           ST_MPIOTHER },
  { "MPI_Intercomm_create",      ST_MPIOTHER },
  { "MPI_Intercomm_merge",       ST_MPIOTHER },
  { "MPI_Iprobe",                ST_MPITEST  },
  { "MPI_Irecv",                 ST_MPIIRECV },
  { "MPI_Irsend",                ST_MPIISEND },
  { "MPI_Isend",                 ST_MPIISEND },
  { "MPI_Issend",                ST_MPIISEND },
  { "MPI_Keyval_create",         ST_MPIOTHER },
  { "MPI_Keyval_free",           ST_MPIOTHER },
  { "MPI_Op_create",             ST_MPIOTHER },
  { "MPI_Op_free",               ST_MPIOTHER },
  { "MPI_Pack",                  ST_MPIOTHER },
  { "MPI_Pack_size",             ST_MPIOTHER },
  { "MPI_Probe",                 ST_MPITEST  },
  { "MPI_Put",                   ST_MPIRMA   },
  { "MPI_Recv",                  ST_MPIRECV  },
  { "MPI_Recv_init",             ST_MPIRECV  },
  { "MPI_Reduce",                ST_MPICOLL  },
  { "MPI_Reduce_scatter",        ST_MPICOLL  },
  { "MPI_Request_free",          ST_MPIOTHER },
  { "MPI_Rsend",                 ST_MPISEND  },
  { "MPI_Rsend_init",            ST_MPISEND  },
  { "MPI_Scan",                  ST_MPICOLL  },
  { "MPI_Scatter",               ST_MPICOLL  },
  { "MPI_Scatterv",              ST_MPICOLL  },
  { "MPI_Send",                  ST_MPISEND  },
  { "MPI_Send_init",             ST_MPISEND  },
  { "MPI_Sendrecv",              ST_MPISDRV  },
  { "MPI_Sendrecv_replace",      ST_MPISDRV  },
  { "MPI_Ssend",                 ST_MPISEND  },
  { "MPI_Ssend_init",            ST_MPISEND  },
  { "MPI_Start",                 ST_MPIOTHER },
  { "MPI_Startall",              ST_MPIOTHER },
  { "MPI_Test",                  ST_MPITEST  },
  { "MPI_Test_cancelled",        ST_MPITEST  },
  { "MPI_Testall",               ST_MPITEST  },
  { "MPI_Testany",               ST_MPITEST  },
  { "MPI_Testsome",              ST_MPITEST  },
  { "MPI_Topo_test",             ST_MPIOTHER },
  { "MPI_Type_commit",           ST_MPIOTHER },
  { "MPI_Type_contiguous",       ST_MPIOTHER },
  { "MPI_Type_extent",           ST_MPIOTHER },
  { "MPI_Type_free",             ST_MPIOTHER },
  { "MPI_Type_hindexed",         ST_MPIOTHER },
  { "MPI_Type_hvector",          ST_MPIOTHER },
  { "MPI_Type_indexed",          ST_MPIOTHER },
  { "MPI_Type_lb",               ST_MPIOTHER },
  { "MPI_Type_size",             ST_MPIOTHER },
  { "MPI_Type_struct",           ST_MPIOTHER },
  { "MPI_Type_ub",               ST_MPIOTHER },
  { "MPI_Type_vector",           ST_MPIOTHER },
  { "MPI_Unpack",                ST_MPIOTHER },
  { "MPI_Wait",                  ST_MPIWAIT  },
  { "MPI_Waitall",               ST_MPIWAIT  },
  { "MPI_Waitany",               ST_MPIWAIT  },
  { "MPI_Waitsome",              ST_MPIWAIT  },
  { "MPI_Win_complete",          ST_MPIRMA   },
  { "MPI_Win_create",            ST_MPIRMA   },
  { "MPI_Win_fence",             ST_MPIRMA   },
  { "MPI_Win_free",              ST_MPIRMA   },
  { "MPI_Win_lock",              ST_MPIRMA   },
  { "MPI_Win_post",              ST_MPIRMA   },
  { "MPI_Win_start",             ST_MPIRMA   },
  { "MPI_Win_test",              ST_MPIRMA   },
  { "MPI_Win_unlock",            ST_MPIRMA   },
  { "MPI_Win_wait",              ST_MPIRMA   },
  { "MPI_Wtick",                 ST_MPIOTHER },
  { "MPI_Wtime",                 ST_MPIOTHER }
};

#endif
