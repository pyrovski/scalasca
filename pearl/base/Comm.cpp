/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Comm.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Comm
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes the data with the given values
 * @a id and @a group, respectively.
 *
 * @param id    Communicator identifier
 * @param group Associated group object
 */
Comm::Comm(ident_t id, Group* group)
  : IdObject(id),
    m_group(group)
{
}


//--- Get communicator information ------------------------------------------

/**
 * Returns the associated Group object.
 *
 * @return Group object
 */
Group* Comm::get_group() const
{
  return m_group;
}
