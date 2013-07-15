/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_MPIDATATYPES_H
#define SCOUT_MPIDATATYPES_H


#include <mpi.h>

/*-------------------------------------------------------------------------*/
/**
 * @file  MpiDatatypes.h
 * @brief Declaration of MPI datatypes and related functions.
 *
 * This header file provides the declaration of SCOUT-specific MPI datatypes
 * and related functions.
 */
/*-------------------------------------------------------------------------*/

namespace scout
{

//--- Global variables ------------------------------------------------------

extern MPI_Datatype TIMEVEC2;
extern MPI_Datatype TOPSEVERE;
extern MPI_Datatype TOPSEVEREARRAY;
#ifdef MOST_SEVERE_MAX
  extern MPI_Datatype TOPSEVERECOLL;
#endif

//--- Related functions -----------------------------------------------------

void InitDatatypes();
void FreeDatatypes();


}   // namespace scout


#endif   // !SCOUT_MPIDATATYPES_H
