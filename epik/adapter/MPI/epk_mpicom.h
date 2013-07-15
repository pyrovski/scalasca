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
 * @file  epk_mpicom.h
 *
 * @brief Communicator, group, window and epoch management
 */

#ifndef _EPK_MPICOM_H
#define _EPK_MPICOM_H

/*
 *---------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - MPI group management
 *  - MPI communicator management
 *  - MPI RMA window management
 *  - MPI RMA epoch management
 *
 *---------------------------------------------------------------------------
 */

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

#include "mpi.h"

#include "elg_types.h"


/*
 *---------------------------------------------------------------------------
 *
 * Group management
 *
 *---------------------------------------------------------------------------
 */

/** Initialize group management */
EXTERN void     epk_group_init();
/** Cleanup group management */
EXTERN void     epk_group_finalize();

/** Create an internal handle for an MPI group */
EXTERN void     epk_group_create(MPI_Group group);
/** Clear internal handle for an MPI group */
EXTERN void     epk_group_free(MPI_Group group);


/*
 *---------------------------------------------------------------------------
 *
 * Communicator management
 *
 *---------------------------------------------------------------------------
 */

/** Flag indicating whether comm/rank is determined for events */
EXTERN int      epk_comm_determination;

/** Initialize communicator management */
EXTERN void     epk_comm_init();
/** Cleanup communicator management */
EXTERN void     epk_comm_finalize();

/** Create an internal handle for an MPI communicator */
EXTERN void     epk_comm_create(MPI_Comm comm);
/** Clear internal handle for an MPI communicator */
EXTERN void     epk_comm_free(MPI_Comm comm);

/** Return internal handle of given MPI communicator */
EXTERN elg_ui4  epk_comm_id(MPI_Comm comm);

/* MPI communicator |-> EPIK communicator id */
#define EPK_COMM_ID(c) (!epk_comm_determination \
                            ? ELG_NO_ID \
                            : (((c)==MPI_COMM_WORLD) \
                                  ? 0 \
                                  : (((c)==MPI_COMM_SELF) \
                                        ? 1 \
                                        : epk_comm_id(c))))

/* MPI rank |-> EPIK rank */
#define EPK_RANK(r) (!epk_comm_determination ? ELG_NO_ID : (r))


#if defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * Window management
 *
 *---------------------------------------------------------------------------
 */

/** Initialize window management */
EXTERN void     epk_win_init();
/** Cleanup window management */
EXTERN void     epk_win_finalize();

/** Create an internal handle for an MPI window */
EXTERN void     epk_win_create(MPI_Win win, MPI_Comm comm);
/** Clear internal handle for an MPI window */
EXTERN void     epk_win_free(MPI_Win win);

/** Return internal handle of given MPI window */
EXTERN elg_ui4  epk_win_id(MPI_Win win);


/*
 *---------------------------------------------------------------------------
 *
 * Window access group management
 *
 *---------------------------------------------------------------------------
 */

/** Initialize window access group management */
EXTERN void     epk_winacc_init();
/** Cleanup window access group management */
EXTERN void     epk_winacc_finalize();

/** Start tracking of an access/exposure epoch */
EXTERN void     epk_winacc_start(MPI_Win win, MPI_Group group, elg_ui1 color);
/** Stop tracking of an access/exposure epoch */
EXTERN void     epk_winacc_end(MPI_Win win, elg_ui1 color);

/** Return internal handle for the group used in an epoch */
EXTERN elg_ui4  epk_winacc_get_gid(MPI_Win win, elg_ui1 color);

#endif   /* HAS_MPI2_1SIDED */


#endif   /* !_EPK_MPICOM_H */
