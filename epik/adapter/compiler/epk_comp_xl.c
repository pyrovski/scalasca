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
 *  - IBM xl (xlf/xlc/xlC) compiler interface
 *    only works for xlf 9.1.1 or later
 *    only works for xl[cC] 7.1.1 or later
 * 
 *-----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>

#ifdef GNU_DEMANGLE
/*#include "demangle.h"*/
extern char * cplus_demangle (const char *mangled, int options);
/* cplus_demangle options */
#define DMGL_NO_OPTS     0      
#define DMGL_PARAMS      (1 << 0)       /* include function arguments */
#define DMGL_ANSI        (1 << 1)       /* include const, volatile, etc. */
#define DMGL_VERBOSE     (1 << 3)       /* include implementation details */
#define DMGL_TYPES       (1 << 4)       /* include type encodings */
static int epk_demangle_style = DMGL_PARAMS | DMGL_ANSI | DMGL_VERBOSE | DMGL_TYPES;
#endif

#include "epk_comp.h"
#include "epk_conf.h"
#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

/*
 *-----------------------------------------------------------------------------
 * Simple hash table to map function data to region identifier
 *-----------------------------------------------------------------------------
 */

typedef struct HN {
  long id;            /* hash code (address of function name) */
  elg_ui4 elgid;      /* associated EPIK region identifier  */
  char* func;
  char* file;
  int lno;
  struct HN* next;
} HashNode;

#define HASH_MAX 1021

static int xl_init = 1;       /* is initialization needed? */

static unsigned nfuncs=0, nblack=0;

static HashNode* htab[HASH_MAX];

#if __IBMC__ > 1100
static HashNode ignored_func = { 0, ELG_NO_ID, NULL, NULL, 0, NULL };
#endif

/* XXXX report filtered functions and hash-depth status */

void epk_filter_status()
{
  const char* filter_file = epk_get(EPK_FILTER);
  if ( (filter_file != NULL) && strlen(filter_file) ) {
    FILE* ffd = fopen(filter_file, "r");
    if (ffd == NULL) 
      elg_warning("Ignored unusable EPK_FILTER file \"%s\"", filter_file);
    else {
      elg_warning("EPK_FILTER \"%s\" filtered %d of %d functions",
                filter_file, nblack, nfuncs);
      fclose(ffd);
    }
  }
}

static void epk_xl_wrapup() {
  /* set after measurement to be reported during finalization */
  epk_comp_status = &epk_filter_status;
}

/*
 * Stores EPIK region identifier `e' under hash code `h'
 */

static HashNode *epk_hash_put(long h, elg_ui4 e) {
  long id = h % HASH_MAX;
  HashNode *add = malloc(sizeof(HashNode));
  if (!add) elg_error_msg("malloc failed in epk_hash_put(%ld,%llu)", h, e);
  add->id = h;
  add->elgid = e;
  add->next = htab[id];
  htab[id] = add;
  return add;
}

/*
 * Lookup hash code `h'
 * Returns pointer to function data if already stored, otherwise 0
 */

static HashNode *epk_hash_get(long h) {
  long id = h % HASH_MAX;
  HashNode *curr = htab[id];
  while ( curr ) {
    if ( curr->id == h ) {
      return curr;
    }
    curr = curr->next;
  }
  return 0;
}


/*
 * filter management
 */

#define FUNC_NAME_MAX 1024

typedef struct BLN {
  char* func;
  struct BLN* next;
} BListNode;

static BListNode* bltab[128];  /* 128 -> no. of characters in ascii7 */

static void epk_filter_gen() {
  FILE* ffd;
  const char* filter_file = epk_get(EPK_FILTER);
  char *pos;
  static char lbuf[FUNC_NAME_MAX];
  BListNode *add;

#ifdef GNU_DEMANGLE
  if (getenv("EPK_DEMANGLE_STYLE")) {
      epk_demangle_style = epk_str2int(getenv("EPK_DEMANGLE_STYLE"));
      if (epk_demangle_style < 0)
          elg_cntl_msg("Demangling disabled");
      else
          elg_cntl_msg("Set demangle style to %d", epk_demangle_style);
  }
#endif

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
#if 0  
        /* ignore optional file name (for now) */
        if ( (pos = strchr(lbuf, '[')) != NULL ) {
	  *(--pos) = '\0';
        }
#endif
	/* add to filter table */
        add = malloc(sizeof(BListNode));
        if (!add) elg_error_msg("malloc failed in epk_filter_gen(%s)", lbuf);
        add->func = strdup(lbuf);
        add->next = bltab[lbuf[0]];
        bltab[lbuf[0]] = add;
        elg_cntl_msg("Filtered \"%s\"", add->func);
      }

      fclose(ffd);
    } else {
      elg_cntl_msg("Ignoring unusable EPK_FILTER file \"%s\"", filter_file);
    }
  }
}

