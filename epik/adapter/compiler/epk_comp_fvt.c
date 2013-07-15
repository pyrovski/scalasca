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
 * Management functions
 * Fortran version
 */

void FSUB(VTinit)(int* ierr) {
  esd_open();
  *ierr = 0;
}

void FSUB(VTfini)(int* ierr) {
  esd_close();
  *ierr = 0;
}

void FSUB(VTtraceon)(int* ierr) {
  /* -- NOT YET IMPLEMENTED -- */
  *ierr = 0;
}

void FSUB(VTtraceoff)(int* ierr) {
  /* -- NOT YET IMPLEMENTED -- */
  *ierr = 0;
}


/*
 * This function defines function handles
 * Fortran version
 */

static char fnambuf[128];
static char fgrpbuf[128];

void FSUB(VTsymdef)(int* handle, char *name, char *group, int* ierr,
                    int nl, int gl) {
  elg_ui4 rid;

  /* -- convert Fortran to C strings -- */
  int namlen = ( nl < 128 ) ? nl : 127;
  int grplen = ( gl < 128 ) ? gl : 127;
  strncpy(fnambuf, name, namlen);
  fnambuf[namlen] = '\0';
  strncpy(fgrpbuf, group, grplen);
  fgrpbuf[grplen] = '\0';

  /* -- register region -- */
  rid = esd_def_region(fnambuf, ELG_NO_ID, ELG_NO_LNO, ELG_NO_LNO,
                       fgrpbuf, ELG_FUNCTION);
  epk_hash_put(*handle, rid);
  *ierr =0;
}

/*
 * This functions mark the entry and exit of a function
 * Fortran version
 */

void FSUB(VTbegin)(int* handle, int* ierr) {
  elg_ui4 rid;

  if ( (rid = epk_hash_get(*handle)) != ELG_NO_ID ) {
    esd_enter(rid);
  }
  *ierr =0;
}

void FSUB(VTend)(int* handle, int* ierr) {
  elg_ui4 rid;

  if ( (rid = epk_hash_get(*handle)) != ELG_NO_ID ) {
    esd_exit(rid);
  }
  *ierr =0;
}
