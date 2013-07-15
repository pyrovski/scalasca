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
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  epk_fmpiwrap_cg.c
 *
 * @brief Fortran interface wrappers for communicator and group functions
 */

#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "elg_error.h"
#include "elg_fmpi_defs.h"
#include "epk_fmpiwrap.h"
#include "epk_defs_mpi.h"
#include "epk_fwrapper_def.h"

/**
 * @name Uppercase names
 * @{*/
/** All uppercase name of MPI_Comm_compare */
#define MPI_Comm_compare_U MPI_COMM_COMPARE
/** All uppercase name of MPI_Comm_create */
#define MPI_Comm_create_U MPI_COMM_CREATE
/** All uppercase name of MPI_Comm_dup */
#define MPI_Comm_dup_U MPI_COMM_DUP
/** All uppercase name of MPI_Comm_free */
#define MPI_Comm_free_U MPI_COMM_FREE
/** All uppercase name of MPI_Comm_group */
#define MPI_Comm_group_U MPI_COMM_GROUP
/** All uppercase name of MPI_Comm_rank */
#define MPI_Comm_rank_U MPI_COMM_RANK
/** All uppercase name of MPI_Comm_remote_group */
#define MPI_Comm_remote_group_U MPI_COMM_REMOTE_GROUP
/** All uppercase name of MPI_Comm_remote_size */
#define MPI_Comm_remote_size_U MPI_COMM_REMOTE_SIZE
/** All uppercase name of MPI_Comm_size */
#define MPI_Comm_size_U MPI_COMM_SIZE
/** All uppercase name of MPI_Comm_split */
#define MPI_Comm_split_U MPI_COMM_SPLIT
/** All uppercase name of MPI_Comm_test_inter */
#define MPI_Comm_test_inter_U MPI_COMM_TEST_INTER
/** All uppercase name of MPI_Group_compare */
#define MPI_Group_compare_U MPI_GROUP_COMPARE
/** All uppercase name of MPI_Group_difference */
#define MPI_Group_difference_U MPI_GROUP_DIFFERENCE
/** All uppercase name of MPI_Group_excl */
#define MPI_Group_excl_U MPI_GROUP_EXCL
/** All uppercase name of MPI_Group_free */
#define MPI_Group_free_U MPI_GROUP_FREE
/** All uppercase name of MPI_Group_incl */
#define MPI_Group_incl_U MPI_GROUP_INCL
/** All uppercase name of MPI_Group_intersection */
#define MPI_Group_intersection_U MPI_GROUP_INTERSECTION
/** All uppercase name of MPI_Group_range_excl */
#define MPI_Group_range_excl_U MPI_GROUP_RANGE_EXCL
/** All uppercase name of MPI_Group_range_incl */
#define MPI_Group_range_incl_U MPI_GROUP_RANGE_INCL
/** All uppercase name of MPI_Group_rank */
#define MPI_Group_rank_U MPI_GROUP_RANK
/** All uppercase name of MPI_Group_size */
#define MPI_Group_size_U MPI_GROUP_SIZE
/** All uppercase name of MPI_Group_translate_ranks */
#define MPI_Group_translate_ranks_U MPI_GROUP_TRANSLATE_RANKS
/** All uppercase name of MPI_Group_union */
#define MPI_Group_union_U MPI_GROUP_UNION
/** All uppercase name of MPI_Intercomm_create */
#define MPI_Intercomm_create_U MPI_INTERCOMM_CREATE
/** All uppercase name of MPI_Intercomm_merge */
#define MPI_Intercomm_merge_U MPI_INTERCOMM_MERGE

/** All uppercase name of MPI_Comm_call_errhandler */
#define MPI_Comm_call_errhandler_U MPI_COMM_CALL_ERRHANDLER
/** All uppercase name of MPI_Comm_create_errhandler */
#define MPI_Comm_create_errhandler_U MPI_COMM_CREATE_ERRHANDLER
/** All uppercase name of MPI_Comm_get_errhandler */
#define MPI_Comm_get_errhandler_U MPI_COMM_GET_ERRHANDLER
/** All uppercase name of MPI_Comm_set_errhandler */
#define MPI_Comm_set_errhandler_U MPI_COMM_SET_ERRHANDLER

/** All uppercase name of MPI_Attr_delete */
#define MPI_Attr_delete_U MPI_ATTR_DELETE
/** All uppercase name of MPI_Attr_get */
#define MPI_Attr_get_U MPI_ATTR_GET
/** All uppercase name of MPI_Attr_put */
#define MPI_Attr_put_U MPI_ATTR_PUT
/** All uppercase name of MPI_Comm_create_keyval */
#define MPI_Comm_create_keyval_U MPI_COMM_CREATE_KEYVAL
/** All uppercase name of MPI_Comm_delete_attr */
#define MPI_Comm_delete_attr_U MPI_COMM_DELETE_ATTR
/** All uppercase name of MPI_Comm_free_keyval */
#define MPI_Comm_free_keyval_U MPI_COMM_FREE_KEYVAL
/** All uppercase name of MPI_Comm_get_attr */
#define MPI_Comm_get_attr_U MPI_COMM_GET_ATTR
/** All uppercase name of MPI_Comm_get_name */
#define MPI_Comm_get_name_U MPI_COMM_GET_NAME
/** All uppercase name of MPI_Comm_set_attr */
#define MPI_Comm_set_attr_U MPI_COMM_SET_ATTR
/** All uppercase name of MPI_Comm_set_name */
#define MPI_Comm_set_name_U MPI_COMM_SET_NAME
/** All uppercase name of MPI_Keyval_create */
#define MPI_Keyval_create_U MPI_KEYVAL_CREATE
/** All uppercase name of MPI_Keyval_free */
#define MPI_Keyval_free_U MPI_KEYVAL_FREE

