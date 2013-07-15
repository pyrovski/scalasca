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

/*
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - Portland Group (PGI) compiler interface
 * 
 *-----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "epk_conf.h"
#include "epk_comp.h"
#include "elg_error.h"
#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

struct s1 {
	long l1;
        long l2;
	double d1;
	double d2;
        long isseen;
	char *c;
	void *p1;
	long lineno;
	void *p2;
	struct s1 *p3;
        int fid;
        int rid;
	char *file;
	char *rout;
};

#define ESD_DEF_REGION(p)                                     \
{                                                             \
    nfuncs++;                                                 \
    if (!epk_filter_check(p->rout)) {                         \
        nblack++;                                             \
        p->rid = ELG_NO_ID;                                   \
    } else {                                                  \
        p->fid = esd_def_file(p->file);                       \
        p->rid = esd_def_region(p->rout, p->fid, p->lineno,   \
                           ELG_NO_LNO, "USR", ELG_FUNCTION);  \
    }                                                         \
    p->isseen = 1;                                            \
}

#if (defined (ELG_OMPI) || defined (ELG_OMP))
#define ELG_MY_THREAD   omp_get_thread_num() 
#define ELG_NUM_THREADS omp_get_max_threads() 
#else
#define ELG_MY_THREAD   0
#define ELG_NUM_THREADS 1 
#endif

/* CHECK_STACK required to support filtering */
#define CHECK_STACK 1

#ifdef CHECK_STACK
static elg_ui4 EpkMaxFrames = 0; /* callstack size limit */
                                 /* deeper frames automatically "filtered" */
static long*     cstkszv = NULL; /* current callstack size of each thread */
static long*     cstkmxv = NULL; /* maximum callstack size of each thread */
static elg_ui4** cstackv = NULL; /* callstacks of region ids for each thread */
#endif

static int   rou_init = 1;  /* initialization necessary ? */
static unsigned nfuncs=0, nblack=0;

/*
 * blacklist/filter management
 */

#define FUNC_NAME_MAX 1024

typedef struct BLN {
  char* func;
  struct BLN* next;
} BListNode;

static BListNode* bltab[128];  /* 128 -> no. of characters in ascii7 */

static void epk_filter_add(char* pattern)
{
  BListNode *add = malloc(sizeof(BListNode));
  add->func = strdup(pattern);
  add->next = bltab[pattern[0]];
  bltab[pattern[0]] = add;
  elg_cntl_msg("Filtering \"%s\"", add->func);
}

/* XXXX report filtered functions and hash-depth status */

void epk_filter_status()
{
  const char* filter_file = epk_get(EPK_FILTER);
  if ( (filter_file != NULL) && strlen(filter_file) ) {
#ifdef CHECK_STACK
    FILE* ffd = fopen(filter_file, "r");
    if (ffd == NULL) 
      elg_warning("Ignored unusable EPK_FILTER file \"%s\"", filter_file);
    else {
      elg_warning("EPK_FILTER \"%s\" filtered %d of %d functions",
                filter_file, nblack, nfuncs);
      fclose(ffd);
    }
#else
    elg_warning("EPK_FILTER file \"%s\" not supported by this installation", filter_file);
#endif
  }
}

static void epk_filter_gen() 
{
  FILE* ffd;
  const char* filter_file = epk_get(EPK_FILTER);
  char *pos;
  static char lbuf[FUNC_NAME_MAX];

  if ( (filter_file != NULL) && strlen(filter_file) ) {
    elg_cntl_msg("%s=%s", epk_key(EPK_FILTER), filter_file);
    if ( (ffd = fopen(filter_file, "r")) != NULL ) {
      unsigned line=0;
      /* read start of lines */
      while ( fgets(lbuf, FUNC_NAME_MAX, ffd) != NULL ) {
        line++;
        /* ignore (if necessary) rest of line */
        if ( (strlen(lbuf) == (FUNC_NAME_MAX-1)) ) {
          while ( fgetc(ffd) != '\n' ) /*skip*/ ;
          elg_warning("Ignoring overlong filter line %d!", line);
          continue;
        } else if ( (pos = strchr(lbuf, '\n')) != NULL ) {
          *pos = '\0';
        }

        /* ignore blank and comment lines */
        if ( (lbuf[0] == '\0') || (lbuf[0] == '#') ) continue;

        /* add to blacklist table */
        epk_filter_add(lbuf);
      }

      fclose(ffd);
    } else {
      elg_cntl_msg("Ignoring unusable EPK_FILTER file \"%s\"", filter_file);
    }
  }
}

/* linear search comparing against each blacklisted function in list */
/* (efficiency depends on order in list, however, candidate functions are
   only checked once (at time of definition), otherwise should consider
   replacement with binary search in sorted list) */

static int epk_filter_check(const char* func) 
{
  BListNode *curr = bltab[func[0]];
  while ( curr ) {
    if ( fnmatch(curr->func, func, 0) == 0 ) {
      elg_cntl_msg("- %s", func);
      return 0;
    }
    curr = curr->next;
  }
  /* continue search through wildcard prefixes */
  curr = bltab['*'];
  while ( curr ) {
    if ( fnmatch(curr->func, func, 0) == 0 ) {
      elg_cntl_msg("~ %s", func);
      return 0;
    }
    curr = curr->next;
  }
  elg_cntl_msg("+ %s", func);
  return 1;
}


/* With PGI 8 series compilers, a pragma is required to indicate that the
   compiler should save the registers, which was previously done explicitly.
   #pragma save_all_regs saves all registers, however, fewer registers can
   be saved with save_all_gp_regs, saved_used_gp_regs, etc.
 */

