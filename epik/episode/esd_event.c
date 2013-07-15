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

#include "esd_event.h"
#include "elg_error.h"
#include "elg_pform.h"
#if defined(EPK_METR)
#include "epk_metric.h"
#endif
#include "esd_paths.h"
#include "elg_thrd.h"

#include "elg_trc.h.gen"

#if 1 /* duplication from esd_run.c */

EXTERN int esd_summary;
EXTERN int esd_tracing;

/* experiment active or not */
EXTERN int esd_status;

EXTERN elg_ui4 esd_pausing_rid;
EXTERN elg_ui4 esd_tracing_rid;

/* number of performance metrics (excluding time) */
EXTERN int esd_num_metrics;

/* vector of thread objects */
EXTERN ElgThrd** thrdv;

#endif /* duplication from esd_run.c */

#if (defined (ELG_OMPI) || defined (ELG_OMP))
#include <omp.h>
#endif

unsigned esd_extra_thrds = 0;

static int esd_check_thrd_id (elg_ui4 tid)
{
#if (defined (ELG_OMPI) || defined (ELG_OMP))

  if( tid >= ElgThrd_get_num_thrds() )
    {
#if 0
      /* allocate and initialise data structures for new thread */
      thrdv[ESD_MY_THREAD] = ElgThrd_create(ESD_MY_THREAD);
      ElgThrd_open(thrdv[ESD_MY_THREAD], ESD_MY_THREAD);
#endif
      int diff = tid - ElgThrd_get_num_thrds() + esd_extra_thrds;
      if (diff >= 0) {
#pragma omp critical (esd_tids)
        {
          if (tid >= (ElgThrd_get_num_thrds() + esd_extra_thrds)) {
              esd_extra_thrds++;
              elg_warning("Ignoring dynamically created thread #%d!", esd_extra_thrds);
          }
        }
      }
      return 1;
    }

#endif
  return 0;
}

/* store provided time in measurement vector (without updating HWC metrics) */

void esd_measurement_time (elg_ui8 valv[], elg_d8 time) 
{
    SET_VALX(valv[esd_num_metrics], time);
}

/* store provided time and associated HWC metrics in measurement vector */

void esd_measurement (elg_ui8 valv[], elg_d8 time) 
{
    SET_VALX(valv[esd_num_metrics], time);
#if defined(EPK_METR)
    if ( esd_num_metrics > 0 ) {
#ifdef VERIFY_METRICS        
        unsigned m;
#endif        
        esd_metric_read(ESDTHRD_METV(thrdv[ESD_MY_THREAD]), valv);
#ifdef VERIFY_METRICS
        for (m=0; m<esd_num_metrics; m++) {
            static elg_ui8 last[ELG_METRIC_MAXNUM]= {0};
            if (valv[m] < last[m]) {
                /* store invalid value in Unknown path */
                EsdPathIndex_t* unk_path = &ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->pathv[0];
                unk_path->record.count++;
                ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->valv_sum[0+m] += (last[m] - valv[m]);
#if 0
                elg_warning("Read metric %d %llu < last %llu", m, valv[m], last[m]);
#endif
                valv[m] = last[m];
            }
            else last[m] = valv[m];
        }
#endif
    }
#endif
}

/* implementations of EPIK->EPISODE event interface */

void esd_enter_pause (void)
{
    elg_d8 time = elg_pform_wtime();

    extern EsdPathIndex_t* esd_forkpath;
    if (esd_forkpath != NULL) {
        elg_warning("PAUSE ignored in parallel region (fork_rid=%u fork=%p)!",
                esd_forkpath->nodeid, esd_forkpath);
        return;
    }

    unsigned stkframe = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->stkframe;
    EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
    int currpathid = currpath - ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->pathv;
    if (esd_status) { /* already pausing! */
        elg_warning("PAUSE ignored during pause: frame=%u pathid=%u, path=%p!", 
                    stkframe, currpathid, currpath);
        return;
    } else
        elg_cntl_msg("PAUSE entered: frame=%u pathid=%u, path=%p!", 
                    stkframe, currpathid, currpath);

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_path_push(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), esd_pausing_rid, time);
    esd_frame_stash(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));

    /* pass event to EPILOG */
    if (esd_tracing) elg_enter(esd_pausing_rid);        /* elg_log_off(); */

    esd_status = 1; /* de-activate event handling */
}

