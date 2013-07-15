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
 * @file  epk_fmpiwrap.c
 *
 * @brief Fortran interface-specific variable declarations
 */

#include <stdlib.h>
#include "epk_defs_mpi.h"


#if defined(HAS_MPI_BOTTOM)
/** address of fortran variable used as reference for MPI_BOTTOM */
void* epk_mpif_bottom = NULL;
#endif

#if defined(HAS_MPI_IN_PLACE)
/** address of fortran variable used as reference for MPI_IN_PLACE */
void* epk_mpif_in_place = NULL;
#endif

#if defined(HAS_MPI_STATUS_IGNORE)
/** address of fortran variable used as reference for MPI_STATUS_IGNORE */
void* epk_mpif_status_ignore = NULL;
#endif

#if defined(HAS_MPI_STATUSES_IGNORE)
/** address of fortran variable used as reference for MPI_STATUSES_IGNORE */
void* epk_mpif_statuses_ignore = NULL;
#endif

#if defined(HAS_MPI_UNWEIGHTED)
/** address of fortran variable used as reference for MPI_UNWEIGHTED */
void* epk_mpif_unweighted = NULL;
#endif

