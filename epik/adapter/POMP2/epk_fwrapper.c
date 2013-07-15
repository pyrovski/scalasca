/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/** @internal
 *
 *  @file       pomp2_fwrapper.c
 *  @status     alpha
 *
 *  @maintainer Dirk Schmidl <schmidl@rz.rwth-aachen.de>
 *
 *  @authors    Bernd Mohr <b.mohr@fz-juelich.de>
 *              Dirk Schmidl <schmidl@rz-rwth-aachen.de>
 *              Peter Philippen <p.philippen@fz-juelich.de>
 *
 *  @brief      This file contains fortran wrapper functions.*/

#define POMP2_Atomic_enter_U		POMP2_ATOMIC_ENTER
#define POMP2_Atomic_exit_U		POMP2_ATOMIC_EXIT
#define POMP2_Implicit_barrier_enter_U	POMP2_IMPLICIT_BARRIER_ENTER
#define POMP2_Implicit_barrier_exit_U	POMP2_IMPLICIT_BARRIER_EXIT
#define POMP2_Barrier_enter_U		POMP2_BARRIER_ENTER
#define POMP2_Barrier_exit_U		POMP2_BARRIER_EXIT
#define POMP2_Flush_enter_U		POMP2_FLUSH_ENTER
#define POMP2_Flush_exit_U		POMP2_FLUSH_EXIT
#define POMP2_Critical_begin_U		POMP2_CRITICAL_BEGIN
#define POMP2_Critical_end_U		POMP2_CRITICAL_END
#define POMP2_Critical_enter_U		POMP2_CRITICAL_ENTER
#define POMP2_Critical_exit_U		POMP2_CRITICAL_EXIT
#define POMP2_Do_enter_U		POMP2_DO_ENTER
#define POMP2_Do_exit_U			POMP2_DO_EXIT
#define POMP2_Parallel_begin_U		POMP2_PARALLEL_BEGIN
#define POMP2_Parallel_end_U		POMP2_PARALLEL_END
#define POMP2_Master_begin_U		POMP2_MASTER_BEGIN
#define POMP2_Master_end_U		POMP2_MASTER_END
#define POMP2_Parallel_fork_U		POMP2_PARALLEL_FORK
#define POMP2_Parallel_join_U		POMP2_PARALLEL_JOIN
#define POMP2_Section_begin_U		POMP2_SECTION_BEGIN
#define POMP2_Section_end_U		POMP2_SECTION_END
#define POMP2_Sections_enter_U		POMP2_SECTIONS_ENTER
#define POMP2_Sections_exit_U		POMP2_SECTIONS_EXIT
#define POMP2_Single_begin_U		POMP2_SINGLE_BEGIN
#define POMP2_Single_end_U		POMP2_SINGLE_END
#define POMP2_Single_enter_U		POMP2_SINGLE_ENTER
#define POMP2_Single_exit_U		POMP2_SINGLE_EXIT
#define POMP2_Ordered_begin_U		POMP2_ORDERED_BEGIN
#define POMP2_Ordered_end_U		POMP2_ORDERED_END
#define POMP2_Ordered_enter_U		POMP2_ORDERED_ENTER
#define POMP2_Ordered_exit_U		POMP2_ORDERED_EXIT
#define POMP2_Task_create_begin_U	POMP2_TASK_CREATE_BEGIN
#define POMP2_Task_create_end_U		POMP2_TASK_CREATE_END
#define POMP2_Task_begin_U		POMP2_TASK_BEGIN
#define POMP2_Task_end_U		POMP2_TASK_END
#define POMP2_Untied_task_create_begin_U  POMP2_UNTIED_TASK_CREATE_BEGIN
#define POMP2_Untied_task_create_end_U	POMP2_UNTIED_TASK_CREATE_END
#define POMP2_Get_current_task_U	POMP2_GET_CURRENT_TASK
#define POMP2_Set_current_task_U	POMP2_SET_CURRENT_TASK
#define POMP2_Untied_task_begin_U	POMP2_UNTIED_TASK_BEGIN
#define POMP2_Untied_task_end_U		POMP2_UNTIED_TASK_END
#define POMP2_Taskwait_begin_U		POMP2_TASKWAIT_BEGIN
#define POMP2_Taskwait_end_U		POMP2_TASKWAIT_END
#define POMP2_Workshare_enter_U		POMP2_WORKSHARE_ENTER
#define POMP2_Workshare_exit_U		POMP2_WORKSHARE_EXIT
#define POMP2_Lib_get_max_threads_U	POMP2_LIB_GET_MAX_THREADS