void esd_exit_pause (void)
{
    elg_d8 time = elg_pform_wtime();

    extern EsdPathIndex_t* esd_forkpath;
    if (esd_forkpath != NULL) {
        elg_warning("RESUME ignored in parallel region (fork_rid=%u fork=%p)!",
                esd_forkpath->nodeid, esd_forkpath);
        return;
    }

    unsigned stkframe = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->stkframe;
    EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
    int currpathid = currpath - ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->pathv;
    if (!esd_status) { /* not pausing! */
        elg_warning("RESUME ignored when not paused: frame=%u pathid=%u, path=%p!", 
                    stkframe, currpathid, currpath);
        return;
    } else
        elg_cntl_msg("PAUSE finished: frame=%u pathid=%u, path=%p!", 
                    stkframe, currpathid, currpath);

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), esd_pausing_rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_exit(/*esd_pausing_rid*/);     /* elg_log_on(); */

    esd_status = 0; /* (re-)activate event handling */
}

void esd_enter_tracing (void)
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_path_push(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), esd_tracing_rid, time);
    esd_frame_stash(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));

    /* pass event to EPILOG */
    if (esd_tracing) elg_enter_tracing();
}

void esd_exit_tracing (void)
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), esd_tracing_rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_exit_tracing();
}

void esd_enter_cs (elg_ui4 csid) 
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_path_push(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), csid, time);
    esd_frame_stash(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));

    /* pass event to EPILOG */
    if (esd_tracing) elg_enter_cs(csid);
}

void esd_enter (elg_ui4 rid) 
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_path_push(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);
    esd_frame_stash(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));

    /* pass event to EPILOG */
    if (esd_tracing) elg_enter(rid);
}

void esd_exit (elg_ui4 rid) 
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_exit(/*rid*/);
}

void esd_exit_catch (elg_ui4 rid) 
{
    elg_d8 time = elg_pform_wtime();
    elg_ui4 frame_rid;

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);

    elg_warning("Exit catch popping frames to rid=%lld", rid);

    do {
        esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
        frame_rid = esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);

        elg_cntl_msg("Exit catch popped to frame rid=%lld", frame_rid);

        /* pass event to EPILOG */
        if (esd_tracing) elg_exit(/*rid*/);
    } while (frame_rid != rid);
}

void esd_collexit (elg_ui4 rid,
                   elg_ui4 rpid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd)
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_collexit(/*rid,*/ rpid, cid, sent, recvd);
}

void esd_omp_collexit (elg_ui4 rid)
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_omp_collexit(/*rid*/);
}

void esd_mpi_collexit (elg_ui4 rid,
                       elg_ui4 rpid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd)
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
#ifdef MSG_HACK /* hack for message statistics */
    if (esd_summary) {
        EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
        if (sent == 0 && recvd == 0) currpath->record.coll_syncs++;
        else if (sent == 0)          currpath->record.coll_c_dst++;
        else if (recvd == 0)         currpath->record.coll_c_src++;
        else                         currpath->record.coll_c_xch++;
        currpath->record.coll_bsent+=sent;
        currpath->record.coll_brcvd+=recvd;
    }
#endif
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_mpi_collexit(rid, rpid, cid, sent, recvd);
}

void esd_mpi_winexit (elg_ui4 rid,
                      elg_ui4 wid, elg_ui4 cid, elg_ui1 synex)
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_mpi_winexit(/*rid,*/ wid, cid, synex);
}

void esd_mpi_wincollexit (elg_ui4 rid,
                          elg_ui4 wid)
{
    elg_d8 time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    esd_frame_leave(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    esd_path_pop(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, time);

    /* pass event to EPILOG */
    if (esd_tracing) elg_mpi_wincollexit(/*rid,*/ wid);
}

/* Events that don't modify callpath call-through to EPILOG */

/* -- MPI-1 -- */

void esd_mpi_send (elg_ui4 dpid, elg_ui4 cid, elg_ui4 tag, elg_ui4 sent)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

#ifdef MSG_HACK /* hack for message statistics */
    if (esd_summary) {
        EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
        if (sent == 0) currpath->record.send_syncs++;
        else           currpath->record.send_comms++;
        currpath->record.send_bytes+=sent;
    }
#endif

    /* pass event to EPILOG with timestamp taken from entering current frame */
    if (esd_tracing) {
        unsigned stkframe = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->stkframe;
        elg_d8 enter_time = esd_frame_time(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), stkframe);
        esd_measurement_time(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), enter_time);
        elg_mpi_send(dpid, cid, tag, sent);
    }
}

