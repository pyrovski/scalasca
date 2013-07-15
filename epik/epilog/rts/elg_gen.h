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

#ifndef _ELG_GEN_H
#define _ELG_GEN_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include <stdlib.h>
#include <stdio.h>

#include "elg_defs.h"

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Low level event trace generation
 *
 *-----------------------------------------------------------------------------
 */

/*
 *-----------------------------------------------------------------------------
 * Typedefs (to make data types opaque)
 *-----------------------------------------------------------------------------
 */

/* Trace file generated at run time */
typedef struct ElgGen_struct ElgGen;

/*
 *-----------------------------------------------------------------------------
 * ElgGen
 *-----------------------------------------------------------------------------
 */

EXTERN ElgGen* ElgGen_open             ( const char* path_name, 
					 elg_ui4 tid, size_t buffer_size );

EXTERN int ElgGen_create               ( ElgGen* gen );

/* flush mode: 0=full 1=final 2=user-defined */
EXTERN void ElgGen_flush               ( ElgGen* gen, int mode );

EXTERN void ElgGen_close               ( ElgGen* gen );

EXTERN void ElgGen_delete              ( ElgGen* gen );

EXTERN int ElgGen_reset_name           ( ElgGen* gen, const char* name );

EXTERN char* ElgGen_get_name           ( ElgGen* gen );

EXTERN size_t ElgGen_get_bsize         ( ElgGen* gen );

EXTERN size_t ElgGen_get_bytes         ( ElgGen* gen );

EXTERN elg_ui4* ElgGen_get_stats       ( ElgGen* gen );

#ifdef USE_SIONLIB
EXTERN int ElgGen_set_sionid           ( ElgGen* gen, int sionid );
EXTERN int ElgGen_get_sionid           ( ElgGen* gen );
#endif

/* -- Attributes -- */

EXTERN void ElgGen_write_ATTR_UI1(ElgGen* gen, elg_ui1 attrtype, elg_ui1 val);

EXTERN void ElgGen_write_ATTR_UI4(ElgGen* gen, elg_ui1 attrtype, elg_ui4 val);

/* -- Writing trace event records -- */

/* Event records do not include any location identifiers. These can be
   inserted during the merge process using ElgOut_localize_record(). */

/* Time stamps are provided in metv[metc], or generated explicitly for
   events without associated vectors of metrics. */

/* For MPI, process ids of message sources and destinations (dpid, spid, rpid)
   are mapped onto location ids by the merging process. */

/* -- automatically generated -- do not change ------------------------------ */
/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */

/* -- Region -- */

EXTERN void ElgGen_write_ENTER(ElgGen* gen, elg_ui4 rid,
       elg_ui1 metc, elg_ui8 metv[]);

EXTERN void ElgGen_write_ENTER_CS(ElgGen* gen, elg_ui4 csid,
       elg_ui1 metc, elg_ui8 metv[]);

EXTERN void ElgGen_write_EXIT(ElgGen* gen, /*elg_ui4 rid,*/ 
       elg_ui1 metc, elg_ui8 metv[]);

/* -- MPI-1 -- */

EXTERN void ElgGen_write_MPI_SEND(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
       elg_ui4 dpid, elg_ui4 cid, elg_ui4 tag, elg_ui4 sent);

EXTERN void ElgGen_write_MPI_RECV(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
       elg_ui4 spid, elg_ui4 cid, elg_ui4 tag, elg_ui4 recvd);

EXTERN void ElgGen_write_MPI_COLLEXIT(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
       elg_ui4 rid, elg_ui4 rpid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd);

/* -- MPI-1 Non-blocking -- */

EXTERN void ElgGen_write_MPI_SEND_COMPLETE(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
                                           elg_ui4 reqid);

EXTERN void ElgGen_write_MPI_RECV_REQUEST(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
                                          elg_ui4 reqid);

EXTERN void ElgGen_write_MPI_REQUEST_TESTED(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[], 
                                            elg_ui4 reqid);

