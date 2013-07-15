/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <algorithm>
#include <cassert>

#include "Error.h"
#include "Group.h"

using namespace std;
using namespace pearl;


//--- Constants -------------------------------------------------------------

static unsigned char GROUP_FLAG_SELF  = 1;
static unsigned char GROUP_FLAG_WORLD = 2;


/*
 *---------------------------------------------------------------------------
 *
 * class Group
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes the data with the given values
 * @a id, @a process_ids, @a is_self and @a is_world, respectively. The
 * flags @a is_self and @a is_world are mutually exclusive, except for
 * single-process experiments.
 *
 * @param id          Group identifier
 * @param is_self     Indicates whether this group refers to 'SELF'
 * @param is_world    Indicates whether this group refers to 'WORLD'
 * @param process_ids Ordered list of global process identifiers defining
 *                    the group
 */
Group::Group(ident_t               id,
             const container_type& process_ids,
             bool                  is_self,
             bool                  is_world)
  : IdObject(id),
    m_process_ids(process_ids),
    m_flags(0)
{
  assert((is_self && is_world) || !process_ids.empty());

  // Set group flags
  if (is_self)
    m_flags |= GROUP_FLAG_SELF;
  if (is_world)
    m_flags |= GROUP_FLAG_WORLD;
}


//--- Get group information -------------------------------------------------

/**
 * Returns the number of processes belonging to this group.
 *
 * @return Number of processes
 */
uint32_t Group::num_ranks() const
{
  return m_process_ids.size();
}


/**
 * Returns the local rank with respect to this group of the process with
 * the given global process identifier @a global_rank.
 *
 * @param global_rank Global process identifier
 *
 * @return Local rank
 *
 * @exception RuntimeError if the process with @a global_rank is not part of
 *                         this group ("Unknown identifier")
 */
ident_t Group::get_local_rank(ident_t global_rank) const
{
  // Special case: for WORLD groups, there is a 1:1 rank mapping
  if (is_world()) {
    if (global_rank >= m_process_ids.size())
      throw RuntimeError("Group::get_local_rank(ident_t) -- Unknown identifier.");

    return global_rank;
  }

  // Search process identifier. This has to be a linear search since members
  // are not ordered by their global process IDs.
  container_type::const_iterator it;
  it = find(m_process_ids.begin(), m_process_ids.end(), global_rank);

  // Found => Return local rank
  if (it != m_process_ids.end())
    return it - m_process_ids.begin();

  // Not found...
  throw RuntimeError("Group::get_local_rank(ident_t) -- Unknown identifier.");
}


/**
 * Returns the global process identifier of the process with the given
 * @a local_rank with respect to this group.
 *
 * @param local_rank Local rank
 *
 * @return Global process identifier
 *
 * @exception RuntimeError if an invalid @a local_rank is given ("Rank out
 *                         of range")
 */
ident_t Group::get_global_rank(ident_t local_rank) const
{
  if (local_rank >= m_process_ids.size())
    throw RuntimeError("Group::get_global_rank(ident_t) -- Rank out of range.");

  return m_process_ids[local_rank];
}


/**
 * Returns whether this group is a 'SELF' group.
 *
 * @return Flag indicating 'SELF' status.
 */
bool Group::is_self() const
{
  return (m_flags & GROUP_FLAG_SELF);
}


/**
 * Returns whether this group is a 'WORLD' group.
 *
 * @return Flag indicating 'WORLD' status.
 */
bool Group::is_world() const
{
  return (m_flags & GROUP_FLAG_WORLD);
}
