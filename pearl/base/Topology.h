/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_TOPOLOGY_H
#define PEARL_TOPOLOGY_H


#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Topology.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Topology.
 *
 * This header file provides the declaration of the class Topology.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   Topology
 * @ingroup PEARL_base
 * @brief   Base class for virtual topologies.
 *
 * The class Topology serves as a base class for all classes related to
 * virtual topologies.
 *
 * The numerical identifiers of the individual topologies are globally
 * defined and continuously enumerated, i.e., the ID is element of
 * [0,@#topologies-1].
 */
/*-------------------------------------------------------------------------*/

class Topology : public IdObject
{
  protected:
    /// @name Constructors & destructor
    /// @{

    Topology(ident_t id);

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_TOPOLOGY_H */
