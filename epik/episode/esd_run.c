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

#include "esd_def.h"
#include "esd_run.h"
#include "esd_event.h"
#include "epk_conf.h"

#include "elg_thrd.h"
#include "esd_paths.h"
#include "epk_archive.h"
#include "epk_memory.h"
#include "esd_gen.h"
#include "elg_gen.h"
#include "epk_mpireg.h"
#include "epk_omplock.h"
#include "epk_metric.h"
#include "elg_pform.h"
#include "elg_error.h"
#include "elg_mrgthrd.h" /* for elg_finalize/elg_mrgthrd */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef USE_SIONLIB
#include "sion.h"
#else 
#define DPRINTFP(A)
#endif

/* External declaration for metric function in esd_metric_papi.c */
extern void esd_metrics_status();

/* initialize compiler adapter interfaces */
void (*epk_comp_status)(void) = NULL;
void (*epk_comp_finalize)(void) = NULL;

#if (defined (ELG_MPI) || defined (ELG_OMPI))
#include <mpi.h>
#include "elg_sync.h"

MPI_Comm esd_comm_world, esd_mach_comm, esd_node_comm;
#endif

#if (defined (ELG_OMPI) || defined (ELG_OMP))
#include <omp.h>
extern void POMP2_Init();
extern void POMP2_On();

/* more robust version of omp_get_max_threads which
 * - can be called outside parallel regions
 * - before main (a problem on at least SUN and NEC)
 */
static int esd_get_max_threads()
{
  int max_threads = 1;
  if (!strcmp(epk_get(ESD_MAX_THREADS),"OMP_NUM_THREADS")) {
    max_threads = omp_get_max_threads();
    if (max_threads < 1) max_threads = elg_pform_num_cpus();
    elg_cntl_msg("ESD_MAX_THREADS=%d (OMP_NUM_THREADS)", max_threads);
  } else {
    max_threads = epk_str2int(epk_get(ESD_MAX_THREADS));
    if (max_threads < 1) elg_error_msg("ESD_MAX_THREADS=%d invalid!", max_threads);
    else elg_cntl_msg("ESD_MAX_THREADS=%d", max_threads);
  }
  return max_threads;
}  
#endif


/* vector of the thread objects */
/*static*/ ElgThrd** thrdv;

static long      my_node    = -1;
static int       my_trace   = -1; /* determined by esd_mpi_init */
static int       num_traces =  1; /* actually num_procs */
static int       init_pid   = -1;

int          esd_num_thrds  = -1; /* threads actually encountered */
int          esd_max_threads = 1; /* limit for thread measurement */

int esd_status = -1; /* episode event handling inactive */

int esd_summary = 0; /* events summarised by EPITOME */
int esd_tracing = 0; /* events forwarded to EPILOG */

EsdProc* proc = 0; /* EsdProc process object for definitions buffer */

/* number of performance metrics */
/*static*/ int esd_num_metrics = 0;

static int esd_open_called = 0;
static int esd_close_called = 0;
#if (defined (ELG_MPI) || defined (ELG_OMPI))
static int esd_mpi_finalized = 0;
#endif

/* clock offsets and local times at initialisation and finalisation */
static double esd_first_offset=0.0, esd_final_offset=0.0;
static double esd_first_ltime, esd_final_ltime;

elg_ui4 esd_unknown_rid = 0, esd_pausing_rid = 0, esd_tracing_rid = 0;

static void elg_finalize (int my_trace, int num_traces);

static elg_ui4 traceBufferSize = 0;

static int epk_base;   /* static memory allocation */

int esd_open()
{
  char *verb_open = strstr(epk_get(EPK_VERBOSE),"open");

  esd_open_called++;
  /* check for multiple initialisation error */
  if ( esd_open_called != 1 ) return esd_open_called;

  /* don't have a rank yet, but check match anyway */
  if (verb_open && elg_rank_match(my_trace, verb_open+strlen("open")))
      elg_verbosity(1);

  epk_base = epk_memusage();

  esd_summary = epk_str2int(epk_get(EPK_SUMMARY));
  esd_tracing = epk_str2int(epk_get(EPK_TRACE));

#if defined (USE_SIONLIB)
  if (esd_tracing && !getenv("ELG_COMPRESSION"))
       epk_conf_set("ELG_COMPRESSION","u"); /* disable compression for SION files */
#endif

  epk_archive_getcwd(); /* stash starting directory */

#if (!defined (ELG_MPI) && !defined (ELG_OMPI))
  my_trace = 0;
  /* verify that measurement archive dir is free */
  /* (defered to MPI_Init/esd_mpi_init for ELG_MPI & ELG_OMPI)*/
  if (epk_archive_verify(NULL))
      epk_archive_create(esd_tracing?EPK_TYPE_ELG:EPK_TYPE_DIR);
  else
      exit(EXIT_FAILURE);
#endif

  /* initialization specific to this platform */
  elg_pform_init();

#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  /* initialize thread support */
  esd_max_threads = esd_get_max_threads();
  elg_error_thread_init(omp_get_thread_num);
#endif

#if (defined (EPK_METR))

  /* initialize hardware counters */
  esd_num_metrics = esd_metric_open();

#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  /* initialize thread support */
  esd_metric_thread_init(omp_get_thread_num);
#endif

#endif

  /* creation of EsdProc process object for definitions buffer */
  proc = EsdProc_open();

  /* creation of ElgThrd objects for callpaths, trace buffers and files */
  thrdv = (ElgThrd**)calloc(esd_max_threads, sizeof(ElgThrd*));
  if ( thrdv == NULL )
    elg_error();
  
  /* allocate and initialize thread data-structures in numeric order */
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
#pragma omp parallel num_threads(esd_max_threads)
{
  int t;
#pragma omp for ordered
  for (t=0; t<esd_max_threads; t++)
  {
#pragma omp ordered
    {
#endif
      elg_ui4 bsize;
      thrdv[ESD_MY_THREAD] = ElgThrd_create(ESD_MY_THREAD);
      bsize = ElgThrd_open(thrdv[ESD_MY_THREAD], ESD_MY_THREAD);
      if (ESD_MY_THREAD == 0) traceBufferSize = bsize;
#if ((!defined (ELG_MPI)) && (!defined(USE_SIONLIB)))
      /* MPI and SIONlib trace file opening deferred to esd_mpi_init or next codeblock */
      if (esd_tracing) ElgGen_create(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]));
#endif
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
    }
  }
}
#endif

  /* trace file opening for OMP iff maybe using SIONLIB */