#define POMP2_Atomic_enter_L		pomp2_atomic_enter
#define POMP2_Atomic_exit_L		pomp2_atomic_exit
#define POMP2_Implicit_barrier_enter_L	pomp2_implicit_barrier_enter
#define POMP2_Implicit_barrier_exit_L	pomp2_implicit_barrier_exit
#define POMP2_Barrier_enter_L		pomp2_barrier_enter
#define POMP2_Barrier_exit_L		pomp2_barrier_exit
#define POMP2_Flush_enter_L		pomp2_flush_enter
#define POMP2_Flush_exit_L		pomp2_flush_exit
#define POMP2_Critical_begin_L		pomp2_critical_begin
#define POMP2_Critical_end_L		pomp2_critical_end
#define POMP2_Critical_enter_L		pomp2_critical_enter
#define POMP2_Critical_exit_L		pomp2_critical_exit
#define POMP2_Do_enter_L		pomp2_do_enter
#define POMP2_Do_exit_L			pomp2_do_exit
#define POMP2_Parallel_begin_L		pomp2_parallel_begin
#define POMP2_Parallel_end_L		pomp2_parallel_end
#define POMP2_Master_begin_L		pomp2_master_begin
#define POMP2_Master_end_L		pomp2_master_end
#define POMP2_Parallel_fork_L		pomp2_parallel_fork
#define POMP2_Parallel_join_L		pomp2_parallel_join
#define POMP2_Section_begin_L		pomp2_section_begin
#define POMP2_Section_end_L		pomp2_section_end
#define POMP2_Sections_enter_L		pomp2_sections_enter
#define POMP2_Sections_exit_L		pomp2_sections_exit
#define POMP2_Single_begin_L		pomp2_single_begin
#define POMP2_Single_end_L		pomp2_single_end
#define POMP2_Single_enter_L		pomp2_single_enter
#define POMP2_Single_exit_L		pomp2_single_exit
#define POMP2_Ordered_begin_L		pomp2_ordered_begin
#define POMP2_Ordered_end_L		pomp2_ordered_end
#define POMP2_Ordered_enter_L		pomp2_ordered_enter
#define POMP2_Ordered_exit_L		pomp2_ordered_exit
#define POMP2_Task_create_begin_L	pomp2_task_create_begin
#define POMP2_Task_create_end_L		pomp2_task_create_end
#define POMP2_Task_begin_L		pomp2_task_begin
#define POMP2_Task_end_L		pomp2_task_end
#define POMP2_Untied_task_create_begin_L  pomp2_untied_task_create_begin
#define POMP2_Untied_task_create_end_L	pomp2_untied_task_create_end
#define POMP2_Untied_task_begin_L	pomp2_untied_task_begin
#define POMP2_Untied_task_end_L		pomp2_untied_task_end
#define POMP2_Taskwait_begin_L		pomp2_taskwait_begin
#define POMP2_Taskwait_end_L		pomp2_taskwait_end
#define POMP2_Workshare_enter_L		pomp2_workshare_enter
#define POMP2_Workshare_exit_L		pomp2_workshare_exit
#define POMP2_Lib_get_max_threads_L	pomp2_lib_get_max_threads

#include "opari2/pomp2_lib.h"
#include "epk_fwrapper_def.h"

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */
void FSUB(POMP2_Atomic_enter)(POMP2_Region_handle* regionHandle, char* ctc_string) {
   POMP2_Atomic_enter(regionHandle, ctc_string);
}

void FSUB(POMP2_Atomic_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_Atomic_exit(regionHandle );
}

