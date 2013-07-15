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
 * @file  epk_mpiwrap_cg.c
 *
 * @brief C interface wrappers for communicator and group functions
 */

#include <mpi.h>

#include "esd_event.h"

#include "epk_defs_mpi.h"
#include "epk_mpicom.h"
#include "epk_mpiwrap.h"

/**
 * @name C wrappers for communicator constructors
 * @{
 */

#if defined(HAS_MPI_COMM_CREATE)
/**
 * Measurement wrapper for MPI_Comm_create
 * @note Auto-generated by wrapgen from template: comm_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_create(MPI_Comm  comm,
                    MPI_Group group,
                    MPI_Comm* newcomm)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_CREATE]);
  }

  return_val = PMPI_Comm_create(comm, group, newcomm);
  if (*newcomm != MPI_COMM_NULL)
  {
    epk_comm_create(*newcomm);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_COMM_CREATE]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

#if defined(HAS_MPI_COMM_DUP)
/**
 * Measurement wrapper for MPI_Comm_dup
 * @note Auto-generated by wrapgen from template: comm_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_dup(MPI_Comm  comm,
                 MPI_Comm* newcomm)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_DUP]);
  }

  return_val = PMPI_Comm_dup(comm, newcomm);
  if (*newcomm != MPI_COMM_NULL)
  {
    epk_comm_create(*newcomm);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_COMM_DUP]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

#if defined(HAS_MPI_COMM_SPLIT)
/**
 * Measurement wrapper for MPI_Comm_split
 * @note Auto-generated by wrapgen from template: comm_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_split(MPI_Comm  comm,
                   int       color,
                   int       key,
                   MPI_Comm* newcomm)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_SPLIT]);
  }

  return_val = PMPI_Comm_split(comm, color, key, newcomm);
  if (*newcomm != MPI_COMM_NULL)
  {
    epk_comm_create(*newcomm);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_COMM_SPLIT]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

#if defined(HAS_MPI_INTERCOMM_CREATE)
/**
 * Measurement wrapper for MPI_Intercomm_create
 * @note Auto-generated by wrapgen from template: comm_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Intercomm_create(MPI_Comm  local_comm,
                         int       local_leader,
                         MPI_Comm  peer_comm,
                         int       remote_leader,
                         int       tag,
                         MPI_Comm* newcomm)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_INTERCOMM_CREATE]);
  }

  return_val = PMPI_Intercomm_create(local_comm,
                                     local_leader,
                                     peer_comm,
                                     remote_leader,
                                     tag,
                                     newcomm);
  if (*newcomm != MPI_COMM_NULL)
  {
    epk_comm_create(*newcomm);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_INTERCOMM_CREATE]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

#if defined(HAS_MPI_INTERCOMM_MERGE)
/**
 * Measurement wrapper for MPI_Intercomm_merge
 * @note Auto-generated by wrapgen from template: comm_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Intercomm_merge(MPI_Comm  intercomm,
                        int       high,
                        MPI_Comm* newcomm)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_INTERCOMM_MERGE]);
  }

  return_val = PMPI_Intercomm_merge(intercomm, high, newcomm);
  if (*newcomm != MPI_COMM_NULL)
  {
    epk_comm_create(*newcomm);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_INTERCOMM_MERGE]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

/**
 * @}
 * @name C wrappers for communicator destructors
 * @{
 */

#if defined(HAS_MPI_COMM_FREE)
/**
 * Measurement wrapper for MPI_Comm_free
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_free(MPI_Comm* comm)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_FREE]);
  }

  epk_comm_free(*comm);
  return_val = PMPI_Comm_free(comm);

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_COMM_FREE]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

/**
 * @}
 * @name C wrappers for group constructors
 * @{
 */

