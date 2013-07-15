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
 * @file  epk_mpiwrap.c
 * @brief Main file for C interface measurement wrappers
 */

#include "epk_defs_mpi.h"
#include "epk_mpicom.h"
#include "epk_mpireg.h"
#include "epk_mpireq.h"
#include "epk_mpiwrap.h"

#include "elg_error.h"

#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/** flag to indicate whether event generation is turned on or off */
int epk_mpi_nogen = 0;
