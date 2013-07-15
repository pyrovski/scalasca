/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
#include "elg_error.h"

#include "epk_pomp2.h"
#include "epk_omplock.h"
#include "epk_ompreg.h"

#include "esd_event.h"
#include "esd_run.h"

#include "opari2/pomp2_lib.h"
#include "pomp2_region_info.h"

/** Id of the currently executing task*/
POMP2_Task_handle pomp2_current_task = 0;
#pragma omp threadprivate(pomp2_current_task)

/** Counter of tasks used to determine task ids for newly created ta*/
POMP2_Task_handle pomp2_task_counter = 1;
#pragma omp threadprivate(pomp2_task_counter)

POMP2_Task_handle
POMP2_Get_new_task_handle()
{
    return ( ( POMP2_Task_handle )omp_get_thread_num() << 32 ) + pomp2_task_counter++;
}

/** @name Functions generated by the instrumenter */
/*@{*/
/**
 * Returns the number of instrumented regions.@n
 * The instrumenter scans all opari2-created include files with nm and greps
 * the POMP2_INIT_uuid_numRegions() function calls. Here we return the sum of
 * all numRegions.
 */
extern size_t
POMP2_Get_num_regions();

/**
 * Init all opari2-created regions.@n
 * The instrumenter scans all opari2-created include files with nm and greps
 * the POMP2_INIT_uuid_numRegions() function calls. The instrumentor then
 * defines this functions by calling all grepped functions.
 */
extern void
POMP2_Init_regions();

/**
 * Returns the opari2 version.
 */
extern const char*
POMP2_Get_opari2_version();

/*@}*/

void
POMP2_Atomic_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_Atomic_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(rid);
}

void
POMP2_Implicit_barrier_enter( POMP2_Region_handle* pomp2_handle,
                              POMP2_Task_handle*   pomp2_old_task )
{
  POMP2_Barrier_enter( pomp2_handle, pomp2_old_task, "" );
}

extern void
POMP2_Implicit_barrier_exit( POMP2_Region_handle* pomp2_handle,
                             POMP2_Task_handle    pomp2_old_task )
{
  pomp2_current_task = pomp2_old_task;
  POMP2_Barrier_exit( pomp2_handle, pomp2_old_task );
}


void
POMP2_Barrier_enter( POMP2_Region_handle* pomp2_handle,
                     POMP2_Task_handle*   pomp2_old_task,
                     const char           ctc_string[] )
{
    *pomp2_old_task = pomp2_current_task;
    CHECK_HANDLE();
    GUARDED_ESD_ENTER_2('b', rid, brid);
}

void
POMP2_Barrier_exit( POMP2_Region_handle* pomp2_handle,
                    POMP2_Task_handle    pomp2_old_task )
{
    pomp2_current_task = pomp2_old_task;
    GUARDED_ESD_OMP_COLL_EXIT_2('b', rid, brid);
}

void
POMP2_Flush_enter( POMP2_Region_handle* pomp2_handle,
		   const char           ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_Flush_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(rid);
}

void
POMP2_Critical_begin( POMP2_Region_handle* pomp2_handle )
{
    EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle);
    if ( pomp2_tracing){
      esd_omp_alock( rdescr->brid );
      esd_enter( rdescr->sbrid );
    }
}

void
POMP2_Critical_end( POMP2_Region_handle* pomp2_handle )
{
    EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle);
    if ( pomp2_tracing ){
      esd_exit( rdescr->sbrid );
      esd_omp_rlock( rdescr->brid );
    }
}

void
POMP2_Critical_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_Critical_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(rid);
}

void
POMP2_For_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER_2('p', sbrid, rid);
}

void
POMP2_For_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_OMP_COLL_EXIT_2('p', sbrid, rid);
}

void
POMP2_Master_begin( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_Master_end( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(rid);
}

void
POMP2_Parallel_begin( POMP2_Region_handle* pomp2_handle)
{
    pomp2_current_task = POMP2_Get_new_task_handle();
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_Parallel_end( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_OMP_COLL_EXIT(rid);
}

void
POMP2_Parallel_fork( POMP2_Region_handle* pomp2_handle,
                     int                  if_clause,
                     int                  num_threads,
                     POMP2_Task_handle*   pomp2_old_task,
                     const char           ctc_string[] )
{
    EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle);
    CHECK_HANDLE();
    *pomp2_old_task = pomp2_current_task;
    if( pomp2_tracing )
    {
      esd_omp_fork( rdescr->rid );
    }
}

void
POMP2_Parallel_join( POMP2_Region_handle* pomp2_handle,
                     POMP2_Task_handle    pomp2_old_task )
{
    EPIK_Region_descriptor* rdescr = (EPIK_Region_descriptor*) (*pomp2_handle);
    if( pomp2_tracing )
    {
        esd_omp_join( rdescr->rid );
    }
}

void
POMP2_Section_begin( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    GUARDED_ESD_ENTER(sbrid);
}

void
POMP2_Section_end( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(sbrid);
}

void
POMP2_Sections_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER_2('p', sbrid, rid);
}

void
POMP2_Sections_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_OMP_COLL_EXIT_2('p', sbrid, rid);
}

void
POMP2_Single_begin( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_ENTER(sbrid);
}

void
POMP2_Single_end( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(sbrid);
}

void
POMP2_Single_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_Single_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(rid);
}

void
POMP2_Workshare_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER_2('p', sbrid, rid);
}

void
POMP2_Workshare_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_OMP_COLL_EXIT_2('p', sbrid, rid);
}

void
POMP2_Ordered_begin( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_ENTER(sbrid);
}

void
POMP2_Ordered_end( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(sbrid);
}

