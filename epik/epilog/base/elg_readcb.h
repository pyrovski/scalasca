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

#ifndef _ELG_READCB_H
#define _ELG_READCB_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"
#include "elg_impl.h"

#include <stdio.h>


/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Interface for reading EPILOG trace records from a file or
 *    a memory buffer.  
 *
 *-----------------------------------------------------------------------------
 */

typedef struct ElgRCB_struct ElgRCB;

/* Open and close trace file */
EXTERN ElgRCB* elg_read_open ( const char* path );
EXTERN ElgRCB* elg_read_open_buffer( const buffer_t buffer, size_t size );
EXTERN void elg_read_close ( ElgRCB* handle );

/* Return version identifier of trace file */
EXTERN elg_ui4 elg_read_version( ElgRCB* handle );
/* Return byte order of trace file */
EXTERN elg_ui1 elg_read_byteorder( ElgRCB* handle );

/* Activate / deactivate the callback for specified event type
   Use constant ELG_ALL_EVENTS for processing all event types 
   By default all events are activated */
EXTERN void elg_read_activate( ElgRCB* handle, elg_ui1 type );
EXTERN void elg_read_deactivate( ElgRCB* handle, elg_ui1 type );

/* Read next record: This function will call the functions defined below
   Two functions are called for every record: elg_read_ALL() and the
   specific function matching the record's type if this type is activated.
   elg_read_next() returns 1 after successfully reading a trace record, 0
   else. */
EXTERN int  elg_read_next ( ElgRCB* handle, void* userdata );

/* skip all event records if any and read next record.
   Do callbacks for found definition record.
   returns 1 after successfully reading an definition record, 0 else */
EXTERN int  elg_read_next_def ( ElgRCB* handle, void* userdata );

/* skip all definition records if any and read next record.
   Do callbacks for found event record.
   returns 1 after successfully reading an event record, 0 else */
EXTERN int  elg_read_next_event ( ElgRCB* handle, void* userdata );

/* get position of current record */
EXTERN int  elg_read_getpos ( ElgRCB* handle, fpos_t* fpos );

/* restore record position  */
EXTERN int  elg_read_setpos ( ElgRCB* handle, const fpos_t* fpos );

/* seek to a given position */
EXTERN int elg_read_seek( ElgRCB* handle, long offset );

/* skip one record, return EOF if no records to read */
EXTERN int  elg_read_skip_record ( ElgRCB* handle );

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Callback functions for read interface
 *
 *-----------------------------------------------------------------------------
 */

/* Two functions are called for every record: elg_read_ALL() and the
   specific function matching the record's type. */

/** - All records - **/

EXTERN void elg_readcb_ALL           ( elg_ui1 type, 
                                       elg_ui1 length,
                                       void* userdata );

/** - Definition records - **/


EXTERN void elg_readcb_STRING        ( elg_ui4 strid,
                                       elg_ui1 cntc,
                                       const char* str,
                                       void* userdata );

EXTERN void elg_readcb_STRING_CNT    ( const char* str,
                                       void* userdata );

EXTERN void elg_readcb_MACHINE       ( elg_ui4 mid,
                                       elg_ui4 nodec,
                                       elg_ui4 mnid,
                                       void* userdata );

EXTERN void elg_readcb_NODE          ( elg_ui4 nid,
                                       elg_ui4 mid,
                                       elg_ui4 cpuc,
                                       elg_ui4 nnid, 
                                       elg_d8  cr,
                                       void* userdata );

EXTERN void elg_readcb_PROCESS       ( elg_ui4 pid,
                                       elg_ui4 pnid,
                                       void* userdata );

EXTERN void elg_readcb_THREAD        ( elg_ui4 tid,
                                       elg_ui4 pid,
                                       elg_ui4 tnid,
                                       void* userdata );

EXTERN void elg_readcb_LOCATION      ( elg_ui4 lid,
                                       elg_ui4 mid,
                                       elg_ui4 nid,
                                       elg_ui4 pid,
                                       elg_ui4 tid,
                                       void* userdata );

EXTERN void elg_readcb_FILE          ( elg_ui4 fid,
                                       elg_ui4 fnid,
                                       void* userdata );

EXTERN void elg_readcb_REGION        ( elg_ui4 rid,
                                       elg_ui4 rnid,
                                       elg_ui4 fid,
                                       elg_ui4 begln,
                                       elg_ui4 endln,
                                       elg_ui4 rdid,
                                       elg_ui1 rtype,
                                       void* userdata );