/**
 * @}
 * @name Lowercase names
 * @{
 */
/** All lowercase name of MPI_Comm_compare */
#define MPI_Comm_compare_L mpi_comm_compare
/** All lowercase name of MPI_Comm_create */
#define MPI_Comm_create_L mpi_comm_create
/** All lowercase name of MPI_Comm_dup */
#define MPI_Comm_dup_L mpi_comm_dup
/** All lowercase name of MPI_Comm_free */
#define MPI_Comm_free_L mpi_comm_free
/** All lowercase name of MPI_Comm_group */
#define MPI_Comm_group_L mpi_comm_group
/** All lowercase name of MPI_Comm_rank */
#define MPI_Comm_rank_L mpi_comm_rank
/** All lowercase name of MPI_Comm_remote_group */
#define MPI_Comm_remote_group_L mpi_comm_remote_group
/** All lowercase name of MPI_Comm_remote_size */
#define MPI_Comm_remote_size_L mpi_comm_remote_size
/** All lowercase name of MPI_Comm_size */
#define MPI_Comm_size_L mpi_comm_size
/** All lowercase name of MPI_Comm_split */
#define MPI_Comm_split_L mpi_comm_split
/** All lowercase name of MPI_Comm_test_inter */
#define MPI_Comm_test_inter_L mpi_comm_test_inter
/** All lowercase name of MPI_Group_compare */
#define MPI_Group_compare_L mpi_group_compare
/** All lowercase name of MPI_Group_difference */
#define MPI_Group_difference_L mpi_group_difference
/** All lowercase name of MPI_Group_excl */
#define MPI_Group_excl_L mpi_group_excl
/** All lowercase name of MPI_Group_free */
#define MPI_Group_free_L mpi_group_free
/** All lowercase name of MPI_Group_incl */
#define MPI_Group_incl_L mpi_group_incl
/** All lowercase name of MPI_Group_intersection */
#define MPI_Group_intersection_L mpi_group_intersection
/** All lowercase name of MPI_Group_range_excl */
#define MPI_Group_range_excl_L mpi_group_range_excl
/** All lowercase name of MPI_Group_range_incl */
#define MPI_Group_range_incl_L mpi_group_range_incl
/** All lowercase name of MPI_Group_rank */
#define MPI_Group_rank_L mpi_group_rank
/** All lowercase name of MPI_Group_size */
#define MPI_Group_size_L mpi_group_size
/** All lowercase name of MPI_Group_translate_ranks */
#define MPI_Group_translate_ranks_L mpi_group_translate_ranks
/** All lowercase name of MPI_Group_union */
#define MPI_Group_union_L mpi_group_union
/** All lowercase name of MPI_Intercomm_create */
#define MPI_Intercomm_create_L mpi_intercomm_create
/** All lowercase name of MPI_Intercomm_merge */
#define MPI_Intercomm_merge_L mpi_intercomm_merge

/** All lowercase name of MPI_Comm_call_errhandler */
#define MPI_Comm_call_errhandler_L mpi_comm_call_errhandler
/** All lowercase name of MPI_Comm_create_errhandler */
#define MPI_Comm_create_errhandler_L mpi_comm_create_errhandler
/** All lowercase name of MPI_Comm_get_errhandler */
#define MPI_Comm_get_errhandler_L mpi_comm_get_errhandler
/** All lowercase name of MPI_Comm_set_errhandler */
#define MPI_Comm_set_errhandler_L mpi_comm_set_errhandler

/** All lowercase name of MPI_Attr_delete */
#define MPI_Attr_delete_L mpi_attr_delete
/** All lowercase name of MPI_Attr_get */
#define MPI_Attr_get_L mpi_attr_get
/** All lowercase name of MPI_Attr_put */
#define MPI_Attr_put_L mpi_attr_put
/** All lowercase name of MPI_Comm_create_keyval */
#define MPI_Comm_create_keyval_L mpi_comm_create_keyval
/** All lowercase name of MPI_Comm_delete_attr */
#define MPI_Comm_delete_attr_L mpi_comm_delete_attr
/** All lowercase name of MPI_Comm_free_keyval */
#define MPI_Comm_free_keyval_L mpi_comm_free_keyval
/** All lowercase name of MPI_Comm_get_attr */
#define MPI_Comm_get_attr_L mpi_comm_get_attr
/** All lowercase name of MPI_Comm_get_name */
#define MPI_Comm_get_name_L mpi_comm_get_name
/** All lowercase name of MPI_Comm_set_attr */
#define MPI_Comm_set_attr_L mpi_comm_set_attr
/** All lowercase name of MPI_Comm_set_name */
#define MPI_Comm_set_name_L mpi_comm_set_name
/** All lowercase name of MPI_Keyval_create */
#define MPI_Keyval_create_L mpi_keyval_create
/** All lowercase name of MPI_Keyval_free */
#define MPI_Keyval_free_L mpi_keyval_free

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

