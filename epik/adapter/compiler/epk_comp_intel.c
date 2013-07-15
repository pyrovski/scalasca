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
 *  - Intel compiler tcollect interface
 *
 *-----------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fnmatch.h>
#ifndef NO_INTEL_HACK
#include <signal.h>
#endif

#include "bfd.h"

#include "epk_comp.h"
#include "epk_conf.h"
#include "elg_error.h"
#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

#if defined (ELG_OMPI) || defined (ELG_OMP)
#include <omp.h>
#endif

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

#if (defined (ELG_OMPI) || defined (ELG_OMP))
#define ELG_MY_THREAD   omp_get_thread_num() 
#define ELG_NUM_THREADS omp_get_max_threads() 
#else
#define ELG_MY_THREAD   0
#define ELG_NUM_THREADS 1 
#endif

/* synthesized names for routines with problematic names */
#define FUNC_NAME_ANON "ANONYMOUS@0x"
#define FUNC_NAME_TRNC "TRUNCATED@0x"

#ifdef __ia64__
#define COMP_INST_KEY "_2$ITC$0"
#else
#define COMP_INST_KEY "_2.ITC.0"
#endif

static int intel_init = 1;       /* is initialization needed? */
static unsigned nfuncs=0, nblack=0;

#ifdef CHECK_STACK
static elg_ui4 EpkMaxFrames = 0; /* callstack size limit */
                                 /* deeper frames automatically "blacklisted" */
static long*     cstkszv = NULL; /* callstack size of each thread */
static elg_ui4** cstackv = NULL; /* callstacks of region ids for each thread */
#endif

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
/*elg_warning("%p:%ld#%ld %s %s;%d", add, h, id, n, fn, lno);*/
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

/* determine split of module name from "/path/to.source/file.ext:f90module.class::func" */
static inline unsigned epk_get_module_split(char* str)
{
    char* c;
    for (c=str+strlen(str); ; c--) {
        if (c < str)
            return 0; /* split not found */
        if (*c == ':') {
            if ((c != str) && (*(c-1) == ':'))
                c--; /* ignore "::" */
            else
                return (c-str); /* got split */
        }
    }
}

/* determine split of return type from "return <value type>* class::func<T>" */
static inline unsigned epk_get_retval_split(char* str)
{
    int nest=0;
    /* start reverse search from "::operator" if found, otherwise end of line */
    char* c = strstr(str, "::operator");
    if (!c) c = str + strlen(str);
    for (c; ; c--) {
        if (c < str) return 0; /* split not found */
        if (!nest && *c == ' ') return (c-str+1); /* got split */
        else if ((*c == '>') || (*c == ')')) nest++;
        else if ((*c == '<') || (*c == '(')) nest--;
    }
}

