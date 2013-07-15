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
 * @file  epk_mpiwrap_env.c
 *
 * @brief C interface wrappers for environmental management
 */

#include <stdlib.h>
#include <mpi.h>

#include "elg_error.h"

#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

#include "epk_defs_mpi.h"
#include "epk_mpicom.h"
#include "epk_mpireq.h"
#include "epk_mpiwrap.h"

/**
 * @name C wrappers
 * @{
 */

/**
 * Measurement wrapper for MPI_Init
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
int MPI_Init(int*    argc,
             char*** argv)
{
  int event_gen_active = 0; /* init is deferred to later */
  int return_val;
  int fflag;
  int already_opened;

  /* open output files and return whether the files had been opened
   * elsewhere in the measurement */
  already_opened = esd_open();
  /* esd_open calls epk_mpi_register(), so we have to defer the setting
   * of this test to now */
  event_gen_active = IS_EVENT_GEN_ON_FOR(ENV);

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    if (!already_opened)
    {
      esd_enter(epk_mpi_regid[EPK_PARALLEL__MPI]);
    }
    esd_enter(epk_mpi_regid[EPK__MPI_INIT]);
  }

  return_val = PMPI_Init(argc, argv);

  /* XXXX should only continue if MPI initialised OK! */

  if ((PMPI_Finalized(&fflag) == MPI_SUCCESS) && (fflag == 0))
  {
    /* complete initialization of measurement core and MPI event handling */
    epk_comm_determination = esd_mpi_init();

    if (return_val == MPI_SUCCESS)
    {
      int rank, ranks, version, subversion;
      PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
      PMPI_Comm_size(MPI_COMM_WORLD, &ranks);
      PMPI_Get_version(&version, &subversion);

      if (rank == 0)
      {
        elg_warning("MPI-%d.%d initialized %d ranks",
                    version,
                    subversion,
                    ranks);
      }
    }

    /* initialize handle & epoch management */
    epk_group_init();
    epk_comm_init();
#if defined(HAS_MPI2_1SIDED)
    epk_win_init();
    epk_winacc_init();
#endif

    /* create entries for implicit communicators */
    epk_comm_create(MPI_COMM_WORLD);
    epk_comm_create(MPI_COMM_SELF);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_INIT]);
    EVENT_GEN_ON();
  }

  return return_val;
} /* MPI_Init */

#if defined(HAS_MPI_INIT_THREAD)
/**
 * Measurement wrapper for MPI_Init_thread, the thread-capable
 * alternative to MPI_Init.
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
int MPI_Init_thread(int*    argc,
                    char*** argv,
                    int     required,
                    int*    provided)
{
  int event_gen_active = 0;
  int return_val;
  int fflag;
  int already_opened;

  /* open output files and return whether the files had been opened
   * elsewhere in the measurement */
  already_opened = esd_open();
  /* esd_open calls epk_mpi_register(), so we have to defer the setting
   * of this test to now */
  event_gen_active = IS_EVENT_GEN_ON_FOR(ENV);

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    if (!already_opened)
    {
      esd_enter(epk_mpi_regid[EPK_PARALLEL__MPI]);
    }
    esd_enter(epk_mpi_regid[EPK__MPI_INIT_THREAD]);
  }

  return_val = PMPI_Init_thread(argc, argv, required, provided);

  /* XXXX should only continue if MPI initialised OK! */

  if ((PMPI_Finalized(&fflag) == MPI_SUCCESS) && (fflag == 0))
  {
    /* complete initialization of measurement core and MPI event handling */
    epk_comm_determination = esd_mpi_init();

    if (return_val == MPI_SUCCESS)
    {
      int rank, ranks, version, subversion;
      PMPI_Comm_rank(MPI_COMM_WORLD, &rank);
      PMPI_Comm_size(MPI_COMM_WORLD, &ranks);
      PMPI_Get_version(&version, &subversion);

      if (rank == 0)
      {
        if (*provided != required)
        {
          elg_warning(
            "MPI-%d.%d initialized %d ranks providing thread support level %d but required %d!",
            version,
            subversion,
            ranks,
            *provided,
            required);
        }
        else
        {
          elg_warning(
            "MPI-%d.%d initialized %d ranks providing thread support level %d as requested",
            version,
            subversion,
            ranks,
            *provided);
        }
        /* XXXX runtime summarization supports MPI_THREAD_SERIALIZED, however,
                automatic trace analysis only supported for MPI_THREAD_FUNNELED
           */
        if (*provided > MPI_THREAD_FUNNELED)
        {
          elg_warning(
            "MPI library initialized with level exceeding MPI_THREAD_FUNNELED=%d!",
            MPI_THREAD_FUNNELED);
        }
      }
    }

    /* initialize handle & epoch management */
    epk_group_init();
    epk_comm_init();
#if defined(HAS_MPI2_1SIDED)
    epk_win_init();
    epk_winacc_init();
#endif

    /* create entries for implicit communicators */
    epk_comm_create(MPI_COMM_WORLD);
    epk_comm_create(MPI_COMM_SELF);
  }

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_INIT_THREAD]);
    EVENT_GEN_ON();
  }

  return return_val;
} /* MPI_Init_thread */

