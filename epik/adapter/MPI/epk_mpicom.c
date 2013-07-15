/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  epk_mpicom.c
 *
 * @brief Communicator, group, window and epoch management
 */

#include "epk_mpicom.h"
#include "epk_conf.h"
#include "elg_error.h"
#include "esd_def.h"

#include <stdlib.h>
#include <string.h>


/** Minimum required number of MPI handles/epochs to track simultaneously */
#define EPK_MPI_MINHANDLES   8


/*--- Global variables ----------------------------------------------------*/

/**
 * Flag indicating whether communicator and rank identifiers should be
 * determined. This (currently) only applies when tracing is configured.
 */
int epk_comm_determination = 0;


/*--- Static variables (for all handles/epochs) ---------------------------*/

/** @internal
 * Maximum number of MPI communicator/group/window handles and RMA epochs
 * that can be handled simultaneously.
 */
static int epk_mpi_handles = -1;


/*--- Helper function (for all handles/epochs) ----------------------------*/

/** @internal
 * Initializes global data related to handle/epoch tracking.
 */
static void epk_initialize_tracking()
{
  static int initialized = 0;

  /* Avoid double initialization */
  if (initialized)
    return;
  initialized = 1;

  /* Query requested number of handles and ensure minimum */
  epk_mpi_handles = epk_str2int(epk_get(EPK_MPI_HANDLES));
  if (epk_mpi_handles < EPK_MPI_MINHANDLES)
    {
      elg_warning("EPK_MPI_HANDLES=%d adjusted to minimum of %d",
                  epk_mpi_handles, EPK_MPI_MINHANDLES);
      epk_mpi_handles = EPK_MPI_MINHANDLES;
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * Group management
 *
 *---------------------------------------------------------------------------
 */

/*--- Data structures -----------------------------------------------------*/

/** @internal
 * Data structure used for tracking MPI groups.
 */
struct EpkGroup
{
  MPI_Group group;         /**< MPI group handle */
  elg_ui4   gid;           /**< EPIK group identifier */
  elg_ui4   refcnt;        /**< Reference count */
  elg_ui1   used_in_rma;   /**< Flag indicating usage in RMA operations */
};


/** @internal
 * Data structure used to hold information about the group of \c MPI_COMM_WORLD.
 */
struct EpkGroupWorld
{
  MPI_Group group;   /**< MPI group handle */
  int       size;    /**< Size of group */
  int*      ranks;   /**< Local |-@> global rank mapping */
};


/*--- Static variables ----------------------------------------------------*/

/** @internal
 * Flag identifying whether group tracking has been initialized.
 */
static int epk_group_initialized = 0;

/** @internal
 * Internal identifier of the next group defined on this process.
 */
static int epk_local_group_id = 0;

/** @internal
 * Index of next free entry in group tracking data structure.
 */
static int epk_last_group = 0;

/** @internal
 * Group tracking data structure.
 */
static struct EpkGroup* epk_groups = NULL;

/** @internal
 * Information about the group of \c MPI_COMM_WORLD.
 */
static struct EpkGroupWorld epk_group_world;

/** @internal
 * Internal buffer used for group rank translation.
 */
static int* epk_ranks = NULL;


/*--- Helper functions ----------------------------------------------------*/

/** @internal
 * Search the group tracking data structure for the entry of the given MPI
 * group handle \a group. Returns the index of the entry if found or -1
 * otherwise.
 */
static int epk_group_search(MPI_Group group)
{
  int i = 0;

  while ((i < epk_last_group) && (epk_groups[i].group != group))
    i++;

  if (i < epk_last_group)
    return i;

  return -1;
}


/** @internal
 * Create group definition record for entry at index \a idx in the
 * process-local \c epk_groups group tracking data structure.
 */
static void epk_group_define(int idx)
{
  int      size;
  int      rank;
  elg_ui1  group_flags = 0;
  elg_ui4* group_ranks = (elg_ui4*)epk_ranks;

  /* Determine the world rank of each process in group and register
   * MPI group definition (as MPI_GROUP).
   *
   * Parameter #3 of MPI_Group_translate_ranks is epk_group_world.ranks
   * here, as we need an array of integers initialized with 0 to n-1,
   * which epk_group_world.ranks happens to be.
   */
  PMPI_Group_size(epk_groups[idx].group, &size);
  PMPI_Group_translate_ranks(epk_groups[idx].group, size, epk_group_world.ranks,
                             epk_group_world.group, epk_ranks);

  /* Determine group flags */
  if (epk_group_world.size == size)
    {
      /* Special handling to detect WORLD group */
      if (0 == memcmp(epk_ranks, epk_group_world.ranks, size * sizeof(elg_ui4)))
        group_flags |= ELG_GROUP_WORLD;
    }
  PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (1 == size && epk_ranks[0] == rank)
    {
      /* Special handling for SELF group */
      group_flags |= ELG_GROUP_SELF;
      group_ranks  = NULL;
      size         = 0;
    }

  /* Define group */
  esd_def_mpi_group(epk_groups[idx].gid, group_flags, size, group_ranks);
}


/*--- Exported functions --------------------------------------------------*/

/**
 * Initialize MPI group tracking data structures.
 */
void epk_group_init()
{
  int i;

  epk_initialize_tracking();

  /* Avoid double initialization */
  if (epk_group_initialized)
    {
      elg_warning("epk_group_init: Duplicate call to MPI group tracking "
                  "initialization ignored!");
      return;
    }
  epk_group_initialized = 1;

  /* Allocate group tracking data structure */
  epk_groups = (struct EpkGroup*)malloc(epk_mpi_handles
                                        * sizeof(struct EpkGroup));
  if (NULL == epk_groups)
    {
      elg_warning("Failed to allocate MPI group tracking data structure!");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }

  /* Retrieve information about group of MPI_COMM_WORLD */
  PMPI_Comm_group(MPI_COMM_WORLD, &epk_group_world.group);
  PMPI_Group_size(epk_group_world.group, &epk_group_world.size);
  epk_group_world.ranks = (int*)malloc(epk_group_world.size * sizeof(int));
  if (NULL == epk_group_world.ranks)
    {
      elg_warning("Failed to allocate MPI group rank mapping table!");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }
  for (i = 0; i < epk_group_world.size; i++)
    epk_group_world.ranks[i] = i;

  /* Allocate rank translation buffer */
  epk_ranks = (int*)malloc(epk_group_world.size * sizeof(int));
  if (NULL == epk_ranks)
    {
      elg_warning("Failed to allocate rank translation buffer!");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }
}


/**
 * Finalize MPI group tracking. Check for groups that have been used in
 * RMA communication but have not yet been freed by the user.
 */
void epk_group_finalize()
{
  int i;

  /* Avoid double finalization */
  if (!epk_group_initialized)
    {
      elg_warning("epk_group_finalize: Duplicate call to MPI group tracking "
                  "finalization ignored!");
      return;
    }

  /* Define pending groups used in RMA operations */
  for (i = 0; i < epk_last_group; i++)
    {
      if (1 == epk_groups[i].used_in_rma)
        epk_group_define(i);
    }

  /* Release resources */
  PMPI_Group_free(&epk_group_world.group);
  free(epk_group_world.ranks);
  free(epk_ranks);
  free(epk_groups);

  /* Reset initialization flag to prevent crashes with broken MPIs */
  epk_group_initialized = 0;
}


/**
 * Starts tracking of the given MPI group handle \a group.
 */
void epk_group_create(MPI_Group group)
{
  int i;

  /* Check if group tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal groups.
   */
  if (!epk_group_initialized)
    {
      int lrank;

      PMPI_Group_rank(group, &lrank);
      if (0 == lrank)
        elg_warning("epk_group_create: Skipping attempt to create MPI group "
                    "outside init->finalize scope");
      return;
    }

  /* Is storage available */
  if (epk_last_group >= epk_mpi_handles)
    {
      elg_warning("Too many simultaneous MPI groups!");
      elg_warning("Hint: Increase EPK_MPI_HANDLES configuration variable.");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }

  /* Search group entry */
  i = epk_group_search(group);
  if (-1 != i)
    {
      /* Found => Increment reference count */
      epk_groups[i].refcnt++;
    }
  else
    {
      /* Not found => Enter group in epk_groups[] array */
      epk_groups[epk_last_group].group       = group;
      epk_groups[epk_last_group].gid         = epk_local_group_id++;
      epk_groups[epk_last_group].refcnt      = 1;
      epk_groups[epk_last_group].used_in_rma = 0;
      epk_last_group++;
    }
}


/**
 * Stops tracking of the given MPI group handle \a group.
 */
void epk_group_free(MPI_Group group)
{
  int         i;
  const char* message = "You are trying to free an MPI group that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Check if group tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal groups.
   */
  if (!epk_group_initialized)
    {
      int lrank;

      PMPI_Group_rank(group, &lrank);
      if (0 == lrank)
        elg_warning("epk_group_free: Skipping attempt to free MPI group "
                    "outside init->finalize scope");
      return;
    }

  /* Search group entry */
  i = epk_group_search(group);
  if (-1 != i)
    {
      /* Found => Decrease reference count on entry */
      epk_groups[i].refcnt--;

      /* Check if entry can be deleted */
      if (0 == epk_groups[i].refcnt)
        {
          /* Create definition only when group was used in RMA operations */
          if (1 == epk_groups[i].used_in_rma)
            epk_group_define(i);

          /* Remove entry */
          epk_last_group--;
          if (epk_last_group > 0)
            epk_groups[i] = epk_groups[epk_last_group];
        }
    }
  else
    {
      /* Not found => Print warning */
      elg_warning("epk_group_free: %s", message);

      /* NOTE:
       * Freeing an untracked group by itself is not an issue for both tracing
       * and summary mode since no event will be generated. Such groups are
       * only problematic when used in RMA operations, which we detect during
       * calls to epk_winacc_start.
       *
       * If you want tracing to abort instead, uncomment the following two
       * lines: */
      /*
      if (epk_comm_determination)
        PMPI_Abort(MPI_COMM_WORLD, 1);
      */
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * Communicator management
 *
 *---------------------------------------------------------------------------
 */

/*--- Data structures -----------------------------------------------------*/

/** @internal
 * Data structure used for tracking MPI communicators.
 */
struct EpkComm
{
  MPI_Comm comm;   /**< MPI communicator handle */
  elg_ui4  cid;    /**< EPIK communicator identifier */
};


/*--- Static variables ----------------------------------------------------*/

/** @internal
 * Flag identifying whether communicator tracking has been initialized.
 */
static int epk_comm_initialized = 0;

/** @internal
 * Internal identifier of the next communicator defined on this process.
 */
static int epk_local_comm_id = 0;

/** @internal
 * Internal identifier of the next non-SELF communicator defined on this
 * process where it is 'root' (i.e., rank 0).
 */
static int epk_root_comm_id = 0;

/** @internal
 * Index of next free entry in communicator tracking data structure.
 */
static int epk_last_comm = 0;

/** @internal
 * Communicator tracking data structure.
 */
static struct EpkComm* epk_comms = NULL;


/*--- Helper functions ----------------------------------------------------*/

/** @internal
 * Search the communicator tracking data structure for the entry of the
 * given MPI communicator handle \a comm. Returns the index of the handle
 * if found and -1 otherwise.
 */
static int epk_comm_search(MPI_Comm comm)
{
  int i = 0;

  while ((i < epk_last_comm) && (epk_comms[i].comm != comm))
    i++;

  if (i < epk_last_comm)
    return i;

  return -1;
}


/*--- Exported functions --------------------------------------------------*/

/**
 * Initialize MPI communicator tracking data structures.
 */
void epk_comm_init()
{
  epk_initialize_tracking();

  /* Avoid double initialization */
  if (epk_comm_initialized)
    {
      elg_warning("epk_comm_init: Duplicate call to MPI communicator tracking "
                  "initialization ignored!");
      return;
    }
  epk_comm_initialized = 1;

  /* Allocate communicator tracking data structure */
  epk_comms = (struct EpkComm*)malloc(epk_mpi_handles * sizeof(struct EpkComm));
  if (NULL == epk_comms)
    {
      elg_warning("Failed to allocate MPI communicator tracking data structure!");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }
}


/**
 * Finalize MPI communicator tracking.
 */
void epk_comm_finalize()
{
  /* Avoid double finalization */
  if (!epk_comm_initialized)
    {
      elg_warning("epk_comm_finalize: Duplicate call to MPI communicator "
                  "tracking finalization ignored!");
      return;
    }

  /* Release resources */
  free(epk_comms);

  /* Reset initialization flag to prevent crashes with broken MPIs */
  epk_comm_initialized = 0;
}


/**
 * Starts tracking of the given MPI communicator handle \a comm.
 */
void epk_comm_create(MPI_Comm comm)
{
  int comm_tuple[2];
  int lrank;
  int lsize;
  int is_intercomm;

  /* Determine local rank within communicator */
  PMPI_Comm_rank(comm, &lrank);

  /* Check if communicator tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal communicators.
   */
  if (!epk_comm_initialized)
    {
      if (0 == lrank)
        elg_warning("epk_comm_create: Skipping attempt to create MPI "
                    "communicator outside init->finalize scope");
      return;
    }

  /* Skip MPI intercommunicators */
  MPI_Comm_test_inter(comm, &is_intercomm);
  if (is_intercomm)
    {
      /* In tracing mode, this is an error! */
      if (epk_comm_determination)
        {
          if (0 == lrank)
            elg_warning("MPI intercommunicator encountered (not supported by "
                        "this version of Scalasca)");
          PMPI_Abort(MPI_COMM_WORLD, 1);
        }
      else
        {
          if (0 == lrank)
            elg_warning("MPI intercommunicator encountered (NB: tracing not "
                        "supported)");
        }
      return;
    }

  /* Is storage available */
  if (epk_last_comm >= epk_mpi_handles)
    {
      elg_warning("Too many simultaneous MPI communicators!");
      elg_warning("Hint: Increase EPK_MPI_HANDLES configuration variable.");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }

  /* Distributed communicator tracking */
  comm_tuple[0] = epk_root_comm_id;
  PMPI_Comm_rank(MPI_COMM_WORLD, &comm_tuple[1]);
  PMPI_Bcast(comm_tuple, 2, MPI_INT, 0, comm);
  PMPI_Comm_size(comm, &lsize);
  esd_def_mpi_comm(comm_tuple[0], comm_tuple[1], epk_local_comm_id, lrank, lsize);
  if (lrank == 0 && lsize > 1)
    epk_root_comm_id++;

  /* Enter comm in epk_comms[] array if it's not COMM_WORLD (ID 0) or
   * COMM_SELF (ID 1) -- they are handled specially by EPK_COMM_ID macro */
  if (epk_local_comm_id >= 2)
    {
      epk_comms[epk_last_comm].comm = comm;
      epk_comms[epk_last_comm].cid  = epk_local_comm_id;
      epk_last_comm++;
    }
  epk_local_comm_id++;
}


/**
 * Stops tracking of the given MPI communicator handle \a comm.
 */
void epk_comm_free(MPI_Comm comm)
{
  int         i;
  int         is_intercomm;
  const char* message = "You are trying to free an MPI communicator that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Check if communicator tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal communicators.
   */
  if (!epk_comm_initialized)
    {
      int lrank;

      PMPI_Comm_rank(comm, &lrank);
      if (0 == lrank)
        elg_warning("epk_comm_free: Skipping attempt to free MPI communicator "
                    "outside init->finalize scope");
      return;
    }

  /* Silently skip MPI inter-communicators */
  MPI_Comm_test_inter(comm, &is_intercomm);
  if (is_intercomm)
    return;

  /* Search communicator entry */
  i = epk_comm_search(comm);
  if (-1 != i)
    {
      /* Found? => Remove entry */
      epk_last_comm--;
      if (epk_last_comm > 0)
        epk_comms[i] = epk_comms[epk_last_comm];
    }
  else
    {
      /* Not found? => Print warning */
      elg_warning("epk_comm_free: %s", message);

      /* NOTE:
       * Freeing an untracked communicator by itself is not an issue for both
       * tracing and summary mode since no event will be generated. Such
       * communicators are only problematic when used in communication or
       * synchronization operations, which we detect during calls to
       * epk_comm_id.
       *
       * If you want tracing to abort instead, uncomment the following two
       * lines: */
      /*
      if (epk_comm_determination)
        PMPI_Abort(MPI_COMM_WORLD, 1);
      */
    }
}


/**
 * Returns the internal EPIK identifier for the given MPI communicator handle
 * \a comm.
 */
elg_ui4 epk_comm_id(MPI_Comm comm)
{
  int         i;
  const char* message = "You are trying to use an MPI communicator that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Search communicator entry */
  i = epk_comm_search(comm);
  if (-1 != i)
    {
      /* Found => Return identifier */
      return epk_comms[i].cid;
    }
  else
    {
      /* Handle special cases (this should never happen!) */
      if (MPI_COMM_WORLD == comm)
        {
          elg_warning("epk_comm_id: This function SHOULD NOT be called for "
                      "MPI_COMM_WORLD");
          return 0;
        }
      else if (MPI_COMM_SELF == comm)
        {
          elg_warning("epk_comm_id: This function SHOULD NOT be called for "
                      "MPI_COMM_SELF");
          return 1;
        }
      else
        {
          /* Not found => Print warning and abort when tracing */
          elg_warning("epk_comm_id: %s", message);
          if (epk_comm_determination)
            PMPI_Abort(MPI_COMM_WORLD, 1);

          return ELG_NO_ID;
        }
    }
}


#if defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * Window management
 *
 *---------------------------------------------------------------------------
 */

/*--- Data structures -----------------------------------------------------*/

/** @internal
 * Data structure used for tracking MPI windows
 */
struct EpkWin
{
  MPI_Win win;   /**< MPI window handle */
  elg_ui4 wid;   /**< EPIK window identifier */
};


/*--- Static variables ----------------------------------------------------*/

/** @internal
 * Flag identifying whether window tracking has been initialized.
 */
static int epk_win_initialized = 0;

/** @internal
 * Internal identifier of the next window defined on this process.
 */
static int epk_local_win_id = 0;

/** @internal
 * Index of next free entry in window tracking data structure.
 */
static int epk_last_win = 0;

/** @internal
 * Window tracking data structure.
 */
static struct EpkWin* epk_wins = NULL;


/*--- Helper functions ----------------------------------------------------*/

/** @internal
 * Search the window tracking data structure for the entry of the given MPI
 * RMA window handle \a win. Returns the index of the handle if found and -1
 * otherwise.
 */
static int epk_win_search(MPI_Win win)
{
  int i = 0;

  while ((i < epk_last_win) && (epk_wins[i].win != win))
    i++;

  if (i < epk_last_win)
    return i;

  return -1;
}


/*--- Exported functions --------------------------------------------------*/

/**
 * Initialize MPI window tracking data structures.
 */
void epk_win_init()
{
  epk_initialize_tracking();

  /* Avoid double initialization */
  if (epk_win_initialized)
    {
      elg_warning("epk_win_init: Duplicate call to MPI window tracking "
                  "initialization ignored!");
      return;
    }
  epk_win_initialized = 1;

  /* Allocate window tracking data structure */
  epk_wins = (struct EpkWin*)malloc(epk_mpi_handles * sizeof(struct EpkWin));
  if (NULL == epk_wins)
    {
      elg_warning("Failed to allocate MPI window tracking data structure!");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }
}


/**
 * Finalize MPI window tracking.
 */
void epk_win_finalize()
{
  /* Avoid double finalization */
  if (!epk_win_initialized)
    {
      elg_warning("epk_win_dinalize: Duplicate call to MPI window tracking "
                  "finalization ignored!");
      return;
    }

  /* Release resources */
  free(epk_wins);

  /* Reset initialization flag to prevent crashes with broken MPIs */
  epk_win_initialized = 0;
}


/**
 * Starts tracking of the given MPI window handle \a win defined on the
 * communicator \a comm.
 */
void epk_win_create(MPI_Win win, MPI_Comm comm)
{
  /* Check if window tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal windows.
   */
  if (!epk_win_initialized)
    {
      int lrank;

      PMPI_Comm_rank(comm, &lrank);
      if (0 == lrank)
        elg_warning("epk_win_create: Skipping attempt to create MPI RMA "
                    "window outside init->finalize scope");
      return;
    }

  /* Is storage available */
  if (epk_last_win >= epk_mpi_handles)
    {
      elg_warning("Too many simultaneous MPI RMA windows!");
      elg_warning("Hint: Increase EPK_MPI_HANDLES configuration variable.");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }

  /* Define MPI RMA window
   * NOTE: MPI_COMM_WORLD and MPI_COMM_SELF are _not_ present in the
   *       internal structures and _must not_ be queried by epk_comm_id */
  esd_def_mpi_win(epk_local_win_id,
                  ((comm == MPI_COMM_WORLD)
                    ? 0
                    : ((comm == MPI_COMM_SELF)
                        ? 1
                        : epk_comm_id(comm))));

  /* Enter window in epk_wins[] arrray */
  epk_wins[epk_last_win].win = win;
  epk_wins[epk_last_win].wid = epk_local_win_id++;
  epk_last_win++;
}


/**
 * Stops tracking of the given MPI window handle \a win.
 */
void epk_win_free(MPI_Win win)
{
  int         i;
  const char* message = "You are trying to free an MPI RMA window that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Check if window tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal windows.
   */
  if (!epk_win_initialized)
    {
      MPI_Group group;
      int       lrank;

      PMPI_Win_get_group(win, &group);
      PMPI_Group_rank(group, &lrank);
      PMPI_Group_free(&group);
      if (0 == lrank)
        elg_warning("epk_win_free: Skipping attempt to create MPI RMA window "
                    "outside init->finalize scope");
      return;
    }

  /* Search window entry */
  i = epk_win_search(win);
  if (-1 != i)
    {
      /* Found => Remove entry */
      epk_last_win--;
      if (epk_last_win > 0)
        epk_wins[i] = epk_wins[epk_last_win];
    }
  else
    {
      /* Not found => Print warning */
      elg_warning("epk_win_free: %s", message);

      /* NOTE:
       * Since freeing an RMA window generates a MPI_WINCOLLEXIT event, we
       * need to abort here when tracing. */
      if (epk_comm_determination)
        PMPI_Abort(MPI_COMM_WORLD, 1);
    }
}


/**
 * Returns the internal EPIK identifier for the given MPI window handle
 * \a win.
 */
elg_ui4 epk_win_id(MPI_Win win)
{
  int         i;
  const char* message = "You are trying to use an MPI RMA window that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Search window entry */
  i = epk_win_search(win);
  if (-1 != i)
    {
      /* Found => Return identifier */
      return epk_wins[i].wid;
    }
  else
    {
      /* Not found => Print warning and abort when tracing */
      elg_warning("epk_win_id: %s", message);
      if (epk_comm_determination)
        PMPI_Abort(MPI_COMM_WORLD, 1);

      return ELG_NO_ID;
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * Window Access Groups -- which window is accessed by which group
 *
 *---------------------------------------------------------------------------
 */

/*--- Data structures -----------------------------------------------------*/

/** @internal
 * Data structure used for tracking access/exposure epochs.
 */
struct EpkWinAcc {
  MPI_Win  win;     /**< MPI RMA window identifier */
  elg_ui4  gid;     /**< EPIK group identifier */
  elg_ui1  color;   /**< Byte to help distiguish accesses on same window */
};


/*--- Static variables ----------------------------------------------------*/

/** @internal
 * Flag identifying whether epoch tracking has been initialized.
 */
static int epk_winacc_initialized = 0;

/** @internal
 * Index of next free entry in epoch tracking data structure.
 */
static int epk_last_wacc = 0;

/** @internal
 * Epoch tracking data structure.
 */
static struct EpkWinAcc* epk_waccs = NULL;


/*--- Helper functions ----------------------------------------------------*/

/** @internal
 * Search the epoch tracking data structure for the entry of the epoch
 * identified the given MPI RMA window handle \a win and the type of the
 * epoch given in \a color (exposure=0, access=1). Returns the index of
 * the entry if found and -1 otherwise.
 */
static int epk_winacc_search(MPI_Win win, elg_ui1 color)
{
  int i = 0;

  while ((i <= epk_last_wacc) &&
         ((epk_waccs[i].win != win) || (epk_waccs[i].color != color)))
    i++;

  if (i < epk_last_wacc)
    return i;

  return -1;
}


/*--- Exported functions --------------------------------------------------*/

/**
 * Initialize MPI access/exposure epoch tracking data structures.
 */
void epk_winacc_init()
{
  epk_initialize_tracking();

  /* Avoid double initialization */
  if (epk_winacc_initialized)
    {
      elg_warning("epk_winacc_init: Duplicate call to MPI epoch tracking "
                  "initialization ignored!");
      return;
    }
  epk_winacc_initialized = 1;

  /* Allocate epoch tracking data structure */
  epk_waccs = (struct EpkWinAcc*)malloc(epk_mpi_handles
                                        * sizeof(struct EpkWinAcc));
  if (NULL == epk_waccs)
    {
      elg_warning("Failed to allocate MPI epoch tracking data structure!");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }
}


/**
 * Finalize MPI access/exposure epoch tracking.
 */
void epk_winacc_finalize()
{
  /* Avoid double finalization */
  if (!epk_winacc_initialized)
    {
      elg_warning("epk_winacc_finalize: Duplicate call to MPI epoch tracking "
                  "finalization ignored!");
      return;
    }

  /* Release resources */
  free(epk_waccs);

  /* Reset initialization flag top prevent crashes with broken MPIs */
  epk_winacc_initialized = 0;
}


/**
 * Starts tracking of an access/exposure epoch on the giben MPI RMA window
 * handle \a win using the MPI group handle \a group and the type of the
 * epoch given in \a color (exposure=0, access=1).
 */
void epk_winacc_start(MPI_Win win, MPI_Group group, elg_ui1 color)
{
  int         i;
  const char* message = "You are trying to use an MPI group that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Check if epoch tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal epochs.
   */
  if (!epk_winacc_initialized)
    {
      int lrank;

      PMPI_Group_rank(group, &lrank);
      if (0 == lrank)
        elg_warning("epk_winacc_start: Skipping attempt to start MPI RMA "
                    "epoch outside init->finalize scope");
      return;
    }

  /* Is storage available */
  if (epk_last_wacc >= epk_mpi_handles)
    {
      elg_warning("Too many simultaneous MPI RMA epochs!");
      elg_warning("Hint: Increase EPK_MPI_HANDLES configuration variable.");
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }

  /* Search for group entry */
  i = epk_group_search(group);
  if (-1 == i)
    {
      /* Not found => Print warning and abort */
      /* NOTE:
       * We need to abort here, since knowledge of the group is essential for
       * the epoch tracking, regardless of whether we are in summary or
       * tracing mode. */
      elg_warning("epk_winacc_start: %s", message);
      PMPI_Abort(MPI_COMM_WORLD, 1);
    }

  /* Mark group as used by RMA operation */
  epk_groups[i].used_in_rma = 1;

  /* Enter epoch in epk_waccs[] array */
  epk_waccs[epk_last_wacc].win   = win;
  epk_waccs[epk_last_wacc].gid   = epk_groups[i].gid;
  epk_waccs[epk_last_wacc].color = color;
  epk_last_wacc++;
}


/**
 * Stops tracking of the access/exposure epoch identified by the given MPI
 * RMA window handle \a win and the type of the epoch given in \a color
 * (exposure=0, access=1).
 */
void epk_winacc_end(MPI_Win win, elg_ui1 color) {
  int         i;
  const char* message = "You are trying to end an MPI RMA epoch that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Check if epoch tracking has been initialized.
   * Prevents crashes with broken MPI implementations that use MPI_ calls
   * instead of PMPI_ calls to create internal epochs.
   */
  if (!epk_winacc_initialized)
    {
      MPI_Group group;
      int       lrank;

      PMPI_Win_get_group(win, &group);
      PMPI_Group_rank(group, &lrank);
      PMPI_Group_free(&group);
      if (0 == lrank)
        elg_warning("epk_winacc_end: Skipping attempt to stop MPI RMA "
                    "epoch outside init->finalize scope");
      return;
    }

  /* Search epoch entry */
  i = epk_winacc_search(win, color);
  if (-1 != i)
    {
      /* Found => Remove entry */
      epk_last_wacc--;
      if (epk_last_wacc > 0)
        epk_waccs[i] = epk_waccs[epk_last_wacc];
    }
  else
    {
       /* Not found => Print warning and abort */
       elg_warning("epk_winacc_end: %s", message);
       PMPI_Abort(MPI_COMM_WORLD, 1);
    }
}


/**
 * Returns the internal EPIK identifier for the MPI group used for the
 * access/exposure epoch identified by the given MPI RMA window handle
 * \a win and the type of the epoch given in \a color (exposure=0, access=1).
 */
elg_ui4 epk_winacc_get_gid(MPI_Win win, elg_ui1 color)
{
  int         i;
  const char* message = "You are trying to use an MPI RMA epoch that was "
                        "not tracked by Scalasca. Please contact the "
                        "Scalasca support team to get help in resolving "
                        "this issue.";

  /* Search epoch entry */
  i = epk_winacc_search(win, color);
  if (-1 != i)
    {
      /* Found => Return group identifier */
      return epk_waccs[i].gid;
    }
  else
    {
      /* Not found => Print warning and abort */
      elg_warning("epk_winacc_get_id: %s", message);
      PMPI_Abort(MPI_COMM_WORLD, 1);

      return ELG_NO_ID;
    }
}

#endif   /* HAS_MPI2_1SIDED */
