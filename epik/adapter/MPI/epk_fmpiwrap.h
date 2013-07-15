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

#ifndef EPK_FMPIWRAP_H
#define EPK_FMPIWRAP_H

#include "epk_defs_mpi.h"

/**
 * @file  epk_fmpiwrap.h
 *
 * @brief Main file for Fortran interface measurement wrappers
 */

#if defined(__sun) || defined(_SX) || defined(OPEN_MPI) || defined(HP_MPI) || \
    defined(SGI_MPT)
#  define NEED_F2C_CONV
#endif

#if defined(HAS_MPI_BOTTOM)
/** lowercase define for initialization call for fortran MPI_BOTTOM */
#define epk_mpif_init_bottom_L epk_mpif_init_bottom
/** uppercase define for initialization call for fortran MPI_BOTTOM */
#define epk_mpif_init_bottom_U EPK_MPIF_INIT_BOTTOM
/** pointer representing the Fortran value for MPI_BOTTOM */
EXTERN void* epk_mpif_bottom;
#endif

#if defined(HAS_MPI_IN_PLACE)
/** lowercase define for initialization call for fortran MPI_IN_PLACE */
#define epk_mpif_init_in_place_L epk_mpif_init_in_place
/** uppercase define for initialization call for fortran MPI_IN_PLACE */
#define epk_mpif_init_in_place_U EPK_MPIF_INIT_IN_PLACE
/** pointer representing the Fortran value for MPI_IN_PLACE */
EXTERN void* epk_mpif_in_place;
#endif

#if defined(HAS_MPI_STATUS_IGNORE)
/** lowercase define for initialization call for fortran MPI_STATUS_IGNORE */
#define epk_mpif_init_status_ignore_L \
  epk_mpif_init_status_ignore
/** uppercase define for initialization call for fortran MPI_STATUS_IGNORE */
#define epk_mpif_init_status_ignore_U \
  EPK_MPIF_INIT_STATUS_IGNORE
/** pointer representing the Fortran value for MPI_STATUS_IGNORE */
EXTERN void* epk_mpif_status_ignore;
#endif

#if defined(HAS_MPI_STATUSES_IGNORE)
/** lowercase define for initialization call for fortran MPI_STATUSES_IGNORE */
#define epk_mpif_init_statuses_ignore_L \
  epk_mpif_init_statuses_ignore
/** uppercase define for initialization call for fortran MPI_STATUSES_IGNORE */
#define epk_mpif_init_statuses_ignore_U \
  EPK_MPIF_INIT_STATUSES_IGNORE
/** pointer representing the Fortran value for MPI_STATUSES_IGNORE */
EXTERN void* epk_mpif_statuses_ignore;
#endif

#if defined(HAS_MPI_UNWEIGHTED)
/** lowercase define for initialization call for fortran MPI_UNWEIGHTED */
#define epk_mpif_init_unweighted_L epk_mpif_init_unweighted
/** uppercase define for initialization call for fortran MPI_UNWEIGHTED */
#define epk_mpif_init_unweighted_U EPK_MPIF_INIT_UNWEIGHTED
/** pointer representing the Fortran value for MPI_UNWEIGHTED */
EXTERN void* epk_mpif_unweighted;
#endif

#if defined(SGI_MPT)
#  include <string.h>
#  include <elg_fmpi_defs.h>

#if !defined(HAS_MPI_STATUS_F2C)
#  define PMPI_Status_f2c(f,c)   memcpy((c), (f), ELG_MPI_STATUS_SIZE)
#endif   /* !HAS_MPI_STATUS_F2C */

#if !defined(HAS_MPI_STATUS_C2F)
#  define PMPI_Status_c2f(c,f)   memcpy((f), (c), ELG_MPI_STATUS_SIZE)
#endif   /* !HAS_MPI_STATUS_C2F */

#endif   /* SGI_MPT */

#endif

