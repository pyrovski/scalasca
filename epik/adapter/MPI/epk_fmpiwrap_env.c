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
 * @file  epk_fmpiwrap_env.c
 *
 * @brief Fortran interface wrappers for environmental management
 */
#include <stdlib.h>
#include <mpi.h>

#include "elg_error.h"
#include "elg_fmpi_defs.h"
#include "epk_defs_mpi.h"
#include "epk_fwrapper_def.h"

#include "epk_fmpiwrap.h"
#include "epk_fmpiwrap_init.h"

/* uppercase defines */
/** All uppercase name of MPI_Finalize */
#define MPI_Finalize_U MPI_FINALIZE
/** All uppercase name of MPI_Finalized */
#define MPI_Finalized_U MPI_FINALIZED
/** All uppercase name of MPI_Init */
#define MPI_Init_U MPI_INIT
/** All uppercase name of MPI_Init_thread */
#define MPI_Init_thread_U MPI_INIT_THREAD
/** All uppercase name of MPI_Initialized */
#define MPI_Initialized_U MPI_INITIALIZED
/** All uppercase name of MPI_Is_thread_main */
#define MPI_Is_thread_main_U MPI_IS_THREAD_MAIN
/** All uppercase name of MPI_Query_thread */
#define MPI_Query_thread_U MPI_QUERY_THREAD

/* lowercase defines */
/** All lowercase name of MPI_Finalize */
#define MPI_Finalize_L mpi_finalize
/** All lowercase name of MPI_Finalized */
#define MPI_Finalized_L mpi_finalized
/** All lowercase name of MPI_Init */
#define MPI_Init_L mpi_init
/** All lowercase name of MPI_Init_thread */
#define MPI_Init_thread_L mpi_init_thread
/** All lowercase name of MPI_Initialized */
#define MPI_Initialized_L mpi_initialized
/** All lowercase name of MPI_Is_thread_main */
#define MPI_Is_thread_main_L mpi_is_thread_main
/** All lowercase name of MPI_Query_thread */
#define MPI_Query_thread_L mpi_query_thread

/**
 * @name Fortran utility functions
 * @{
 */

/** lowercase macro for fortran callback */
#define epk_mpif_init_cb_L epk_mpif_init_cb
/** uppercase macro for fortran callback */
#define epk_mpif_init_cb_U EPK_MPIF_INIT_CB

#if defined(HAS_MPI_BOTTOM)
/** initialization function for fortran MPI_BOTTOM
 * @param bottom address of variable acting as reference for MPI_BOTTOM
 */
void FSUB(epk_mpif_init_bottom)(MPI_Fint* bottom)
{
  epk_mpif_bottom = bottom;
}

#endif

#if defined(HAS_MPI_IN_PLACE)
/** initialization function for fortran MPI_IN_PLACE
 * @param in_place address of variable acting as reference for MPI_IN_PLACE
 */
void FSUB(epk_mpif_init_in_place)(MPI_Fint* in_place)
{
  epk_mpif_in_place = in_place;
}

#endif

#if defined(HAS_MPI_STATUS_IGNORE)
/** initialization function for fortran MPI_STATUS_IGNORE
 * @param status_ignore address of variable acting as reference for
 *MPI_STATUS_IGNORE
 */
void FSUB(epk_mpif_init_status_ignore)(MPI_Fint* status_ignore)
{
  epk_mpif_status_ignore = status_ignore;
}

#endif

#if defined(HAS_MPI_STATUSES_IGNORE)
/** initialization function for fortran MPI_STATUSES_IGNORE
 * @param statuses_ignore address of variable acting as reference for
 *MPI_STATUSES_IGNORE
 */
void FSUB(epk_mpif_init_statuses_ignore)(MPI_Fint* statuses_ignore)
{
  epk_mpif_statuses_ignore = statuses_ignore;
}

#endif