void FSUB(POMP2_Implicit_barrier_enter)( POMP2_Region_handle* regionHandle,
                                         POMP2_Task_handle*   pomp2_old_task) {
  POMP2_Implicit_barrier_enter( regionHandle, pomp2_old_task );
}

void FSUB(POMP2_Implicit_barrier_exit)( POMP2_Region_handle* regionHandle,
                                        POMP2_Task_handle*   pomp2_old_task) {
  POMP2_Implicit_barrier_exit( regionHandle, *pomp2_old_task );
}

void FSUB(POMP2_Barrier_enter)( POMP2_Region_handle* regionHandle,
                                POMP2_Task_handle*   pomp2_old_task,
                                char*                ctc_string) {
  POMP2_Barrier_enter( regionHandle, pomp2_old_task, ctc_string);
}

void FSUB(POMP2_Barrier_exit)( POMP2_Region_handle* regionHandle,
                               POMP2_Task_handle*   pomp2_old_task ) {
  POMP2_Barrier_exit( regionHandle, *pomp2_old_task );
}

void FSUB(POMP2_Flush_enter)(POMP2_Region_handle* regionHandle, char* ctc_string ) {
  POMP2_Flush_enter(regionHandle, ctc_string );
}

void FSUB(POMP2_Flush_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_Flush_exit(regionHandle );
}

void FSUB(POMP2_Critical_begin)(POMP2_Region_handle* regionHandle ) {
   POMP2_Critical_begin(regionHandle );
}

void FSUB(POMP2_Critical_end)(POMP2_Region_handle* regionHandle ) {
   POMP2_Critical_end(regionHandle );
}

void FSUB(POMP2_Critical_enter)(POMP2_Region_handle* regionHandle, char* ctc_string) {
   POMP2_Critical_enter(regionHandle, ctc_string);
}

void FSUB(POMP2_Critical_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_Critical_exit(regionHandle );
}

void FSUB(POMP2_Do_enter)(POMP2_Region_handle* regionHandle, char* ctc_string) {
   POMP2_For_enter(regionHandle, ctc_string);
}

void FSUB(POMP2_Do_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_For_exit(regionHandle );
}

void FSUB(POMP2_Parallel_begin)( POMP2_Region_handle* regionHandle,
                                 POMP2_Task_handle*   newTask,
                                 char*                ctc_string ){
  POMP2_Parallel_begin(regionHandle);
}

void FSUB(POMP2_Parallel_end)(POMP2_Region_handle* regionHandle) {
   POMP2_Parallel_end(regionHandle);
}

void FSUB(POMP2_Master_begin)(POMP2_Region_handle* regionHandle, char* ctc_string ) {
  POMP2_Master_begin(regionHandle, ctc_string );
}

void FSUB(POMP2_Master_end)(POMP2_Region_handle* regionHandle ) {
   POMP2_Master_end(regionHandle );
}

void FSUB(POMP2_Parallel_fork)(POMP2_Region_handle* regionHandle,
                               int*                 if_clause,
                               int*                 num_threads,
                               POMP2_Task_handle*   pomp2_old_task,
                               char*                ctc_string) {
  POMP2_Parallel_fork(regionHandle, *if_clause, *num_threads, pomp2_old_task, "dummy");
}

void FSUB(POMP2_Parallel_join)(POMP2_Region_handle* regionHandle,
                               POMP2_Task_handle*   pomp2_old_task ) {
  POMP2_Parallel_join(regionHandle, *pomp2_old_task );
}

void FSUB(POMP2_Section_begin)(POMP2_Region_handle* regionHandle, char* ctc_string ) {
  POMP2_Section_begin(regionHandle, ctc_string );
}

void FSUB(POMP2_Section_end)(POMP2_Region_handle* regionHandle ) {
   POMP2_Section_end(regionHandle );
}

void FSUB(POMP2_Sections_enter)(POMP2_Region_handle* regionHandle, char* ctc_string) {
   POMP2_Sections_enter(regionHandle, ctc_string);
}

void FSUB(POMP2_Sections_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_Sections_exit(regionHandle );
}

void FSUB(POMP2_Single_begin)(POMP2_Region_handle* regionHandle ) {
   POMP2_Single_begin(regionHandle );
}