#if (defined (USE_SIONLIB) && defined (ELG_OMP))
 if (epk_str2int(epk_get(ELG_SION_FILES)) == 0)
   {
     /* not using SIONlib therefore open individual thread trace files now */
     if (esd_tracing) ElgGen_create(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]));
   } 
   else 
   {
     int trace=1, traces=0;
     if (strcmp(epk_get(EPK_LDIR), epk_get(EPK_GDIR)) == 0) {
       if ( ElgGen_reset_name (ESDTHRD_GEN(thrdv[ESD_MY_THREAD]),
			       epk_archive_rankname(EPK_TYPE_ELG,epk_archive_get_name(), my_trace) ) )
	 thrdv[ESD_MY_THREAD]->arc_name = strdup(ElgGen_get_name(ESDTHRD_GEN(thrdv[ESD_MY_THREAD])));
     }
     if (esd_tracing)
       {
         double sion_time = elg_pform_wtime();
         int sionid=-1;
#pragma omp parallel private (sionid)
	 {
	   FILE *fp;
	   int aux_trace;
	   char *sionfname, *sionnewfilename;
	   sion_int64 chunksize   = traceBufferSize;
	   sion_int32 fsblocksize = -1;  /* will be determined by sionlib */
	   int        numfiles    = epk_str2int(epk_get(ELG_SION_FILES));
	   //	   int threadnum = omp_get_thread_num();
	   if (my_trace == 0)  {
	     elg_cntl_msg("ELG_SION_FILES %s %d",epk_get(ELG_SION_FILES),epk_str2int(epk_get(ELG_SION_FILES)));
	   }
	   if (numfiles != 0)
	     {
#pragma omp barrier
	       if (numfiles<0) {
#pragma omp master
		 if (my_trace == 0)  {
		   elg_warning("WARNING: ELG_SION_FILES=%d reset to 1 (no special handling)", numfiles);
		 }
		 numfiles=1;
	       } else
		 if (numfiles>num_traces) {
#pragma omp master
		   if (my_trace == 0)  {
		     elg_warning("WARNING: ELG_SION_FILES=%d reduced to %d (more requested files than processes)",
				 numfiles, num_traces);
		   }
		   numfiles=num_traces;
 	                                       		}
	       if (numfiles>0) {
#pragma omp barrier
		 if (my_trace == 0) elg_cntl_msg("Using %d SION files", numfiles);
		 numfiles=0; /* sionlib will determine actual number */
	       }
#pragma omp master
	       {
		 if (my_trace == 0)  {
		   elg_warning("Pre-Activated %s [%u bytes] (%0.3fs)",
			       epk_archive_get_name(), traceBufferSize,
			       elg_pform_wtime() - sion_time);
		 }
	       }
	       sionfname=epk_archive_filename(EPK_TYPE_SION,epk_archive_directory(EPK_TYPE_ELG));
	       DPRINTFP((1024,"esd_open",my_trace," after sprintf sionfname=%s\n",sionfname));
	       ElgGen_reset_name (ESDTHRD_GEN(thrdv[ESD_MY_THREAD]),sionfname);
	       thrdv[ESD_MY_THREAD]->arc_name = strdup(sionfname);
	       DPRINTFP((1024,"esd_open",my_trace," after sprintf sionfname=%s tbs=%ld\n",sionfname,chunksize));
#pragma omp barrier	
	       sion_time = elg_pform_wtime();
	       /*Create SIONFile for OpenMP code */
	       sionid=sion_paropen_omp(sionfname,
				       "bw",
				       &chunksize,
				       &fsblocksize,
				       &aux_trace,
				       &fp,
				       &sionnewfilename);
#pragma omp barrier
#pragma omp master
	       {
		 if (my_trace == 0)  {
		   elg_warning("Sion-Open     %s [%u bytes] (%0.3fs)",
			       epk_archive_get_name(), traceBufferSize,
			       elg_pform_wtime() - sion_time);
		 }
	       }
	       sion_time = elg_pform_wtime();
	       ElgGen_set_sionid(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]),sionid);
	       free(sionfname);sionfname=NULL;
	     }
	   else {
	     sionid=-1;
	   }
	 }
       }
#pragma omp parallel
     {
#ifndef LAZYOPEN /* open trace files only when required */
       if (!esd_tracing)
	 elg_cntl_msg("Skipping trace creation!");
       else {
         trace = ElgGen_create(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]));
       }
#endif
       
       if (trace != num_traces) {
	 if (my_trace == 0)
	   elg_warning("Abort: %d traces couldn't be created in archive %s",
		       num_traces-traces, epk_archive_get_name());
                                		esd_close_called = 1;
						exit(EXIT_FAILURE);
       } else
	 if (my_trace ==0)
	   elg_cntl_msg("Active measurement archive %s", epk_archive_get_name());	
     }
   }
#endif /* USE_SIONLIB && ELG_OMP */

  /* machine and SMP-node identification */
  my_node = elg_pform_node_id();
#if (!defined (ELG_MPI) && !defined (ELG_OMPI)) 
  /* single process */
  esd_num_thrds = ElgThrd_get_num_thrds();
  esd_def_machine(my_node, 0, 1, esd_max_threads);
  elg_cntl_msg("esd_open %d thrds", esd_max_threads);
#endif

  /* register metrics */
  esd_def_metrics();

  /* register internal regions */
  esd_unknown_rid = esd_def_region("UNKNOWN", esd_def_file("EPIK"),
                                ELG_NO_NUM, ELG_NO_NUM, "EPIK", ELG_UNKNOWN);
  esd_tracing_rid = esd_def_region("TRACING", esd_def_file("EPIK"),
                                ELG_NO_NUM, ELG_NO_NUM, "EPIK", ELG_UNKNOWN);
  esd_pausing_rid = esd_def_region("PAUSING", esd_def_file("EPIK"),
                                ELG_NO_NUM, ELG_NO_NUM, "EPIK", ELG_UNKNOWN);

  /* register MPI and OpenMP API routines if necessary */
#ifndef ELG_CSITE_INST
#if (defined (ELG_OMPI) || defined (ELG_MPI))
  epk_mpi_register();
#endif
#endif
#if (defined (ELG_OMPI) || defined (ELG_OMP))
  POMP2_Init();
  if (esd_tracing) POMP2_On(); /* increment POMP "tracing" level */
#endif
  atexit(esd_close);
  init_pid = getpid();
  elg_cntl_msg("esd_open:init_pid=%d", init_pid);
  esd_status = 0; /* activate event handling */

#if (!defined (ELG_MPI) && !defined (ELG_OMPI))
#if (defined (EPK_METR))
  if (esd_num_metrics > 0) esd_metrics_status();
#endif
  if (epk_comp_status) {
      epk_comp_status(); /* compiler adapter status report */
      epk_comp_status = NULL;
  }

  if (esd_tracing)
      elg_warning("Activated %s [%u bytes]", epk_archive_get_name(),
                traceBufferSize);
  else
      elg_warning("Activated %s [NO TRACE]", epk_archive_get_name());
#endif
  elg_cntl_msg("mem=%d extra=%d", epk_memusage(), epk_memusage()-epk_base);

  if (verb_open) elg_verbosity(0);

  return 0; /* opened/initialized for the first time */
}

void esd_flush()
{
  if (esd_tracing) ElgGen_flush(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]), 2); /* explicit */
}

