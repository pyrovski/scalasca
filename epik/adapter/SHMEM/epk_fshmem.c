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
#include "epk_shmem.h"

/*
 * Fortran Wrappers
 */

void shmem_init(void)
{
  esd_open();
  epk_shm_register();

#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMEM_INIT]);
#endif
  TRACE_OFF();
  pshmem_init();
  TRACE_ON();

  epk_shmem_init();
  esd_collexit(epk_shm_regid[EPK__SHMEM_INIT],
               ELG_NO_ID, epk_shm_all_cid, 0, 0);
}

void shmem_finalize(void)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMEM_FINALIZE]);
#endif
  TRACE_OFF();
  pshmem_finalize();
  TRACE_ON();
  esd_collexit(epk_shm_regid[EPK__SHMEM_FINALIZE],
                ELG_NO_ID, epk_shm_all_cid, 0, 0);
}

void barrier(void)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__BARRIER]);
#endif
  TRACE_OFF();
  pbarrier();
  TRACE_ON();
  esd_collexit(epk_shm_regid[EPK__BARRIER],
               ELG_NO_ID, epk_shm_all_cid, 0, 0);
}

void shmem_barrier_all(void)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMEM_BARRIER_ALL]);
#endif
  TRACE_OFF();
  pshmem_barrier_all();
  TRACE_ON();
  esd_collexit(epk_shm_regid[EPK__SHMEM_BARRIER_ALL],
                ELG_NO_ID, epk_shm_all_cid, 0, 0);
}

void shmem_barrier(int* PE_start, int* logPE_stride, int* PE_size, long* pSync)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMEM_BARRIER]);
#endif
  TRACE_OFF();
  pshmem_barrier(PE_start, logPE_stride, PE_size, pSync);
  TRACE_ON();
  esd_collexit(epk_shm_regid[EPK__SHMEM_BARRIER],
                ELG_NO_ID, epk_shmem_create_com(*PE_start, *logPE_stride, *PE_size), 0, 0);
}

void shmem_fence(void)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMEM_FENCE]);
#endif
  pshmem_fence();
#ifndef EPK_CSITE_INST 
  esd_exit(epk_shm_regid[EPK__SHMEM_FENCE]); 
#endif
}

void shmem_quiet(void)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMEM_QUIET]);
#endif
  pshmem_fence();
#ifndef EPK_CSITE_INST 
  esd_exit(epk_shm_regid[EPK__SHMEM_QUIET]); 
#endif
}

void shmem_wait(int* ivar, int* value)
{
#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_WAIT]);
#endif
  pshmem_wait(ivar, value);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_WAIT]);
#endif
}

#include "epk_fshmem.c.wrapper.gen"
