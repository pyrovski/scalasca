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

#ifndef _ELG_RW_H
#define _ELG_RW_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include <stdlib.h>
#include <stdio.h>

#include "elg_defs.h"
#define ELG_NO_TIME -1.0

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Low level event trace reading / (re)writing
 *
 *-----------------------------------------------------------------------------
 */

/* Flags for ElgOut_open */
#define ELG_UNCOMPRESSED    1


/*
 *-----------------------------------------------------------------------------
 * Typedefs (to make data types opaque)
 *-----------------------------------------------------------------------------
 */


/* Trace file for reading */
typedef struct ElgIn_struct ElgIn;

/* Trace file for writing resp. merging */
typedef struct ElgOut_struct ElgOut;

/* Trace recortd */
typedef struct ElgRec_struct ElgRec;


/*
 *-----------------------------------------------------------------------------
 * ElgRec
 *----------------------------------------------------------------------------- 
 */

/* Create records (list is not complete yet) */

EXTERN ElgRec* ElgRec_create_empty( elg_ui1 length,
                                    elg_ui1 type );

EXTERN ElgRec* ElgRec_copy( ElgRec* rec );

EXTERN ElgRec* ElgRec_create_NODE       ( elg_ui4 nid,
					  elg_ui4 mid,
					  elg_ui4 cpuc,
					  elg_ui4 nnid, 
					  elg_d8  cr );
EXTERN ElgRec* ElgRec_create_NUM_EVENTS ( elg_ui4 eventc );
EXTERN ElgRec* ElgRec_create_LAST_DEF   ();

/* Free record */

EXTERN void  ElgRec_free                ( ElgRec* rec );

/* Reading record header */

EXTERN elg_ui1 ElgRec_get_type          ( ElgRec* rec ); 
EXTERN elg_ui1 ElgRec_get_length        ( ElgRec* rec ); 
EXTERN elg_ui1 ElgRec_get_byteorder     ( ElgRec* rec ); 

/* Writing record header */
EXTERN void ElgRec_set_type             ( ElgRec* rec, elg_ui1 type );
EXTERN void ElgRec_set_byteorder        ( ElgRec* rec, elg_ui1 byteorder );

/* Predicate on record type */

EXTERN int ElgRec_is_event              ( ElgRec* rec ); 
EXTERN int ElgRec_is_attribute          ( ElgRec* rec ); 

/* Reading record data -*/

EXTERN elg_ui1 ElgRec_read_ui1          ( ElgRec* rec ); 
EXTERN elg_ui4 ElgRec_read_ui4          ( ElgRec* rec ); 
EXTERN elg_ui8 ElgRec_read_ui8          ( ElgRec* rec ); 
EXTERN elg_d8  ElgRec_read_d8           ( ElgRec* rec );
EXTERN char*   ElgRec_read_string       ( ElgRec* rec );
EXTERN elg_d8  ElgRec_read_time         ( ElgRec* rec );
EXTERN elg_ui4 ElgRec_read_location     ( ElgRec* rec );

/* Set read position */

EXTERN int     ElgRec_seek              ( ElgRec* rec,
					  size_t offset );
/* Writing record data -*/

EXTERN ElgRec* ElgRec_write_ui1         ( ElgRec* rec,
					 elg_ui1 ui1,
					 size_t offset ); 
EXTERN ElgRec* ElgRec_write_ui4         ( ElgRec* rec,
					  elg_ui4 ui4,
					  size_t offset ); 
EXTERN ElgRec* ElgRec_write_ui8         ( ElgRec* rec,
					  elg_ui8 ui8,
					  size_t offset ); 
EXTERN ElgRec* ElgRec_write_d8          ( ElgRec* rec,
					  elg_d8 d8,
					  size_t offset );

EXTERN int ElgRec_write_time            ( ElgRec* rec, elg_d8 time );

/*
 *-----------------------------------------------------------------------------
 * ElgIn
 *-----------------------------------------------------------------------------
 */

/* Open and close trace file */