void esd_close()
{  
#if (defined (ELG_MPI) || defined(ELG_OMPI))
  extern int epk_mpi_nogen; /* from epk_mpiwrap.c */
#endif
  double flush_time, maxFT=0.0, close_time = elg_pform_wtime();
  double clock_drift = 0.0;
  unsigned traceBytes = 0, maxTraceBytes = 0;
  float traceGBytes, totalGBytes;
  double HeapMB, maxHeapMB, sumHeapMB;
  int unified;
  int i;

  char *verb_rank = strstr(epk_get(EPK_VERBOSE),"rank");
  char *verb_close = verb_rank ? NULL : strstr(epk_get(EPK_VERBOSE),"close");
  if (verb_close && elg_rank_match(my_trace, verb_close+strlen("close")))
      elg_verbosity(1);

  elg_cntl_msg("esd_close(called=%d) p%d", esd_close_called, getpid());

  /* catch esd_close called from child processes through atexit */
  if ( init_pid != getpid() ) return;

  esd_close_called++;
  /* check for multiple finalization error */
  if ( esd_close_called != 1 ) return;

#if (defined (ELG_MPI) || defined(ELG_OMPI))
  epk_mpi_nogen = 1; /* disable MPI tracing */

  if (my_trace == -1)
      elg_error_msg("p%d: Abort: MPI_Init interposition unsuccessful!", getpid());
  if (esd_mpi_finalized == 0)
      elg_error_msg("p%d: Abort: apparent exit without MPI_Finalize!", getpid());

  PMPI_Barrier(esd_comm_world);
#else
  /* these additional records maintain consistency with the (O)MPI records */
  /* esd_enter_tracing(); */ /* mark start of finalization */
  /* esd_exit_tracing(); */ /* mark finish of finalization */
#endif

  fflush(stdout);
  if (my_trace == 0)
      elg_cntl_msg("Finalizing experiment %s", epk_archive_get_name());

  epk_archive_return(); /* ensure (back) in starting directory */

  if (esd_status) { /* pause still in effect */
      elg_warning("Re-activating paused event handling");
      esd_exit_pause();
  }

  if (epk_comp_finalize)
      epk_comp_finalize(); /* finalize compiler adapter */

#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  elg_cntl_msg("esd_close %d thrds (%d thrds registered)", ElgThrd_get_num_thrds(), esd_num_thrds);
  /* XXXX how to handle changes in number of threads? */
  omp_set_num_threads(ElgThrd_get_num_thrds());
#pragma omp parallel
  {
#endif
      /* clear stack of un-exited regions */
      esd_paths_exit(ESDTHRD_PATHS(thrdv[ESD_MY_THREAD]));
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  }
#endif

  esd_status = 1; /* deactivate event handling */

#if DEBUG
  /* verify integrity of thread object(s) */
  elg_cntl_msg("Verifying %d thread object(s)", ElgThrd_get_num_thrds());
  for (i = 0; i < ElgThrd_get_num_thrds(); i++)
      ElgThrd_verify(thrdv[i], i);
#endif

  /* determine if VOID threads need to be defined */
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  /* check paths for each (worker) thread */
  for (i = 1; i < ElgThrd_get_num_thrds(); i++) {
      elg_ui4 npaths = esd_paths_size(ESDTHRD_PATHS(thrdv[i]));
      elg_ui4 nroots = esd_paths_roots(ESDTHRD_PATHS(thrdv[i]));
      unsigned unused = esd_paths_void(ESDTHRD_PATHS(thrdv[i]));
      elg_cntl_msg("Thread %d with %d paths and %d roots (unused=%d)",
                  i, npaths, nroots, unused);
      if (unused)
#if defined (USE_SIONLIB)
        if (!(esd_tracing && epk_str2int(epk_get(ELG_SION_FILES))))
#endif
          esd_def_thread(my_trace, i, "VOID");
  }
#endif

  /* write event statistics records */
  if (esd_tracing) {
      unsigned type, types=0, events=0;
      elg_ui4  accum [ELG_ALL_EVENTS] = { 0 };
      elg_ui1  etypes[ELG_ALL_EVENTS] = { 0 };
      elg_ui4  counts[ELG_ALL_EVENTS] = { 0 };

      for (i = 0; i < ElgThrd_get_num_thrds(); i++) {
          size_t   bsize = ElgGen_get_bsize(ESDTHRD_GEN(thrdv[i]));
          size_t   bytes = ElgGen_get_bytes(ESDTHRD_GEN(thrdv[i]));
          elg_ui4* stats = ElgGen_get_stats(ESDTHRD_GEN(thrdv[i]));
          int    flushed = (bytes > bsize);
          traceBytes += bytes;
          if (bytes > maxTraceBytes) maxTraceBytes = bytes;
          for (type = 0; type < ELG_ALL_EVENTS; type++)
              accum[type] += stats[type];
          elg_cntl_msg("Trace [%u] contains %u bytes (flushed=%d)", i, bytes, flushed);
      }
      for (type = 0; type < ELG_ALL_EVENTS; type++) {
          if (accum[type]) {
              etypes[types]=type;
              counts[types]=accum[type];
              types++;
              events += accum[type];
#if DEBUG
              elg_cntl_msg("event_type[%03d]=%u", type, accum[type]);
#endif
          }
      }
      elg_cntl_msg("Trace contains %u bytes for %u events of %u types",
                   traceBytes, events, types);
      esd_def_event_types(types, etypes);
      esd_def_event_counts(types, counts);

      totalGBytes = traceGBytes = (float)traceBytes/(1024*1024*1024);

#if (defined (ELG_MPI) || defined(ELG_OMPI))
      PMPI_Reduce(&traceGBytes, &totalGBytes, 1, MPI_FLOAT, MPI_SUM, 0, esd_comm_world);
      traceBytes = maxTraceBytes;
      PMPI_Reduce(&traceBytes, &maxTraceBytes, 1, MPI_UNSIGNED, MPI_MAX, 0, esd_comm_world);
#endif
  }

  if (my_trace == 0) {
      if (esd_tracing) elg_warning("Closing experiment %s [%0.3fGB] (max %u)",
                   epk_archive_get_name(), totalGBytes, maxTraceBytes);
      else elg_warning("Closing experiment %s", epk_archive_get_name());
      if (epk_comp_status) {
          epk_comp_status(); /* compiler adapter status report */
          epk_comp_status = NULL;
      }
  }

  if (esd_tracing && my_trace == 0) { /* master-only trace flush and free thread buffers */
  elg_cntl_msg("mem=%d extra=%d", epk_memusage(), epk_memusage()-epk_base);
  flush_time = elg_pform_wtime();
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  omp_set_num_threads(ElgThrd_get_num_thrds());
#pragma omp parallel
  {
#endif
      /* close thread (trace event) file */
      ElgThrd_close(thrdv[ESD_MY_THREAD]);
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  }
#endif
  flush_time = elg_pform_wtime() - flush_time;
  elg_cntl_msg("mem=%d extra=%d", epk_memusage(), epk_memusage()-epk_base);
  }

  /* unify worker threads' callpaths with master thread's */
  for (i=1; i<esd_num_thrds; i++)
      esd_paths_unify(ESDTHRD_PATHS(thrdv[0]),ESDTHRD_PATHS(thrdv[i]));

  if (esd_tracing && (num_traces > 1) && !elg_pform_is_gclock()) {
      clock_drift = (esd_final_offset - esd_first_offset) /
                    (esd_final_ltime - esd_first_ltime);
      if ((esd_first_offset > 0.0) || (esd_final_offset > 0.0))
           elg_warning("Offsets %.9f@%.9f %.9f@%.9f / %g",
                       esd_first_offset, esd_first_ltime,
                       esd_final_offset, esd_final_ltime, clock_drift);
  }
  /* add callpath definitions (only for master thread!) */
  esd_paths_def(ESDTHRD_PATHS(thrdv[0]),
                esd_first_ltime, esd_first_offset, clock_drift);

#if defined(ESD_DEBUG_DEFINITIONS)
  /* XXXX write local definitions for debugging */
  epk_archive_create(EPK_TYPE_ESD);
  EsdGen_flush(proc->gen);
#endif

  /* unify definitions */
  unified = EsdProc_unify(proc, my_trace, num_traces, 1);

  /* collate report */
  if (unified && esd_summary) EsdProc_collate(proc, my_trace, num_traces, esd_num_thrds);

  if (unified && esd_tracing && my_trace != 0) { /* flush worker traces */
  flush_time = elg_pform_wtime();
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  omp_set_num_threads(ElgThrd_get_num_thrds());
#pragma omp parallel
  {
#endif
      /* close thread (trace event) file */
      ElgThrd_close(thrdv[ESD_MY_THREAD]);
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  }
#endif
  flush_time = elg_pform_wtime() - flush_time;
  }

#ifdef USE_SIONLIB
#ifdef ELG_MPI
  /* closing the SIONfile for MPI */
  if (esd_tracing) {
    if(epk_str2int(epk_get(ELG_SION_FILES))!=0) {
      int sionid=ElgGen_get_sionid(ESDTHRD_GEN(thrdv[0]));
      DPRINTFP((1024,"esd_close",-1,"before sion_parclose_mpi sionid=%d\n",sionid));
      elg_cntl_msg("esd_close: before sion_parclose_mpi sionid=%d\n",sionid);
      if(!sion_parclose_mpi(sionid))
	elg_warning("Could not close SION file!");
      elg_cntl_msg("esd_close: after sion_parclose_mpi sionid=%d\n",sionid);
      DPRINTFP((1024,"esd_close",-1,"after sion_parclose_mpi sionid=%d\n",sionid));
    }
  }
#endif
#if defined(ELG_OMPI)
  /* closing the SIONfile for OMPI */
  if (esd_tracing) {
    if(epk_str2int(epk_get(ELG_SION_FILES))!=0) {
      omp_set_num_threads(ElgThrd_get_num_thrds());
#pragma omp parallel 
      {
	int sionid;
	sionid=ElgGen_get_sionid(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]));
	DPRINTFP((1024,"esd_close",-1,"before sion_parclose_ompi sionid=%d\n",sionid));
	elg_cntl_msg("esd_close: before sion_parclose_mpi sionid=%d\n",sionid);
	if (sionid>=0)
	  if (!sion_parclose_ompi(sionid))
            elg_warning("Could not close SION file!"); 
	elg_cntl_msg("esd_close: after sion_parclose_mpi sionid=%d\n",sionid);
	DPRINTFP((1024,"esd_close",-1,"after sion_parclose_ompi sionid=%d\n",sionid));
      }
    }
  }
