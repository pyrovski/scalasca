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
 *  - Hitachi SR8000 compiler interface
 * 
 *-----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

#include "epk_conf.h"
#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"

/*
 *-----------------------------------------------------------------------------
 * Simple hash table to map function names to region identifier
 *-----------------------------------------------------------------------------
 */

typedef struct HN {
  long id;            /* hash code (address of function name) */
  elg_ui4 elgid;      /* associated EPILOG region identifier  */
  struct HN* next;
} HashNode;

#define HASH_MAX 1021

static int hit_init = 1;       /* is initialization needed? */
static char *hit_main_id = 0;  /* name of main function */

static HashNode* htab[HASH_MAX];

/*
 * Stores EPILOG region identifier `e' under hash code `h'
 */

static void epk_hash_put(long h, elg_ui4 e) {
  long id = h % HASH_MAX;
  HashNode *add = malloc(sizeof(HashNode));
  add->id = h;
  add->elgid = e;
  add->next = htab[id];
  htab[id] = add;
}

/*
 * Lookup hash code `h'
 * Returns EPILOG region identifier if already stored, otherwise ELG_NO_ID
 */

static elg_ui4 epk_hash_get(long h) {
  long id = h % HASH_MAX;
  HashNode *curr = htab[id];
  while ( curr ) {
    if ( curr->id == h ) {
      return curr->elgid;
    }
    curr = curr->next;
  }
  return ELG_NO_ID;
}

void epk_filter_status()
{
  const char* filter_file = epk_get(EPK_FILTER);
  if ( (filter_file != NULL) && strlen(filter_file) ) {
    elg_warning("EPK_FILTER file \"%s\" not supported for Hitachi compilers", filter_file);
  }
}

/*
 * Register new region with EPILOG system
 * `str' is passed in from Hitachi compiler and has the form "file+func"
 */

static elg_ui4 epk_register_region(char *str, int lno) {
  elg_ui4 rid;
  static char file[1024];
  char *func;
  elg_ui4 fid;

  /* -- separate file and func part -- */
  strcpy(file, str);
  func = strchr(file, '+');
  *func = '\0';
  ++func;

  /* -- register file and region with EPILOG and store region identifier -- */
  fid = esd_def_file(file);
  rid = esd_def_region(func, fid, lno, ELG_NO_LNO, "USR", ELG_FUNCTION);
  epk_hash_put((long) str, rid);
  return rid;
}

/*
 * This function is called at the entry of each function
 * The call is generated by the Hitachi compilers
 */

void _h_perform_func_start(char *str, int lno) {
  elg_ui4 rid;

  /* -- if not yet initialized, initialize EPILOG -- */
  if ( hit_init ) {
    hit_init = 0;
    hit_main_id = str;
    epk_comp_status = &epk_filter_status;
    esd_open();
  }

  /* -- get region identifier -- */
  if ( (rid = epk_hash_get((long) str)) == ELG_NO_ID ) {
    /* -- region entered the first time, register region -- */
#   if defined (ELG_OMPI) || defined (ELG_OMP)
    if (omp_in_parallel()) {
#     pragma omp critical (epk_comp_register_region)
      {
        if ( (rid = epk_hash_get((long) str)) == ELG_NO_ID ) {
          rid = epk_register_region(str, lno);
        }
      }
    } else {
      rid = register_region(str, lno);
    }
#   else
    rid = register_region(str, lno);
#   endif
  }

  /* -- enter event -- */
  esd_enter(rid);
}

/*
 * This function is called at the exit of each function
 * The call is generated by the Hitachi compilers
 */

void _h_perform_func_end(char *str) {
  elg_ui4 rid = epk_hash_get((long) str);

  /* -- exit event -- */
  esd_exit(rid);

#if 0
  /* -- if main function, finalize EPILOG -- */
  if ( str == hit_main_id ) esd_close();
#endif
}
