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

#ifndef _EPK_METRIC_H
#define _EPK_METRIC_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_types.h"

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Performance counter metric support
 * 
 *-----------------------------------------------------------------------------
 */

/* NB: metrics are included in event records of maximum size 255 bytes
       which have a maximum capacity for 28 8-byte metrics:
       when metrics are not traced, this maximum does not apply. */
/* maximum number of events */
#define ELG_METRIC_MAXNUM 28

typedef enum {
    EPK_METMAP_UNKNOWN=0x0,
    EPK_METMAP_MEASURE=0x1,
    EPK_METMAP_AGGROUP=0x2,
    EPK_METMAP_COMPOSE=0x4,
    EPK_METMAP_COMPUTE=0x8,
    EPK_METMAP_INVALID=0x10
} epk_metmap_t;

typedef struct epk_metricmap_t {
  epk_metmap_t             type;
  char*              event_name;
  char*              alias_name;
  struct epk_metricmap_t*  next;
} epk_metricmap_t;

/* read metric mapping definitions into elg_metricmap vector */
EXTERN epk_metricmap_t* epk_metricmap_init(int match);
EXTERN void             epk_metricmap_dump(epk_metricmap_t* map);
EXTERN void             epk_metricmap_free(epk_metricmap_t* map);

EXTERN int              esd_metric_open();                           /* returns number of counters */ 
EXTERN void             esd_metric_close();
EXTERN void             esd_metric_thread_init(int (*id_fn)(void));  /* supply omp_get_thread_num() as argument */

EXTERN int              esd_metric_num();                            /* returns number of counters */ 
EXTERN double           esd_metric_clckrt();                         /* returns clock rate in Hz */ 

/* for i provide a value between 0 and elg_metric_num() - 1 */

EXTERN const char*      esd_metric_name(int i);
EXTERN const char*      esd_metric_descr(int i);
EXTERN int              esd_metric_dtype(int i);
EXTERN int              esd_metric_mode(int i);
EXTERN int              esd_metric_iv(int i);

struct esd_metv;

/* create and free per-thread counter sets */

EXTERN struct esd_metv* esd_metric_create();
EXTERN void             esd_metric_free(struct esd_metv* metv);

/* reads values of counters relative to the time of esd_metric_open() */
EXTERN void             esd_metric_read(struct esd_metv* metv, elg_ui8 values[]);

/* report metric settings */
EXTERN void             esd_metric_status();

#endif