#endif
#if defined(ELG_OMP)
  /* closing the SIONfile for OMP */
  if (esd_tracing) {
    if(epk_str2int(epk_get(ELG_SION_FILES))!=0) {
      omp_set_num_threads(ElgThrd_get_num_thrds());
#pragma omp parallel
      {
	int sionid;
	sionid=ElgGen_get_sionid(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]));
	DPRINTFP((1024,"esd_close",-1,"before sion_parclose_ompi sionid=%d\n",sionid));
	elg_cntl_msg("esd_close: before sion_parclose_mpi sionid=%d\n",sionid);
	if (sionid>=0)
          if (!sion_parclose_omp(sionid))
            elg_warning("Could not close SION file!"); 
	elg_cntl_msg("esd_close: after sion_parclose_mpi sionid=%d\n",sionid);
	DPRINTFP((1024,"esd_close",-1,"after sion_parclose_ompi sionid=%d\n",sionid));
      }
    }
  }
#endif
#endif /* USE_SIONLIB */

  /* call cleanup functions */

#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  extern unsigned esd_extra_thrds;
  if (esd_extra_thrds)
      elg_warning("Only %d of %d threads included in measurement! (ESD_MAX_THREADS=%s)",
                esd_num_thrds, esd_num_thrds+esd_extra_thrds, epk_get(ESD_MAX_THREADS));
  /* OpenMP locks */
  epk_lock_close();
#endif

  /* hardware counters */
#if (defined (EPK_METR))
  if ( esd_num_metrics > 0 )
    esd_metric_close();
#endif
       
  /* merge thread and process trace files */
  if (esd_tracing) elg_finalize(my_trace, num_traces);

#if DEBUG
  /* finalize callpath tracking */
  for (i = 0; i < num_traces; i++) {
#if (defined (ELG_MPI) || defined(ELG_OMPI))
      PMPI_Barrier(esd_comm_world);
#endif
      sleep(1);
      if (my_trace == i) {
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
          omp_set_num_threads(ElgThrd_get_num_thrds());
#pragma omp parallel
          {
#endif
          int t;
          for (t = 0; t<ElgThrd_get_num_thrds(); t++) {
              if (t==ESD_MY_THREAD) esd_paths_dump(ESDTHRD_PATHS(thrdv[t]));
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
#pragma omp barrier        
#endif
          }
          fflush(stderr);
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
          }
#endif
      }
  }
#if (defined (ELG_MPI) || defined(ELG_OMPI))
  PMPI_Barrier(esd_comm_world);
#endif
#endif /* DEBUG */

  /* delete temporary trace files and free temporary file names. This
     has to be done inside a parallel region because esd_metric_free()
     needs to be called by the thread itself. */

#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  omp_set_num_threads(ElgThrd_get_num_thrds());
#pragma omp parallel
  {
#endif
    if (esd_tracing) ElgThrd_delete(thrdv[ESD_MY_THREAD], ESD_MY_THREAD);
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
  }
#endif
  
  free(thrdv);    

  elg_cntl_msg("mem=%d extra=%d", epk_memusage(), epk_memusage()-epk_base);

  sumHeapMB = maxHeapMB = HeapMB = (double) epk_memusage() / (1024.0);
