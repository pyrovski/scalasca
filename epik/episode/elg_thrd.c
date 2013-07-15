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

#include "elg_thrd.h"
#include "epk_metric.h"
#include "elg_pform.h"
#include "elg_error.h"
#include "elg_impl.h"
#include "epk_conf.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#if (defined (ELG_MPI) || defined (ELG_OMPI))
#include <mpi.h>
#include "elg_sync.h"
#endif
#if (defined (ELG_OMPI) || defined (ELG_OMP))
#include <omp.h>
#endif

static elg_ui4 tnum = 0;        /* count of known threads */
EXTERN int esd_max_threads;     /* limit for thread creation */
EXTERN int esd_num_metrics;

/* create thread object with initialised base filename and metrics,
   but without allocating (trace) buffers or files */

ElgThrd* ElgThrd_create(elg_ui4 tid)
{
  ElgThrd *thread;

  if (tnum > esd_max_threads)
    elg_error_msg("FATAL: Cannot create more than %d threads", esd_max_threads);

  thread = (ElgThrd*) malloc(sizeof(ElgThrd));
  if ( thread == NULL )
    elg_error();

  /* initialise callpaths object */
  thread->paths = esd_paths_init();

  thread->arc_name = NULL; /* not known yet */
  thread->tmp_name = (char*)calloc(PATH_MAX, sizeof(char*));
  if ( thread->tmp_name == NULL )
    elg_error();

  /* basename includes local path but neither thread identifier nor suffix */
  sprintf(thread->tmp_name, "%s/%s.%lX.%u", epk_get(EPK_LDIR),
          epk_get(EPK_TITLE), elg_pform_node_id(), getpid());

#if (defined (EPK_METR))  
  if (esd_num_metrics > 0) {
    /* create event set */
    thread->metv = esd_metric_create();
  }
#endif

  /* initialize per-thread arrays for counter values + time */
  thread->valv = (elg_ui8*)calloc(esd_num_metrics+1, sizeof(elg_ui8));
  if ( thread->valv == NULL )
      elg_error();

  /* increment the thread object counter (for successful creations) */

#if (defined (ELG_OMPI) || defined (ELG_OMP))
#pragma omp atomic
    tnum++;
#else 
  tnum++;
#endif

  elg_cntl_msg("Thread object #%u created, total number is %u", tid, tnum);
  
  return thread;
}


elg_ui4 ElgThrd_open(ElgThrd* thrd, elg_ui4 tid)
{
  elg_ui4 bsize = epk_str2size(epk_get(ELG_BUFFER_SIZE));
  if (bsize < ELG_MIN_BUFSIZE) {
      elg_warning("ELG_BUFFER_SIZE=%s (%d) resized to %d bytes",
                  epk_get(ELG_BUFFER_SIZE), bsize, ELG_MIN_BUFSIZE);
      bsize = ELG_MIN_BUFSIZE;
  }
#if defined(PARTITION_BSIZE) && (defined (ELG_OMPI) || defined (ELG_OMP))
  if (tid == 0) { /* master thread gets most buffer space */
    bsize = (bsize / 10) * 7;
  } else {        /* worker threads get less buffer space */
    bsize = (bsize / 10);
  }
#endif

  if (thrd && thrd->tmp_name)
    thrd->gen = ElgGen_open(thrd->tmp_name, tid, bsize);

  return bsize;
}


size_t ElgThrd_bsize(ElgThrd* thrd)
{
  size_t bsize = 0;
  if (thrd && thrd->gen) bsize = ElgGen_get_bsize(thrd->gen);
  return bsize;
}

void ElgThrd_close(ElgThrd* thrd)
{
  if (thrd && thrd->tmp_name)
    ElgGen_close(thrd->gen);
}


void ElgThrd_verify(ElgThrd* thrd, elg_ui4 tid)
{
  if (!thrd) { elg_warning("ElgThrd[%u] is NULL", tid); return; }
  if (!thrd->gen) elg_warning("ElgThrd[%u]->gen is NULL", tid);
  if (!thrd->arc_name) elg_warning("ElgThrd[%u]->arc_name is NULL", tid);
  else elg_cntl_msg("ElgThrd[%u]->arc_name=\"%s\"", tid, thrd->arc_name);
  if (!thrd->tmp_name) elg_warning("ElgThrd[%u]->tmp_name is NULL", tid);
  else elg_cntl_msg("ElgThrd[%u]->tmp_name=\"%s\"", tid, thrd->tmp_name);
  if (!thrd->paths) elg_warning("ElgThrd[%u]->paths is NULL", tid);
  if (!thrd->valv) elg_warning("ElgThrd[%u]->valv is NULL", tid);
#if (defined (EPK_METR))
  if (!thrd->metv && esd_metric_num())
        elg_warning("ElgThrd[%u]->metv is NULL (%d)", tid, esd_metric_num());
#endif
}

void ElgThrd_delete(ElgThrd* thrd, elg_ui4 tid)
{
  /* skip delete when actual name matches archive name */
  if (thrd && thrd->gen && (!thrd->arc_name ||
                strcmp(thrd->arc_name, ElgGen_get_name(thrd->gen))))
    ElgGen_delete(thrd->gen);

#if (defined (EPK_METR))
  if ( esd_metric_num() > 0 )
    esd_metric_free(thrd->metv);
#endif
  
  /* XXXX free(thrd->valv) ? */
  free(thrd->tmp_name);
  free(thrd);

  /* decrement the thread object counter */
#if (defined (ELG_OMPI) || defined (ELG_OMP))
#pragma omp atomic
    tnum--;
#else 
  tnum--;
#endif

  elg_cntl_msg("Thread object #%u deleted, leaving %u", tid, tnum);
}


elg_ui4 ElgThrd_get_num_thrds()
{
  return tnum;
}
