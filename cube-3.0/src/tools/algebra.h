/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
* \file algebra.h
* \brief Provides a set of global operations on "cube" like "difference", 
"mean" and so on.
*/
#ifndef CUBE_ALGEBRA_H
#define CUBE_ALGEBRA_H

#include <string>
#include <map>
#include "Filter.h"
#include "Metric.h"
#include "Cnode.h"
#include "Thread.h"

namespace cube {

class Cube;
class Cnode;
class Metric;
class Thread;

/* Type definition */
/**
* A general structure of the mapping of one Cube on another.
*/
typedef struct {
  std::map<Metric*, Metric*>      metm;
  std::map<Cnode*,  Cnode*>       cnodem;
  std::map<Thread*, Thread*>      sysm;
} CubeMapping;

void cube_apply_filter(Cube& newCube, Cube& rhs, Filter& filter,
  CubeMapping& mapping);
void createMapping (Cube& newCube, Cube& oldCube, CubeMapping& cubeMap, bool collapse);

bool  cube_stat  (const std::string& filename);
Cube* cube_diff  (Cube* minCube, Cube* subCube, bool subset, bool collapse);
Cube* cube_mean  (Cube* lhsCube, Cube* rhsCube, bool subset, double num, bool collapse);
Cube* cube_merge (Cube* lhsCube, Cube* rhsCube, bool subset, bool collapse);
Cube* cube_clean (Cube* lhsCube, bool subset, bool collapse);
Cube* cube_cut   (Cube* lhsCube, Cube* rhsCube, bool subset, bool collapse, 
                  const std::string& cn_root, const std::vector<std::string> cn_prunes);
bool  cube_cmp   (Cube* lhsCube, Cube* rhsCube);
Cube* cube_remap (Cube* inCube, const std::string& dirname="", bool skip_omp=false);

}   /* namespace cube */


#endif   /* !CUBE_ALGEBRA_H */
