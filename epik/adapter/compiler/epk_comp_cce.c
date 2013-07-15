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
 *  - Cray CCE compiler interface
 *
 *-----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>

#if defined (ELG_OMPI) || defined (ELG_OMP)
#include <omp.h>
#endif

#include "bfd.h"

#ifdef GNU_DEMANGLE
/*#include "demangle.h"*/
extern char * cplus_demangle (const char *mangled, int options);
/* cplus_demangle options */
#define DMGL_NO_OPTS	 0	
#define DMGL_PARAMS	 (1 << 0)	/* include function arguments */
#define DMGL_ANSI	 (1 << 1)	/* include const, volatile, etc. */
#define DMGL_VERBOSE	 (1 << 3)	/* include implementation details */
#define DMGL_TYPES	 (1 << 4)	/* include type encodings */
static int epk_demangle_style = DMGL_PARAMS | DMGL_ANSI | DMGL_VERBOSE | DMGL_TYPES;
#endif

#include "epk_comp.h"
#include "epk_conf.h"
#include "elg_error.h"
#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

static int cce_init = 1;       /* is initialization needed? */
static unsigned nfuncs=0, nblack=0;

extern void EPIK_Gen_start();
/* This function marks the start of the EPIK measurement libraries */
/* It must be the first function in this module, so don't move it! */
static void EPIK_Comp_start() { return; }

/*
 *-----------------------------------------------------------------------------
 * Simple hash table to map function addresses to region names/identifier
 *-----------------------------------------------------------------------------
 */

typedef struct HN {
  long id;            /* hash code (address of function)      */
  const char* name;   /* associated function name             */
  const char* fname;  /*            file name                 */
  int lno;            /*            line number               */
  elg_ui4 elgid;      /* associated EPIK region identifier    */
  struct HN* next;
} HashNode;

#define HASH_MAX 1021

static HashNode* htab[HASH_MAX];

/*
 * Stores function name `n' under hash code `h'
 */

static void epk_hash_put(long h, const char* n, const char* fn, int lno) {
  long id = h % HASH_MAX;
  HashNode *add = malloc(sizeof(HashNode));
  add->id = h;
  add->name  = n;
  add->fname = fn ? strdup(fn) : fn;
  add->lno   = fn ? lno : ELG_NO_LNO;
  add->elgid = ELG_NO_ID;
  add->next = htab[id];
  htab[id] = add;
  elg_cntl_msg("%p: %s %s;%d", h, n, fn, lno);
}

/*
 * Lookup hash code `h'
 * Returns hash table entry if already stored, otherwise NULL
 */

