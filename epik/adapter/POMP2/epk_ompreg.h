/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _EPK_OMPREG_H
#define _EPK_OMPREG_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

/* 
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - Registration of OMP functions  
 *
 *-----------------------------------------------------------------------------
 */

#define EPK__OMP_DESTROY_LOCK                     0 
#define EPK__OMP_DESTROY_NEST_LOCK                1
#define EPK__OMP_GET_DYNAMIC                      2
#define EPK__OMP_GET_MAX_THREADS                  3
#define EPK__OMP_GET_NESTED                       4
#define EPK__OMP_GET_NUM_PROCS                    5
#define EPK__OMP_GET_NUM_THREADS                  6
#define EPK__OMP_GET_THREAD_NUM                   7
#define EPK__OMP_IN_PARALLEL                      8
#define EPK__OMP_INIT_LOCK                        9 
#define EPK__OMP_INIT_NEST_LOCK                  10
#define EPK__OMP_SET_DYNAMIC                     11
#define EPK__OMP_SET_LOCK                        12
#define EPK__OMP_SET_NEST_LOCK                   13
#define EPK__OMP_SET_NESTED                      14
#define EPK__OMP_SET_NUM_THREADS                 15  
#define EPK__OMP_TEST_LOCK                       16
#define EPK__OMP_TEST_NEST_LOCK                  17 
#define EPK__OMP_UNSET_LOCK                      18
#define EPK__OMP_UNSET_NEST_LOCK                 19

extern int     epk_omp_regid[20];

EXTERN void    epk_omp_register();


#endif









