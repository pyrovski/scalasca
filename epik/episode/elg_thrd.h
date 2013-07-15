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

#ifndef _ELG_THRD_H
#define _ELG_THRD_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"
#include "esd_paths.h"

#if (defined (ELG_OTF))
#include "elg_otf_gen.h"
#else
#include "elg_gen.h"
#endif


/*
 *-----------------------------------------------------------------------------
 * ElgThrd struct holds all thread-specific data:
 * - Trace buffer and file including file name
 * - Event sets and value vector
 * - Path (and stack) management
 *-----------------------------------------------------------------------------
 */

typedef struct 
{

    EsdPaths_t* paths;          /* callpaths management object */

    ElgGen* gen;                /* trace buffer and file */
    char*   tmp_name;           /* temporary filename */
    char*   arc_name;           /* archived filename */

    elg_ui8*          valv;     /* vector of metric values */
#if (defined (EPK_METR)) 
    struct esd_metv*  metv;     /* vector of metric objects (the event set) */
#endif

} ElgThrd;


/* verify thread object */
EXTERN void     ElgThrd_verify( ElgThrd* thrd, elg_ui4 tid );

/* create thread object */
EXTERN ElgThrd* ElgThrd_create( elg_ui4 tid );

/* free thread object */
EXTERN void     ElgThrd_delete( ElgThrd* thrd, elg_ui4 tid );

/* open associated trace file */
EXTERN elg_ui4  ElgThrd_open( ElgThrd* thrd, elg_ui4 tid );

/* close associated trace file */
EXTERN void     ElgThrd_close( ElgThrd* thrd );

/* get total number of thread objects created */
EXTERN elg_ui4  ElgThrd_get_num_thrds();


/* Accessor macros */

/* callpaths management object */
#define ESDTHRD_PATHS(thrd)               thrd->paths

/* trace file and buffer */
#define ESDTHRD_GEN(thrd)                 thrd->gen

/* base name of the temporary files */
#define ESDTHRD_TMP_NAME(thrd)            thrd->tmp_name

/* vector of measurement values */
#define ESDTHRD_VALV(thrd)                thrd->valv

/* vector of metric objects (i.e., event sets) */
#define ESDTHRD_METV(thrd)                thrd->metv

#if (defined (ELG_OMPI) || defined (ELG_OMP))
#include "omp.h"
#define ESD_MY_THREAD omp_get_thread_num() 
#define ESD_NUM_THRDS omp_get_num_threads() 
#else 
#define ESD_MY_THREAD 0
#define ESD_NUM_THRDS 1 
#endif

#endif
