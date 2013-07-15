/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Topology.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Topology
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor (protected) ---------------------------------

/**
 * Creates a new instance and initializes its identifier with the given
 * value @a id.
 *
 * @param id Topology identifier
 */
Topology::Topology(ident_t id)
  : IdObject(id)
{
}
