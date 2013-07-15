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

#ifndef _ESD_PATHS_H
#define _ESD_PATHS_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

/*
 *-----------------------------------------------------------------------------
 * EsdPath handles callpath/stack management,
 * measurement acquisition and event forwarding
 *-----------------------------------------------------------------------------
 */

#include "elg_defs.h"

/* EsdMeasurement_t records correspond to a set of measurements:
   they may be entry measurements stored in the stackframe buffer while
   that frame is active on the callpath stack, elapsed measurements for
   the current callpath which are passed to summary and trace processing
   routines, or aggregate measurements in a callpath summary record */

#if 1
typedef union {                         /* generic single measurement value */
    elg_ui8     u;
    elg_d8      d;
} elg_g8;

#define SET_VALX(val,x) { elg_g8 tmpval; tmpval.d=(x); val = tmpval.u; }
#define PUT_VALX(val,x) { elg_g8 tmpval; tmpval.u=(val); x = tmpval.d; }

typedef elg_ui8 EsdMeasurement_t;
#else
typedef union {                         /* generic single measurement value */
    elg_d8      d;
    elg_ui8     u;
} elg_g8;

typedef union EsdMeasurement {          /*** measurement (set) ***/
    elg_g8      metx;                   /* default metric (wall time) */
    elg_g8*     metv;                   /* vector of metric values */
} EsdMeasurement_t;

#define mets esd_num_metrics
#define SET_METX(U,x)      if (mets) U.metv[mets].d=(x); else U.metx.d=(x)
#define GET_METX(U)          (mets ? U.metv[mets].d : U.metx.d)
#define GET_METX_P(U)        (mets ? U->metv[mets].d : U->metx.d)
#define INC_METX(U,x)      if (mets) U.metv[mets].d+=(x); else U.metx.d+=(x)
#define DEC_METX(U,x)      if (mets) U.metv[mets].d-=(x); else U.metx.d-=(x)
#define ADD_METX(U,U2)     if (mets) U.metv[mets].d+=U2.metv[mets].d;\
                                else U.metx.d+=U2.metx.d
#define SUB_METX(U,U2)     if (mets) U.metv[mets].d-=U2.metv[mets].d;\
                                else U.metx.d-=U2.metx.d

#define SET_METV(U,meti,x)           U.metv[meti].u=(x)
#define GET_METV(U,meti)             U.metv[meti].u
#define GET_METV_P(U,meti)           U->metv[meti].u
#define INC_METV(U,meti,x)           U.metv[meti].u+=(x)
#define DEC_METV(U,meti,x)           U.metv[meti].u-=(x)
#define ADD_METV(U,meti,U2)          U.metv[meti].u+=U2.metv[meti].u
#define SUB_METV(U,meti,U2)          U.metv[meti].u-=U2.metv[meti].u

#define TYP_METV(U)                  (elg_ui8*)(U.metv)
#endif

#define MSG_HACK 1

typedef struct EsdPathRecord {          /*** path data record ***/
    elg_ui4             count;          /* instance/visit accumulator */
#ifdef MSG_HACK /* hack for message statistics */
    elg_ui8             coll_bsent;     /* collective bytes sent */
    elg_ui8             coll_brcvd;     /* collective bytes received */
    elg_ui4             coll_syncs;     /* collective sync accumulator */
    elg_ui4             coll_c_dst;     /* collective comm as destination */
    elg_ui4             coll_c_src;     /* collective comm as source */
    elg_ui4             coll_c_xch;     /* collective comm as src & dest */
    elg_ui8             send_bytes;     /* sent bytes accumulator */
    elg_ui4             send_comms;     /* message send accumulator */
    elg_ui4             send_syncs;     /* zero-sized sends */
    elg_ui8             recv_bytes;     /* received bytes accumulator */
    elg_ui4             recv_comms;     /* message recv accumulator */
    elg_ui4             recv_syncs;     /* zero-sized recvs */
    elg_ui8             file_bytes;     /* file I/O bytes accumulator */
#endif
    elg_d8              first_time;     /* first visit start time */
    elg_d8              final_time;     /* final visit finish time */
} EsdPathRecord_t;