#if defined(HAS_MPI_GROUP_DIFFERENCE)
/**
 * Measurement wrapper for MPI_Group_difference
 * @note Auto-generated by wrapgen from template: group_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_difference(MPI_Group  group1,
                         MPI_Group  group2,
                         MPI_Group* newgroup)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_DIFFERENCE]);
  }

  return_val = PMPI_Group_difference(group1, group2, newgroup);
  if (*newgroup != MPI_GROUP_NULL)
  {
    epk_group_create(*newgroup);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_DIFFERENCE]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_EXCL)
/**
 * Measurement wrapper for MPI_Group_excl
 * @note Auto-generated by wrapgen from template: group_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_excl(MPI_Group  group,
                   int        n,
                   int*       ranks,
                   MPI_Group* newgroup)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_EXCL]);
  }

  return_val = PMPI_Group_excl(group, n, ranks, newgroup);
  if (*newgroup != MPI_GROUP_NULL)
  {
    epk_group_create(*newgroup);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_EXCL]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_INCL)
/**
 * Measurement wrapper for MPI_Group_incl
 * @note Auto-generated by wrapgen from template: group_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_incl(MPI_Group  group,
                   int        n,
                   int*       ranks,
                   MPI_Group* newgroup)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_INCL]);
  }

  return_val = PMPI_Group_incl(group, n, ranks, newgroup);
  if (*newgroup != MPI_GROUP_NULL)
  {
    epk_group_create(*newgroup);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_INCL]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_INTERSECTION)
/**
 * Measurement wrapper for MPI_Group_intersection
 * @note Auto-generated by wrapgen from template: group_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_intersection(MPI_Group  group1,
                           MPI_Group  group2,
                           MPI_Group* newgroup)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_INTERSECTION]);
  }

  return_val = PMPI_Group_intersection(group1, group2, newgroup);
  if (*newgroup != MPI_GROUP_NULL)
  {
    epk_group_create(*newgroup);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_INTERSECTION]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_RANGE_EXCL)
/**
 * Measurement wrapper for MPI_Group_range_excl
 * @note Auto-generated by wrapgen from template: group_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_range_excl(MPI_Group  group,
                         int        n,
                         int        ranges[][3],
                         MPI_Group* newgroup)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_RANGE_EXCL]);
  }

  return_val = PMPI_Group_range_excl(group, n, ranges, newgroup);
  if (*newgroup != MPI_GROUP_NULL)
  {
    epk_group_create(*newgroup);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_RANGE_EXCL]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_RANGE_INCL)
/**
 * Measurement wrapper for MPI_Group_range_incl
 * @note Auto-generated by wrapgen from template: group_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_range_incl(MPI_Group  group,
                         int        n,
                         int        ranges[][3],
                         MPI_Group* newgroup)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_RANGE_INCL]);
  }

  return_val = PMPI_Group_range_incl(group, n, ranges, newgroup);
  if (*newgroup != MPI_GROUP_NULL)
  {
    epk_group_create(*newgroup);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_RANGE_INCL]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_UNION)
/**
 * Measurement wrapper for MPI_Group_union
 * @note Auto-generated by wrapgen from template: group_mgnt.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_union(MPI_Group  group1,
                    MPI_Group  group2,
                    MPI_Group* newgroup)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_UNION]);
  }

  return_val = PMPI_Group_union(group1, group2, newgroup);
  if (*newgroup != MPI_GROUP_NULL)
  {
    epk_group_create(*newgroup);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_UNION]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_GROUP)
/**
 * Measurement wrapper for MPI_Comm_group
 * @note Auto-generated by wrapgen from template: group.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_group(MPI_Comm   comm,
                   MPI_Group* group)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_GROUP]);
  }

  return_val = PMPI_Comm_group(comm, group);
  if (*group != MPI_GROUP_NULL)
  {
    epk_group_create(*group);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_COMM_GROUP]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_REMOTE_GROUP)
/**
 * Measurement wrapper for MPI_Comm_remote_group
 * @note Auto-generated by wrapgen from template: group.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_remote_group(MPI_Comm   comm,
                          MPI_Group* group)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_REMOTE_GROUP]);
  }

  return_val = PMPI_Comm_remote_group(comm, group);
  if (*group != MPI_GROUP_NULL)
  {
    epk_group_create(*group);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_COMM_REMOTE_GROUP]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

/**
 * @}
 * @name C wrappers for group destructors
 * @{
 */

#if defined(HAS_MPI_GROUP_FREE)
/**
 * Measurement wrapper for MPI_Group_free
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_free(MPI_Group* group)
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(CG);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_FREE]);
  }

  epk_group_free(*group);
  return_val = PMPI_Group_free(group);

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_FREE]);
    EVENT_GEN_ON();
  }

  return return_val;
}

#endif

/**
 * @}
 * @name C wrappers for miscelaneous functions
 * @{
 */