void
POMP2_Ordered_enter( POMP2_Region_handle* pomp2_handle,
                    const char           ctc_string[] )
{
    CHECK_HANDLE();
    GUARDED_ESD_ENTER(rid);
}

void
POMP2_Ordered_exit( POMP2_Region_handle* pomp2_handle )
{
    GUARDED_ESD_EXIT(rid);
}


void
POMP2_Task_create_begin( POMP2_Region_handle* pomp2_handle,
                         POMP2_Task_handle*   pomp2_new_task,
                         POMP2_Task_handle*   pomp2_old_task,
                         int                  pomp2_if,
                         const char           ctc_string[])
{
    *pomp2_old_task = pomp2_current_task;

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Task_create_end( POMP2_Region_handle* pomp2_handle,
                       POMP2_Task_handle    pomp2_old_task )
{
    pomp2_current_task = pomp2_old_task;

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Task_begin( POMP2_Region_handle* pomp2_handle,
                  POMP2_Task_handle    pomp2_new_task )
{
    pomp2_current_task = POMP2_Get_new_task_handle();

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Task_end( POMP2_Region_handle* pomp2_handle )
{
    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Untied_task_create_begin( POMP2_Region_handle* pomp2_handle,
                                POMP2_Task_handle*   pomp2_new_task,
                                POMP2_Task_handle*   pomp2_old_task,
                                int                  pomp2_if,
                                const char           ctc_string[] )
{
    *pomp2_old_task = pomp2_current_task;

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Untied_task_create_end( POMP2_Region_handle* pomp2_handle,
                              POMP2_Task_handle    pomp2_old_task )
{
    pomp2_current_task = pomp2_old_task;

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Untied_task_begin( POMP2_Region_handle* pomp2_handle,
                         POMP2_Task_handle    pomp2_new_task )
{
    pomp2_current_task = POMP2_Get_new_task_handle();

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Untied_task_end( POMP2_Region_handle* pomp2_handle )
{

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Taskwait_begin( POMP2_Region_handle* pomp2_handle,
                      POMP2_Task_handle*   pomp2_old_task,
                      const char           init_string[] )
{
    *pomp2_old_task = pomp2_current_task;

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

void
POMP2_Taskwait_end( POMP2_Region_handle* pomp2_handle,
                    POMP2_Task_handle    pomp2_old_task )
{
    pomp2_current_task = pomp2_old_task;

    elg_error_msg("ERROR: This version of Scalasca does not support OpenMP 3.0 tasks.");
}

/*
 *----------------------------------------------------------------
 * C Wrapper for OpenMP API
 *----------------------------------------------------------------
 */


void POMP2_Init_lock(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_INIT_LOCK]);
    omp_init_lock(s);
    epk_lock_init(s);
    esd_exit(epk_omp_regid[EPK__OMP_INIT_LOCK]);
  } else {
    omp_init_lock(s);
    epk_lock_init(s);
  }
}

void POMP2_Destroy_lock(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_DESTROY_LOCK]);
    omp_destroy_lock(s);
    epk_lock_destroy(s);
    esd_exit(epk_omp_regid[EPK__OMP_DESTROY_LOCK]);
  } else {
    omp_destroy_lock(s);
    epk_lock_destroy(s);
  }
}

void POMP2_Set_lock(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_SET_LOCK]);
    omp_set_lock(s);
    if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_SET_LOCK]);
  } else {
    omp_set_lock(s);
  }
}

void POMP2_Unset_lock(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_UNSET_LOCK]);
    omp_unset_lock(s);
    if ( pomp2_tracing > 1 ) esd_omp_rlock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_UNSET_LOCK]);
  } else {
    omp_unset_lock(s);
  }
}

int  POMP2_Test_lock(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    int result;

    esd_enter(epk_omp_regid[EPK__OMP_TEST_LOCK]);
    result = omp_test_lock(s);
    if (result) if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_TEST_LOCK]);
    return result;     
  } else {
    return omp_test_lock(s);
  }
}

void POMP2_Init_nest_lock(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_INIT_NEST_LOCK]);
    omp_init_nest_lock(s);
    epk_lock_init(s);
    esd_exit(epk_omp_regid[EPK__OMP_INIT_NEST_LOCK]);
  } else {
    omp_init_nest_lock(s);
    epk_lock_init(s);
  }
}

void POMP2_Destroy_nest_lock(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_DESTROY_NEST_LOCK]);
    omp_destroy_nest_lock(s);
    epk_lock_destroy(s);
    esd_exit(epk_omp_regid[EPK__OMP_DESTROY_NEST_LOCK]);
  } else {
    omp_destroy_nest_lock(s);
    epk_lock_destroy(s);
  }
}

void POMP2_Set_nest_lock(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_SET_NEST_LOCK]);
    omp_set_nest_lock(s);
    if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_SET_NEST_LOCK]);
  } else {
    omp_set_nest_lock(s);
  }
}

void POMP2_Unset_nest_lock(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    esd_enter(epk_omp_regid[EPK__OMP_UNSET_NEST_LOCK]);
    omp_unset_nest_lock(s);
    if ( pomp2_tracing > 1 ) esd_omp_rlock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_UNSET_NEST_LOCK]);
  } else {
    omp_unset_nest_lock(s);
  }
}

int  POMP2_Test_nest_lock(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    int result;

    esd_enter(epk_omp_regid[EPK__OMP_TEST_NEST_LOCK]);
    result = omp_test_nest_lock(s);
    if (result) if ( pomp2_tracing > 1 ) esd_omp_alock(epk_lock_id(s));
    esd_exit(epk_omp_regid[EPK__OMP_TEST_NEST_LOCK]);
    return result;
  } else {
    return omp_test_nest_lock(s);
  }
}
