/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <omp.h>

#define POMP2_Init_lock_U		POMP2_INIT_LOCK
#define POMP2_Destroy_lock_U		POMP2_DESTROY_LOCK
#define POMP2_Set_lock_U		POMP2_SET_LOCK
#define POMP2_Unset_lock_U		POMP2_UNSET_LOCK
#define POMP2_Test_lock_U		POMP2_TEST_LOCK
#define POMP2_Init_nest_lock_U		POMP2_INIT_NEST_LOCK
#define POMP2_Destroy_nest_lock_U	POMP2_DESTROY_NEST_LOCK
#define POMP2_Set_nest_lock_U		POMP2_SET_NEST_LOCK
#define POMP2_Unset_nest_lock_U		POMP2_UNSET_NEST_LOCK
#define POMP2_Test_nest_lock_U		POMP2_TEST_NEST_LOCK
#define omp_init_lock_U			OMP_INIT_LOCK
#define omp_destroy_lock_U		OMP_DESTROY_LOCK
#define omp_set_lock_U			OMP_SET_LOCK
#define omp_unset_lock_U		OMP_UNSET_LOCK
#define omp_test_lock_U			OMP_TEST_LOCK
#define omp_init_nest_lock_U		OMP_INIT_NEST_LOCK
#define omp_destroy_nest_lock_U		OMP_DESTROY_NEST_LOCK
#define omp_set_nest_lock_U		OMP_SET_NEST_LOCK
#define omp_unset_nest_lock_U		OMP_UNSET_NEST_LOCK
#define omp_test_nest_lock_U		OMP_TEST_NEST_LOCK

#define POMP2_Init_lock_L		pomp2_init_lock
#define POMP2_Destroy_lock_L		pomp2_destroy_lock
#define POMP2_Set_lock_L		pomp2_set_lock
#define POMP2_Unset_lock_L		pomp2_unset_lock
#define POMP2_Test_lock_L		pomp2_test_lock
#define POMP2_Init_nest_lock_L		pomp2_init_nest_lock
#define POMP2_Destroy_nest_lock_L	pomp2_destroy_nest_lock
#define POMP2_Set_nest_lock_L		pomp2_set_nest_lock
#define POMP2_Unset_nest_lock_L		pomp2_unset_nest_lock
#define POMP2_Test_nest_lock_L		pomp2_test_nest_lock
#define omp_init_lock_L			omp_init_lock
#define omp_destroy_lock_L		omp_destroy_lock
#define omp_set_lock_L			omp_set_lock
#define omp_unset_lock_L		omp_unset_lock
#define omp_test_lock_L			omp_test_lock
#define omp_init_nest_lock_L		omp_init_nest_lock
#define omp_destroy_nest_lock_L		omp_destroy_nest_lock
#define omp_set_nest_lock_L		omp_set_nest_lock
#define omp_unset_nest_lock_L		omp_unset_nest_lock
#define omp_test_nest_lock_L		omp_test_nest_lock

#include "epk_fwrapper_def.h"

#ifndef EPK_FWRAPPER_BASE_H
#define EPK_FWRAPPER_BASE_H

#if defined(__OPEN64__)
    #define VOLATILE   volatile
#else
    #define VOLATILE
#endif

extern void
FSUB( omp_init_lock )( VOLATILE omp_lock_t* s );
extern void
FSUB( omp_destroy_lock )( VOLATILE omp_lock_t* s );
extern void
FSUB( omp_set_lock )( VOLATILE omp_lock_t* s );
extern void
FSUB( omp_unset_lock )( VOLATILE omp_lock_t* s );
extern int
FSUB( omp_test_lock )( VOLATILE omp_lock_t* s );
extern void
FSUB( omp_init_nest_lock )( VOLATILE omp_nest_lock_t* s );
extern void
FSUB( omp_destroy_nest_lock )( VOLATILE omp_nest_lock_t* s );
extern void
FSUB( omp_set_nest_lock )( VOLATILE omp_nest_lock_t* s );
extern void
FSUB( omp_unset_nest_lock )( VOLATILE omp_nest_lock_t* s );
extern int
FSUB( omp_test_nest_lock )( VOLATILE omp_nest_lock_t* s );

#undef VOLATILE

#endif