/**
 * @}
 * @name Fortran wrappers for communicator constructors
 * @{
 */

#if defined(HAS_MPI_COMM_CREATE)
/**
 * Measurement wrapper for MPI_Comm_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_create)(MPI_Comm*  comm,
                           MPI_Group* group,
                           MPI_Comm*  newcomm,
                           int*       ierr)
{
  *ierr = MPI_Comm_create(*comm, *group, newcomm);
}

#endif
#if defined(HAS_MPI_COMM_DUP)
/**
 * Measurement wrapper for MPI_Comm_dup
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_dup)(MPI_Comm* comm,
                        MPI_Comm* newcomm,
                        int*      ierr)
{
  *ierr = MPI_Comm_dup(*comm, newcomm);
}

#endif
#if defined(HAS_MPI_COMM_SPLIT)
/**
 * Measurement wrapper for MPI_Comm_split
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_split)(MPI_Comm* comm,
                          int*      color,
                          int*      key,
                          MPI_Comm* newcomm,
                          int*      ierr)
{
  *ierr = MPI_Comm_split(*comm, *color, *key, newcomm);
}

#endif
#if defined(HAS_MPI_INTERCOMM_CREATE)
/**
 * Measurement wrapper for MPI_Intercomm_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Intercomm_create)(MPI_Comm* local_comm,
                                int*      local_leader,
                                MPI_Comm* peer_comm,
                                int*      remote_leader,
                                int*      tag,
                                MPI_Comm* newcomm,
                                int*      ierr)
{
  *ierr = MPI_Intercomm_create(*local_comm,
                               *local_leader,
                               *peer_comm,
                               *remote_leader,
                               *tag,
                               newcomm);
}

#endif
#if defined(HAS_MPI_INTERCOMM_MERGE)
/**
 * Measurement wrapper for MPI_Intercomm_merge
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Intercomm_merge)(MPI_Comm* intercomm,
                               int*      high,
                               MPI_Comm* newcomm,
                               int*      ierr)
{
  *ierr = MPI_Intercomm_merge(*intercomm, *high, newcomm);
}

#endif

/**
 * @}
 * @name Fortran wrappers for communicator destructors
 * @{
 */

#if defined(HAS_MPI_COMM_FREE)
/**
 * Measurement wrapper for MPI_Comm_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_free)(MPI_Comm* comm,
                         int*      ierr)
{
  *ierr = MPI_Comm_free(comm);
}

#endif

/**
 * @}
 * @name Fortran wrappers for group constructors
 * @{
 */

#if defined(HAS_MPI_GROUP_DIFFERENCE)
/**
 * Measurement wrapper for MPI_Group_difference
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_difference)(MPI_Group* group1,
                                MPI_Group* group2,
                                MPI_Group* newgroup,
                                int*       ierr)
{
  *ierr = MPI_Group_difference(*group1, *group2, newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_EXCL)
/**
 * Measurement wrapper for MPI_Group_excl
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_excl)(MPI_Group* group,
                          int*       n,
                          int*       ranks,
                          MPI_Group* newgroup,
                          int*       ierr)
{
  *ierr = MPI_Group_excl(*group, *n, ranks, newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_INCL)
/**
 * Measurement wrapper for MPI_Group_incl
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_incl)(MPI_Group* group,
                          int*       n,
                          int*       ranks,
                          MPI_Group* newgroup,
                          int*       ierr)
{
  *ierr = MPI_Group_incl(*group, *n, ranks, newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_INTERSECTION)
/**
 * Measurement wrapper for MPI_Group_intersection
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_intersection)(MPI_Group* group1,
                                  MPI_Group* group2,
                                  MPI_Group* newgroup,
                                  int*       ierr)
{
  *ierr = MPI_Group_intersection(*group1, *group2, newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_RANGE_EXCL)
/**
 * Measurement wrapper for MPI_Group_range_excl
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_range_excl)(MPI_Group* group,
                                int*       n,
                                int        ranges[][3],
                                MPI_Group* newgroup,
                                int*       ierr)
{
  *ierr = MPI_Group_range_excl(*group, *n, ranges, newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_RANGE_INCL)
/**
 * Measurement wrapper for MPI_Group_range_incl
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_range_incl)(MPI_Group* group,
                                int*       n,
                                int        ranges[][3],
                                MPI_Group* newgroup,
                                int*       ierr)
{
  *ierr = MPI_Group_range_incl(*group, *n, ranges, newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_UNION)
/**
 * Measurement wrapper for MPI_Group_union
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_union)(MPI_Group* group1,
                           MPI_Group* group2,
                           MPI_Group* newgroup,
                           int*       ierr)
{
  *ierr = MPI_Group_union(*group1, *group2, newgroup);
}

#endif
#if defined(HAS_MPI_COMM_GROUP)
/**
 * Measurement wrapper for MPI_Comm_group
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_group)(MPI_Comm*  comm,
                          MPI_Group* group,
                          int*       ierr)
{
  *ierr = MPI_Comm_group(*comm, group);
}

#endif
#if defined(HAS_MPI_COMM_REMOTE_GROUP)
/**
 * Measurement wrapper for MPI_Comm_remote_group
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_remote_group)(MPI_Comm*  comm,
                                 MPI_Group* group,
                                 int*       ierr)
{
  *ierr = MPI_Comm_remote_group(*comm, group);
}

#endif

/**
 * @}
 * @name Fortran wrappers for group destructors
 * @{
 */

