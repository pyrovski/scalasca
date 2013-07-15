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
/** 
* \file enums.h
* \brief Provides a callpath type as an enumeration. 
*/ 


#ifndef CUBE_ENUMS
#define CUBE_ENUMS

namespace cube
{
/**
* Defines a set of different kinds of callpath type.
*/
  enum CallpathType
    {
      MPI, 		///< MPI-related
      OMP, 		///< OMP-related
      USR, 		///< User defined code
      COM, 		///< Common USR+MPI/OMP
      EPK,		///< EPIK-related
      NUL, 		///< NULL ?
      SEN, 		///< Send
      REC, 		///< Receive
      SENREC, 		///< Sendrecv
      COL, 		///< Collective communication
      FORK, 		///< OpenMP fork
      CRIT,		///< OpenMP critical section
      LOCK,		///< OpenMP lock
      MPI_RMA_OP,       ///< MPI-2 RMA Op
      MPI_RMA_GATS,     ///< MPI-2 RMA General Active Target Sync
      MPI_RMA_COLL,     ///< MPI-2 RMA Collectives
      MPI_RMA_LOCK,     ///< MPI-2 RMA Lock
      MPI_RMA_UNLOCK    ///< MPI-2 RMA Unlock
    };
}

#endif