EXTERN void elg_readcb_CALL_SITE     ( elg_ui4 csid,
                                       elg_ui4 fid,
                                       elg_ui4 lno,
                                       elg_ui4 erid,
                                       elg_ui4 lrid,
                                       void* userdata );

EXTERN void elg_readcb_CALL_PATH     ( elg_ui4 cpid,
                                       elg_ui4 rid,
                                       elg_ui4 ppid,
                                       elg_ui8 order,
                                       void* userdata );

EXTERN void elg_readcb_METRIC        ( elg_ui4 metid,
                                       elg_ui4 metnid,
                                       elg_ui4 metdid,
                                       elg_ui1 metdtype,
                                       elg_ui1 metmode,
                                       elg_ui1 metiv,
                                       void* userdata );

EXTERN void elg_readcb_MPI_GROUP     ( elg_ui4 gid,
                                       elg_ui1 mode,
                                       elg_ui4 grpc,
                                       elg_ui4 grpv[],
                                       void* userdata );

EXTERN void elg_readcb_MPI_COMM_DIST ( elg_ui4 cid,
                                       elg_ui4 root,
                                       elg_ui4 lcid,
                                       elg_ui4 lrank,
                                       elg_ui4 size,
                                       void* userdata );

EXTERN void elg_readcb_MPI_COMM_REF  ( elg_ui4 cid,
                                       elg_ui4 gid,
                                       void* userdata );

EXTERN void elg_readcb_MPI_COMM      ( elg_ui4 cid,
                                       elg_ui1 mode,
                                       elg_ui4 grpc,
                                       elg_ui1 grpv[],
                                       void* userdata );

EXTERN void elg_readcb_CART_TOPOLOGY ( elg_ui4 topid,
                                       elg_ui4 tnid,
                                       elg_ui4 cid,
                                       elg_ui1 ndims,
                                       elg_ui4 dimv[],
                                       elg_ui1 periodv[],
                                       elg_ui4 dimids[],
                                       void* userdata );

EXTERN void elg_readcb_CART_COORDS   ( elg_ui4 topid,
                                       elg_ui4 lid,
                                       elg_ui1 ndims,
                                       elg_ui4 coordv[],
                                       void* userdata);

EXTERN void elg_readcb_OFFSET        ( elg_d8  ltime,
                                       elg_d8  offset,
                                       void* userdata );

EXTERN void elg_readcb_NUM_EVENTS    ( elg_ui4 eventc,
                                       void* userdata );

EXTERN void elg_readcb_EVENT_TYPES   ( elg_ui4 ntypes,
                                       elg_ui1 typev[],
                                       void* userdata );

EXTERN void elg_readcb_EVENT_COUNTS  ( elg_ui4 ntypes,
                                       elg_ui4 countv[],
                                       void* userdata );

EXTERN void elg_readcb_MAP_SECTION   ( elg_ui4 rank,
                                       void* userdata );

EXTERN void elg_readcb_MAP_OFFSET    ( elg_ui4 rank,
                                       elg_ui4 offset,
                                       void* userdata );

EXTERN void elg_readcb_IDMAP         ( elg_ui1 type,
                                       elg_ui1 mode,
                                       elg_ui4 count,
                                       elg_ui4 mapv[],
                                       void*   userdata );

EXTERN void elg_readcb_LAST_DEF      ( void* userdata );

EXTERN void elg_readcb_MPI_WIN       ( elg_ui4 wid,
                                       elg_ui4 cid,
                                       void* userdata );

/** - Attribute records - **/

EXTERN void elg_readcb_ATTR_UI1(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui1 val,       /* value                       */
                      void* userdata);

EXTERN void elg_readcb_ATTR_UI4(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui4 val,       /* value                       */
                      void* userdata);

/** - Event records - **/

/* -- automatically generated -- do not change ------------------------------ */
/* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */

/* -- Region -- */

EXTERN void elg_readcb_ENTER(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 rid,       /* region identifier of the region being entered */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

EXTERN void elg_readcb_ENTER_CS(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 csid,      /* call-site identifier of the call-site being entered */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

EXTERN void elg_readcb_EXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

/* -- MPI-1 -- */

EXTERN void elg_readcb_MPI_SEND(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui4 tag,       /* message tag                      */
                      elg_ui4 sent,      /* bytes sent                       */
                      void* userdata);

EXTERN void elg_readcb_MPI_RECV(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 slid,      /* source location identifier       */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui4 tag,       /* message tag                      */
                      void* userdata);

EXTERN void elg_readcb_MPI_COLLEXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      elg_ui4 rlid,      /* root location identifier         */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui4 sent,      /* bytes sent                       */
                      elg_ui4 recvd,     /* bytes received                   */
                      void* userdata);

/* -- MPI-1 Non-blocking -- */

EXTERN void elg_readcb_MPI_SEND_COMPLETE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 reqid,     /* request identifier of completed send */
                      void* userdata);