#if defined(HAS_MPI_GROUP_FREE)
/**
 * Measurement wrapper for MPI_Group_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_free)(MPI_Group* group,
                          int*       ierr)
{
  *ierr = MPI_Group_free(group);
}

#endif

/**
 * @}
 * @name Fortran wrappers for miscelaneous functions
 * @{
 */

#if defined(HAS_MPI_COMM_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_compare
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_compare)(MPI_Comm* comm1,
                            MPI_Comm* comm2,
                            int*      result,
                            int*      ierr)
{
  *ierr = MPI_Comm_compare(*comm1, *comm2, result);
}

#endif
#if defined(HAS_MPI_COMM_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_rank
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_rank)(MPI_Comm* comm,
                         int*      rank,
                         int*      ierr)
{
  *ierr = MPI_Comm_rank(*comm, rank);
}

#endif
#if defined(HAS_MPI_COMM_REMOTE_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_remote_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_remote_size)(MPI_Comm* comm,
                                int*      size,
                                int*      ierr)
{
  *ierr = MPI_Comm_remote_size(*comm, size);
}

#endif
#if defined(HAS_MPI_COMM_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_size)(MPI_Comm* comm,
                         int*      size,
                         int*      ierr)
{
  *ierr = MPI_Comm_size(*comm, size);
}

#endif
#if defined(HAS_MPI_COMM_TEST_INTER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_test_inter
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_test_inter)(MPI_Comm* comm,
                               int*      flag,
                               int*      ierr)
{
  *ierr = MPI_Comm_test_inter(*comm, flag);
}

#endif
#if defined(HAS_MPI_GROUP_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_compare
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_compare)(MPI_Group* group1,
                             MPI_Group* group2,
                             int*       result,
                             int*       ierr)
{
  *ierr = MPI_Group_compare(*group1, *group2, result);
}

#endif
#if defined(HAS_MPI_GROUP_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_rank
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_rank)(MPI_Group* group,
                          int*       rank,
                          int*       ierr)
{
  *ierr = MPI_Group_rank(*group, rank);
}

#endif
#if defined(HAS_MPI_GROUP_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_size)(MPI_Group* group,
                          int*       size,
                          int*       ierr)
{
  *ierr = MPI_Group_size(*group, size);
}

#endif
#if defined(HAS_MPI_GROUP_TRANSLATE_RANKS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_translate_ranks
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_translate_ranks)(MPI_Group* group1,
                                     int*       n,
                                     int*       ranks1,
                                     MPI_Group* group2,
                                     int*       ranks2,
                                     int*       ierr)
{
  *ierr = MPI_Group_translate_ranks(*group1, *n, ranks1, *group2, ranks2);
}

#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if defined(HAS_MPI_COMM_CALL_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_call_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_call_errhandler)(MPI_Comm* comm,
                                    int*      errorcode,
                                    int*      ierr)
{
  *ierr = MPI_Comm_call_errhandler(*comm, *errorcode);
}

#endif
#if defined(HAS_MPI_COMM_CREATE_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_create_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_create_errhandler)(MPI_Comm_errhandler_fn* function,
                                      MPI_Errhandler*         errhandler,
                                      int*                    ierr)
{
  *ierr = MPI_Comm_create_errhandler(function, errhandler);
}

#endif
#if defined(HAS_MPI_COMM_GET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_get_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_get_errhandler)(MPI_Comm*       comm,
                                   MPI_Errhandler* errhandler,
                                   int*            ierr)
{
  *ierr = MPI_Comm_get_errhandler(*comm, errhandler);
}

#endif
#if defined(HAS_MPI_COMM_SET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_set_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_set_errhandler)(MPI_Comm*       comm,
                                   MPI_Errhandler* errhandler,
                                   int*            ierr)
{
  *ierr = MPI_Comm_set_errhandler(*comm, *errhandler);
}

#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if defined(HAS_MPI_ATTR_DELETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_delete
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Attr_delete)(MPI_Comm* comm,
                           int*      keyval,
                           int*      ierr)
{
  *ierr = MPI_Attr_delete(*comm, *keyval);
}

#endif
#if defined(HAS_MPI_ATTR_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_get
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Attr_get)(MPI_Comm* comm,
                        int*      keyval,
                        void*     attribute_val,
                        int*      flag,
                        int*      ierr)
{
  *ierr = MPI_Attr_get(*comm, *keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_ATTR_PUT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_put
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Attr_put)(MPI_Comm* comm,
                        int*      keyval,
                        void*     attribute_val,
                        int*      ierr)
{
  *ierr = MPI_Attr_put(*comm, *keyval, attribute_val);
}

#endif
#if defined(HAS_MPI_COMM_CREATE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_create_keyval
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_create_keyval)(
  MPI_Comm_copy_attr_function* comm_copy_attr_fn,
  MPI_Comm_delete_attr_function*
  comm_delete_attr_fn,
  int*
  comm_keyval,
  void*
  extra_state,
  int*                         ierr)
{
  *ierr = MPI_Comm_create_keyval(comm_copy_attr_fn,
                                 comm_delete_attr_fn,
                                 comm_keyval,
                                 extra_state);
}