static elg_ui4 epk_register_region(char* str)
{
    long addr=(long)str; /* address of string containing function name */
    if (epk_hash_get(addr) == NULL) {
        elg_ui4 fid = ELG_NO_ID;
        unsigned split = (str[0] == '/') ? epk_get_module_split(str) : 0;
        if (split) {
            char* file = malloc(split+1);
            strncpy(file, str, split);
            file[split] = '\0';
            fid = esd_def_file(file); /* XXX new file defined for every region! */
            /*free(file);*/
            str+=(split+1); /* advance past module name */
        }
        unsigned len=strlen(str);
        if (len == 0) { /* synthesize anonymous routine name */
            str = malloc(32);
            sprintf(str, FUNC_NAME_ANON "%lX", addr);
            elg_warning(str);
        } else if (str[len-1] == ' ') { /* synthesize truncated routine name */
            char *tmpstr = malloc(32+len);
            sprintf(tmpstr, FUNC_NAME_TRNC "%lX \"%s\"", addr, str);
            str=tmpstr;
            elg_warning(str);
        } else if (strncmp(str,"operator ",9) != 0) {
            /* if return value included then skip it */
            str += epk_get_retval_split(str);
        }
        return esd_def_region(str, fid, ELG_NO_LNO, ELG_NO_LNO, "USR", ELG_FUNCTION);
    }
    return ELG_NO_ID; /* found in blacklist => filtered! */
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
  add->next = bltab[key];
  bltab[key] = add;
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
      long addr=0;
      unsigned line=0, len=0;
      /* read start of lines into buffer, leaving space to "demangle" */
      while ( fgets(lbuf, FUNC_NAME_MAX-3, ffd) != NULL ) {
        line++;

        /* ignore (if necessary) rest of line */
        if ( (len=strlen(lbuf)) == (FUNC_NAME_MAX-4) ) {
          while ( fgetc(ffd) != '\n' ) /* skip */ ;
          elg_warning("Ignoring overlong filter line %d!", line);
          continue;
        } else if ( (pos = strchr(lbuf, '\n')) != NULL ) {
          *pos = '\0';
        }

        /* ignore blank and comment lines */
        if ( (lbuf[0] == '\0') || (lbuf[0] == '#') ) continue;

        /* skip MPI functions */
        if ( strncmp(lbuf, "MPI_", 4) == 0 ) {
            elg_warning("Ignoring filter specified for %s!", lbuf);
            continue;
        }

        /* blacklist anonymous routine by address */
        if ( sscanf(lbuf, FUNC_NAME_ANON "%lX", &addr) == 1 ) {
            elg_cntl_msg("Filtering anonymous @0x%lX", addr);
            epk_hash_put(addr, lbuf, NULL, 0);
            continue;
        }

        /* replace first "." occurrence with "_mp_" module separator */
        if ( (pos = strchr(lbuf, '.')) != NULL ) {
            memmove(pos+4, pos+1, len-(pos-lbuf));
            strncpy(pos, "_mp_", 4);
        }

        /* add to filter table */
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


/*
 *-----------------------------------------------------------------------------
 * Symbol Table Processing
 *-----------------------------------------------------------------------------
 */

static int epk_open_exe()
{
   bfd * BfdImage = 0;
   int nr_all_syms;
   int i; 
   size_t size;
   asymbol **syms; 
   static char exe[64];
   int pid;

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
   
   epk_filter_gen(); /* initialise blacklist from file (if defined) */

   for (i=0; i<nr_all_syms; ++i) {
      if ( syms[i]->flags & BSF_OBJECT ) {
          char* eon; /* end of function name in symbol */
          char* name, *dem_name = 0;
          long addr;
          size_t len;
          const char* filename = NULL;
          unsigned int lno = ELG_NO_NUM;
          /*elg_warning("Got [0x%x] symbol %s", syms[i]->flags, syms[i]->name);*/
          if ((eon = strstr(syms[i]->name, COMP_INST_KEY)) == NULL) continue;
          len = eon-(syms[i]->name);          
	  name = strdup(syms[i]->name);
          name[len]='\0';
          addr = syms[i]->section->vma + syms[i]->value;
          /*elg_warning("Got ITC symbol 0x%lx:%s", addr, name);*/

          /* use demangled name if possible */
#ifdef GNU_DEMANGLE
          if (epk_demangle_style >= 0)
              dem_name = cplus_demangle(name, epk_demangle_style);
          if (!dem_name) dem_name = name;
#else
          dem_name = name;
#endif

          /* hash blacklisted functions' names with symbol address */

          nfuncs++;
          if (!epk_filter_check(dem_name)) {
              epk_hash_put(addr, dem_name, filename, lno);
              nblack++;
          }
      }
   }
   free(syms);
   bfd_close(BfdImage);
   elg_cntl_msg("Found %d functions (%d filtered)", nfuncs, nblack);
   return 1;
}

static void epk_intel_wrapup() {
  int t;
#ifndef NO_INTEL_HACK
  /* ignore "I/O possible" signal */
  struct sigaction act;
  act.sa_handler = SIG_IGN;
  act.sa_flags = 0;
  sigaction(SIGIO, &act, NULL);
#endif
  esd_close();

#ifdef CHECK_STACK
  while (cstkszv[ELG_MY_THREAD] > 0) {
      elg_ui4 rid=ELG_NO_ID;
      long frame = --cstkszv[ELG_MY_THREAD];
      if (frame < EpkMaxFrames) rid=cstackv[ELG_MY_THREAD][frame];
      elg_warning("Missed exit for frame %d (rid=%d)", frame, rid);
  }
  for (t=0; t<ELG_NUM_THREADS; t++) if (cstackv[t]) free(cstackv[t]);
  if (cstkszv) free(cstkszv);
  if (cstackv) free(cstackv);
  elg_cntl_msg("Freed %d-depth callstacks for %d threads",
        	EpkMaxFrames, ELG_NUM_THREADS);
#endif
}

/*
 * This function is called at the entry of each function
 */

void __VT_IntelEntry(char* str, elg_ui4* id, elg_ui4* id2) {
  long frame;
  /* -- if not yet initialized, initialize EPIK -- */
  if ( intel_init ) {
    int t;
    if (intel_init != 1) {
        elg_cntl_msg("Ignoring function \"%s\" entered during initialization", str);
        *id2 = ELG_NO_ID;
        return;
    }
    intel_init = -1;
#ifdef GNU_DEMANGLE
    if (getenv("EPK_DEMANGLE_STYLE")) {
        epk_demangle_style = epk_str2int(getenv("EPK_DEMANGLE_STYLE"));
        if (epk_demangle_style < 0)
            elg_cntl_msg("Demangling disabled");
        else
            elg_cntl_msg("Set demangle style to %d", epk_demangle_style);
    } else if (str[0] == '/') {
        /* set style based on format of first instrumented function entered */
        epk_demangle_style = DMGL_NO_OPTS;
        elg_cntl_msg("Set demangle style to plain function name");
    }
#endif
    epk_open_exe();
    epk_comp_status = &epk_filter_status;
    epk_comp_finalize = &epk_intel_wrapup;
    esd_open();

#ifdef CHECK_STACK
    EpkMaxFrames = epk_str2int(epk_get(ESD_FRAMES));
    cstkszv = calloc(ELG_NUM_THREADS, sizeof(long));
    cstackv = calloc(ELG_NUM_THREADS, sizeof(elg_ui4*));
    if (!cstkszv || !cstackv)
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

#ifndef NO_INTEL_HACK
    putenv("KMP_IGNORE_MPPEND=1"); /* skip __kmpc_end before exit handlers */
#endif
    intel_init = 0; /* initialization complete and ready for measurement */
  }

  /*elg_warning("Entering [0x%lx]:\"%s\" with id=%d", (long)str, str, *id);*/

  /* -- get region identifier -- */
  if ( *id == 0 ) {
    /* -- region entered the first time, register region -- */
#   if defined (ELG_OMPI) || defined (ELG_OMP)
    if (omp_in_parallel()) {
#     pragma omp critical (epk_comp_register_region)
      {
	if (*id == 0) *id = epk_register_region(str);
      }
    } else {
      *id = epk_register_region(str);
    }
#   else
    *id = epk_register_region(str);
#   endif
    elg_cntl_msg("Register [0x%lx]:\"%s\" with id=%d", (long)str, str, *id);
  }

#ifdef CHECK_STACK
  /* update callstack tracking structures */
  frame = cstkszv[ELG_MY_THREAD]++;
  if (frame < EpkMaxFrames) cstackv[ELG_MY_THREAD][frame]=*id;
#endif

  /* -- enter event, if not blacklisted -- */
  if (*id != ELG_NO_ID)
      esd_enter(*id); /* -- region enter event -- */

  /* -- set identifier for corresponding exit call -- */
  *id2 = *id;
}

void VT_IntelEntry(char* str, elg_ui4* id, elg_ui4* id2) {
  __VT_IntelEntry(str, id, id2);
}


/*
 * This function is called at the exit of each function
 */

void __VT_IntelExit(elg_ui4* id2) {
  elg_ui4 rid = *id2;

  /*elg_warning("-Exiting id=%d", *id2);*/

#ifdef CHECK_STACK
  /* update callstack tracking structures */
  long frame = --cstkszv[ELG_MY_THREAD];
  if (frame < 0) elg_error_msg("Exiting frame %d!", frame);
  if (frame < EpkMaxFrames) {
      elg_ui4 sid=cstackv[ELG_MY_THREAD][frame];
      if ((sid != ELG_NO_ID) && (rid != sid))
          elg_warning("Id mismatch on exit: %d <> %d", rid, sid);
  }
#endif

  /* -- exit event, if not blacklisted -- */
  if (rid != ELG_NO_ID)
      esd_exit(rid); /* -- region exit event -- */
}

void VT_IntelExit(elg_ui4* id2) {
  __VT_IntelExit(id2);
}

/*
 * This function is called when an exception is caught
 */

void __VT_IntelCatch(elg_ui4* id2) {
  elg_ui4 rid = *id2;

  elg_cntl_msg("Catch in region id=%d", *id2);
  if (rid != ELG_NO_ID)
      esd_exit_catch(rid); /* -- catch exit event -- */
}

void VT_IntelCatch(elg_ui4* id2) {
  __VT_IntelCatch(id2);
}

void __VT_IntelCheck(elg_ui4* id2) {
  elg_ui4 rid = *id2;

  elg_warning("Check in region id=%d: attempting callstack recovery!", *id2);
  if (rid != ELG_NO_ID)
      esd_exit_catch(rid); /* -- catch exit event -- */
}

void VT_IntelCheck(elg_ui4* id2) {
  __VT_IntelCheck(id2);
}
