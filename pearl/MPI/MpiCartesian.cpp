/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>

#include "MpiCartesian.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiCartesian
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes its identifier and communicator
 * to the given values @a id and @a communicator, respectively.
 *
 * @param id            Topology identifier
 * @param num_locations Contains the number of locations in each dimension
 * @param is_periodic   Contains boolean values defining whether the
 *                      corresponding dimension is periodic or not
 * @param communicator  Associated MPI communicator
 */
MpiCartesian::MpiCartesian(ident_t            id,
                           const cart_dims&   num_locations,
                           const cart_period& is_periodic,
                           MpiComm*           communicator,
                           const std::string  cart_name,
                           const std::vector<std::string> dim_names)
  : Cartesian(id, num_locations, is_periodic, cart_name, dim_names),
    m_communicator(communicator)
{
  assert(communicator);
}


//--- Get MPI topology information ------------------------------------------

/**
 * Returns the MPI communicator associated to this topology.
 *
 * @return Associated communicator
 */
MpiComm* MpiCartesian::get_comm() const
{
  return m_communicator;
}