#endif
#if defined(HAS_MPI_COMM_DELETE_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_delete_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_delete_attr)(MPI_Comm* comm,
                                int*      comm_keyval,
                                int*      ierr)
{
  *ierr = MPI_Comm_delete_attr(*comm, *comm_keyval);
}

#endif
#if defined(HAS_MPI_COMM_FREE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_free_keyval
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_free_keyval)(int* comm_keyval,
                                int* ierr)
{
  *ierr = MPI_Comm_free_keyval(comm_keyval);
}

#endif
#if defined(HAS_MPI_COMM_GET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_get_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_get_attr)(MPI_Comm* comm,
                             int*      comm_keyval,
                             void*     attribute_val,
                             int*      flag,
                             int*      ierr)
{
  *ierr = MPI_Comm_get_attr(*comm, *comm_keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_COMM_GET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_get_name
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_get_name)(MPI_Comm* comm,
                             char*     comm_name,
                             int*      resultlen,
                             int*      ierr,
                             int       comm_name_len)
{
  char* c_comm_name     = NULL;
  int   c_comm_name_len = 0;

  c_comm_name = (char*)malloc((comm_name_len + 1) * sizeof (char));
  if (!c_comm_name)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Comm_get_name(*comm, c_comm_name, resultlen);

  c_comm_name_len = strlen(c_comm_name);
  strncpy(comm_name, c_comm_name, c_comm_name_len);
  memset(comm_name + c_comm_name_len, ' ', comm_name_len - c_comm_name_len);
  free(c_comm_name);
}

#endif
#if defined(HAS_MPI_COMM_SET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_set_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_set_attr)(MPI_Comm* comm,
                             int*      comm_keyval,
                             void*     attribute_val,
                             int*      ierr)
{
  *ierr = MPI_Comm_set_attr(*comm, *comm_keyval, attribute_val);
}

#endif
#if defined(HAS_MPI_COMM_SET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_set_name
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_set_name)(MPI_Comm* comm,
                             char*     comm_name,
                             int*      ierr,
                             int       comm_name_len)
{
  char* c_comm_name     = NULL;
  int   c_comm_name_len = comm_name_len;

  while ((c_comm_name_len > 0) && (comm_name[c_comm_name_len - 1] == ' '))
  {
    c_comm_name_len--;
  }
  c_comm_name = (char*)malloc((comm_name_len + 1) * sizeof (char));
  if (!c_comm_name)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_comm_name, comm_name, comm_name_len);
  c_comm_name[c_comm_name_len] = '\0';

  *ierr = MPI_Comm_set_name(*comm, c_comm_name);

  free(c_comm_name);
}

#endif
#if defined(HAS_MPI_KEYVAL_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Keyval_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Keyval_create)(MPI_Copy_function*   copy_fn,
                             MPI_Delete_function* delete_fn,
                             int*                 keyval,
                             void*                extra_state,
                             int*                 ierr)
{
  *ierr = MPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);
}

#endif
#if defined(HAS_MPI_KEYVAL_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Keyval_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Keyval_free)(int* keyval,
                           int* ierr)
{
  *ierr = MPI_Keyval_free(keyval);
}

#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */

#else /* !NEED_F2C_CONV */

/**
 * @}
 * @name Fortran wrappers for communicator constructors
 * @{
 */

#if defined(HAS_MPI_COMM_CREATE)
/**
 * Measurement wrapper for MPI_Comm_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_create)(MPI_Fint* comm,
                           MPI_Fint* group,
                           MPI_Fint* newcomm,
                           MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  *ierr = MPI_Comm_create(PMPI_Comm_f2c(*comm), PMPI_Group_f2c(
                            *group), &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_COMM_DUP)
/**
 * Measurement wrapper for MPI_Comm_dup
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_dup)(MPI_Fint* comm,
                        MPI_Fint* newcomm,
                        MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  *ierr = MPI_Comm_dup(PMPI_Comm_f2c(*comm), &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_COMM_SPLIT)
/**
 * Measurement wrapper for MPI_Comm_split
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_split)(MPI_Fint* comm,
                          MPI_Fint* color,
                          MPI_Fint* key,
                          MPI_Fint* newcomm,
                          MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  *ierr = MPI_Comm_split(PMPI_Comm_f2c(*comm), *color, *key, &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_INTERCOMM_CREATE)
/**
 * Measurement wrapper for MPI_Intercomm_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Intercomm_create)(MPI_Fint* local_comm,
                                MPI_Fint* local_leader,
                                MPI_Fint* peer_comm,
                                MPI_Fint* remote_leader,
                                MPI_Fint* tag,
                                MPI_Fint* newcomm,
                                MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  *ierr = MPI_Intercomm_create(PMPI_Comm_f2c(
                                 *local_comm), *local_leader,
                               PMPI_Comm_f2c(
                                 *peer_comm), *remote_leader, *tag,
                               &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif
#if defined(HAS_MPI_INTERCOMM_MERGE)
/**
 * Measurement wrapper for MPI_Intercomm_merge
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Intercomm_merge)(MPI_Fint* intercomm,
                               MPI_Fint* high,
                               MPI_Fint* newcomm,
                               MPI_Fint* ierr)
{
  MPI_Comm c_newcomm;

  *ierr = MPI_Intercomm_merge(PMPI_Comm_f2c(*intercomm), *high, &c_newcomm);

  *newcomm = PMPI_Comm_c2f(c_newcomm);
}

#endif

/**
 * @}
 * @name Fortran wrappers for communicator destructors
 * @{
 */

