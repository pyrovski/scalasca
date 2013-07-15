/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "epk_ompreg.h"

#include "esd_def.h"

int epk_omp_regid[20];

void epk_omp_register()
{
  elg_ui4 fid = esd_def_file("OMP");

  epk_omp_regid[EPK__OMP_DESTROY_LOCK] =
    esd_def_region("omp_destroy_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_DESTROY_NEST_LOCK] =
    esd_def_region("omp_destroy_nest_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_GET_DYNAMIC] =
    esd_def_region("omp_get_dynamic", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_GET_MAX_THREADS] =
    esd_def_region("omp_get_max_threads", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_GET_NESTED] =
    esd_def_region("omp_get_nested", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_GET_NUM_PROCS] =
    esd_def_region("omp_get_num_procs", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_GET_NUM_THREADS] =
    esd_def_region("omp_get_num_threads", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_GET_THREAD_NUM] =
    esd_def_region("omp_get_thread_num", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_IN_PARALLEL] =
    esd_def_region("omp_in_parallel", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_INIT_LOCK] =
    esd_def_region("omp_init_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_INIT_NEST_LOCK] =
    esd_def_region("omp_init_nest_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_SET_DYNAMIC] =
    esd_def_region("omp_set_dynamic", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_SET_LOCK] =
    esd_def_region("omp_set_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_SET_NEST_LOCK] =
    esd_def_region("omp_set_nest_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_SET_NESTED] =
    esd_def_region("omp_set_nested", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_SET_NUM_THREADS] =
    esd_def_region("omp_set_num_threads", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_TEST_LOCK] =
    esd_def_region("omp_test_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_TEST_NEST_LOCK] =
    esd_def_region("omp_test_nest_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_UNSET_LOCK] =
    esd_def_region("omp_unset_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
  epk_omp_regid[EPK__OMP_UNSET_NEST_LOCK] =
    esd_def_region("omp_unset_nest_lock", fid, ELG_NO_LNO, ELG_NO_LNO, "OMP", ELG_FUNCTION);
}