EXTERN void elg_readcb_MPI_RECV_REQUEST(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 reqid,     /* receive request identifier       */
                      void* userdata);

EXTERN void elg_readcb_MPI_REQUEST_TESTED(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 reqid,     /* identifier of tested request     */
                      void* userdata);

EXTERN void elg_readcb_MPI_CANCELLED(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 reqid,     /* identifier of cancelled request  */
                      void* userdata);

/* -- MPI-2 One-sided Operations -- */

EXTERN void elg_readcb_MPI_PUT_1TS(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      elg_ui4 nbytes,    /* message length in bytes          */
                      void* userdata);

EXTERN void elg_readcb_MPI_PUT_1TE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 slid,      /* source location identifier       */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      void* userdata);

EXTERN void elg_readcb_MPI_PUT_1TE_REMOTE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      void* userdata);

EXTERN void elg_readcb_MPI_GET_1TO(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      void* userdata);

EXTERN void elg_readcb_MPI_GET_1TS(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      elg_ui4 nbytes,    /* message length in bytes          */
                      void* userdata);

EXTERN void elg_readcb_MPI_GET_1TS_REMOTE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      elg_ui4 nbytes,    /* message length in bytes          */
                      void* userdata);

EXTERN void elg_readcb_MPI_GET_1TE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 slid,      /* source location identifier       */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      void* userdata);

EXTERN void elg_readcb_MPI_WINEXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui1 synex,     /* synchronization exit flag        */
                      void* userdata);

EXTERN void elg_readcb_MPI_WINCOLLEXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      elg_ui4 wid,       /* window identifier                */
                      void* userdata);

EXTERN void elg_readcb_MPI_WIN_LOCK(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 llid,      /* lock location identifier         */
                      elg_ui4 wid,       /* window identifier                */
                      elg_ui1 ltype,     /* lock type                        */
                      void* userdata);

EXTERN void elg_readcb_MPI_WIN_UNLOCK(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 llid,      /* lock location identifier         */
                      elg_ui4 wid,       /* window identifier                */
                      void* userdata);

/* -- Generic One-sided Operations -- */

EXTERN void elg_readcb_PUT_1TS(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      elg_ui4 nbytes,    /* message length in bytes          */
                      void* userdata);

EXTERN void elg_readcb_PUT_1TE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 slid,      /* source location identifier       */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      void* userdata);

EXTERN void elg_readcb_PUT_1TE_REMOTE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      void* userdata);

EXTERN void elg_readcb_GET_1TS(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      elg_ui4 nbytes,    /* message length in bytes          */
                      void* userdata);

EXTERN void elg_readcb_GET_1TS_REMOTE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      elg_ui4 nbytes,    /* message length in bytes          */
                      void* userdata);

EXTERN void elg_readcb_GET_1TE(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 slid,      /* source location identifier       */
                      elg_ui4 rmaid,     /* RMA operation identifier         */
                      void* userdata);

EXTERN void elg_readcb_COLLEXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      elg_ui4 rlid,      /* root location identifier         */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui4 sent,      /* bytes sent                       */
                      elg_ui4 recvd,     /* bytes received                   */
                      void* userdata);

EXTERN void elg_readcb_ALOCK(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 lkid,      /* identifier of the lock being acquired */
                      void* userdata);

EXTERN void elg_readcb_RLOCK(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 lkid,      /* identifier of the lock being released */
                      void* userdata);

/* -- OpenMP -- */

EXTERN void elg_readcb_OMP_FORK(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      void* userdata);

EXTERN void elg_readcb_OMP_JOIN(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      void* userdata);

EXTERN void elg_readcb_OMP_ALOCK(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 lkid,      /* identifier of the lock being acquired */
                      void* userdata);

EXTERN void elg_readcb_OMP_RLOCK(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 lkid,      /* identifier of the lock being released */
                      void* userdata);

EXTERN void elg_readcb_OMP_COLLEXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

/* -- EPILOG Internal -- */

EXTERN void elg_readcb_LOG_OFF(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

EXTERN void elg_readcb_LOG_ON(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

EXTERN void elg_readcb_ENTER_TRACING(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

EXTERN void elg_readcb_EXIT_TRACING(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata);

/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
/* -- end automatically generated ------------------------------------------- */

#endif