EXTERN void ElgGen_write_MPI_CANCELLED(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
                                       elg_ui4 reqid);

/* -- MPI-2 One-sided Operations -- */

EXTERN void ElgGen_write_MPI_PUT_1TS(ElgGen* gen, elg_ui4 dpid, elg_ui4 wid,
       elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void ElgGen_write_MPI_PUT_1TE(ElgGen* gen, elg_ui4 spid, elg_ui4 wid,
       elg_ui4 rmaid);

EXTERN void ElgGen_write_MPI_PUT_1TE_REMOTE(ElgGen* gen, elg_ui4 dpid,
       elg_ui4 wid, elg_ui4 rmaid);

EXTERN void ElgGen_write_MPI_GET_1TS(ElgGen* gen, elg_ui4 dpid, elg_ui4 wid,
       elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void ElgGen_write_MPI_GET_1TS_REMOTE(ElgGen* gen, elg_ui4 dpid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void ElgGen_write_MPI_GET_1TE(ElgGen* gen, elg_ui4 spid, elg_ui4 wid,
       elg_ui4 rmaid);

EXTERN void ElgGen_write_MPI_WINEXIT(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
       elg_ui4 wid, elg_ui4 cid, elg_ui1 synex);

EXTERN void ElgGen_write_MPI_WINCOLLEXIT(ElgGen* gen,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid);

EXTERN void ElgGen_write_MPI_WIN_LOCK(ElgGen* gen, elg_ui4 lpid, elg_ui4 wid,
       elg_ui1 ltype);

EXTERN void ElgGen_write_MPI_WIN_UNLOCK(ElgGen* gen, elg_ui4 lpid, elg_ui4 wid);

/* -- Generic One-sided Operations -- */

EXTERN void ElgGen_write_PUT_1TS(ElgGen* gen, elg_ui4 dpid, elg_ui4 rmaid,
       elg_ui4 nbytes);

EXTERN void ElgGen_write_PUT_1TE(ElgGen* gen, elg_ui4 spid, elg_ui4 rmaid);

EXTERN void ElgGen_write_PUT_1TE_REMOTE(ElgGen* gen, elg_ui4 dpid,
       elg_ui4 rmaid);

EXTERN void ElgGen_write_GET_1TS(ElgGen* gen, elg_ui4 dpid, elg_ui4 rmaid,
       elg_ui4 nbytes);

EXTERN void ElgGen_write_GET_1TS_REMOTE(ElgGen* gen, elg_ui4 dpid,
       elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void ElgGen_write_GET_1TE(ElgGen* gen, elg_ui4 spid, elg_ui4 rmaid);

EXTERN void ElgGen_write_COLLEXIT(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[],
       elg_ui4 rpid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd);

EXTERN void ElgGen_write_ALOCK(ElgGen* gen, elg_ui4 lkid);

EXTERN void ElgGen_write_RLOCK(ElgGen* gen, elg_ui4 lkid);

/* -- OpenMP -- */

EXTERN void ElgGen_write_OMP_FORK(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[]);

EXTERN void ElgGen_write_OMP_JOIN(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[]);

EXTERN void ElgGen_write_OMP_ALOCK(ElgGen* gen, elg_ui4 lkid);

EXTERN void ElgGen_write_OMP_RLOCK(ElgGen* gen, elg_ui4 lkid);

EXTERN void ElgGen_write_OMP_COLLEXIT(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[]);

/* -- EPILOG Internal -- */

EXTERN void ElgGen_write_LOG_OFF(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[]);

EXTERN void ElgGen_write_LOG_ON(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[]);

EXTERN void ElgGen_write_ENTER_TRACING(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[]);

EXTERN void ElgGen_write_EXIT_TRACING(ElgGen* gen, elg_ui1 metc, elg_ui8 metv[]);

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
/* -- end automatically generated ------------------------------------------- */

#endif