#if (defined (ELG_MPI) || defined(ELG_OMPI))
  {
    PMPI_Reduce(&HeapMB, &maxHeapMB, 1, MPI_DOUBLE, MPI_MAX, 0, esd_comm_world);
    PMPI_Reduce(&HeapMB, &sumHeapMB, 1, MPI_DOUBLE, MPI_SUM, 0, esd_comm_world);
    PMPI_Reduce(&flush_time, &maxFT, 1, MPI_DOUBLE, MPI_MAX, 0, esd_comm_world);
  }
#endif

  if (my_trace == 0) {
      if (esd_tracing)
          elg_warning("1flush=%0.3fGB@%0.3fMB/s, Pflush=%0.3fGB@%0.3fMB/s",
                traceGBytes, (traceGBytes*1024/flush_time), totalGBytes-traceGBytes,
                (num_traces>1) ? (totalGBytes-traceGBytes)*1024/maxFT : 0.0);
      elg_warning("Closed experiment %s (%0.3fs) maxHeap(%s)=%0.3f/%0.3fMB",
                    epk_archive_get_name(), elg_pform_wtime() - close_time,
                    (HeapMB==maxHeapMB) ? "0" : "*", maxHeapMB, sumHeapMB);
      epk_archive_unlock();
  }

#if (defined (ELG_MPI) || defined(ELG_OMPI))
  {
    int retval = PMPI_Finalize();
    if (retval != 0) elg_warning("MPI_Finalize=%d", retval);
  }
#endif
}

/* -- unused function? BM 090828
#if (defined (ELG_MPI) || defined (ELG_OMPI))
static int longcmp(const void *a, const void *b)
{
  long x = *(long*)a;
  long y = *(long*)b;
  return x-y;
}
#endif
-- */

/* NB: elg_otf_trc.c contains a similar (but slightly different) esd_mpi_init */

/* handle MPI process collection initialization and finalization to
   determine rank identifiers and clock offsets */

