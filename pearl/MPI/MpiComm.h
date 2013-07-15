/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPICOMM_H
#define PEARL_MPICOMM_H


#include <mpi.h>

#include "Comm.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    MpiComm.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiComm.
 *
 * This header file provides the declaration of the class MpiComm.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class MpiGroup;


/*-------------------------------------------------------------------------*/
/**
 * @class   MpiComm
 * @ingroup PEARL_mpi
 * @brief   Stores information related to an MPI communicator.
 *
 * Instances of the MpiComm class provide information about MPI
 * communicators. In addition to the functionality of the base class
 * Comm, they also provide a "real" MPI communicator resembling the one
 * used in the target application.
 */
/*-------------------------------------------------------------------------*/

class MpiComm : public Comm
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiComm(ident_t id, MpiGroup* group);
    virtual ~MpiComm();

    /// @}
    /// @name Get communicator information
    /// @{

    MPI_Comm get_comm() const;

    /// @}
    /// @name Communicator duplication
    /// @{

    MpiComm* duplicate() const;

    /// @}
  

  protected:

    /// copy constructor
    MpiComm(const MpiComm& comm);

  private:

    /// Corresponding MPI communicator
    MPI_Comm m_communicator;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPICOMM_H */
