/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_PEARL_H
#define PEARL_PEARL_H


/*-------------------------------------------------------------------------*/
/**
 * @file    pearl.h
 * @ingroup PEARL_base
 * @ingroup PEARL_mpi
 * @ingroup PEARL_omp
 * @ingroup PEARL_hybrid
 * @brief   Declarations of global library functions.
 *
 * This header file provides declarations of global library functions and
 * support routines. It includes declarations for both, generic as well as
 * programming-model-specific routines.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class GlobalDefs;
class LocalTrace;


//--- Function prototypes ---------------------------------------------------

/// @name Library initialization
/// @{

extern void PEARL_init();
extern void PEARL_mpi_init();
extern void PEARL_omp_init();
extern void PEARL_hybrid_init();

/// @}
/// @name Call tree verification
/// @{

extern void PEARL_verify_calltree    (      GlobalDefs& defs,
                                      const LocalTrace& trace);
extern void PEARL_omp_verify_calltree(      GlobalDefs& defs,
                                      const LocalTrace& trace);

/// @}
/// @name Call tree unification
/// @{

extern void PEARL_mpi_unify_calltree(GlobalDefs& defs);

/// @}
/// @name Trace preprocessing
/// @{

extern void PEARL_preprocess_trace    (const GlobalDefs& defs,
                                       const LocalTrace& trace);
extern void PEARL_omp_preprocess_trace(const GlobalDefs& defs,
                                       const LocalTrace& trace);

/// @}


}   /* namespace pearl */


#endif   /* !PEARL_PEARL_H */