int esd_mpi_init()
{
#if (defined (ELG_MPI) || defined (ELG_OMPI))
  int myrank_mach=-1, myrank_node=-1;
  int archive, archives=0, created=0;
  MPI_Status status;
  int trace=1, traces=0;
  double start_time = elg_pform_wtime();

  /* verbose "rank" overrides separate stages */
  char *verb_rank = strstr(epk_get(EPK_VERBOSE),"rank");
  char *verb_init = verb_rank ? NULL : strstr(epk_get(EPK_VERBOSE),"init");
  char *verb_parallel = verb_rank ? NULL : strstr(epk_get(EPK_VERBOSE),"parallel");

#ifdef USE_SIONLIB
  double sion_time  = elg_pform_wtime();
  int sionid=-1;
#endif

  fflush(stdout);
  fflush(stderr);
  esd_enter_tracing(); /* mark start of initialization */

  /* ensure we have a dedicated communicator */
  PMPI_Comm_dup(MPI_COMM_WORLD, &esd_comm_world);

  /* determine my_trace global rank and number of traces/processes */
  PMPI_Comm_rank(esd_comm_world, &my_trace);
  PMPI_Comm_size(esd_comm_world, &num_traces);
  
  elg_error_pid(my_trace);

  if (verb_rank && elg_rank_match(my_trace, verb_rank+strlen("rank")))
      elg_verbosity(1);
  if (verb_init && elg_rank_match(my_trace, verb_init+strlen("init")))
      elg_verbosity(1);

  /* pgCC on CrayXT (sometimes?) results in forked child processes doing MPI_Init */
  if (init_pid!=getpid()) {
      elg_warning("esd_mpi_init resetting init_pid %d to %d!", init_pid, getpid());
      init_pid=getpid();
  }

  /* create communicator containing all processes on the same machine */
  PMPI_Comm_split(esd_comm_world,
        (epk_str2int(epk_get(EPK_MACHINE_ID)) & 0x7FFFFFFF), 0, &esd_mach_comm);
  PMPI_Comm_rank(esd_mach_comm, &myrank_mach);
  
  /* create communicator containing all processes on the same node */
  PMPI_Comm_split(esd_mach_comm,
        (elg_pform_node_id() & 0x7FFFFFFF), 0, &esd_node_comm);
  PMPI_Comm_rank(esd_node_comm, &myrank_node);

  /* verify that measurement archive dir is free */
  archive = epk_archive_exists(NULL);
  PMPI_Allreduce(&archive, &archives, 1, MPI_INT, MPI_SUM, esd_comm_world);

  if (archives > 0) {
        if (my_trace == 0) epk_archive_verify(NULL); /* report */
        esd_close_called = 1;
        PMPI_Finalize();
        exit(EXIT_FAILURE);
  }

  /* (only) master attempts to create measurement archive */
  if (my_trace == 0) {
      created = epk_archive_create(esd_tracing?EPK_TYPE_ELG:EPK_TYPE_DIR);
      if (created)
          elg_warning("Created new measurement archive %s", epk_archive_get_name());
  } else
      elg_cntl_msg("Awaiting new measurement archive...");

  /* master needs to inform workers of archive status */
  PMPI_Bcast(&created, 1, MPI_INT, 0, esd_comm_world);
  if (created <= 0) { /* archive creation failed */
      esd_close_called = 1;
      PMPI_Finalize();
      exit(EXIT_FAILURE);
  }

  if (esd_tracing) {
      /* deputy machine check for new archive */
      if ((myrank_mach == 0) && (my_trace != 0) &&
                            ((archive = epk_archive_exists(NULL)) == 0)) {
          elg_warning("Creating missing measurement archive [mach: %s]...", elg_pform_name());
          created = epk_archive_create(esd_tracing?EPK_TYPE_ELG:EPK_TYPE_DIR);
      }

      PMPI_Barrier(esd_comm_world);

      /* deputy node check for new archive */
      if ((myrank_node == 0) && (myrank_mach != 0) &&
                            ((archive = epk_archive_exists(NULL)) == 0)) {
          elg_warning("Creating missing measurement archive [node: %s]...", elg_pform_node_name());
          created = epk_archive_create(esd_tracing?EPK_TYPE_ELG:EPK_TYPE_DIR);
      }

      PMPI_Barrier(esd_comm_world);

      /* final check by remaining processes for new archive */
      if ((myrank_node != 0) && (archive = epk_archive_exists(NULL)) == 0) {
          elg_warning("Creating missing measurement archive [rank]...");
          created = epk_archive_create(esd_tracing?EPK_TYPE_ELG:EPK_TYPE_DIR);
      }

      PMPI_Barrier(esd_comm_world);
  }

  /* verify visibility of new archive for all processes */
  archive = epk_archive_exists(NULL);
  PMPI_Allreduce(&archive, &archives, 1, MPI_INT, MPI_SUM, esd_comm_world);

  if (archives != num_traces) {
        if (my_trace == 0)
            elg_warning("%s: %d processes couldn't locate archive %s",
                        esd_tracing ? "Abort" : "Warning",
                        num_traces-archives, epk_archive_get_name());
        /* only continue if archive not required for tracing */
        if (esd_tracing) {
            esd_close_called = 1;
            PMPI_Finalize();
            exit(EXIT_FAILURE);
        }
  }

  /* attempt to switch temporary definitions filename to archive filename */
  if ( EsdGen_reset_name (ESDPROC_GEN(proc),
              epk_archive_rankname(EPK_TYPE_ESD, epk_archive_get_name(), my_trace) ) )
      proc->arc_name = strdup(EsdGen_get_name(ESDPROC_GEN(proc)));

#if (defined(ELG_MPI)) /* not ELG_OMPI for now! */
  if (strcmp(epk_get(EPK_LDIR), epk_get(EPK_GDIR)) == 0) {
      /* attempt to switch temporary event trace filename to archive filename */
      if ( ElgGen_reset_name (ESDTHRD_GEN(thrdv[0]),
                     epk_archive_rankname(EPK_TYPE_ELG,
                                          epk_archive_get_name(), my_trace) ) )
          thrdv[0]->arc_name = strdup(ElgGen_get_name(ESDTHRD_GEN(thrdv[0])));
  }
#endif

#if (defined (USE_SIONLIB) && defined(ELG_MPI))
  if (esd_tracing) 
    {
      FILE *fp;
      char *sionfname, *sionnewfilename;
      sion_int64 chunksize   = traceBufferSize;
      sion_int32 fsblocksize = -1;  /* will be determined by sionlib */
      int        numfiles    = epk_str2int(epk_get(ELG_SION_FILES));
      MPI_Comm gcomm         = MPI_COMM_WORLD;
      MPI_Comm lcomm         = MPI_COMM_NULL;
      
      if (my_trace == 0)  {
	elg_cntl_msg("ELG_SION_FILES %s %d",epk_get(ELG_SION_FILES),epk_str2int(epk_get(ELG_SION_FILES)));
      }
      if (numfiles != 0) 
	{
	  PMPI_Barrier(esd_comm_world);
	  if (numfiles<0) {
#if defined(__bgq__)
            int bridge = elg_pform_bridge();
	    /* communicator consists of all tasks working with the same I/O-node bridge on BG/Q */
	    PMPI_Comm_split(gcomm, bridge, my_trace, &lcomm);
	    if (my_trace == 0) elg_warning("Generated BG/Q I/O bridge communicator for SION files");
#elif defined(__bgp__)
	    /* communicator consists of all tasks working with the same I/O-node on BG/P */
	    MPIX_Pset_same_comm_create(&lcomm);
	    if (my_trace == 0) elg_warning("Generated BG/P MPIX_Pset communicator for SION files");
#else
	    if (my_trace == 0)  {
	      elg_warning("WARNING: ELG_SION_FILES=%d reset to 1 (no special handling)", numfiles);
	    }
	    numfiles=1;
#endif       
	  } else 
	    if (numfiles>num_traces) {
	      if (my_trace == 0)  {
		elg_warning("WARNING: ELG_SION_FILES=%d reduced to %d (more requested files than tasks)",
			    numfiles, num_traces);
	      }
	      numfiles=num_traces;
	    }
	  if (numfiles>0) { /* prepare communicators for sionlib */
#ifdef ESD_SION_ROUNDROBIN
	    int color = my_trace % numfiles; /* round-robin */
#else
	    int color = ((float)numfiles)*my_trace/num_traces; /* blockwise */
#endif
	    PMPI_Comm_split(gcomm, color, my_trace, &lcomm);
	    if (my_trace == 0) elg_cntl_msg("Using %d SION_FILES", numfiles);
	    numfiles=0; /* sionlib will determine actual number */
	  }	
	  
	  if (my_trace == 0)  {
	    elg_cntl_msg("Pre-Activated %s [%u bytes] (%0.3fs)",
			epk_archive_get_name(), traceBufferSize,
			elg_pform_wtime() - sion_time);
	  }		
	  /* bcast pid of first mpi task to all other tasks, it will be used to build a common filename for the sion file */
	  sionfname=epk_archive_filename(EPK_TYPE_SION,epk_archive_directory(EPK_TYPE_ELG));
	  DPRINTFP((1024,"esd_mpi_init",my_trace," after sprintf sionfname=%s\n",sionfname));
	  
	  /* collective open of sion file */
	  PMPI_Barrier(esd_comm_world);
	  sion_time = elg_pform_wtime();
	  
	  sionid=sion_paropen_mpi( sionfname,       
				   "bw",
				   &numfiles,
				   gcomm,
				   &lcomm,
				   &chunksize, 
				   &fsblocksize, 
				   &my_trace, 
				   &fp,
				   &sionnewfilename);

	  ElgGen_reset_name (ESDTHRD_GEN(thrdv[0]),sionnewfilename);
	  DPRINTFP((1024,"esd_mpi_init",my_trace," after sprintf sionfname=%s tbs=%ld\n",sionnewfilename,chunksize));
	  thrdv[0]->arc_name = strdup(sionnewfilename);
	  
	  PMPI_Barrier(esd_comm_world);
	  if (my_trace == 0)  {
	    elg_cntl_msg("Sion-Open     %s [%u bytes] (%0.3fs)",
			epk_archive_get_name(), traceBufferSize,
			elg_pform_wtime() - sion_time);
	  }
	  
	  sion_time = elg_pform_wtime();
	  ElgGen_set_sionid(ESDTHRD_GEN(thrdv[0]),sionid);
	  free(sionfname);sionfname=NULL;
	} 
      else {
	sionid=-1;
      }
    }
#endif /* USE_SIONLIB && ELG_MPI */

#if (defined(USE_SIONLIB) && defined(ELG_OMPI))
  if (esd_tracing)
    {
#pragma omp parallel private (sionid)
      {
	FILE *fp;
	int aux_trace;
	char *sionfname, *sionnewfilename;
	sion_int64 chunksize   = traceBufferSize;
	sion_int32 fsblocksize = -1;  /* will be determined by sionlib */
	int        numfiles    = epk_str2int(epk_get(ELG_SION_FILES));
	MPI_Comm gcomm         = MPI_COMM_WORLD;
	MPI_Comm lcomm         = MPI_COMM_NULL;
	/* int threadnum = omp_get_thread_num(); */
	
	if (my_trace == 0)  {
	  elg_cntl_msg("ELG_SION_FILES %s %d",epk_get(ELG_SION_FILES),epk_str2int(epk_get(ELG_SION_FILES)));
	}
	if (numfiles != 0)
	  {
	    
#pragma omp master
	    {       
	      PMPI_Barrier(esd_comm_world);
	    }		
	    
	    if (numfiles<0) {
#if defined(__bgq__)
#pragma omp master
              {
                int bridge = elg_pform_bridge();
                /* communicator consists of all tasks working with the same I/O-node bridge on BG/Q */
                PMPI_Comm_split(gcomm, bridge, my_trace, &lcomm);
                if (my_trace == 0) elg_warning("Generated BG/Q I/O bridge communicator for SION files");
              }
#elif defined(__bgp__)
	      /* communicator consists of all tasks working with the same I/O-node on BG/P */
	      MPIX_Pset_same_comm_create(&lcomm);
	      if (my_trace == 0) elg_warning("Generated BG/P MPIX_Pset communicator for SION files");
#else
	      numfiles=num_traces;
#pragma omp master
	      if (my_trace == 0)  {
		elg_warning("WARNING: ELG_SION_FILES reset to %d (one per MPI process)", numfiles);
	      }
#endif
	    } else
	      if (numfiles>num_traces) {
#pragma omp master
		if (my_trace == 0)  {
		  elg_warning("WARNING: ELG_SION_FILES=%d reduced to %d (more requested files than processes)",
			      numfiles, num_traces);
		}
		numfiles=num_traces;
	      }
	    
	    if (numfiles>0) { /* prepare communicators for sionlib */
#ifdef ESD_SION_ROUNDROBIN
	      int color = my_trace % numfiles; /* round-robin */
#else
	      int color = ((float)numfiles)*my_trace/num_traces; /* blockwise */
#endif
#pragma omp master
	      {       
		PMPI_Comm_split(gcomm, color, my_trace, &lcomm);
	      }
	      if (my_trace == 0) elg_cntl_msg("Using %d SION_FILES", numfiles);
              
	      numfiles=0; /* sionlib will determine actual number */
	    }
#pragma omp master
	    {
	      if (my_trace == 0)  {
		elg_cntl_msg("Pre-Activated %s [%u bytes] (%0.3fs)",
			    epk_archive_get_name(), traceBufferSize,
			    elg_pform_wtime() - sion_time);
	      }
	    }
	    
	    /* bcast pid of first mpi task to all other tasks, it will be used to build a common filename for the sion file */
	    sionfname=epk_archive_filename(EPK_TYPE_SION,epk_archive_directory(EPK_TYPE_ELG));
	    DPRINTFP((1024,"esd_mpi_init",my_trace," after sprintf sionfname=%s\n",sionfname));

	    /* collective open of sion file */
#pragma omp barrier					
	    sion_time = elg_pform_wtime();
	    sionid=sion_paropen_ompi( sionfname,
				      "bw",
				      &numfiles,
				      gcomm,
				      &lcomm,
				      &chunksize,
				      &fsblocksize,
				      &aux_trace,
				      &fp,
				      &sionnewfilename);

	    ElgGen_reset_name (ESDTHRD_GEN(thrdv[ESD_MY_THREAD]),sionnewfilename);
	    DPRINTFP((1024,"esd_mpi_init",my_trace," after sprintf sionfname=%s tbs=%ld\n",sionnewfilname,chunksize));
    	    thrdv[ESD_MY_THREAD]->arc_name = strdup(sionnewfilename);
#pragma omp barrier
#pragma omp master
	    {	
              PMPI_Barrier(esd_comm_world);
	      if (my_trace == 0)  {
		elg_cntl_msg("Sion-Open     %s [%u bytes] (%0.3fs)",
			    epk_archive_get_name(), traceBufferSize,
			    elg_pform_wtime() - sion_time);
	      }
	    }
	    sion_time = elg_pform_wtime();
	    ElgGen_set_sionid(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]),sionid);
	    free(sionfname);sionfname=NULL;
	  }
	else {
	  sionid=-1;
	}	
	
      } /* end of open mp threads */
    }
