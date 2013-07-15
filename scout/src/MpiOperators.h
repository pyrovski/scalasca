/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_MPIOPERATORS_H
#define SCOUT_MPIOPERATORS_H


#include <mpi.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  MpiOperators.h
 * @brief Declaration of MPI reduction operators and related functions.
 *
 * This header file provides the declaration of SCOUT-specific MPI reduction
 * operators and related functions.
 */
/*-------------------------------------------------------------------------*/

namespace scout
{

//--- Global variables ------------------------------------------------------

extern MPI_Op MINMAX_TIMEVEC2;
extern MPI_Op MAX_TOPSEVEREARRAY;
#ifndef MOST_SEVERE_MAX
  extern MPI_Op SUM_TOPSEVERE;
#else
  extern MPI_Op MAXSUM_TOPSEVERECOLL;
#endif

//--- Related functions -----------------------------------------------------

void InitOperators();
void FreeOperators();

}   // namespace scout


#endif   // !SCOUT_MPIOPERATORS_H