#if defined(HAS_MPI_COMM_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_compare
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_compare(MPI_Comm comm1,
                     MPI_Comm comm2,
                     int*     result)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_COMPARE]);

    return_val = PMPI_Comm_compare(comm1, comm2, result);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_COMPARE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_compare(comm1, comm2, result);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_rank
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_rank(MPI_Comm comm,
                  int*     rank)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_RANK]);

    return_val = PMPI_Comm_rank(comm, rank);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_RANK]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_rank(comm, rank);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_REMOTE_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_remote_size
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_remote_size(MPI_Comm comm,
                         int*     size)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_REMOTE_SIZE]);

    return_val = PMPI_Comm_remote_size(comm, size);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_REMOTE_SIZE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_remote_size(comm, size);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_size
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_size(MPI_Comm comm,
                  int*     size)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_SIZE]);

    return_val = PMPI_Comm_size(comm, size);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_SIZE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_size(comm, size);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_TEST_INTER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Comm_test_inter
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Comm_test_inter(MPI_Comm comm,
                        int*     flag)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_TEST_INTER]);

    return_val = PMPI_Comm_test_inter(comm, flag);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_TEST_INTER]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_test_inter(comm, flag);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_compare
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_compare(MPI_Group group1,
                      MPI_Group group2,
                      int*      result)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_COMPARE]);

    return_val = PMPI_Group_compare(group1, group2, result);

    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_COMPARE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Group_compare(group1, group2, result);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_rank
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_rank(MPI_Group group,
                   int*      rank)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_RANK]);

    return_val = PMPI_Group_rank(group, rank);

    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_RANK]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Group_rank(group, rank);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_size
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_size(MPI_Group group,
                   int*      size)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_SIZE]);

    return_val = PMPI_Group_size(group, size);

    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_SIZE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Group_size(group, size);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_TRANSLATE_RANKS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
/**
 * Measurement wrapper for MPI_Group_translate_ranks
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg
 */
int MPI_Group_translate_ranks(MPI_Group group1,
                              int       n,
                              int*      ranks1,
                              MPI_Group group2,
                              int*      ranks2)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_TRANSLATE_RANKS]);

    return_val = PMPI_Group_translate_ranks(group1,
                                            n,
                                            ranks1,
                                            group2,
                                            ranks2);

    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_TRANSLATE_RANKS]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Group_translate_ranks(group1,
                                            n,
                                            ranks1,
                                            group2,
                                            ranks2);
  }

  return return_val;
}

#endif

/**
 * @}
 * @name C wrappers for error handling functions
 * @{
 */

#if defined(HAS_MPI_COMM_CALL_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_call_errhandler
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
int MPI_Comm_call_errhandler(MPI_Comm comm,
                             int      errorcode)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_ERR))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_CALL_ERRHANDLER]);

    return_val = PMPI_Comm_call_errhandler(comm, errorcode);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_CALL_ERRHANDLER]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_call_errhandler(comm, errorcode);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_CREATE_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_create_errhandler
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
int MPI_Comm_create_errhandler(MPI_Comm_errhandler_fn* function,
                               MPI_Errhandler*         errhandler)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_ERR))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_CREATE_ERRHANDLER]);

    return_val = PMPI_Comm_create_errhandler(function, errhandler);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_CREATE_ERRHANDLER]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_create_errhandler(function, errhandler);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_GET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_get_errhandler
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
int MPI_Comm_get_errhandler(MPI_Comm        comm,
                            MPI_Errhandler* errhandler)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_ERR))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_GET_ERRHANDLER]);

    return_val = PMPI_Comm_get_errhandler(comm, errhandler);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_GET_ERRHANDLER]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_get_errhandler(comm, errhandler);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_SET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Comm_set_errhandler
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_err
 */
int MPI_Comm_set_errhandler(MPI_Comm       comm,
                            MPI_Errhandler errhandler)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_ERR))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_SET_ERRHANDLER]);

    return_val = PMPI_Comm_set_errhandler(comm, errhandler);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_SET_ERRHANDLER]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_set_errhandler(comm, errhandler);
  }

  return return_val;
}

#endif

/**
 * @}
 * @name C wrappers for external interface functions
 * @{
 */

