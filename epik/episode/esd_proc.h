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

#ifndef _ESD_PROC_H
#define _ESD_PROC_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"

#include "esd_gen.h"


/*
 *-----------------------------------------------------------------------------
 * EsdProc struct holds all process-specific data:
 * - Definitions buffer and associated filenames
 *-----------------------------------------------------------------------------
 */

typedef struct 
{
  EsdGen* gen;                  /* definitions buffer (and file) */
  char*   tmp_name;             /* temporary filename */
  char*   arc_name;             /* archived filename */
} EsdProc;


/* allocate and open process definitions file */
EXTERN EsdProc* EsdProc_open( void );

/* close and free process definitions file */
EXTERN void     EsdProc_close( EsdProc* proc, elg_ui4 pid );

/* unify process definitions */
EXTERN int      EsdProc_unify( EsdProc* proc, elg_ui4 rank, elg_ui4 ranks, int sync );

/* collate process summaries */
EXTERN void     EsdProc_collate( EsdProc* proc, elg_ui4 rank, elg_ui4 ranks, elg_ui4 thrds );

/* Accessor macros */

/* definitions file and buffer */
#define ESDPROC_GEN(proc)                 proc->gen

/* base name of the temporary files */
#define ESDPROC_TMP_NAME(proc)            proc->tmp_name

#endif
