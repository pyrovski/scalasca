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

#ifndef EPK_MPIWRAP_H
#define EPK_MPIWRAP_H

/**
 * @file  epk_mpiwrap.h
 *
 * @brief Macros and Declarations common to all MPI wrapper modules
 */

#include "epk_mpireg.h"

/** flag to indicate whether event generation is turned on or off */
EXTERN int epk_mpi_nogen;

/** check whether event generation is turned on */
#define IS_EVENT_GEN_ON            (!epk_mpi_nogen)
/** check whether event generation is turned on for a specific group */
#define IS_EVENT_GEN_ON_FOR(group) ((!epk_mpi_nogen)      \
        && ((epk_mpi_enabled & EPK_MPI_ENABLED_ ## group) \
                            == EPK_MPI_ENABLED_ ## group))
/** turn off event generation */
#define EVENT_GEN_OFF()            epk_mpi_nogen = 1
/** turn on event generation */
#define EVENT_GEN_ON()             epk_mpi_nogen = 0

#endif