static int epk_filter_check(char* func) {
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

/*
 * Register new region with EPIK
 */

static HashNode *epk_register_region(char *func, char *file, int lno) {
  elg_ui4 rid = ELG_NO_ID;
  HashNode* nhn;
  char *dem_name = NULL;

#ifdef GNU_DEMANGLE
  if (epk_demangle_style >= 0)
      dem_name = cplus_demangle(func, epk_demangle_style);
#endif
  if (!dem_name) dem_name=func;

  if (epk_filter_check(dem_name)) {
      /* -- register file and region with EPIK and store region identifier -- */
      elg_ui4 fid;
      fid = esd_def_file(file); /* XXX new file defined for every region! */
      rid = esd_def_region(dem_name, fid, lno, ELG_NO_LNO, "USR", ELG_FUNCTION);
  } else nblack++;
  nhn = epk_hash_put((long) func, rid);
  nhn->func = func;
  nhn->file = file;
  nhn->lno  = lno;
  nfuncs++;
  return nhn;
}

/*
 * This function is called at the entry of each function
 * The call is generated by the IBM xl compilers
 *
 * XL C/C++ 11.x / XLF 13.x or later pass the address of a static pointer
 * variable, initialized with NULL, as additional parameter.
 */
#if __IBMC__ > 1100
void __func_trace_enter(char* name, char *fname, int lno, HashNode** ihn)
#else
void __func_trace_enter(char* name, char *fname, int lno)
#endif
{
  HashNode *hn;

#if __IBMC__ > 1100
  if ( *ihn == NULL ) {
    /* -- region entered the first time, register region -- */
#endif

    /* -- ignore IBM OMP runtime functions -- */
    if ( ( strchr(name, '@') != NULL ) || (strchr(name, '$') != NULL) ) {
#     if __IBMC__ > 1100
      *ihn = &ignored_func;
#     endif
      return;
    }

    /* -- if not yet initialized, initialize EPIK -- */
    if ( xl_init ) {
      xl_init = 0;
      epk_filter_gen();
      esd_open();
      epk_comp_finalize = &epk_xl_wrapup;
    }

    /* -- get region identifier -- */
    if ( (hn = epk_hash_get((long) name)) == 0 ) {
      /* -- region entered the first time, register region -- */
#     if defined (ELG_OMPI) || defined (ELG_OMP)
      if (omp_in_parallel()) {
#       pragma omp critical (epk_comp_register_region)
        {
          if ( (hn = epk_hash_get((long) name)) == 0 ) {
            hn = epk_register_region(name, fname, lno);
#           if __IBMC__ > 1100
            *ihn = hn;
#           endif
          }
        }
      } else {
        hn = epk_register_region(name, fname, lno);
#       if __IBMC__ > 1100
        *ihn = hn;
#       endif
      }
#     else
      hn = epk_register_region(name, fname, lno);
#     if __IBMC__ > 1100
      *ihn = hn;
#     endif
#     endif
    }

#if __IBMC__ > 1100
  } else {
    /* -- region already registered -- */
    hn = *ihn;
  }
#endif

  /* -- enter event -- */
  if ( hn->elgid != ELG_NO_ID ) {
    esd_enter(hn->elgid);
  }
}

/*
 * This function is called at the exit of each function
 * The call is generated by the IBM xl compilers
 *
 * XL C/C++ 11.x / XLF 13.x or later pass the address of a static pointer
 * variable, initialized with NULL, as additional parameter.
 */
#if __IBMC__ > 1100
void __func_trace_exit(char* name, char *fname, int lno, HashNode** ihn)
#else
void __func_trace_exit(char* name, char *fname, int lno)
#endif
{
  HashNode *hn;

#if __IBMC__ > 1100
  if ( NULL == *ihn )
    elg_error_msg("Unexpected function exit encountered "
                  "(%s, file %s, line %d)!", name, fname, lno);

  hn = *ihn;
#else
  /* -- ignore IBM OMP runtime functions -- */
  if ( ( strchr(name, '@') != NULL ) || (strchr(name, '$') != NULL) ) return;

  hn = epk_hash_get((long) name);
#endif

  /* -- exit event -- */
  if ( hn->elgid != ELG_NO_ID ) {
    esd_exit(hn->elgid);
  }
}

#if __IBMC__ > 1100
void __func_trace_catch(char* name, char *fname, int lno, HashNode** ihn)
#else
void __func_trace_catch(char* name, char *fname, int lno)
#endif
{
  HashNode *hn;

#if __IBMC__ > 1100
  if ( NULL == *ihn )
    elg_error_msg("Unexpected function exit catch encountered "
                  "(%s, file %s, line %d)!", name, fname, lno);

  hn = *ihn;
#else
  /* -- ignore IBM OMP runtime functions -- */
  if ( ( strchr(name, '@') != NULL ) || (strchr(name, '$') != NULL) ) return;

  hn = epk_hash_get((long) name);
#endif

  elg_cntl_msg("Catch in region id=%d", hn->elgid);
  /* -- catch exit event -- */
  if ( hn->elgid != ELG_NO_ID ) {
    esd_exit_catch(hn->elgid);
  }

}