#if defined(HAS_MPI_UNWEIGHTED)
/** initialization function for fortran MPI_UNWEIGHTED
 * @param unweighted address of variable acting as reference for MPI_UNWEIGHTED
 */
void FSUB(epk_mpif_init_unweighted)(MPI_Fint* unweighted)
{
  epk_mpif_unweighted = unweighted;
}

#endif

/**
 * @}
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if defined(HAS_MPI_INIT_THREAD)
/**
 * Measurement wrapper for MPI_Init_tread
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Init_thread)(int* required,
                           int* provided,
                           int* ierr)
{
  *ierr = MPI_Init_thread(0, (char***)0, *required, provided);

  EPK_MPIF_INIT_CALLBACK();
}

#endif

/**
 * Measurement wrapper for MPI_Init
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
void FSUB(MPI_Init)(int* ierr)
{
  *ierr = MPI_Init(0, (char***)0);

  EPK_MPIF_INIT_CALLBACK();
}

#if defined(HAS_MPI_FINALIZE)
/**
 * Measurement wrapper for MPI_Finalize
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
void FSUB(MPI_Finalize)(int* ierr)
{
  *ierr = MPI_Finalize();
}

#endif
#if defined(HAS_MPI_FINALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Finalized)(int* flag,
                         int* ierr)
{
  *ierr = MPI_Finalized(flag);
}

#endif
#if defined(HAS_MPI_IS_THREAD_MAIN) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Is_thread_main)(int* flag,
                              int* ierr)
{
  *ierr = MPI_Is_thread_main(flag);
}

#endif
#if defined(HAS_MPI_QUERY_THREAD) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Query_thread)(int* provided,
                            int* ierr)
{
  *ierr = MPI_Query_thread(provided);
}

#endif

#if defined(HAS_MPI_INITIALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
void FSUB(MPI_Initialized)(int* flag,
                           int* ierr)
{
  *ierr = MPI_Initialized(flag);
}

#endif

#else /* !NEED_F2C_CONV */

/**
 * Measurement wrapper for MPI_Init_tread
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Init_thread)(MPI_Fint* required,
                           MPI_Fint* provided,
                           MPI_Fint* ierr)
{
  if (sizeof (MPI_Fint) != sizeof (int))
  {
    elg_error_msg("Fortran and C integer size mismatch.");
  }
  *ierr = MPI_Init_thread(0, (char***)0, *required, provided);

  EPK_MPIF_INIT_CALLBACK();
}

/**
 * Measurement wrapper for MPI_Init
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
void FSUB(MPI_Init)(MPI_Fint* ierr)
{
  if (sizeof (MPI_Fint) != sizeof (int))
  {
    elg_error_msg("Fortran and C integer size mismatch.");
  }
  *ierr = MPI_Init(0, (char***)0);

  EPK_MPIF_INIT_CALLBACK();
}

#if defined(HAS_MPI_FINALIZE)
/**
 * Measurement wrapper for MPI_Finalize
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
void FSUB(MPI_Finalize)(MPI_Fint* ierr)
{
  *ierr = MPI_Finalize();
}

#endif
#if defined(HAS_MPI_FINALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Finalized)(MPI_Fint* flag,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Finalized(flag);
}

#endif
#if defined(HAS_MPI_IS_THREAD_MAIN) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Is_thread_main)(MPI_Fint* flag,
                              MPI_Fint* ierr)
{
  *ierr = MPI_Is_thread_main(flag);
}

#endif
#if defined(HAS_MPI_QUERY_THREAD) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup env
 */
void FSUB(MPI_Query_thread)(MPI_Fint* provided,
                            MPI_Fint* ierr)
{
  *ierr = MPI_Query_thread(provided);
}

#endif

#if defined(HAS_MPI_INITIALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup env
 */
void FSUB(MPI_Initialized)(int* flag,
                           int* ierr)
{
  *ierr = MPI_Initialized(flag);
}

#endif

#endif

/**
 * @}
 */