EXTERN ElgIn*  ElgIn_open               ( const char* path_name );
EXTERN ElgIn*  ElgIn_open_buffer        ( const elg_ui1* buffer, size_t size );
EXTERN int     ElgIn_close              ( ElgIn* in );

/* EPILOG version */

EXTERN elg_ui4  ElgIn_get_version       ( ElgIn* in );

/* Byte order */

EXTERN elg_ui1  ElgIn_get_byte_order    ( ElgIn* in );

/* File position and error management */

EXTERN int ElgIn_setpos                 ( ElgIn* in, 
					  const fpos_t* pos );
EXTERN int ElgIn_getpos                 ( ElgIn* in, 
					  fpos_t* pos );
EXTERN int  ElgIn_rewind                ( ElgIn* in );
EXTERN long ElgIn_tell                  ( ElgIn* in );
EXTERN int  ElgIn_seek                  ( ElgIn* in,
                                          long   offset );

EXTERN int ElgIn_ferror                 ( ElgIn* in );
EXTERN int ElgIn_feof                   ( ElgIn* in );

/* Reading trace records */

EXTERN ElgRec* ElgIn_read_record        ( ElgIn* in ); 
EXTERN ElgRec* ElgIn_localize_record    ( ElgIn* in, elg_ui4 lid );
EXTERN int     ElgIn_skip_record        ( ElgIn* in );

/*
 *-----------------------------------------------------------------------------
 * ElgOut
 *-----------------------------------------------------------------------------
 */

/* Open and close trace file */

EXTERN ElgOut* ElgOut_open              ( const char* path_name,
					  elg_ui1 byte_order,
                                          elg_ui1 flags );
EXTERN ElgOut* ElgOut_open_buffer       ( elg_ui1* buffer,
                                          size_t size,
                                          elg_ui1 byte_order );
EXTERN int     ElgOut_close             ( ElgOut* out );

/* File position management */

EXTERN long    ElgOut_tell              ( ElgOut* out );
EXTERN int     ElgOut_seek              ( ElgOut* out,
                                          long   offset );

/* convenience functions */

EXTERN int ElgOut_write_string(ElgOut* out, elg_ui4 strid, const char* str);

EXTERN elg_ui4 ElgOut_define_string(ElgOut* out, const char* str);

EXTERN elg_ui4 ElgOut_define_file(ElgOut* out, const char* fname);

EXTERN elg_ui4 ElgOut_define_region(ElgOut* out, const char* rname,
       elg_ui4 fid, elg_ui4 begln, elg_ui4 endln,
       const char* rdesc, elg_ui1 rtype);

/* Write records */

EXTERN int   ElgOut_localize_record     ( ElgOut* out, 
					  ElgRec* rec,
					  elg_ui4 lid);
EXTERN int   ElgOut_write_record        ( ElgOut* out, 
					  ElgRec* rec );

EXTERN int ElgOut_write_STRING(ElgOut* out, elg_ui4 strid, elg_ui1 cntc,
                               const char* str);

EXTERN int ElgOut_write_STRING_CNT(ElgOut* out, const char* str);

EXTERN int ElgOut_write_MACHINE(ElgOut* out, elg_ui4 mid, elg_ui4 nodec,
                                elg_ui4 mnid);

EXTERN int ElgOut_write_NODE(ElgOut* out, elg_ui4 nid,
                             elg_ui4 mid, elg_ui4 cpuc, elg_ui4 nnid,
                             elg_d8 cr);

EXTERN int ElgOut_write_PROCESS(ElgOut* out, elg_ui4 pid, elg_ui4 pnid);

EXTERN int ElgOut_write_THREAD(ElgOut* out, elg_ui4 tid, elg_ui4 pid,
                               elg_ui4 tnid);

EXTERN int ElgOut_write_LOCATION(ElgOut* out, elg_ui4 lid, elg_ui4 mid,
                                 elg_ui4 nid, elg_ui4 pid, elg_ui4 tid);

EXTERN int ElgOut_write_FILE(ElgOut* out, elg_ui4 fid, elg_ui4 fnid);