/*
 *-----------------------------------------------------------------------------
 * called during program termination
 *-----------------------------------------------------------------------------
 */

#pragma save_all_regs
void __rouexit() {

#ifdef CHECK_STACK
  unsigned t;
  long maxFrames = 0;
  while (cstkszv[ELG_MY_THREAD] > 0) {
      elg_ui4 rid=ELG_NO_ID;
      long frame = --cstkszv[ELG_MY_THREAD];
      if (frame < EpkMaxFrames) rid=cstackv[ELG_MY_THREAD][frame];
      elg_warning("Missed exit for frame %d (rid=%d)", frame, rid);
      /* esd_exit(rid); */
  }
  for (t=0; t<ELG_NUM_THREADS; t++) {
      if (cstackv[t]) free(cstackv[t]);
      if (cstkmxv[t] > maxFrames) maxFrames = cstkmxv[t];
  }
  if (cstackv) free(cstackv);
  if (cstkszv) free(cstkszv);
  if (cstkmxv) free(cstkmxv);
  elg_cntl_msg("Freed %d-depth callstacks (max=%d) for %d threads",
        	EpkMaxFrames, maxFrames, ELG_NUM_THREADS);
  if (maxFrames > EpkMaxFrames)
        elg_warning("Depth %d callstacks truncated by ESD_FRAMES=%d",
                maxFrames, EpkMaxFrames);
#endif

  /* set after measurement to be reported during finalization */
  epk_comp_status = &epk_filter_status;

  /* close measurement */
  esd_close();
}

/*
 *-----------------------------------------------------------------------------
 * called during program initialization
 *-----------------------------------------------------------------------------
 */

#pragma save_all_regs
void __rouinit() {

  if (rou_init) {
    unsigned t;

    esd_open(); /* open measurement */

#ifdef CHECK_STACK
    EpkMaxFrames = epk_str2int(epk_get(ESD_FRAMES));
    cstkszv = calloc(ELG_NUM_THREADS, sizeof(long));
    cstkmxv = calloc(ELG_NUM_THREADS, sizeof(long));
    cstackv = calloc(ELG_NUM_THREADS, sizeof(elg_ui4*));
    if (!cstkszv || !cstkmxv || !cstackv)
        elg_error_msg("Failed to allocate callstack vectors for %d threads",
                ELG_NUM_THREADS);
    for (t=0; t<ELG_NUM_THREADS; t++) {
        cstackv[t] = calloc(EpkMaxFrames, sizeof(elg_ui4));
        if (!cstackv[t])
            elg_error_msg("Failed to allocate callstack for thread %t", t);
    }
    elg_cntl_msg("Allocated %d-depth callstacks for %d threads",
	EpkMaxFrames, ELG_NUM_THREADS);
#endif

    epk_filter_gen(); /* initialise filter from file (if defined) */

    epk_comp_finalize = &__rouexit;
    rou_init = 0;
  }
}

/*
 *-----------------------------------------------------------------------------
 * called at the beginning of each instrumented routine
 *----------------------------------------------------------------------------- */

#pragma save_all_regs
void ___rouent2(struct s1 *p) {

  elg_ui4 rid;

  if (rou_init)
    {
      __rouinit();
    }

  if (!p->isseen)
    {
      char* modpos;

      /* fix OPARI output file names */
      if ( (modpos = strstr(p->file, ".mod.")) != NULL )
        {
          strcpy(modpos, modpos+4);
        }
      
#if (defined (ELG_OMPI) || defined (ELG_OMP))
      if (omp_in_parallel())
	{
#pragma omp critical (epk_comp_register_region)
	  {
	    if (!p->isseen) ESD_DEF_REGION(p);
	  }
	}
      else
          ESD_DEF_REGION(p);
#else
      ESD_DEF_REGION(p);
#endif
      elg_cntl_msg("Register [0x%lx]:\"%s\" with id=%d", (long)(p->rout), p->rout, p->rid);
    }
      
  rid = p->rid; /* local copy of region identifier */

#ifdef CHECK_STACK
  {
      /* update callstack tracking structures */
      long frame = cstkszv[ELG_MY_THREAD]++;
      if (frame > cstkmxv[ELG_MY_THREAD]) cstkmxv[ELG_MY_THREAD] = frame;
      if (frame < EpkMaxFrames) cstackv[ELG_MY_THREAD][frame] = rid;
      else rid = ELG_NO_ID; /* truncate frame when too deep */
  }

  /* -- enter event, if neither filtered nor truncated -- */
  if (rid != ELG_NO_ID)
      esd_enter(rid); /* -- region enter event -- */
#else
  esd_enter(rid); /* -- region enter event -- */
#endif
}

/*
 *-----------------------------------------------------------------------------
 * called at the end of each instrumented routine
 *-----------------------------------------------------------------------------
 */

#pragma save_all_regs
void ___rouret2(void) {
#ifdef CHECK_STACK
  elg_ui4 rid = ELG_NO_ID;

  /* update callstack tracking structures */
  long frame = --cstkszv[ELG_MY_THREAD];
  if (frame < 0) elg_error_msg("Exiting frame %d!", frame);
  if (frame < EpkMaxFrames) {
      rid = cstackv[ELG_MY_THREAD][frame];
  }

  /* -- exit event, if neither filtered nor truncated -- */
  if (rid != ELG_NO_ID)
      esd_exit(rid); /* -- region exit event -- */
#else
  esd_exit(ELG_NO_ID); /* -- region exit event -- */
#endif
}

#pragma save_used_gp_regs
void ___linent2(void *l) {
}