void esd_mpi_recv (elg_ui4 spid, elg_ui4 cid, elg_ui4 tag, elg_ui4 recvd)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

#ifdef MSG_HACK /* hack for message statistics */
    if (esd_summary) {
        EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
        if (recvd == 0) currpath->record.recv_syncs++;
        else            currpath->record.recv_comms++;
        currpath->record.recv_bytes+=recvd;
    }
#endif

    /* pass event to EPILOG with timestamp of first request in current frame */
    if (esd_tracing) {
        EsdPaths_t* paths = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]);
        unsigned stkframe = paths->stkframe;
        elg_d8 enter_time = esd_frame_time(paths, stkframe);
        if (enter_time != paths->last_recv_enter) { /* new call-path instance */
            elg_d8 time = elg_pform_wtime();
            paths->first_recv_time = time;
            paths->last_recv_enter = enter_time;
        }
        esd_measurement_time(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), paths->first_recv_time);
        elg_mpi_recv(spid, cid, tag, recvd);
    }
}

/* -- MPI-1 Non-blocking Operations -- */

void esd_mpi_send_complete (elg_ui4 reqid)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

    /* pass event to EPILOG with timestamp of first request in current frame */
    if (esd_tracing) {
        EsdPaths_t* paths = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]);
        unsigned stkframe = paths->stkframe;
        elg_d8 enter_time = esd_frame_time(paths, stkframe);
        if (enter_time != paths->last_recv_enter) { /* new call-path instance */
            elg_d8 time = elg_pform_wtime();
            paths->first_recv_time = time;
            paths->last_recv_enter = enter_time;
        }
        esd_measurement_time(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), paths->first_recv_time);
        elg_mpi_send_complete(reqid);
    }
}

void esd_mpi_recv_request (elg_ui4 reqid)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

    /* pass event to EPILOG with timestamp taken from entering current frame */
    if (esd_tracing) {
        unsigned stkframe = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->stkframe;
        elg_d8 enter_time = esd_frame_time(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), stkframe);
        esd_measurement_time(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), enter_time);
        elg_mpi_recv_request(reqid);
    }
}

void esd_mpi_request_tested (elg_ui4 reqid)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

    /* pass event to EPILOG with timestamp of first request in current frame */
    if (esd_tracing) {
        EsdPaths_t* paths = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]);
        unsigned stkframe = paths->stkframe;
        elg_d8 enter_time = esd_frame_time(paths, stkframe);
        if (enter_time != paths->last_recv_enter) { /* new call-path instance */
            elg_d8 time = elg_pform_wtime();
            paths->first_recv_time = time;
            paths->last_recv_enter = enter_time;
        }
        esd_measurement_time(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), paths->first_recv_time);
        elg_mpi_request_tested(reqid);
    }
}

void esd_mpi_cancelled (elg_ui4 reqid)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

    elg_d8 time = elg_pform_wtime();

    esd_measurement_time(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), time);
    elg_mpi_cancelled(reqid);
}

/* -- MPI-2 File I/O Operations -- */

void esd_mpi_file_read (elg_ui4 bytes)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

#ifdef MSG_HACK /* hack for message statistics */
    if (esd_summary) {
        EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
        currpath->record.file_bytes+=bytes;
    }
#endif

    /* pass to EPILOG as event/attribute? */
}

void esd_mpi_file_write (elg_ui4 bytes)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

#ifdef MSG_HACK /* hack for message statistics */
    if (esd_summary) {
        EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
        currpath->record.file_bytes+=bytes;
    }
#endif

    /* pass to EPILOG as event/attribute? */
}

/* -- MPI-2 RMA Operations -- */

void esd_mpi_put_1ts (
                elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes)
{
     if (esd_tracing) elg_mpi_put_1ts(dpid, wid, rmaid, nbytes);
}               

void esd_mpi_put_1te (elg_ui4 spid, elg_ui4 wid, elg_ui4 rmaid)
{
     if (esd_tracing) elg_mpi_put_1te(spid, wid, rmaid);
}

