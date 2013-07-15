/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIGROUP_H
#define PEARL_MPIGROUP_H


#include <mpi.h>

#include "Group.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    MpiGroup.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiGroup.
 *
 * This header file provides the declaration of the class MpiGroup.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   MpiGroup
 * @ingroup PEARL_mpi
 * @brief   Stores information related to an MPI group.
 *
 * Instances of the MpiGroup class provide information about MPI
 * groups. In addition to the functionality of the base class
 * Group, they also provide a "real" MPI group resembling the one
 * used in the target application.
 */
/*-------------------------------------------------------------------------*/

class MpiGroup : public Group
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiGroup(ident_t               id,
             const container_type& process_ids,
             bool                  is_self,
             bool                  is_world);
    virtual ~MpiGroup();

    /// @}
    /// @name Get group information
    /// @{

    MPI_Group get_group();

    /// @}


  private:
    /// Corresponding MPI group
    MPI_Group m_group;


    /* Private methods */
    MPI_Group create_group() const;


    /* Declare friends */
    friend class MpiComm;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPICOMM_H */
