/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <CNode.h>

#include "OmpPattern.h"
#include "Predicates.h"

using namespace std;
using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpPattern
 *
 *---------------------------------------------------------------------------
 */

//--- Data collation control ------------------------------------------------

/**
 * @brief Controls collation of data for the given call-tree node.
 *
 * This method controls the collation of severity data for the given call-tree
 * node @a cnode. The method returns @em true for all non-OpenMP call paths
 * since it is guaranteed that no OpenMP-related severity data exists for them.
 * For all other call paths, it returns @em false.
 *
 * @param  cnode  Current call-tree node
 * @return Returns @em true if @p cnode is an OpenMP call path, @em false
 *         otherwise
 */
bool OmpPattern::skip_cnode(const pearl::CNode& cnode) const
{
  return !is_omp_api(cnode.get_region());
}
