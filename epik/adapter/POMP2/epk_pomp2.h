/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _EPK_POMP2_H
#define _EPK_POMP2_H

#include <assert.h>

#include "elg_defs.h"
#include "esd_event.h"

extern int pomp2_tracing;

#define CHECK_HANDLE() \
  assert ( *pomp2_handle != 0 );

#define GUARDED_ESD_ENTER(id)                \
  EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle); \
  if ( pomp2_tracing ) {                 \
    esd_enter( rdescr->id );             \
  }

#define GUARDED_ESD_ENTER_2(qual,id1,id2)          \
  EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle); \
  if ( pomp2_tracing ) {                     \
    if ( rdescr->regionQualifier == qual ) { \
      esd_enter(rdescr->id1);                \
    }                                        \
    else{                                    \
      esd_enter(rdescr->id2);                \
      }\
  }

#define GUARDED_ESD_EXIT(id)               \
  EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle); \
  if ( pomp2_tracing ) {               \
    esd_exit( rdescr->id );            \
  }

#define GUARDED_ESD_OMP_COLL_EXIT(id)                 \
  EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle); \
  if ( pomp2_tracing ) {                      \
    esd_omp_collexit( rdescr->id );           \
  }

#define GUARDED_ESD_OMP_COLL_EXIT_2(qual,id1,id2)       \
  EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle); \
  if ( pomp2_tracing ) {                      \
    if ( rdescr->regionQualifier == qual )    \
      esd_omp_collexit(rdescr->id1);          \
    else                                      \
      esd_omp_collexit(rdescr->id2);          \
  }

typedef struct EPIK_Region_descriptor {
  elg_ui4 rid;     /* region id */
  elg_ui4 brid;    /* region id of implicit barrier */
                   /* also: lockid for critical     */
  elg_ui4 sbrid;   /* region id of enclosed structured block */
                   /* also: inner construct for combined constructs */
  char regionQualifier;  /* 1. character of region name to indicate type */
} EPIK_Region_descriptor;

#endif