#if defined(HAS_MPI_COMM_FREE)
/**
 * Measurement wrapper for MPI_Comm_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_free)(MPI_Fint* comm,
                         MPI_Fint* ierr)
{
  MPI_Comm c_comm = PMPI_Comm_f2c(*comm);

  *ierr = MPI_Comm_free(&c_comm);

  *comm = PMPI_Comm_c2f(c_comm);
}

#endif

/**
 * @}
 * @name Fortran wrappers for group constructors
 * @{
 */

#if defined(HAS_MPI_GROUP_DIFFERENCE)
/**
 * Measurement wrapper for MPI_Group_difference
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_difference)(MPI_Fint* group1,
                                MPI_Fint* group2,
                                MPI_Fint* newgroup,
                                MPI_Fint* ierr)
{
  MPI_Group c_newgroup;

  *ierr =
    MPI_Group_difference(PMPI_Group_f2c(*group1), PMPI_Group_f2c(
                           *group2), &c_newgroup);

  *newgroup = PMPI_Group_c2f(c_newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_EXCL)
/**
 * Measurement wrapper for MPI_Group_excl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_excl)(MPI_Fint* group,
                          MPI_Fint* n,
                          MPI_Fint* ranks,
                          MPI_Fint* newgroup,
                          MPI_Fint* ierr)
{
  MPI_Group c_newgroup;

  *ierr = MPI_Group_excl(PMPI_Group_f2c(*group), *n, ranks, &c_newgroup);

  *newgroup = PMPI_Group_c2f(c_newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_INCL)
/**
 * Measurement wrapper for MPI_Group_incl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_incl)(MPI_Fint* group,
                          MPI_Fint* n,
                          MPI_Fint* ranks,
                          MPI_Fint* newgroup,
                          MPI_Fint* ierr)
{
  MPI_Group c_newgroup;

  *ierr = MPI_Group_incl(PMPI_Group_f2c(*group), *n, ranks, &c_newgroup);

  *newgroup = PMPI_Group_c2f(c_newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_INTERSECTION)
/**
 * Measurement wrapper for MPI_Group_intersection
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_intersection)(MPI_Fint* group1,
                                  MPI_Fint* group2,
                                  MPI_Fint* newgroup,
                                  MPI_Fint* ierr)
{
  MPI_Group c_newgroup;

  *ierr =
    MPI_Group_intersection(PMPI_Group_f2c(*group1), PMPI_Group_f2c(
                             *group2), &c_newgroup);

  *newgroup = PMPI_Group_c2f(c_newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_RANGE_EXCL)
/**
 * Measurement wrapper for MPI_Group_range_excl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_range_excl)(MPI_Fint* group,
                                MPI_Fint* n,
                                MPI_Fint  ranges[][3],
                                MPI_Fint* newgroup,
                                MPI_Fint* ierr)
{
  MPI_Group c_newgroup;

  *ierr = MPI_Group_range_excl(PMPI_Group_f2c(
                                 *group), *n, (int(*)[3])ranges,
                               &c_newgroup);

  *newgroup = PMPI_Group_c2f(c_newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_RANGE_INCL)
/**
 * Measurement wrapper for MPI_Group_range_incl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_range_incl)(MPI_Fint* group,
                                MPI_Fint* n,
                                MPI_Fint  ranges[][3],
                                MPI_Fint* newgroup,
                                MPI_Fint* ierr)
{
  MPI_Group c_newgroup;

  *ierr = MPI_Group_range_incl(PMPI_Group_f2c(
                                 *group), *n, (int(*)[3])ranges,
                               &c_newgroup);

  *newgroup = PMPI_Group_c2f(c_newgroup);
}

#endif
#if defined(HAS_MPI_GROUP_UNION)
/**
 * Measurement wrapper for MPI_Group_union
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_union)(MPI_Fint* group1,
                           MPI_Fint* group2,
                           MPI_Fint* newgroup,
                           MPI_Fint* ierr)
{
  MPI_Group c_newgroup;

  *ierr = MPI_Group_union(PMPI_Group_f2c(*group1), PMPI_Group_f2c(
                            *group2), &c_newgroup);

  *newgroup = PMPI_Group_c2f(c_newgroup);
}

#endif
#if defined(HAS_MPI_COMM_GROUP)
/**
 * Measurement wrapper for MPI_Comm_group
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_group)(MPI_Fint* comm,
                          MPI_Fint* group,
                          MPI_Fint* ierr)
{
  MPI_Group c_group;

  *ierr = MPI_Comm_group(PMPI_Comm_f2c(*comm), &c_group);

  *group = PMPI_Group_c2f(c_group);
}

#endif
#if defined(HAS_MPI_COMM_REMOTE_GROUP)
/**
 * Measurement wrapper for MPI_Comm_remote_group
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_remote_group)(MPI_Fint* comm,
                                 MPI_Fint* group,
                                 MPI_Fint* ierr)
{
  MPI_Group c_group;

  *ierr = MPI_Comm_remote_group(PMPI_Comm_f2c(*comm), &c_group);

  *group = PMPI_Group_c2f(c_group);
}

#endif

/**
 * @}
 * @name Fortran wrappers for group destructors
 * @{
 */

