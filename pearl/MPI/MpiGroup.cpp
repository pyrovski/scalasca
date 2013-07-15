/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MpiGroup.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiGroup
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes the data with the given values
 * @a id, @a process_ids, @a is_self and @a is_world, respectively.
 *
 * @param id          Group identifier
 * @param process_ids Ordered list of global process identifiers defining
 *                    the group
 * @param is_self     Indicates whether this group refers to 'SELF'
 * @param is_world    Indicates whether this group refers to 'WORLD'
 */
MpiGroup::MpiGroup(ident_t               id,
                   const container_type& process_ids,
                   bool                  is_self,
                   bool                  is_world)
  : Group(id, process_ids, is_self, is_world),
    m_group(MPI_GROUP_NULL)
{
  // MPI group handle will be created on first access by get_group() method.
}


/**
 * Destructor. Destroys the instance.
 */
MpiGroup::~MpiGroup()
{
  // Free MPI resources
  if (MPI_GROUP_NULL != m_group && MPI_GROUP_EMPTY != m_group)
    MPI_Group_free(&m_group);
}


//--- Get group information -------------------------------------------------

/**
 * Returns the MPI group handle associated with this group object. The MPI
 * group is lazily created on first access and then cached until the instance
 * is destructed.
 * 
 * @return MPI group handle
 */
MPI_Group MpiGroup::get_group() 
{
  // Lazy generation of MPI group handle at first access
  if (MPI_GROUP_NULL == m_group)
    m_group = create_group();

  return m_group;
}


//--- Private methods -------------------------------------------------------

/**
 * Creates an MPI group consisting of the processes corresponding to the
 * global process identifiers stored in this Group object and returns its
 * handle.
 *
 * @return MPI group handle
 */
MPI_Group MpiGroup::create_group() const
{
  MPI_Group result;

  // Special case 1: GROUP_EMPTY
  if (0 == m_process_ids.size()) {
    result = MPI_GROUP_EMPTY;
  }

  // Special case 2: GROUP_SELF
  else if (is_self()) {
    MPI_Comm_group(MPI_COMM_SELF, &result);
  }

  // Special case 3: GROUP_WORLD
  else if (is_world()) {
    MPI_Comm_group(MPI_COMM_WORLD, &result);
  }

  // Arbitrary group
  else {
    // Create ranks array
    uint32_t size  = m_process_ids.size();
    int*     ranks = new int[size];
    for (uint32_t rank = 0; rank < size; ++rank)
      ranks[rank] = get_global_rank(rank);

    // Retrieve world group
    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    // Create group based on world group
    MPI_Group_incl(world_group, size, ranks, &result);

    // Release resources
    MPI_Group_free(&world_group);
    delete[] ranks;
  }

  return result;
}
