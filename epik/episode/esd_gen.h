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

#ifndef _ESD_GEN_H
#define _ESD_GEN_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include <stdlib.h>
#include <stdio.h>

#include "elg_defs.h"
#include "elg_impl.h"
#include "epik_topol.h"

typedef struct EsdGen_struct EsdGen;

/*
 *-----------------------------------------------------------------------------
 * EsdGen (replacement for ElgGen definitions, without event logging)
 *-----------------------------------------------------------------------------
 */

EXTERN EsdGen* EsdGen_open             ( const char* path_name, 
					 elg_ui4 tid, size_t buffer_size );

EXTERN void EsdGen_flush               ( EsdGen* gen );

EXTERN void EsdGen_close               ( EsdGen* gen );

EXTERN int EsdGen_reset_name           ( EsdGen* gen, const char* name );

EXTERN char* EsdGen_get_name           ( EsdGen* gen );

EXTERN buffer_t EsdGen_get_data        ( EsdGen* gen );

EXTERN size_t EsdGen_get_held          ( EsdGen* gen );

EXTERN size_t EsdGen_get_bytes         ( EsdGen* gen );
      
/* - Definition records - */


EXTERN void EsdGen_write_STRING        ( EsdGen* gen,
					 elg_ui4 strid,
					 elg_ui1 cntc,
					 const char* str );

EXTERN void EsdGen_write_STRING_CNT    ( EsdGen* gen,
					 const char* str );

EXTERN void EsdGen_write_MACHINE       ( EsdGen* gen,
					 elg_ui4 mid,
					 elg_ui4 nodec,
					 elg_ui4 mnid );

EXTERN void EsdGen_write_NODE          ( EsdGen* gen,
					 elg_ui4 nid,
					 elg_ui4 mid,
					 elg_ui4 cpuc,
					 elg_ui4 nnid, 
					 elg_d8  cr);

EXTERN void EsdGen_write_PROCESS       ( EsdGen* gen,
					 elg_ui4 pid,
					 elg_ui4 pnid );

EXTERN void EsdGen_write_THREAD        ( EsdGen* gen,
					 elg_ui4 tid,
					 elg_ui4 pid,
					 elg_ui4 tnid );

EXTERN void EsdGen_write_LOCATION      ( EsdGen* gen,
					 elg_ui4 lid,
					 elg_ui4 mid,
					 elg_ui4 nid,
					 elg_ui4 pid,
					 elg_ui4 tid );

EXTERN void EsdGen_write_FILE          ( EsdGen* gen,
					 elg_ui4 fid,
					 elg_ui4 fnid );

EXTERN void EsdGen_write_REGION        ( EsdGen* gen,
					 elg_ui4 rid,
					 elg_ui4 rnid,
					 elg_ui4 fid,
					 elg_ui4 begln,
					 elg_ui4 endln,
					 elg_ui4 rdid,
					 elg_ui1 rtype );

EXTERN void EsdGen_write_CALL_SITE     ( EsdGen* gen,
					 elg_ui4 csid,
					 elg_ui4 fid,
					 elg_ui4 lno,
					 elg_ui4 erid,
					 elg_ui4 lrid );

EXTERN void EsdGen_write_CALL_PATH     ( EsdGen* gen,
					 elg_ui4 cpid,
					 elg_ui4 rid,
					 elg_ui4 ppid,
					 elg_ui8 order );

EXTERN void EsdGen_write_METRIC        ( EsdGen* gen,
					 elg_ui4 metid,
					 elg_ui4 metnid,
					 elg_ui4 metdid,
					 elg_ui1 metdtype,
					 elg_ui1 metmode,
					 elg_ui1 metiv );

EXTERN void EsdGen_write_MPI_COMM      ( EsdGen* gen,
					 elg_ui4 cid,
					 elg_ui1 mode,
					 elg_ui4 grpc,
					 elg_ui1 grpv[] );

EXTERN void EsdGen_write_MPI_COMM_DIST ( EsdGen* gen,
                                         elg_ui4 cid,
                                         elg_ui4 root,
                                         elg_ui4 lcid,
                                         elg_ui4 lrank,
                                         elg_ui4 size);

EXTERN void EsdGen_write_MPI_GROUP     ( EsdGen* gen,
                                         elg_ui4 gid,
                                         elg_ui1 mode,
                                         elg_ui4 grpc,
                                         elg_ui4 grpv[] );

EXTERN void EsdGen_write_CART_TOPOLOGY ( EsdGen* gen,
					 elg_ui4 cid,
					 elg_ui4 topo_name_id,
					 elg_ui4* dim_names_ids,
					 EPIK_TOPOL * top);

EXTERN void EsdGen_write_CART_COORDS   ( EsdGen* gen,
					 elg_ui4 lid,
					 EPIK_TOPOL * top);

EXTERN void EsdGen_write_MPI_WIN       ( EsdGen* gen,
                                         elg_ui4 wid,
                                         elg_ui4 cid );

EXTERN void EsdGen_write_OFFSET        ( EsdGen* gen,
					 elg_d8  ltime,
                                         elg_d8  offset );

EXTERN void EsdGen_write_EVENT_TYPES   ( EsdGen* gen,
                                         elg_ui4 ntypes,
                                         elg_ui1 typev[] );

EXTERN void EsdGen_write_EVENT_COUNTS  ( EsdGen* gen,
                                         elg_ui4 ntypes,
                                         elg_ui4 countv[] );

#if 0
EXTERN void EsdGen_write_NUM_EVENTS    ( EsdGen* gen,
					 elg_ui4 eventc );

EXTERN void EsdGen_write_LAST_DEF      ( EsdGen* gen );
#endif

#endif
