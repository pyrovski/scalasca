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

#include <stdlib.h>

#include "esd_event.h"
#include "elg_error.h"
#include "epk_shmem.h"

int epk_shm_regid[EPK__SHMEM_MAX];

void epk_shm_register()
{
  elg_ui4 fid = esd_def_file("SHMEM");

  epk_shm_regid[EPK__SHMEM_INIT] = 
    esd_def_region("shmem_init", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION_COLL_BARRIER);
  epk_shm_regid[EPK__SHMEM_FINALIZE] = 
    esd_def_region("shmem_finalize", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION_COLL_BARRIER);
  epk_shm_regid[EPK__SHMEM_SWAP] = 
    esd_def_region("shmem_swap", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);
  epk_shm_regid[EPK__SHMEM_GETMEM] = 
    esd_def_region("shmem_getmem", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);
  epk_shm_regid[EPK__SHMEM_PUTMEM] = 
    esd_def_region("shmem_putmem", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);
  epk_shm_regid[EPK__BARRIER] = 
    esd_def_region("barrier", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION_COLL_BARRIER);
  epk_shm_regid[EPK__SHMEM_BARRIER_ALL] = 
    esd_def_region("shmem_barrier_all", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION_COLL_BARRIER);
  epk_shm_regid[EPK__SHMEM_BARRIER] = 
    esd_def_region("shmem_barrier", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION_COLL_BARRIER);
  epk_shm_regid[EPK__SHMEM_FENCE] = 
    esd_def_region("shmem_fence", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);
  epk_shm_regid[EPK__SHMEM_QUIET] = 
    esd_def_region("shmem_quiet", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);

  epk_shm_regid[EPK__SHMALLOC] = 
    esd_def_region("shmalloc", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION_COLL_BARRIER);
  epk_shm_regid[EPK__SHMALLOC_NB] = 
    esd_def_region("shmalloc_nb", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);
  epk_shm_regid[EPK__SHFREE] = 
    esd_def_region("shfree", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION_COLL_BARRIER);
  epk_shm_regid[EPK__SHFREE_NB] = 
    esd_def_region("shfree_nb", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);

  epk_shm_regid[EPK__SHMEM_WAIT] =
    esd_def_region("shmem_wait", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);

  epk_shm_regid[EPK__SHMEM_WAIT_UNTIL] =
    esd_def_region("shmem_wait_until", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);

  epk_shm_regid[EPK__SHMEM_CLEAR_LOCK] =
    esd_def_region("shmem_clear_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);

  epk_shm_regid[EPK__SHMEM_SET_LOCK] =
    esd_def_region("shmem_set_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);

  epk_shm_regid[EPK__SHMEM_TEST_LOCK] =
    esd_def_region("shmem_test_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "SHMEM", EPK_FUNCTION);

#include "epk_shmem.c.reg2.gen"
}

/*
 * Local definitions
 */

int epk_shm_id = 0;
int epk_shm_all_cid = -1;

static int nbytes = -1;
static unsigned char* grpv = 0;

typedef struct {
  int start, stride, size, cid;
} EPK_SHMEM_COM;

#define EPK_SHMEM_COM_MAX 10

static EPK_SHMEM_COM epk_shmem_com[EPK_SHMEM_COM_MAX];

int epk_shmem_create_com(int PE_start, int logPE_stride, int PE_size)
{
  int c, i, stride, end, npes, cid;
  extern int epk_comm_get_cid();

  /* init on first call */
  if ( nbytes == -1 ) {
    npes = pshmem_n_pes();
    for (c=0; c<EPK_SHMEM_COM_MAX; ++c) epk_shmem_com[c].cid = -1;
    nbytes = npes / 8 + (npes % 8 ? 1 : 0);
    grpv = calloc(nbytes, sizeof(unsigned char));
  }

  /* search for existing communicator */
  for (c=0; c<EPK_SHMEM_COM_MAX; ++c) {
    if ( epk_shmem_com[c].cid == -1 ) {
      /* not found */
      break;
    } else if ( epk_shmem_com[c].start  == PE_start &&
                epk_shmem_com[c].stride == logPE_stride &&
                epk_shmem_com[c].size   == PE_size ) {
      /* found */
      return epk_shmem_com[c].cid;
    }
  }

  if (c >= EPK_SHMEM_COM_MAX) {
    elg_error_msg("Too many SHMEM communicators");
  }

  /* not found: create new communicator */
  for (i=0; i<nbytes; ++i) grpv[i] = '\0';
  stride = (1<<logPE_stride);
  end    = (PE_start + stride * PE_size);
  for (i = PE_start; i < end; i += stride) grpv[i / 8] |= (1 << (i % 8));

  epk_shmem_com[c].start  = PE_start;
  epk_shmem_com[c].stride = logPE_stride;
  epk_shmem_com[c].size   = PE_size;
  epk_shmem_com[c].cid    = cid = epk_comm_get_cid();
  esd_def_mpi_comm(cid, nbytes, grpv);
  return cid;
}

/*
 * C Wrappers
 */

#include <shmem.h>

void epk_shmem_init()
{
  int i, me, npes;

  /* -- register size with EPIK -- */
  me   = pshmem_my_pe();
  npes = pshmem_n_pes();
  esd_init_trc_id(me, npes);

  /* -- allocate "all pe" group -- */
  epk_comm_init();
  epk_shm_all_cid = epk_shmem_create_com(0, 0, npes);
  free(grpv);
}

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

int shmem_swap(int* target, int value, int pe)
{
  int res;

#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_SWAP]);
#endif
  esd_get_1ts_remote(pe, ELG_NEXT_SHM_ID, sizeof(int));
  esd_get_1te(pe, ELG_CURR_SHM_ID);
  res = pshmem_int_swap(target, value, pe);
  esd_put_1ts(pe, ELG_NEXT_SHM_ID, sizeof(int));
  esd_put_1te_remote(pe, ELG_CURR_SHM_ID);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_SWAP]);
#endif

  return res;
}

void shmem_getmem(void* target, void* source, int len, int pe)
{
#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_GETMEM]);
#endif
  esd_get_1ts_remote(pe, ELG_NEXT_SHM_ID, len);
  pshmem_getmem(target, source, len, pe);
  esd_get_1te(pe, ELG_CURR_SHM_ID);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_GETMEM]);
