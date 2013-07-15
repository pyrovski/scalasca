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

#ifndef _EPK_SHMEM_H
#define _EPK_SHMEM_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

/* 
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - SHMEM support
 *
 *-----------------------------------------------------------------------------
 */


/*
 * SHMEM function registration
 */

#define EPK__SHMEM_INIT         0
#define EPK__SHMEM_FINALIZE     1
#define EPK__SHMEM_SWAP         2
#define EPK__SHMEM_GETMEM       3
#define EPK__SHMEM_PUTMEM       4
#define EPK__BARRIER            5
#define EPK__SHMEM_BARRIER_ALL  6
#define EPK__SHMEM_BARRIER      7
#define EPK__SHMEM_FENCE        8
#define EPK__SHMEM_QUIET        9
#define EPK__SHMALLOC          10
#define EPK__SHMALLOC_NB       11
#define EPK__SHFREE            12
#define EPK__SHFREE_NB         13
#define EPK__SHMEM_WAIT        14
#define EPK__SHMEM_WAIT_UNTIL  15
#define EPK__SHMEM_CLEAR_LOCK  16
#define EPK__SHMEM_SET_LOCK    17
#define EPK__SHMEM_TEST_LOCK   18

#include "epk_shmem.c.reg1.gen"

EXTERN int epk_shm_regid[EPK__SHMEM_MAX];

/*
 * Local definitions
 */

EXTERN int epk_shm_id;

#define EPK_CURR_SHM_ID   epk_shm_id
#define EPK_NEXT_SHM_ID ++epk_shm_id

EXTERN int epk_mpi_notrace;
#define TRACE_OFF()  epk_mpi_notrace = 1
#define TRACE_ON()   epk_mpi_notrace = 0

EXTERN int epk_shm_all_cid;

EXTERN int epk_shmem_create_com(int PE_start, int logPE_stride, int PE_size);
EXTERN void epk_shmem_init();
EXTERN void epk_shm_register();

#endif