void esd_mpi_put_1te_remote (elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid)
{
     if (esd_tracing) elg_mpi_put_1te_remote(dpid, wid, rmaid);
}

void esd_mpi_get_1ts (
                elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes)
{
     if (esd_tracing) elg_mpi_get_1ts(dpid, wid, rmaid, nbytes);
}

void esd_mpi_get_1ts_remote (
                elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes)
{
     if (esd_tracing) elg_mpi_get_1ts_remote(dpid, wid, rmaid, nbytes);
}

void esd_mpi_get_1te (elg_ui4 spid, elg_ui4 wid, elg_ui4 rmaid)
{
     if (esd_tracing) elg_mpi_get_1te(spid, wid, rmaid);
}

void esd_mpi_win_lock (elg_ui4 lpid, elg_ui4 wid, elg_ui1 ltype)
{
     if (esd_tracing) elg_mpi_win_lock(lpid, wid, ltype);
}

void esd_mpi_win_unlock (elg_ui4 lpid, elg_ui4 wid)
{
     if (esd_tracing) elg_mpi_win_unlock(lpid, wid);
}

/* -- Generic RMA Operations -- */

void esd_put_1ts (elg_ui4 dpid, elg_ui4 rmaid, elg_ui4 nbytes)
{
     if (esd_tracing) elg_put_1ts(dpid, rmaid, nbytes);
}

void esd_put_1te (elg_ui4 spid, elg_ui4 rmaid)
{
     if (esd_tracing) elg_put_1te(spid, rmaid);
}

void esd_put_1te_remote (elg_ui4 dpid, elg_ui4 rmaid)
{
     if (esd_tracing) elg_put_1te_remote(dpid, rmaid);
}

void esd_get_1ts (elg_ui4 dpid, elg_ui4 rmaid, elg_ui4 nbytes)
{
     if (esd_tracing) elg_get_1ts(dpid, rmaid, nbytes);
}

void esd_get_1ts_remote (elg_ui4 dpid, elg_ui4 rmaid, elg_ui4 nbytes)
{
     if (esd_tracing) elg_get_1ts_remote(dpid, rmaid, nbytes);
}

void esd_get_1te (elg_ui4 spid, elg_ui4 rmaid)
{
     if (esd_tracing) elg_get_1te(spid, rmaid);
}

void esd_alock (elg_ui4 lkid)
{
     if (esd_tracing) elg_alock(lkid);
}

void esd_rlock (elg_ui4 lkid)
{
     if (esd_tracing) elg_rlock(lkid);
}

/* -- OpenMP -- */

static elg_d8 esd_fork_time;

