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
 *  - Simple Vampirtrace Compiler Interface
 * 
 *-----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"
#include "VT.h"

#define VTinit_U     VTINIT
#define VTfini_U     VTFINI
#define VTtraceon_U  VTTRACEON
#define VTtraceoff_U VTTRACEOFF
#define VTsymdef_U   VTSYMDEF
#define VTbegin_U    VTBEGIN
#define VTend_U      VTEND

#define VTinit_L     vtinit
#define VTfini_L     vtfini
#define VTtraceon_L  vttraceon
#define VTtraceoff_L vttraceoff
#define VTsymdef_L   vtsymdef
#define VTbegin_L    vtbegin
#define VTend_L      vtend

#include "epk_fwrapper_def.h"

/*
 *-----------------------------------------------------------------------------
 * Simple hash table to map function handles to region identifier
 *-----------------------------------------------------------------------------
 */

typedef struct HN {
  long id;            /* hash code                            */
  elg_ui4 elgid;      /* associated EPILOG region identifier  */
  struct HN* next;
} HashNode;

#define HASH_MAX 1021

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

/*
 * Management functions
 * C/C++ version
 */

int VT_initialize(int* argc, char*** argv) {
  esd_open();
  return 0;
}

int VT_finalize() {
  esd_close();
  return 0;
}

int VT_traceon() {
  /* -- NOT YET IMPLEMENTED -- */
  return 0;
}

int VT_traceoff() {
  /* -- NOT YET IMPLEMENTED -- */
  return 0;
}


/*
 * This function defines function handles
 * C/C++ version
 */

int VT_symdef(int handle, char* name, char* group) {
  elg_ui4 rid;

  rid = esd_def_region(name, ELG_NO_ID, ELG_NO_LNO, ELG_NO_LNO,
                       group, ELG_FUNCTION);
  epk_hash_put(handle, rid);
  return 0;
}

/*
 * This functions mark the entry and exit of a function
 * C/C++ version
 */

int VT_begin(int handle) {
  elg_ui4 rid;

  if ( (rid = epk_hash_get(handle)) != ELG_NO_ID ) {
    esd_enter(rid);
  }
  return 0;
}

int VT_end(int handle) {
  elg_ui4 rid;

  if ( (rid = epk_hash_get(handle)) != ELG_NO_ID ) {
    esd_exit(rid);
  }
  return 0;
}

/*
 * Fortran version
 */

#define FSUB FSUB1
#include "epk_comp_fvt.c"

#undef FSUB
#define FSUB FSUB2
#include "epk_comp_fvt.c"

#undef FSUB
#define FSUB FSUB3
#include "epk_comp_fvt.c"

#undef FSUB
#define FSUB FSUB4
#include "epk_comp_fvt.c"
