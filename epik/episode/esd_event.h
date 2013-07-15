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

#ifndef _ESD_EVENT_H
#define _ESD_EVENT_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"


/* -- Internal -- */

EXTERN void esd_enter_pause (void);

EXTERN void esd_exit_pause (void);

EXTERN void esd_enter_tracing (void);

EXTERN void esd_exit_tracing (void);

/* -- Region -- */

EXTERN void esd_enter_cs (elg_ui4 csid);

EXTERN void esd_enter (elg_ui4 rid);

EXTERN void esd_exit (elg_ui4 rid);

EXTERN void esd_exit_catch (elg_ui4 rid);

/* -- MPI-1 -- */

EXTERN void esd_mpi_collexit (elg_ui4 rid,
                elg_ui4 rpid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd);

EXTERN void esd_mpi_send (
                elg_ui4 dpid, elg_ui4 cid, elg_ui4 tag, elg_ui4 sent);

EXTERN void esd_mpi_recv (
                elg_ui4 spid, elg_ui4 cid, elg_ui4 tag, elg_ui4 recvd);

/* -- MPI-1 Non-blocking -- */

EXTERN void esd_mpi_send_complete (elg_ui4 reqid);

EXTERN void esd_mpi_recv_request (elg_ui4 reqid);

EXTERN void esd_mpi_request_tested (elg_ui4 reqid);

EXTERN void esd_mpi_cancelled (elg_ui4 reqid);

/* -- MPI-2 File I/O Operations -- */

EXTERN void esd_mpi_file_read (elg_ui4 bytes);

EXTERN void esd_mpi_file_write (elg_ui4 bytes);

/* -- MPI-2 One-sided Operations -- */

EXTERN void esd_mpi_winexit (elg_ui4 rid,
                elg_ui4 wid, elg_ui4 cid, elg_ui1 synex);

EXTERN void esd_mpi_wincollexit (elg_ui4 rid,
                elg_ui4 wid);

EXTERN void esd_mpi_put_1ts (
                elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void esd_mpi_put_1te (
                elg_ui4 spid, elg_ui4 wid, elg_ui4 rmaid);

EXTERN void esd_mpi_put_1te_remote (
                elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid);

EXTERN void esd_mpi_get_1ts (
                elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void esd_mpi_get_1ts_remote (
                elg_ui4 dpid, elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void esd_mpi_get_1te (
                elg_ui4 spid, elg_ui4 wid, elg_ui4 rmaid);

EXTERN void esd_mpi_win_lock (
                elg_ui4 lpid, elg_ui4 wid, elg_ui1 ltype);

EXTERN void esd_mpi_win_unlock (
                elg_ui4 lpid, elg_ui4 wid);

/* -- Generic One-sided Operations -- */

EXTERN void esd_collexit (elg_ui4 rid,
                elg_ui4 rpid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd);

EXTERN void esd_put_1ts (
                elg_ui4 dpid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void esd_put_1te (
                elg_ui4 spid, elg_ui4 rmaid);

EXTERN void esd_put_1te_remote (
                elg_ui4 dpid, elg_ui4 rmaid);

EXTERN void esd_get_1ts (
                elg_ui4 dpid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void esd_get_1ts_remote (
                elg_ui4 dpid, elg_ui4 rmaid, elg_ui4 nbytes);

EXTERN void esd_get_1te (
                elg_ui4 spid, elg_ui4 rmaid);

EXTERN void esd_alock (elg_ui4 lkid);

EXTERN void esd_rlock (elg_ui4 lkid);

/* -- OpenMP -- */

EXTERN void esd_omp_collexit (elg_ui4 rid);

EXTERN void esd_omp_fork (elg_ui4 rid);

EXTERN void esd_omp_join (elg_ui4 rid);

EXTERN void esd_omp_alock (elg_ui4 lkid);

EXTERN void esd_omp_rlock (elg_ui4 lkid);

/* -- Attributes -- */

EXTERN void esd_attr_ui1 (elg_ui1 type, elg_ui1 val);

EXTERN void esd_attr_ui4 (elg_ui1 type, elg_ui4 val);

#endif