typedef struct EsdPathIndex EsdPathIndex_t;

struct EsdPathIndex {                   /*** path index record ***/
    elg_ui4             nodeid;         /* node region identifier */
    EsdPathIndex_t*     parent;         /* link back to parent */
    EsdPathIndex_t*     next;           /* link to next sibling */
    EsdPathIndex_t*     child;          /* link to first child */
    EsdPathRecord_t     record;         /* path data record */
};

typedef struct EsdPaths {               /*** paths management object ***/
    int stkframe;                       /* current stack frame */
    int stkdepth;                       /* maximum stack depth */
    EsdMeasurement_t** framev;          /* vector of frame entry measurements */

    elg_ui4 pathid;                     /* (last) path identifier */
    EsdPathIndex_t* currpath;           /* current callpath */
    EsdPathIndex_t* pathv;              /* vector of callpaths */

    EsdMeasurement_t* valv_sum;         /* vector of callpath sum values */

    /* value-pair used to replicate timestamp for multiple receives */
    elg_d8 last_recv_enter;             /* last receive frame enter time */
    elg_d8 first_recv_time;             /* timestamp of first recv event */

    elg_ui4* masterv;                   /* map from local to master paths */
    elg_ui4* rev_idmapv;                /* reverse mapping for path ids */
} EsdPaths_t;

EXTERN EsdPaths_t* esd_paths_init (void);
EXTERN void esd_paths_exit (EsdPaths_t* paths);
EXTERN void esd_paths_dump (EsdPaths_t* paths);
EXTERN unsigned esd_paths_size (EsdPaths_t* paths);
EXTERN unsigned esd_paths_void (EsdPaths_t* paths);
EXTERN unsigned esd_paths_roots (EsdPaths_t* paths);

EXTERN void esd_paths_unify (EsdPaths_t* m_paths, EsdPaths_t* w_paths);
EXTERN void esd_paths_def (EsdPaths_t* paths,
                           double ltime, double offset, double drift);

#ifdef MSG_HACK /* hacked message statistics */
EXTERN elg_ui8 esd_path_coll_bytes (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui8 esd_path_coll_bsent (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui8 esd_path_coll_brcvd (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_coll_c_src (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_coll_c_dst (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_coll_c_xch (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_coll_syncs (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui8 esd_path_send_bytes (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_send_comms (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_send_syncs (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui8 esd_path_recv_bytes (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_recv_comms (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui4 esd_path_recv_syncs (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui8 esd_path_file_bytes (EsdPaths_t* paths, elg_ui4 pathid);
#endif
EXTERN elg_d8  esd_path_tmgmt_time (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_d8  esd_path_tfork_time (EsdPaths_t* paths, elg_ui4 pathid);

EXTERN elg_ui4 esd_path_visits (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_d8  esd_path_timing (EsdPaths_t* paths, elg_ui4 pathid);
EXTERN elg_ui8 esd_path_metric (EsdPaths_t* paths, elg_ui4 pathid, unsigned m);

EXTERN void esd_path_push (EsdPaths_t* paths, elg_ui4 rid, elg_d8 enter_time);
EXTERN elg_ui4 esd_path_pop (EsdPaths_t* paths, elg_ui4 rid, elg_d8 exit_time);

EXTERN void esd_frame_stash (EsdPaths_t* paths, elg_ui8 valv[]);
EXTERN void esd_frame_leave (EsdPaths_t* paths, elg_ui8 valv[]);
EXTERN elg_d8 esd_frame_time (EsdPaths_t* paths, unsigned frame);
EXTERN void esd_framev_dump (EsdPaths_t* paths);
EXTERN unsigned esd_frame_depth (EsdPaths_t* paths);

#endif
