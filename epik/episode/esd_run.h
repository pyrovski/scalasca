/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _ESD_RUN_H
#define _ESD_RUN_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"

#ifndef _OLD_ELG_TRC_INTERFACE_FOR_TAU_
#  include "esd_proc.h"

   /* process object */
   extern EsdProc* proc;
#endif


/*
 *-----------------------------------------------------------------------------
 *
 *  EPISODE run-time control and measurement management
 *
 *-----------------------------------------------------------------------------
 */

EXTERN int  esd_open               ( );

EXTERN void esd_flush              ( );

EXTERN void esd_close              ( );

/*
  The next two functions have to be called immediately after
  initializing and immediately before finalizing the communication
  middle-ware, e.g. after MPI_Init() and before MPI_Finalize().  
*/

EXTERN int  esd_mpi_init           ( );
EXTERN void esd_mpi_finalize       ( );

/*
  For non-MPI and non-OpenMP programs, the next function has to be
  called after elg_open() to register a unique id (0..num-1) for the
  calling process and the number of processes
*/

EXTERN void esd_init_trc_id        ( int my_id, int num_procs );

#endif
