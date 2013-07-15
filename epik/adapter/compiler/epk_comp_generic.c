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
 *  - Generic Compiler & Application Program (User) Interface
 * 
 *-----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#if (defined (ELG_OMPI) || defined (ELG_OMP))
  #include <omp.h>
#endif
#include "esd_def.h"
#include "esd_event.h"
#include "esd_run.h"
#include "elg_error.h"

#define EPIK
#include "epik_user.h"
#undef EPIK_USER_START
#undef EPIK_USER_END
#undef EPIK_PAUSE_START
#undef EPIK_PAUSE_END
#undef EPIK_FLUSH_TRACE

#define EPIK_User_start_U  EPIK_USER_START
#define EPIK_User_end_U    EPIK_USER_END
#define EPIK_User_start_L  epik_user_start
#define EPIK_User_end_L    epik_user_end

#define EPIK_Pause_start_U EPIK_PAUSE_START
#define EPIK_Pause_end_U   EPIK_PAUSE_END
#define EPIK_Pause_start_L epik_pause_start
#define EPIK_Pause_end_L   epik_pause_end

#define EPIK_Flush_trace_U EPIK_FLUSH_TRACE
#define EPIK_Flush_trace_L epik_flush_trace

#include "epk_fwrapper_def.h"

/* This function marks the start of the EPIK measurement libraries */
/* It must be the first function in this module, so don't move it! */
void EPIK_Gen_start() { return; }

/*
 *-----------------------------------------------------------------------------
 * Simple hash table to map function names to region identifier
 *-----------------------------------------------------------------------------
 */

typedef struct HN {
  long id;            /* hash code (address of function name) */
  elg_ui4 elgid;      /* associated EPIK region identifier  */
  struct HN* next;
} HashNode;

#define HASH_MAX 1021

static int gen_init = 1;       /* is initialization needed? */

static HashNode* htab[HASH_MAX];

/*
 * Stores EPIK region identifier `e' under hash code `h'
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
 * Returns EPIK region identifier if already stored, otherwise ELG_NO_ID
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
 * Register new region with EPIK system
 */

static elg_ui4 epk_register_region(char *name, long addr, char* file, int lno) {
  elg_ui4 rid;
  elg_ui4 fid;

  /* -- register file and region with EPIK and store region identifier -- */
  fid = esd_def_file(file);
  rid = esd_def_region(name, fid, lno, ELG_NO_LNO, "USR", ELG_FUNCTION);
  epk_hash_put(addr == 0 ? (long) name : addr, rid);
  return rid;
}

/*
 * This function is called to explicitly flush trace buffer to disk
 */

void EPIK_Flush_trace (const char *file, const int lno)
{
  elg_cntl_msg("EPIK_Flush_trace(%s:%d)", file, lno);
  esd_flush();
}

/*
 * This function is called at the entry of each user-defined pause
 */

void EPIK_Pause_start (const char *file, const int lno)
{
  /* -- if not yet initialized, initialize -- */
  if ( gen_init ) {
    gen_init = 0;
    esd_open();
  }

  elg_cntl_msg("EPIK_Pause_start(%s:%d)", file, lno);
  esd_enter_pause();
}

/*
 * This function is called at the exit of each user-defined pause
 */

void EPIK_Pause_end (const char *file, const int lno)
{
  /* -- if not yet initialized, abort -- */
  if ( gen_init ) {
    elg_error_msg("EPIK not yet initialized to end PAUSE!");
  }

  elg_cntl_msg("EPIK_Pause_end(%s:%d)", file, lno);
  esd_exit_pause();
}

/*
 * This function is called at the entry of each user-defined region
 * C/C++ version
 */

void EPIK_User_start (const char* name, const char *file, const int lno)
{
  elg_ui4 rid;

  /* -- if not yet initialized, initialize -- */
  if ( gen_init ) {
    gen_init = 0;
    esd_open();
  }

  /* -- get region identifier -- */
  if ( (rid = epk_hash_get((long) name)) == ELG_NO_ID ) {
    /* -- region entered the first time, register region -- */
#   if defined (ELG_OMPI) || defined (ELG_OMP)
    if (omp_in_parallel()) {
#     pragma omp critical (epk_comp_register_region)
      {
        if ( (rid = epk_hash_get((long) name)) == ELG_NO_ID ) {
          rid = epk_register_region((char*)name, 0, (char*)file, lno);
        }
      }
    } else {
      rid = epk_register_region((char*)name, 0, (char*)file, lno);
    }
#   else
    rid = epk_register_region((char*)name, 0, (char*)file, lno);
#   endif
  }

#if DEBUG
  elg_cntl_msg("EPIK_User_start(%s): 0x%X [%u]", name, name, rid);
#endif

  /* -- enter region event -- */
  esd_enter(rid);
}

/*
 * This function is called at the exit of each user-defined region
 * C/C++ version
 */

void EPIK_User_end (const char* name, const char *file, const int lno)
{
  elg_ui4 rid;

  /* -- if not yet initialized, abort -- */
  if ( gen_init ) {
    elg_error_msg("EPIK not yet initialized!");
  }

  /* -- get region identifier -- */
  rid = epk_hash_get((long) name);

#if DEBUG
  elg_cntl_msg("EPIK_User_end  (%s): 0x%X [%u]",
        name, name, rid);
#endif

  /* -- exit region event -- */
  esd_exit(rid);
}

/*
 * These functions are called at the entry/exit of each user-defined region
 * Fortran version
 */

#define FSUB FSUB1
#include "epk_comp_fgeneric.c"

#undef FSUB
#define FSUB FSUB2
#include "epk_comp_fgeneric.c"

#undef FSUB
#define FSUB FSUB3
#include "epk_comp_fgeneric.c"

#undef FSUB
#define FSUB FSUB4
#include "epk_comp_fgeneric.c"
