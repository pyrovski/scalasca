/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
#include "epk_fwrapper_def.h"
#include "epk_fpomp.h"

#include "epk_pomp2.h"
#include "epk_omplock.h"
#include "epk_ompreg.h"

#include "esd_event.h"
#include "esd_run.h"

/*
 *----------------------------------------------------------------
 * Fortran Wrapper for OpenMP API
 *----------------------------------------------------------------
 */

#if defined(__ICC) || defined(__ECC) || defined(_SX)
  #define CALLFSUB(name) name
#else
  #define CALLFSUB(name) FSUB(name)
#endif

void FSUB(POMP2_Init_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_INIT_LOCK]);
    CALLFSUB(omp_init_lock)(s);
    epk_lock_init(s);
    esd_exit(epk_omp_regid[EPK__OMP_INIT_LOCK]);
  } else {
    CALLFSUB(omp_init_lock)(s);
    epk_lock_init(s);
  }
}

void FSUB(POMP2_Destroy_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_DESTROY_LOCK]);
    CALLFSUB(omp_destroy_lock)(s);
    epk_lock_destroy(s);
    esd_exit(epk_omp_regid[EPK__OMP_DESTROY_LOCK]);
  } else {
    CALLFSUB(omp_destroy_lock)(s);
    epk_lock_destroy(s);
  }
}

void FSUB(POMP2_Set_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_SET_LOCK]);
    CALLFSUB(omp_set_lock)(s);
    if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_SET_LOCK]);
  } else {
    CALLFSUB(omp_set_lock)(s);
  }
}

void FSUB(POMP2_Unset_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_UNSET_LOCK]);
    CALLFSUB(omp_unset_lock)(s);
    if ( pomp2_tracing > 1 ) esd_omp_rlock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_UNSET_LOCK]);
  } else {
    CALLFSUB(omp_unset_lock)(s);
  }
}

int  FSUB(POMP2_Test_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    int result;

    esd_enter(epk_omp_regid[EPK__OMP_TEST_LOCK]);
    result = CALLFSUB(omp_test_lock)(s);
    if (result) if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_TEST_LOCK]);
    return result;     
  } else {
    return CALLFSUB(omp_test_lock)(s);
  }
}

void FSUB(POMP2_Init_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_INIT_NEST_LOCK]);
    CALLFSUB(omp_init_nest_lock)(s);
    epk_lock_init(s);
    esd_exit(epk_omp_regid[EPK__OMP_INIT_NEST_LOCK]);
  } else {
    CALLFSUB(omp_init_nest_lock)(s);
    epk_lock_init(s);
  }
}

void FSUB(POMP2_Destroy_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_DESTROY_NEST_LOCK]);
    CALLFSUB(omp_destroy_nest_lock)(s);
    epk_lock_destroy(s);
    esd_exit(epk_omp_regid[EPK__OMP_DESTROY_NEST_LOCK]);
  } else {
    CALLFSUB(omp_destroy_nest_lock)(s);
    epk_lock_destroy(s);
  }
}

void FSUB(POMP2_Set_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_SET_NEST_LOCK]);
    CALLFSUB(omp_set_nest_lock)(s);
    if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_SET_NEST_LOCK]);
  } else {
    CALLFSUB(omp_set_nest_lock)(s);
  }
}

void FSUB(POMP2_Unset_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_UNSET_NEST_LOCK]);
    CALLFSUB(omp_unset_nest_lock)(s);
    if ( pomp2_tracing > 1 ) esd_omp_rlock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_UNSET_NEST_LOCK]);
  } else {
    CALLFSUB(omp_unset_nest_lock)(s);
  }
}

int  FSUB(POMP2_Test_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    int result;

    esd_enter(epk_omp_regid[EPK__OMP_TEST_NEST_LOCK]);
    result = CALLFSUB(omp_test_nest_lock)(s);
    if (result) if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_TEST_NEST_LOCK]);
    return result;
  } else {
    return CALLFSUB(omp_test_nest_lock)(s);
  }
}
