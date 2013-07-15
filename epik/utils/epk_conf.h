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

#ifndef _EPK_CONF_H
#define _EPK_CONF_H

/*
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing, Investigating, and Logging)
 *
 *  - Accessing the EPIK configuration and environment variables
 * 
 *-----------------------------------------------------------------------------
 */

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include <stdio.h>

/* indices into epk_vars configuration variable table */

typedef enum {
  EPK_CONF,     /* EPIK configuration */
  EPK_TITLE,                    /* ELG_FILE_PREFIX */
  EPK_GDIR,                     /* ELG_PFORM_GDIR */
  EPK_LDIR,                     /* ELG_PFORM_LDIR */
  EPK_FILTER,                   /* ELG_BLACKLIST */
  EPK_METRICS,                  /* ELG_METRICS */
  EPK_METRICS_SPEC,             /* ELG_METRICS_SPEC */
  EPK_CLEAN,                    /* ELG_CLEAN */
  EPK_SUMMARY,
  EPK_TRACE,
  EPK_MACHINE_ID,
  EPK_MACHINE_NAME,
  EPK_VERBOSE,                  /* ELG_VERBOSE */
  EPK_MPI_ENABLED,              /* EPK_MPI_ENABLED */
  EPK_MPI_HANDLES,              /* EPK_MPI_HANDLES */

  ESD_CONF,     /* EPISODE configuration */
  ESD_PATHS,
  ESD_FRAMES,
  ESD_BUFFER_SIZE,
  ESD_MAX_THREADS,              /* ESD_MAX_NUM_THRDS, ELG_MAX_NUM_THRDS */

  ELG_CONF,     /* EPILOG configuration */
  ELG_BUFFER_SIZE,
  ELG_COMPRESSION,
  ELG_MERGE,
  ELG_VT_MODE,
#ifdef USE_SIONLIB
  ELG_SION_FILES,
#endif

  EPK_CONF_VARS /* number of configuration variable indices */
} epk_type_i;

EXTERN const char *epk_get(epk_type_i index);   /* Return value string */
EXTERN const char *epk_key(unsigned index);     /* Return key string */
EXTERN void epk_conf_print(FILE* stream);       /* Print current configuration */
EXTERN int epk_str2int(const char *val);        /* Convert string to int */
EXTERN int epk_str2size(const char *val);       /* Convert string to size */
EXTERN int epk_str2bool(const char *str);       /* Convert string to bool */

#endif