#endif /* USE_SIONLIB && ELG_OMPI */
  
#ifndef LAZYOPEN /* open trace files only when required */
#if defined(ELG_OMPI)
#pragma omp parallel
#endif
  {
    if (!esd_tracing)
      elg_cntl_msg("Skipping trace creation!");
    else {
      int serial = (getenv("ESD_CREATE_SERIAL") != NULL);
      if (serial) { /* serialize trace file creation */
	if (my_trace > 0)     /* wait for token from predecessor */
	  PMPI_Recv(&trace, 1, MPI_INT, my_trace-1, 0, esd_comm_world, &status);
	else
	  elg_warning("Serializing creation of trace files");
      }
      if (trace != 0)       /* create file for each thread */
	trace = ElgGen_create(ESDTHRD_GEN(thrdv[ESD_MY_THREAD]));
      if (serial) { /* serialize trace file creation */
	if (num_traces > 1) { /* avoid deadlock with single MPI process */
	  /* send status token to successor rank */
	  PMPI_Send(&trace, 1, MPI_INT, (my_trace+1)%num_traces, 0, esd_comm_world);
	  if (my_trace == 0) { /* collect final token from last rank */
	    int ack = 0;
	    PMPI_Recv(&ack, 1, MPI_INT, num_traces-1, 0, esd_comm_world, &status);
	    if (ack == 0) elg_warning("Failed to create all tracefiles!");
	  }
	}
      }
    }
  }	
#endif /* LAZYOPEN */
  
  /* check that all traces are ready */
  PMPI_Allreduce(&trace, &traces, 1, MPI_INT, MPI_SUM, esd_comm_world);
  if (traces != num_traces) {
    if (my_trace == 0)
      elg_warning("Abort: %d traces couldn't be created in archive %s",
		  num_traces-traces, epk_archive_get_name());
    esd_close_called = 1;
    PMPI_Finalize();
    exit(EXIT_FAILURE);
  } else if (my_trace == 0)
    elg_cntl_msg("Active measurement archive %s", epk_archive_get_name());

  /* define node in machine and topology */
  esd_num_thrds = ElgThrd_get_num_thrds();
  elg_cntl_msg("MPI_Init %d/%d thrds", esd_num_thrds, esd_max_threads);
  esd_def_machine(my_node, my_trace, num_traces, esd_max_threads);

  /* 1. clock synchronization if necessary */
  if (esd_tracing && (num_traces > 1) && !elg_pform_is_gclock())
    {
      /* measure offset */
      esd_first_offset = elg_offset(&esd_first_ltime, esd_comm_world, esd_mach_comm, esd_node_comm);
      /* write OFFSET record */  
      EsdGen_write_OFFSET(ESDPROC_GEN(proc), esd_first_ltime, esd_first_offset);
    }

  elg_pform_mpi_init();

#ifdef USE_SIONLIB
  if(sionid>=0)   DPRINTFP((1024,"esd_mpi_init",my_trace," before atexit\n"));
#endif

  atexit(esd_close); /* re-register to be called on exit before MPI's atexit */

#ifdef USE_SIONLIB
  if(sionid>=0)   DPRINTFP((1024,"esd_mpi_init",my_trace," after  atexit\n"));
#endif

  if (my_trace == 0) {
#if (defined (EPK_METR))
      if (esd_num_metrics > 0) esd_metrics_status();
#endif
      if (epk_comp_status) {
          epk_comp_status(); /* compiler adapter status report */
          epk_comp_status = NULL;
      }
      if (esd_tracing)
          elg_warning("Activated %s [%u bytes] (%0.3fs)",
                epk_archive_get_name(), traceBufferSize,
                elg_pform_wtime() - start_time);
      else
          elg_warning("Activated %s [NO TRACE] (%0.3fs)",
                epk_archive_get_name(),
                elg_pform_wtime() - start_time);
  }


