/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
**  Small assistant to include virtual topologies into an existing cube    **
****************************************************************************/

#include "Cartesian.h"
#include "Cube.h"
#include "Process.h"
#include "Thread.h"

using namespace std;
using namespace cube;


void write_cube(Cube * inCube);
void create_topology(Cube * inCube);
void rename_topology(Cube * inCube);
void rename_dimensions(Cube * inCube);
void show_topologies(std::vector<Cartesian *> topologies);
