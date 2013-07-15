/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiComm.h"
#include "MpiGroup.h"
#include "Process.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiComm
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes the data with the given values
 * @a id and @a group, respectively.
 *
 * @param id    Communicator identifier
 * @param group Associated MPI group object
 */
MpiComm::MpiComm(ident_t id, MpiGroup* group)
  : Comm(id, group),
    m_communicator(MPI_COMM_NULL)
{
  MPI_Group group_handle;

  // Create communicator
  group_handle = group->create_group();
  MPI_Comm_create(MPI_COMM_WORLD, group_handle, &m_communicator);
  MPI_Group_free(&group_handle);
}

/**
 * Copy constructor. Duplicates an MPI communicator using
 * MPI_Comm_dup. It is 'protected' to prevent accidential
 * invocation; users need to call MpiComm::duplicate() explicitely to
 * create a communicator copy.
 *
 * @param comm  MPI communicator to duplicate
 */

MpiComm::MpiComm(const MpiComm& comm)
  : Comm(comm.get_id(), comm.get_group()),
    m_communicator(MPI_COMM_NULL)
{
  MPI_Comm_dup(comm.m_communicator, &m_communicator);
}


/**
 * Destructor. Destroys the instance.
 */
MpiComm::~MpiComm()
{
  // Free resources
  if (m_communicator != MPI_COMM_NULL)
    MPI_Comm_free(&m_communicator);
}


//--- Get communicator information ------------------------------------------

/**
 * Returns the MPI communicator handle associated with this communicator
 * object.
 * 
 * @return MPI communicator handle
 */
MPI_Comm MpiComm::get_comm() const
{
  return m_communicator;
}


//--- Communicator duplication ----------------------------------------------

/**
 * Returns a new MpiComm object that is a duplicate of the current
 * communicator. Note that this is a collective operation on the
 * communicator's group. The user is responsible for free'ing the
 * returned object.
 * 
 * @return duplicate MpiComm object
 */

MpiComm* MpiComm::duplicate() const 
{
  return new MpiComm(*this);
}