EXTERN int ElgOut_write_REGION(ElgOut* out, elg_ui4 rid, elg_ui4 rnid,
                               elg_ui4 fid, elg_ui4 begln,
                               elg_ui4 endln, elg_ui4 rdid, elg_ui1 rtype);

EXTERN int ElgOut_write_CALL_SITE(ElgOut* out, elg_ui4 csid, elg_ui4 fid,
                                  elg_ui4 lno, elg_ui4 erid, elg_ui4 lrid);

EXTERN int ElgOut_write_CALL_PATH(ElgOut* out, elg_ui4 cpid, elg_ui4 rid,
                                  elg_ui4 ppid, elg_ui8 order);

EXTERN int ElgOut_write_METRIC(ElgOut* out, elg_ui4 metid,
                               elg_ui4 metnid, elg_ui4 metdid,
                               elg_ui1 metdtype, elg_ui1 metmode,
                               elg_ui1 metiv);

EXTERN int ElgOut_write_MPI_GROUP(ElgOut* out, elg_ui4 gid, elg_ui1 mode,
                                  elg_ui4 grpc, elg_ui4 grpv[]);

EXTERN int ElgOut_write_MPI_COMM_DIST(ElgOut* out, elg_ui4 cid, elg_ui4 root, 
                                      elg_ui4 lcid, elg_ui4 lrank, 
                                      elg_ui4 size);

EXTERN int ElgOut_write_MPI_COMM_REF(ElgOut* out, elg_ui4 cid, elg_ui4 gid);

EXTERN int ElgOut_write_MPI_COMM(ElgOut* out, elg_ui4 cid, elg_ui1 mode,
                                 elg_ui4 grpc, elg_ui1 grpv[]);

EXTERN int ElgOut_write_CART_TOPOLOGY(ElgOut* out, elg_ui4 topid, elg_ui4 tnid, elg_ui4 cid,
                                      elg_ui1 ndims, elg_ui4 dimv[],
                                      elg_ui1 periodv[], elg_ui4 dimids[]);

EXTERN int ElgOut_write_CART_COORDS(ElgOut* out, elg_ui4 topid,
                                    elg_ui4 lid, elg_ui1 ndims,
                                    elg_ui4 coordv[]);

EXTERN int ElgOut_write_MPI_WIN(ElgOut* out, elg_ui4 wid, elg_ui4 cid);

EXTERN int ElgOut_write_OFFSET(ElgOut* out, elg_d8 ltime, elg_d8 offset);

EXTERN int ElgOut_write_EVENT_TYPES(ElgOut* out, elg_ui4 ntypes, elg_ui1 typev[]);

EXTERN int ElgOut_write_EVENT_COUNTS(ElgOut* out, elg_ui4 ntypes, elg_ui4 countv[]);

EXTERN int ElgOut_write_MAP_SECTION(ElgOut* out, elg_ui4 rank);

EXTERN int ElgOut_write_MAP_OFFSET(ElgOut* out, elg_ui4 rank, elg_ui4 offset);

EXTERN int ElgOut_write_IDMAP(ElgOut* out, elg_ui1 type, elg_ui1 mode,
                              elg_ui4 count, elg_ui4* mapv);

EXTERN int ElgOut_write_NUM_EVENTS(ElgOut* out, elg_ui4 eventc);

EXTERN int ElgOut_write_LAST_DEF(ElgOut* out);

/* -- Attribute writers -- */

EXTERN int ElgOut_write_ATTR_UI1(ElgOut* out, elg_ui1 type, elg_ui1 val);

EXTERN int ElgOut_write_ATTR_UI4(ElgOut* out, elg_ui1 type, elg_ui4 val);

/* -- automatically generated -- do not change ------------------------------ */
/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */

/* -- Region -- */

EXTERN int ElgOut_write_ENTER(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 rid, elg_ui1 metc, elg_ui8 metv[]);

EXTERN int ElgOut_write_ENTER_CS(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 csid, elg_ui1 metc, elg_ui8 metv[]);