static HashNode* epk_hash_get(long h) {
  long id = h % HASH_MAX;
  HashNode *curr = htab[id];
  while ( curr ) {
    if ( curr->id == h ) {
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}

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
  char key = pattern[0];
  if ((key == '?') || (key == '[')) key='*'; /* combine all wildcards */
  add->func = strdup(pattern);
  add->next = bltab[(unsigned)key];
  bltab[(unsigned)key] = add;
  elg_cntl_msg("Filtering \"%s\"", add->func);
}

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

        /* skip MPI functions */
        if ( strncmp(lbuf, "MPI_", 4) == 0) {
            elg_warning("Ignoring filter specified for %s!", lbuf);
            continue;
        }

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
  BListNode *curr = bltab[(unsigned)func[0]];
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
 *-----------------------------------------------------------------------------
 * Symbol Table Processing
 *-----------------------------------------------------------------------------
 */

static int epk_open_exe()
{
   long EPIK_start = (long) &EPIK_Comp_start;
   bfd * BfdImage = 0;
   int nr_all_syms;
   int i; 
   size_t size;
   asymbol **syms; 
   static char exe[64];
   int pid;

#ifdef GNU_DEMANGLE
   if (getenv("EPK_DEMANGLE_STYLE")) {
       epk_demangle_style = epk_str2int(getenv("EPK_DEMANGLE_STYLE"));
       if (epk_demangle_style < 0)
           elg_cntl_msg("Demangling disabled");
       else
           elg_cntl_msg("Set demangle style to %d", epk_demangle_style);
   }
#endif

   /* Initialize BFD */
   bfd_init();
   pid = getpid();

   sprintf(exe, "/proc/%d/exe", pid);
   BfdImage = bfd_openr(exe, 0 );
   if ( ! BfdImage ) {
     sprintf(exe, "/proc/%d/object/a.out", pid);
     BfdImage = bfd_openr(exe, 0 );
      
     if ( ! BfdImage ) {
       elg_error_msg("Cannot get executable image.");
     }
   }

   /* Check image format */
   if ( ! bfd_check_format(BfdImage, bfd_object) ) { 
      elg_error_msg("ERROR: BFD format failure.");
   }
   
   /* Get & check size of symbol table */
   size = bfd_get_symtab_upper_bound(BfdImage);
   if ( size < 1 ) {
      return 0;
   }
   
   /* Read canonicalized symbols */
   syms = malloc(size);
   nr_all_syms = bfd_canonicalize_symtab(BfdImage, syms);
   elg_cntl_msg("Got %d symbols in image.", nr_all_syms);
   if ( nr_all_syms < 1 ) {
      return 0;
   }
   
   epk_filter_add("EPIK_*"); /* required for EPIK_Tracer */

   epk_filter_gen(); /* initialise filter from file (if defined) */

   if (EPIK_start > (long) &EPIK_Gen_start)
       EPIK_start = (long) &EPIK_Gen_start;
   elg_cntl_msg("EPIK_start=%p", EPIK_start);

   for (i=0; i<nr_all_syms; ++i) {
      char* dem_name = 0;
      long addr;
      const char* filename;
      const char* funcname;
      unsigned int lno;
      
      if ( syms[i]->flags & BSF_FUNCTION ) {

	/* ignore system functions */
	if ( strncmp(syms[i]->name, "bfd_", 4) == 0 ||
	     strstr(syms[i]->name, "@@") != NULL ) continue;

	/* get filename and linenumber from debug info */
	/* needs -g */
	filename = NULL;
	lno = -1;
        bfd_find_nearest_line(BfdImage, bfd_get_section(syms[i]), syms,
                              syms[i]->value, &filename, &funcname, &lno);

	/* calculate function address */
	addr = syms[i]->section->vma + syms[i]->value;

        /* XXXX probably remove this (for smg2000) */
        /* ignore functions outside of program itself */
        if (addr >= EPIK_start) continue;

#ifdef GNU_DEMANGLE
	/* use demangled name if possible */
        if (epk_demangle_style >= 0)
            dem_name = cplus_demangle(syms[i]->name, epk_demangle_style);
#endif

        /* filtered functions are not included in hashtable */

        nfuncs++;
        if( dem_name ) {
          if (epk_filter_check(dem_name))
            epk_hash_put(addr, dem_name, filename, lno);
          else nblack++;
        } else if (epk_filter_check(syms[i]->name)) {
	  char *n = strdup(syms[i]->name);
          epk_hash_put(addr, n, filename, lno);
        } else nblack++;
      }
   }
   free(syms);
   bfd_close(BfdImage);
   elg_cntl_msg("Found %d functions (%d filtered)", nfuncs, nblack);
   return 1;
}

/*
 * Register new region with EPIK
 */

static elg_ui4 epk_register_region(const char *func, const char *filename, int lno) {
  elg_ui4 rid, fid = ELG_NO_ID;

  /* define filename if known and stored in hash */
  if ( filename ) { /* XXX new file defined for every region! */
    fid = esd_def_file(filename);
  }
  rid = esd_def_region(func, fid, lno, ELG_NO_LNO, "USR", ELG_FUNCTION);

  return rid;
}


/*
 * This function is called at program end
 */

void cyg_profile_finalize () {
  esd_close();
}

/*
 * This function is called at the entry of each function
 * The call is generated by the CCE compilers
 */

void __pat_tp_func_entry(void* func, void* callsite) {
  HashNode *hn;

  void * funcptr = func;

#ifdef __ia64__
  funcptr = *( void ** )func;
#endif

  /* -- if not yet initialized, initialize EPIK -- */
  if ( cce_init ) {
    if (cce_init != 1) {
        elg_cntl_msg("Ignoring function @%p entered during initialization", func);
        return;
    }
    cce_init = -1;
    epk_open_exe();
    esd_open();
    epk_comp_status = &epk_filter_status;
    epk_comp_finalize = &cyg_profile_finalize;
    cce_init = 0;
  }

  if ( (hn = epk_hash_get((long)funcptr)) ) {
    if ( hn->elgid == ELG_NO_ID ) {
      /* -- region entered the first time, register region -- */
#     if defined (ELG_OMPI) || defined (ELG_OMP)
      if (omp_in_parallel()) {
#       pragma omp critical (epk_comp_register_region)
        {
          if ( hn->elgid == ELG_NO_ID ) {
            hn->elgid = epk_register_region(hn->name, hn->fname, hn->lno);
          }
        }
      } else {
        hn->elgid = epk_register_region(hn->name, hn->fname, hn->lno);
      }
#     else
      hn->elgid = epk_register_region(hn->name, hn->fname, hn->lno);
#     endif
    }

    esd_enter(hn->elgid);
  }
}

/*
 * This function is called at the exit of each function
 * The call is generated by the CCE compilers
 */

void __pat_tp_func_return(void* func, void* callsite) {
  HashNode *hn;

  void * funcptr = func;

#ifdef __ia64__
  funcptr = *( void ** )func;
#endif

  if ( (hn = epk_hash_get((long)funcptr)) ) {
    esd_exit(hn->elgid);
  }
}