#else
int MPI_Init_thread(int*    argc,
                    char*** argv,
                    int     required,
                    int*    provided)
{
  elg_error_msg(
    "Abort: MPI_Init_thread wrapper was not built correctly. Please reinstall Scalasca with enabled MPI_Init_thread support.");
  return -1;
}

#endif

/**
 * Measurement wrapper for MPI_Finalize
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
int MPI_Finalize()
{
  const int event_gen_active = IS_EVENT_GEN_ON_FOR(ENV);
  int       return_val;

  if (event_gen_active)
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_FINALIZE]);
  }

  /* finalize handle & epoch management */
#if defined(HAS_MPI2_1SIDED)
  epk_winacc_finalize();
  epk_win_finalize();
#endif
  epk_comm_finalize();
  epk_group_finalize();
  epk_request_finalize();

  /* finalize MPI event handling */
  esd_mpi_finalize();

  /* fake finalization, so that MPI can be used during EPIK finalization */
  return_val = PMPI_Barrier(MPI_COMM_WORLD);

  if (event_gen_active)
  {
    esd_exit(epk_mpi_regid[EPK__MPI_FINALIZE]);
    EVENT_GEN_ON();
  }

  return return_val;
} /* MPI_Finalize */

#if defined(HAS_MPI_IS_THREAD_MAIN) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
int MPI_Is_thread_main(int* flag)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(ENV))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_IS_THREAD_MAIN]);

    return_val = PMPI_Is_thread_main(flag);

    esd_exit(epk_mpi_regid[EPK__MPI_IS_THREAD_MAIN]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Is_thread_main(flag);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_QUERY_THREAD) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
int MPI_Query_thread(int* provided)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(ENV))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_QUERY_THREAD]);

    return_val = PMPI_Query_thread(provided);

    esd_exit(epk_mpi_regid[EPK__MPI_QUERY_THREAD]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Query_thread(provided);
  }

  return return_val;
}

#endif

#if defined(HAS_MPI_FINALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
int MPI_Finalized(int* flag)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(ENV))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_FINALIZED]);

    return_val = PMPI_Finalized(flag);

    esd_exit(epk_mpi_regid[EPK__MPI_FINALIZED]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Finalized(flag);
  }

  return return_val;
}

#endif
#if defined(HAS_MPI_INITIALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: std.w
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
int MPI_Initialized(int* flag)
{
  int return_val;

  if (IS_EVENT_GEN_ON_FOR(ENV))
  {
    EVENT_GEN_OFF();
    esd_enter(epk_mpi_regid[EPK__MPI_INITIALIZED]);

    return_val = PMPI_Initialized(flag);

    esd_exit(epk_mpi_regid[EPK__MPI_INITIALIZED]);
    EVENT_GEN_ON();
  }
  else
  {
    return_val = PMPI_Initialized(flag);
  }

  return return_val;
}

#endif
/**
 * @}
 */