void FSUB(POMP2_Single_end)(POMP2_Region_handle* regionHandle ) {
   POMP2_Single_end(regionHandle );
}

void FSUB(POMP2_Single_enter)(POMP2_Region_handle* regionHandle, char* ctc_string) {
   POMP2_Single_enter(regionHandle, ctc_string);
}

void FSUB(POMP2_Ordered_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_Ordered_exit(regionHandle );
}

void FSUB(POMP2_Ordered_begin)(POMP2_Region_handle* regionHandle ) {
   POMP2_Ordered_begin(regionHandle );
}

void FSUB(POMP2_Ordered_end)(POMP2_Region_handle* regionHandle ) {
   POMP2_Ordered_end(regionHandle );
}

void FSUB(POMP2_Ordered_enter)(POMP2_Region_handle* regionHandle, char* ctc_string) {
   POMP2_Ordered_enter(regionHandle, ctc_string);
}

void FSUB(POMP2_Single_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_Single_exit(regionHandle );
}

void FSUB(POMP2_Task_create_begin)(POMP2_Region_handle* regionHandle,
                                   POMP2_Task_handle*   pomp2_new_task,
                                   POMP2_Task_handle*   pomp2_old_task,
                                   int*                 pomp2_if,
                                   char*                ctc_string){
  POMP2_Task_create_begin(regionHandle, pomp2_new_task, pomp2_old_task, *pomp2_if, ctc_string);
}

void FSUB(POMP2_Task_create_end)(POMP2_Region_handle* regionHandle,
                                 POMP2_Task_handle*   pomp2_old_task ){
  POMP2_Task_create_end(regionHandle, *pomp2_old_task);
}

void FSUB(POMP2_Task_begin)( POMP2_Region_handle* regionHandle,
                             POMP2_Task_handle* pomp2_new_task ){
  POMP2_Task_begin(regionHandle, *pomp2_new_task);
}

void FSUB(POMP2_Task_end)(POMP2_Region_handle* regionHandle){
  POMP2_Task_end(regionHandle);
}

void FSUB(POMP2_Untied_task_create_begin)(POMP2_Region_handle* regionHandle,
                                          POMP2_Task_handle*   pomp2_new_task,
                                          POMP2_Task_handle*   pomp2_old_task,
                                          int*                 pomp2_if,
                                          char*                ctc_string){
  POMP2_Task_create_begin(regionHandle, pomp2_new_task, pomp2_old_task, *pomp2_if, ctc_string);
}

void FSUB(POMP2_Untied_task_create_end)(POMP2_Region_handle* regionHandle,
                                        POMP2_Task_handle*   pomp2_old_task ){
    POMP2_Task_create_end(regionHandle, *pomp2_old_task);
}

void FSUB(POMP2_Untied_task_begin)( POMP2_Region_handle* regionHandle,
                                    POMP2_Task_handle*   pomp2_new_task ){
  POMP2_Task_begin(regionHandle, *pomp2_new_task);
}

void FSUB(POMP2_Untied_task_end)(POMP2_Region_handle* regionHandle){
  POMP2_Task_end(regionHandle);
}

void FSUB(POMP2_Taskwait_begin)(POMP2_Region_handle* regionHandle,
                                POMP2_Task_handle*   pomp2_old_task,
                                char*                ctc_string ){
  POMP2_Taskwait_begin(regionHandle, pomp2_old_task, ctc_string );
}

void FSUB(POMP2_Taskwait_end)(POMP2_Region_handle* regionHandle,
                              POMP2_Task_handle*   pomp2_old_task ){
  POMP2_Taskwait_end(regionHandle, *pomp2_old_task);
}

void FSUB(POMP2_Workshare_enter)(POMP2_Region_handle* regionHandle, char* ctc_string) {
   POMP2_Workshare_enter(regionHandle, ctc_string);
}

void FSUB(POMP2_Workshare_exit)(POMP2_Region_handle* regionHandle ) {
   POMP2_Workshare_exit(regionHandle );
}

int FSUB(POMP2_Lib_get_max_threads)() {
   return omp_get_max_threads();
}