#endif
}

void shmem_putmem(void* target, void* source, int len, int pe)
{
#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_PUTMEM]);
#endif
  esd_put_1ts(pe, ELG_NEXT_SHM_ID, len);
  pshmem_putmem(target, source, len, pe);
  esd_put_1te_remote(pe, ELG_CURR_SHM_ID);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_PUTMEM]);
#endif
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

void shmem_barrier(int PE_start, int logPE_stride, int PE_size, long* pSync)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMEM_BARRIER]);
#endif
  TRACE_OFF();
  pshmem_barrier(PE_start, logPE_stride, PE_size, pSync);
  TRACE_ON();
  esd_collexit(epk_shm_regid[EPK__SHMEM_BARRIER], ELG_NO_ID,
               epk_shmem_create_com(PE_start, logPE_stride, PE_size), 0, 0);
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

void *shmalloc(size_t size)
{
  void *res;
  extern void *pshmalloc(size_t);

#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMALLOC]);
#endif
  TRACE_OFF();
  res = pshmalloc(size);
  TRACE_ON();
  esd_collexit(epk_shm_regid[EPK__SHMALLOC], ELG_NO_ID, epk_shm_all_cid, 0, 0);
  return res;
}

void *shmalloc_nb(size_t size)
{
  void *res;
  extern void *pshmalloc(size_t);

#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHMALLOC_NB]);
#endif
  TRACE_OFF();
  res = pshmalloc(size);
  TRACE_ON();
#ifndef EPK_CSITE_INST 
  esd_exit(epk_shm_regid[EPK__SHMALLOC_NB]);
#endif
  return res;
}

void shfree(void *ptr)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHFREE]);
#endif
  TRACE_OFF();
  pshfree(ptr);
  TRACE_ON();
  esd_collexit(epk_shm_regid[EPK__SHFREE], ELG_NO_ID, epk_shm_all_cid, 0, 0);
}

void shfree_nb(void *ptr)
{
#ifndef EPK_CSITE_INST 
  esd_enter(epk_shm_regid[EPK__SHFREE_NB]); 
#endif
  TRACE_OFF();
  pshfree(ptr);
  TRACE_ON();
#ifndef EPK_CSITE_INST 
  esd_exit(epk_shm_regid[EPK__SHFREE_NB]);
#endif
}

void shmem_wait(int* ivar, int value)
{
#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_WAIT]);
#endif
  pshmem_wait(ivar, value);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_WAIT]);
#endif
}

void shmem_wait_until(int* ivar, int cmp, int value)
{
#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_WAIT_UNTIL]);
#endif
  pshmem_wait_until(ivar, cmp, value);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_WAIT_UNTIL]);
#endif
}

void shmem_clear_lock(int *lock)
{
#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_CLEAR_LOCK]);
#endif
  pshmem_clear_lock(lock);
  esd_rlock((elg_ui4)lock);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_CLEAR_LOCK]);
#endif
}

void shmem_set_lock(int *lock)
{
#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_SET_LOCK]);
#endif
  esd_alock((elg_ui4)lock);
  pshmem_set_lock(lock);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_SET_LOCK]);
#endif
}

int shmem_test_lock(int *lock)
{
  int res;

#ifndef EPK_CSITE_INST
  esd_enter(epk_shm_regid[EPK__SHMEM_TEST_LOCK]);
#endif
  res = pshmem_test_lock(lock);
  if ( res == 0 ) esd_alock((elg_ui4)lock);
#ifndef EPK_CSITE_INST
  esd_exit(epk_shm_regid[EPK__SHMEM_TEST_LOCK]);
#endif

  return res;
}

#include "epk_shmem.c.wrapper.gen"