#ifdef USE_SIONLIB
  if(sionid>=0)  {
    if (my_trace == 0)  {
      elg_warning("Post-Activatd %s [%u bytes] (%0.3fs)",
		  epk_archive_get_name(), traceBufferSize,
		  elg_pform_wtime() - sion_time);
    }
  }
  sion_time = elg_pform_wtime();
#endif

  fflush(stderr);
  esd_exit_tracing(); /* mark end of initialization */

#ifdef USE_SIONLIB
  if(sionid>=0)  {
    if (my_trace == 0)  {
      elg_warning("Post-flush    %s [%u bytes] (%0.3fs)",
		  epk_archive_get_name(), traceBufferSize,
		  elg_pform_wtime() - sion_time);
    }
  }
#endif

  if (verb_init) elg_verbosity(0);

  /* "parallel" stage from end of MPI "init" to start of "finalize" */
  if (verb_parallel && elg_rank_match(my_trace, verb_parallel+strlen("parallel")))
      elg_verbosity(1);

#endif /* (defined (ELG_MPI) || defined (ELG_OMPI)) */

  return esd_tracing; /* inform adapter whether tracing is configured */
}

void esd_mpi_finalize( )
{
#if (defined (ELG_MPI) || defined (ELG_OMPI))

  /* verbose "rank" overrides separate stages */
  char *verb_rank = strstr(epk_get(EPK_VERBOSE),"rank");
  char *verb_finalize = verb_rank ? NULL : strstr(epk_get(EPK_VERBOSE),"finalize");
  char *verb_parallel = verb_rank ? NULL : strstr(epk_get(EPK_VERBOSE),"parallel");

  /* XXXX check MPI_Finalized? */

  PMPI_Barrier(esd_comm_world);

  esd_enter_tracing(); /* mark start of finalization */

  if (verb_parallel) elg_verbosity(0);
  if (verb_finalize && elg_rank_match(my_trace, verb_finalize+strlen("finalize")))
      elg_verbosity(1);

  /* 2. clock synchronization if necessary */ 
  if (esd_tracing && (num_traces > 1) && !elg_pform_is_gclock())
    {
      /* measure offset */
      esd_final_offset = elg_offset(&esd_final_ltime, esd_comm_world, esd_mach_comm, esd_node_comm);
      /* write OFFSET record */
      EsdGen_write_OFFSET(ESDPROC_GEN(proc), esd_final_ltime, esd_final_offset);
    }

  PMPI_Barrier(esd_comm_world);

  esd_exit_tracing(); /* mark end of finalization */

  esd_mpi_finalized=1;

  /* emergency exit for cases where atexit not handled */
  if (getenv("ESD_CLOSE_ON_MPI_FINALIZE")) {
      elg_cntl_msg("ESD_CLOSE_ON_MPI_FINALIZE");
      esd_close(); /* close on finalize */
  }

  if (verb_finalize) elg_verbosity(0);
#endif

  elg_pform_mpi_finalize();
}

/* process rank and number of processes provided by epk_shmem_init */

void esd_init_trc_id(int my_id, int num_procs)
{
  my_trace   = my_id;
  num_traces = num_procs;
}


static void elg_finalize (int my_trace, int num_traces)
{  
  int i;
  char* local_name = epk_archive_rankname(EPK_TYPE_ELG,
                                          epk_archive_get_name(), my_trace);

#if !defined (ELG_MPI) /* MPI traces are already in archive */
  int numsionfiles = 0;
#if defined(USE_SIONLIB)
  numsionfiles = epk_str2int(epk_get(ELG_SION_FILES));
#endif
  if (numsionfiles == 0) /* not using sionlib */
    { /* merge & archive thread traces */
      double start_time = elg_pform_wtime();
      char** tmp_namev;
      if (my_trace==0) elg_warning("Merging thread traces...");
      epk_archive_update(local_name); /* prepare to update archive */
      tmp_namev = (char**)calloc(ElgThrd_get_num_thrds(), sizeof(char*));
      for(i=0; i<ElgThrd_get_num_thrds(); i++)
	tmp_namev[i] = ElgGen_get_name(thrdv[i]->gen);
      /* merge per-thread traces and copy per-process trace
      	 from temporary directory to archive directory */
      elg_mrgthrd(local_name, ElgThrd_get_num_thrds(), tmp_namev);
      free(tmp_namev);
      
      /* commit process trace with archive name */
      epk_archive_commit(local_name);
      elg_warning("Merging thread traces... (%0.3fs)", elg_pform_wtime()-start_time);
    }
  else /* sionlib thread archives don't need merging ...*/
#endif 
  if (strcmp(epk_get(EPK_LDIR), epk_get(EPK_GDIR)) == 0) {
    elg_cntl_msg("Skipping elg_mrgthrd for archived %s", local_name);
  }

#if (defined (ELG_MPI) || defined(ELG_OMPI))
  /* ensure all rank traces are ready */
  PMPI_Barrier(esd_comm_world);
  if (my_trace == 0) elg_cntl_msg("All %d rank traces ready", num_traces);
#endif

  /*- Rank 0: merge trace files -*/
  if (my_trace == 0 && epk_str2bool(epk_get(ELG_MERGE)))
    {
      double start_time = elg_pform_wtime();
      int len = strlen(epk_get(EPK_GDIR)) + strlen(epk_get(EPK_TITLE)) + 16;
      int ret;
      char *cmd;

      /*- check files are ready -*/
      for (i = 0; i < num_traces; i++)
        {
          local_name = epk_archive_rankname(EPK_TYPE_ELG, epk_archive_get_name(), i);
          if (access(local_name, R_OK) != 0 )
              elg_error_msg("Missing trace %s", local_name);
        }

      /*- do actual merge -*/
#ifdef BINDIR
      if (access(BINDIR "/elg_merge", X_OK) == 0 )
        {
	  cmd = calloc(strlen(BINDIR) + 16 + len, sizeof(char));
	  if ( cmd == NULL )
	    elg_error();

          sprintf(cmd, "%s/elg_merge %s", BINDIR, epk_archive_get_name());
        }
      else
        {
#endif
	  cmd = calloc(10 + len, sizeof(char));
	  if ( cmd == NULL )
	    elg_error();

          sprintf(cmd, "elg_merge %s", epk_archive_get_name());
#ifdef BINDIR
        }
#endif
      elg_cntl_msg(cmd);
      ret = system(cmd);
      if (ret == -1)
          elg_warning("system(\"%s\") failed: %s", cmd, strerror(errno));
      else
          elg_warning("Merging process traces... done (%0.3fs)", elg_pform_wtime()-start_time);
      free(cmd);
    }

  free(local_name);
}