EXTERN int ElgOut_write_EXIT(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]);

/* -- MPI-1 -- */

EXTERN int ElgOut_write_MPI_SEND(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 dlid, elg_ui4 cid, elg_ui4 tag, elg_ui4 sent);

EXTERN int ElgOut_write_MPI_RECV(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 slid, elg_ui4 cid, elg_ui4 tag);

EXTERN int ElgOut_write_MPI_COLLEXIT(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd);

/* -- MPI-1 Non-blocking -- */

EXTERN int ElgOut_write_MPI_SEND_COMPLETE(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 reqid);

EXTERN int ElgOut_write_MPI_RECV_REQUEST(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 reqid);

EXTERN int ElgOut_write_MPI_REQUEST_TESTED(ElgOut* out, elg_ui4 lid,
       elg_d8 time, elg_ui4 reqid);

EXTERN int ElgOut_write_MPI_CANCELLED(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 reqid);

/* -- MPI-2 One-sided Operations -- */

EXTERN int ElgOut_write_MPI_PUT_1TS(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 dlid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN int ElgOut_write_MPI_PUT_1TE(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 slid, elg_ui4 wid, elg_ui4 rmaid);

EXTERN int ElgOut_write_MPI_PUT_1TE_REMOTE(ElgOut* out, elg_ui4 lid,
       elg_d8 time, elg_ui4 dlid, elg_ui4 wid, elg_ui4 rmaid);

EXTERN int ElgOut_write_MPI_GET_1TO(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 rmaid);

EXTERN int ElgOut_write_MPI_GET_1TS(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 dlid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN int ElgOut_write_MPI_GET_1TS_REMOTE(ElgOut* out, elg_ui4 lid,
       elg_d8 time, elg_ui4 dlid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN int ElgOut_write_MPI_GET_1TE(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 slid, elg_ui4 wid, elg_ui4 rmaid);

EXTERN int ElgOut_write_MPI_WINEXIT(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, elg_ui4 cid, elg_ui1 synex);

EXTERN int ElgOut_write_MPI_WINCOLLEXIT(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid);

EXTERN int ElgOut_write_MPI_WIN_LOCK(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 llid, elg_ui4 wid, elg_ui1 ltype);

EXTERN int ElgOut_write_MPI_WIN_UNLOCK(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 llid, elg_ui4 wid);

/* -- Generic One-sided Operations -- */

EXTERN int ElgOut_write_PUT_1TS(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 dlid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN int ElgOut_write_PUT_1TE(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 slid, elg_ui4 rmaid);

EXTERN int ElgOut_write_PUT_1TE_REMOTE(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 dlid, elg_ui4 rmaid);

EXTERN int ElgOut_write_GET_1TS(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 dlid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN int ElgOut_write_GET_1TS_REMOTE(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 dlid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN int ElgOut_write_GET_1TE(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 slid, elg_ui4 rmaid);

EXTERN int ElgOut_write_COLLEXIT(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd);

EXTERN int ElgOut_write_ALOCK(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 lkid);

EXTERN int ElgOut_write_RLOCK(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 lkid);

/* -- OpenMP -- */

EXTERN int ElgOut_write_OMP_FORK(ElgOut* out, elg_ui4 lid, elg_d8 time);

EXTERN int ElgOut_write_OMP_JOIN(ElgOut* out, elg_ui4 lid, elg_d8 time);

EXTERN int ElgOut_write_OMP_ALOCK(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 lkid);

EXTERN int ElgOut_write_OMP_RLOCK(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui4 lkid);

EXTERN int ElgOut_write_OMP_COLLEXIT(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]);

/* -- EPILOG Internal -- */

EXTERN int ElgOut_write_LOG_OFF(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]);

EXTERN int ElgOut_write_LOG_ON(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]);

EXTERN int ElgOut_write_ENTER_TRACING(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]);

EXTERN int ElgOut_write_EXIT_TRACING(ElgOut* out, elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]);

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
/* -- end automatically generated ------------------------------------------- */

#endif