#if defined(HAS_MPI_ATTR_DELETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_delete
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
int MPI_Attr_delete(MPI_Comm comm,
                    int      keyval)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_ATTR_DELETE]);

    return_val = PMPI_Attr_delete(comm, keyval);

    esd_exit(epk_mpi_regid[EPK__MPI_ATTR_DELETE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Attr_delete(comm, keyval);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_ATTR_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_get
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
int MPI_Attr_get(MPI_Comm comm,
                 int      keyval,
                 void*    attribute_val,
                 int*     flag)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_ATTR_GET]);

    return_val = PMPI_Attr_get(comm, keyval, attribute_val, flag);

    esd_exit(epk_mpi_regid[EPK__MPI_ATTR_GET]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Attr_get(comm, keyval, attribute_val, flag);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_ATTR_PUT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Attr_put
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
int MPI_Attr_put(MPI_Comm comm,
                 int      keyval,
                 void*    attribute_val)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_ATTR_PUT]);

    return_val = PMPI_Attr_put(comm, keyval, attribute_val);

    esd_exit(epk_mpi_regid[EPK__MPI_ATTR_PUT]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Attr_put(comm, keyval, attribute_val);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_CREATE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_create_keyval
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
int MPI_Comm_create_keyval(
  MPI_Comm_copy_attr_function* comm_copy_attr_fn,
  MPI_Comm_delete_attr_function*
  comm_delete_attr_fn,
  int*                         comm_keyval,
  void*                        extra_state)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_CREATE_KEYVAL]);

    return_val = PMPI_Comm_create_keyval(comm_copy_attr_fn,
                                         comm_delete_attr_fn,
                                         comm_keyval,
                                         extra_state);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_CREATE_KEYVAL]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_create_keyval(comm_copy_attr_fn,
                                         comm_delete_attr_fn,
                                         comm_keyval,
                                         extra_state);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_DELETE_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_delete_attr
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
int MPI_Comm_delete_attr(MPI_Comm comm,
                         int      comm_keyval)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_DELETE_ATTR]);

    return_val = PMPI_Comm_delete_attr(comm, comm_keyval);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_DELETE_ATTR]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_delete_attr(comm, comm_keyval);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_FREE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_free_keyval
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
int MPI_Comm_free_keyval(int* comm_keyval)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_FREE_KEYVAL]);

    return_val = PMPI_Comm_free_keyval(comm_keyval);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_FREE_KEYVAL]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_free_keyval(comm_keyval);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_GET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_get_attr
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
int MPI_Comm_get_attr(MPI_Comm comm,
                      int      comm_keyval,
                      void*    attribute_val,
                      int*     flag)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_GET_ATTR]);

    return_val = PMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_GET_ATTR]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_GET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_get_name
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
int MPI_Comm_get_name(MPI_Comm comm,
                      char*    comm_name,
                      int*     resultlen)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_GET_NAME]);

    return_val = PMPI_Comm_get_name(comm, comm_name, resultlen);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_GET_NAME]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_get_name(comm, comm_name, resultlen);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_SET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_set_attr
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
int MPI_Comm_set_attr(MPI_Comm comm,
                      int      comm_keyval,
                      void*    attribute_val)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_SET_ATTR]);

    return_val = PMPI_Comm_set_attr(comm, comm_keyval, attribute_val);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_SET_ATTR]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_set_attr(comm, comm_keyval, attribute_val);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_SET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Comm_set_name
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_ext
 */
int MPI_Comm_set_name(MPI_Comm comm,
                      char*    comm_name)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_SET_NAME]);

    return_val = PMPI_Comm_set_name(comm, comm_name);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_SET_NAME]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_set_name(comm, comm_name);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_KEYVAL_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Keyval_create
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
int MPI_Keyval_create(MPI_Copy_function*   copy_fn,
                      MPI_Delete_function* delete_fn,
                      int*                 keyval,
                      void*                extra_state)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_KEYVAL_CREATE]);

    return_val = PMPI_Keyval_create(copy_fn,
                                    delete_fn,
                                    keyval,
                                    extra_state);

    esd_exit(epk_mpi_regid[EPK__MPI_KEYVAL_CREATE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Keyval_create(copy_fn,
                                    delete_fn,
                                    keyval,
                                    extra_state);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_KEYVAL_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Keyval_free
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup cg_ext
 */
int MPI_Keyval_free(int* keyval)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_EXT))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_KEYVAL_FREE]);

    return_val = PMPI_Keyval_free(keyval);

    esd_exit(epk_mpi_regid[EPK__MPI_KEYVAL_FREE]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Keyval_free(keyval);
  }

  return return_val;
}

#endif

/**
 * @}
 * @name C wrappers for handle conversion functions
 * @{
 */

#if defined(HAS_MPI_COMM_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Comm_c2f
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_misc
 */
MPI_Fint MPI_Comm_c2f(MPI_Comm comm)
{
  MPI_Fint return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_MISC))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_C2F]);

    return_val = PMPI_Comm_c2f(comm);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_C2F]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_c2f(comm);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_COMM_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Comm_f2c
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_misc
 */
MPI_Comm MPI_Comm_f2c(MPI_Fint comm)
{
  MPI_Comm return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_MISC))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_COMM_F2C]);

    return_val = PMPI_Comm_f2c(comm);

    esd_exit(epk_mpi_regid[EPK__MPI_COMM_F2C]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Comm_f2c(comm);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Group_c2f
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_misc
 */
MPI_Fint MPI_Group_c2f(MPI_Group group)
{
  MPI_Fint return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_MISC))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_C2F]);

    return_val = PMPI_Group_c2f(group);

    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_C2F]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Group_c2f(group);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_GROUP_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Group_f2c
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup cg_misc
 */
MPI_Group MPI_Group_f2c(MPI_Fint group)
{
  MPI_Group return_val;

  if (IS_EVENT_GEN_ON_FOR(CG_MISC))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_GROUP_F2C]);

    return_val = PMPI_Group_f2c(group);

    esd_exit(epk_mpi_regid[EPK__MPI_GROUP_F2C]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Group_f2c(group);
  }

  return return_val;
}

#endif

/**
 * @}
 */