#if defined(HAS_MPI_GROUP_FREE)
/**
 * Measurement wrapper for MPI_Group_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_free)(MPI_Fint* group,
                          MPI_Fint* ierr)
{
  MPI_Group c_group = PMPI_Group_f2c(*group);

  *ierr = MPI_Group_free(&c_group);

  *group = PMPI_Group_c2f(c_group);
}

#endif

/**
 * @}
 * @name Fortran wrappers for miscelaneous functions
 * @{
 */

#if defined(HAS_MPI_COMM_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_compare
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_compare)(MPI_Fint* comm1,
                            MPI_Fint* comm2,
                            MPI_Fint* result,
                            MPI_Fint* ierr)
{
  *ierr = MPI_Comm_compare(PMPI_Comm_f2c(*comm1), PMPI_Comm_f2c(
                             *comm2), result);
}

#endif
#if defined(HAS_MPI_COMM_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_rank
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_rank)(MPI_Fint* comm,
                         MPI_Fint* rank,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Comm_rank(PMPI_Comm_f2c(*comm), rank);
}

#endif
#if defined(HAS_MPI_COMM_REMOTE_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_remote_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_remote_size)(MPI_Fint* comm,
                                MPI_Fint* size,
                                MPI_Fint* ierr)
{
  *ierr = MPI_Comm_remote_size(PMPI_Comm_f2c(*comm), size);
}

#endif
#if defined(HAS_MPI_COMM_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_size)(MPI_Fint* comm,
                         MPI_Fint* size,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Comm_size(PMPI_Comm_f2c(*comm), size);
}

#endif
#if defined(HAS_MPI_COMM_TEST_INTER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_test_inter
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Comm_test_inter)(MPI_Fint* comm,
                               MPI_Fint* flag,
                               MPI_Fint* ierr)
{
  *ierr = MPI_Comm_test_inter(PMPI_Comm_f2c(*comm), flag);
}

#endif
#if defined(HAS_MPI_GROUP_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_compare
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_compare)(MPI_Fint* group1,
                             MPI_Fint* group2,
                             MPI_Fint* result,
                             MPI_Fint* ierr)
{
  *ierr =
    MPI_Group_compare(PMPI_Group_f2c(*group1), PMPI_Group_f2c(
                        *group2), result);
}

#endif
#if defined(HAS_MPI_GROUP_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_rank
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_rank)(MPI_Fint* group,
                          MPI_Fint* rank,
                          MPI_Fint* ierr)
{
  *ierr = MPI_Group_rank(PMPI_Group_f2c(*group), rank);
}

#endif
#if defined(HAS_MPI_GROUP_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_size)(MPI_Fint* group,
                          MPI_Fint* size,
                          MPI_Fint* ierr)
{
  *ierr = MPI_Group_size(PMPI_Group_f2c(*group), size);
}

#endif
#if defined(HAS_MPI_GROUP_TRANSLATE_RANKS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_translate_ranks
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
void FSUB(MPI_Group_translate_ranks)(MPI_Fint* group1,
                                     MPI_Fint* n,
                                     MPI_Fint* ranks1,
                                     MPI_Fint* group2,
                                     MPI_Fint* ranks2,
                                     MPI_Fint* ierr)
{
  *ierr = MPI_Group_translate_ranks(PMPI_Group_f2c(
                                      *group1), *n, ranks1,
                                    PMPI_Group_f2c(*group2), ranks2);
}

#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if defined(HAS_MPI_COMM_CALL_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_call_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_call_errhandler)(MPI_Fint* comm,
                                    MPI_Fint* errorcode,
                                    MPI_Fint* ierr)
{
  *ierr = MPI_Comm_call_errhandler(PMPI_Comm_f2c(*comm), *errorcode);
}

#endif
#if defined(HAS_MPI_COMM_CREATE_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_create_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_create_errhandler)(void*     function,
                                      void*     errhandler,
                                      MPI_Fint* ierr)
{
  *ierr =
    MPI_Comm_create_errhandler((MPI_Comm_errhandler_fn*)function,
                               (MPI_Errhandler*)errhandler);
}

#endif
#if defined(HAS_MPI_COMM_GET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_get_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_get_errhandler)(MPI_Fint* comm,
                                   void*     errhandler,
                                   MPI_Fint* ierr)
{
  *ierr = MPI_Comm_get_errhandler(PMPI_Comm_f2c(
                                    *comm), (MPI_Errhandler*)errhandler);
}

#endif
#if defined(HAS_MPI_COMM_SET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_set_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
void FSUB(MPI_Comm_set_errhandler)(MPI_Fint* comm,
                                   void*     errhandler,
                                   MPI_Fint* ierr)
{
  MPI_Comm c_comm = PMPI_Comm_f2c(*comm);

  *ierr = MPI_Comm_set_errhandler(c_comm, *((MPI_Errhandler*)errhandler));

  *comm = PMPI_Comm_c2f(c_comm);
}

