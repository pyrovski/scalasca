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

#define CONVERT_STRINGS

/*
 * This function is called at the entry of each user-defined region
 * Fortran version
 */

void FSUB(EPIK_User_start)(const char *name, const char *file, const int *lno, int nl, int fl)
{
  elg_ui4 rid;
#ifdef CONVERT_STRINGS
  /* -- convert Fortran to C strings -- */
  char fnambuf[128];
  char ffilbuf[1024];
  int namlen = ( nl < 128 ) ? nl : 127;
  int fillen = ( fl < 1024 ) ? fl : 1023;
  strncpy(fnambuf, name, namlen);
  fnambuf[namlen] = '\0';
  strncpy(ffilbuf, file, fillen);
  ffilbuf[fillen] = '\0';
#else
  char *fnambuf = (char*) name;
  char *ffilbuf = (char*) file;
#endif
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
          rid = epk_register_region(fnambuf, (long) name, ffilbuf, *lno);
        }
      }
    } else {
      rid = epk_register_region(fnambuf, (long) name, ffilbuf, *lno);
    }
#   else
    rid = epk_register_region(fnambuf, (long) name, ffilbuf, *lno);
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
 * Fortran version
 */

void FSUB(EPIK_User_end)(const char *name, const char *file, const int *lno, int nl, int fl)
{
  elg_ui4 rid;
#ifdef CONVERT_STRINGS
  /* -- convert Fortran to C strings -- */
  char fnambuf[128];
  int namlen = ( nl < 128 ) ? nl : 127;
  strncpy(fnambuf, name, namlen);
  fnambuf[namlen] = '\0';
#else
  char *fnambuf = (char*) name;
#endif
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
 * This function is called at the entry of each user-defined pause
 * Fortran version
 */

void FSUB(EPIK_Pause_start)(const char *file, const int *lno, int fl)
{
#ifdef CONVERT_STRINGS
  /* -- convert Fortran to C strings -- */
  char ffilbuf[1024];
  int fillen = ( fl < 1024 ) ? fl : 1023;
  strncpy(ffilbuf, file, fillen);
  ffilbuf[fillen] = '\0';
#else
  char *ffilbuf = (char*) file;
#endif
  EPIK_Pause_start(ffilbuf, *lno);
}

/*
 * This function is called at the exit of each user-defined pause
 * Fortran version
 */

void FSUB(EPIK_Pause_end)(const char *file, const int *lno, int fl)
{
#ifdef CONVERT_STRINGS
  /* -- convert Fortran to C strings -- */
  char ffilbuf[1024];
  int fillen = ( fl < 1024 ) ? fl : 1023;
  strncpy(ffilbuf, file, fillen);
  ffilbuf[fillen] = '\0';
#else
  char *ffilbuf = (char*) file;
#endif
  EPIK_Pause_end(ffilbuf, *lno);
}

/*
 * This function is called to explicitly flush trace buffer to disk
 * Fortran version
 */

void FSUB(EPIK_Flush_trace)(const char *file, const int *lno, int fl)
{
#ifdef CONVERT_STRINGS
  /* -- convert Fortran to C strings -- */
  char ffilbuf[1024];
  int fillen = ( fl < 1024 ) ? fl : 1023;
  strncpy(ffilbuf, file, fillen);
  ffilbuf[fillen] = '\0';
#else
  char *ffilbuf = (char*) file;
#endif
  EPIK_Flush_trace(ffilbuf, *lno);
}