void esd_omp_fork (elg_ui4 rid)
{
    extern EsdPathIndex_t* esd_forkpath;

    if (esd_status) return;
    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

    if (esd_forkpath != NULL) {
        unsigned stkframe = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->stkframe;
        elg_warning("[%u]FORK ignoring nested fork(rid=%u) fork_rid=%u fork=%p!",
                stkframe, rid, esd_forkpath->nodeid, esd_forkpath);
        return;
    }

    esd_fork_time = elg_pform_wtime();
    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), esd_fork_time);
    /* push fork path on (master's) callpath */
    esd_path_push(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), rid, esd_fork_time);
    esd_frame_stash(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));

    /* determine forkpath on master and set it for worker threads */
    esd_forkpath = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->currpath;
    int currpathid = esd_forkpath - ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->pathv;
    elg_cntl_msg("fork(rid=%u, pathid=%d) fork=%p", rid, currpathid, esd_forkpath);

    /* duplicate master's fork callpath & measurement on each worker thread */
    unsigned t;
    for (t=1; t<ElgThrd_get_num_thrds(); t++) {
        /* XXXX esd_fork_time expected to be identical for all threads,
           however, HWC values would be different for each thread! */
        esd_path_push(ESDTHRD_PATHS(thrdv[t]), rid, esd_fork_time);
        esd_frame_stash(ESDTHRD_PATHS(thrdv[t]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
    }

#if (defined (ELG_OMPI) || defined (ELG_OMP))
    if (!omp_in_parallel()) /* XXXX nested? */
        if (esd_tracing) elg_omp_fork();
#endif
}

void esd_omp_join (elg_ui4 rid)
{
    extern EsdPathIndex_t* esd_forkpath;
    elg_d8 join_time = elg_pform_wtime();

    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;
    if (!esd_forkpath || (rid != esd_forkpath->nodeid)) {
        unsigned stkframe = ESDTHRD_PATHS(thrdv[ESD_MY_THREAD])->stkframe;
        elg_warning("[%u]JOIN ignoring unexpected join(rid=%u<>%u) fork=%p!",
                stkframe, rid, esd_forkpath?esd_forkpath->nodeid:ELG_NO_ID, esd_forkpath);
        return;
    }

    elg_cntl_msg("parallel region for %.6fs (%.6f-%.6f)",
                join_time-esd_fork_time, join_time, esd_fork_time);

    esd_measurement(ESDTHRD_VALV(thrdv[ESD_MY_THREAD]), join_time);

    /* explicitly pop fork callpath from each thread's pathv */
    unsigned t;
    for (t=0; t<ElgThrd_get_num_thrds(); t++) {
        unsigned stkframe = ESDTHRD_PATHS(thrdv[t])->stkframe;
        EsdPathIndex_t* currpath = ESDTHRD_PATHS(thrdv[t])->currpath;
        int currpathid = currpath - ESDTHRD_PATHS(thrdv[t])->pathv;
        if (t==0) {
            if (currpath != esd_forkpath)
                elg_warning("t%d join(rid=%u, pathid=%u) from unexpected path=%p (fork=%p)!",
                            t, rid, currpathid, currpath, esd_forkpath);
            else
                elg_cntl_msg("t%d join(rid=%u, pathid=%d) fork=%p", t, rid, currpathid, esd_forkpath);
        } else {
            if (stkframe != 0)
                elg_warning("t%d join(rid=%u, pathid=%d) from unexpected path=%p (frame=%u)",
                            t, rid, currpathid, currpath, stkframe);
            else
                elg_cntl_msg("t%d join(rid=%u, pathid=%d)", t, rid, currpathid);
        }

        /*esd_framev_dump(ESDTHRD_PATHS(thrdv[t]));*/
        elg_d8 fork_done = esd_frame_time(ESDTHRD_PATHS(thrdv[t]), stkframe+1);
        elg_d8 fork_time = fork_done - esd_fork_time;
        elg_cntl_msg("t%d fr%2d: fork overhead %.6fs (%.6f-%.6f)", t, stkframe+1,
                    fork_time, fork_done, esd_fork_time);

        if (fork_time<0.0) { /* thread didn't execute parallel region */
            ESDTHRD_PATHS(thrdv[t])->stkframe--; /* discard fork frame */
            ESDTHRD_PATHS(thrdv[t])->currpath = NULL;
            continue;
        }

#ifdef MSG_HACK
        /* XXXX hack fork_time into message statistics record */
        elg_d8 curr_time=0.0;
        PUT_VALX(currpath->record.coll_bsent, curr_time);
        SET_VALX(currpath->record.coll_bsent, curr_time+fork_time);
#endif

        esd_frame_leave(ESDTHRD_PATHS(thrdv[t]), ESDTHRD_VALV(thrdv[ESD_MY_THREAD]));
        esd_path_pop(ESDTHRD_PATHS(thrdv[t]), rid, join_time);
    }

    esd_forkpath = NULL; /* reset to root (when no longer required by worker threads) */

#if (defined (ELG_OMPI) || defined (ELG_OMP))
    if (!omp_in_parallel()) /* XXXX nested? */
       if (esd_tracing) elg_omp_join();
#endif
}

void esd_omp_alock (elg_ui4 lkid)
{
     if (esd_tracing) elg_omp_alock(lkid);
}

void esd_omp_rlock (elg_ui4 lkid)
{
     if (esd_tracing) elg_omp_rlock(lkid);
}

/* -- Attributes -- */

void esd_attr_ui1 (elg_ui1 type, elg_ui1 val)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

    if (esd_tracing) elg_attr_ui1(type, val);
}

void esd_attr_ui4 (elg_ui1 type, elg_ui4 val)
{
    if (esd_status) return;

    if (esd_check_thrd_id(ESD_MY_THREAD)) return;

    if (esd_tracing) elg_attr_ui4(type, val);
}