#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if defined(HAS_MPI_ATTR_DELETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_delete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Attr_delete)(MPI_Fint* comm,
                           MPI_Fint* keyval,
                           MPI_Fint* ierr)
{
  *ierr = MPI_Attr_delete(PMPI_Comm_f2c(*comm), *keyval);
}

#endif
#if defined(HAS_MPI_ATTR_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Attr_get)(MPI_Fint* comm,
                        MPI_Fint* keyval,
                        void*     attribute_val,
                        MPI_Fint* flag,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Attr_get(PMPI_Comm_f2c(*comm), *keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_ATTR_PUT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_put
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Attr_put)(MPI_Fint* comm,
                        MPI_Fint* keyval,
                        void*     attribute_val,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Attr_put(PMPI_Comm_f2c(*comm), *keyval, attribute_val);
}

#endif
#if defined(HAS_MPI_COMM_CREATE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_create_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_create_keyval)(void*     comm_copy_attr_fn,
                                  void*     comm_delete_attr_fn,
                                  MPI_Fint* comm_keyval,
                                  void*     extra_state,
                                  MPI_Fint* ierr)
{
  *ierr = MPI_Comm_create_keyval(
    (MPI_Comm_copy_attr_function*)comm_copy_attr_fn,
    (MPI_Comm_delete_attr_function*)comm_delete_attr_fn,
    comm_keyval,
    extra_state);
}

#endif
#if defined(HAS_MPI_COMM_DELETE_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_delete_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_delete_attr)(MPI_Fint* comm,
                                MPI_Fint* comm_keyval,
                                MPI_Fint* ierr)
{
  MPI_Comm c_comm = PMPI_Comm_f2c(*comm);

  *ierr = MPI_Comm_delete_attr(c_comm, *comm_keyval);

  *comm = PMPI_Comm_c2f(c_comm);
}

#endif
#if defined(HAS_MPI_COMM_FREE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_free_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_free_keyval)(MPI_Fint* comm_keyval,
                                MPI_Fint* ierr)
{
  *ierr = MPI_Comm_free_keyval(comm_keyval);
}

#endif
#if defined(HAS_MPI_COMM_GET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_get_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_get_attr)(MPI_Fint* comm,
                             MPI_Fint* comm_keyval,
                             void*     attribute_val,
                             MPI_Fint* flag,
                             MPI_Fint* ierr)
{
  *ierr = MPI_Comm_get_attr(PMPI_Comm_f2c(
                              *comm), *comm_keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_COMM_GET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_get_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_get_name)(MPI_Fint* comm,
                             char*     comm_name,
                             MPI_Fint* resultlen,
                             MPI_Fint* ierr,
                             int       comm_name_len)
{
  char* c_comm_name     = NULL;
  int   c_comm_name_len = 0;

  c_comm_name = (char*)malloc((comm_name_len + 1) * sizeof (char));
  if (!c_comm_name)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Comm_get_name(PMPI_Comm_f2c(*comm), c_comm_name, resultlen);

  c_comm_name_len = strlen(c_comm_name);
  strncpy(comm_name, c_comm_name, c_comm_name_len);
  memset(comm_name + c_comm_name_len, ' ', comm_name_len - c_comm_name_len);
  free(c_comm_name);
}

#endif
#if defined(HAS_MPI_COMM_SET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_set_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_set_attr)(MPI_Fint* comm,
                             MPI_Fint* comm_keyval,
                             void*     attribute_val,
                             MPI_Fint* ierr)
{
  MPI_Comm c_comm = PMPI_Comm_f2c(*comm);

  *ierr = MPI_Comm_set_attr(c_comm, *comm_keyval, attribute_val);

  *comm = PMPI_Comm_c2f(c_comm);
}

#endif
#if defined(HAS_MPI_COMM_SET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_set_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Comm_set_name)(MPI_Fint* comm,
                             char*     comm_name,
                             MPI_Fint* ierr,
                             int       comm_name_len)
{
  MPI_Comm c_comm          = PMPI_Comm_f2c(*comm);
  char*    c_comm_name     = NULL;
  int      c_comm_name_len = comm_name_len;

  while ((c_comm_name_len > 0) && (comm_name[c_comm_name_len - 1] == ' '))
  {
    c_comm_name_len--;
  }
  c_comm_name = (char*)malloc((comm_name_len + 1) * sizeof (char));
  if (!c_comm_name)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_comm_name, comm_name, comm_name_len);
  c_comm_name[c_comm_name_len] = '\0';

  *ierr = MPI_Comm_set_name(c_comm, c_comm_name);

  *comm = PMPI_Comm_c2f(c_comm);
  free(c_comm_name);
}

#endif
#if defined(HAS_MPI_KEYVAL_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Keyval_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Keyval_create)(void*     copy_fn,
                             void*     delete_fn,
                             MPI_Fint* keyval,
                             void*     extra_state,
                             MPI_Fint* ierr)
{
  *ierr =
    MPI_Keyval_create((MPI_Copy_function*)copy_fn,
                      (MPI_Delete_function*)delete_fn,
                      keyval,
                      extra_state);
}

#endif
#if defined(HAS_MPI_KEYVAL_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Keyval_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
void FSUB(MPI_Keyval_free)(MPI_Fint* keyval,
                           MPI_Fint* ierr)
{
  *ierr = MPI_Keyval_free(keyval);
}

#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */

#endif /* !NEED_F2C_CONV */

/**
 * @}
 */
