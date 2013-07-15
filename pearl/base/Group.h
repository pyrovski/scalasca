/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_GROUP_H
#define PEARL_GROUP_H


#include <vector>

#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Group.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Group.
 *
 * This header file provides the declaration of the class Group.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   Group
 * @ingroup PEARL_base
 * @brief   Defines a group of processes by means of global process
 *          identifiers.
 *
 * The class Group provides a way to group processes and to identify such
 * process groups using an unique identifier. This is accomplished by
 * storing an ordered set of global process identifiers (global ranks).
 * Due to the ordering, each process in the group is implicitly assigned
 * a local identifier (local rank) in the range [0,@#members-1] with
 * respect to this group. Group objects are used to define MPI groups
 * (see MpiGroup) and and referenced by MPI communicator objects (see
 * Comm and MpiComm).
 * 
 * The numerical identifiers of the individual groups are globally
 * defined and continuously enumerated, i.e., the ID is element of
 * [0,@#groups-1].
 */
/*-------------------------------------------------------------------------*/

class Group : public IdObject
{
  public:
    /// Container type for process identifiers
    typedef std::vector<ident_t> container_type;


    /// @name Constructors & destructor
    /// @{

    Group(ident_t               id,
          const container_type& process_ids,
          bool                  is_self,
          bool                  is_world);

    /// @}
    /// @name Get group information
    /// @{

    uint32_t num_ranks() const;

    ident_t  get_local_rank (ident_t global_rank) const;
    ident_t  get_global_rank(ident_t local_rank) const;

    bool is_self() const;
    bool is_world() const;

    /// @}


  protected:
    /// Ordered list of global process identifiers defining this group
    container_type m_process_ids;

    /// Group flags (SELF, WORLD, etc.)
    unsigned char m_flags;
};


}   /* namespace pearl */


#endif   /* !PEARL_GROUP_H */
