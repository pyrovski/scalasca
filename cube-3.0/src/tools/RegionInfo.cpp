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
* \file RegionInfo.cpp
* \brief Provides a class with an information about all callers for every 
region in a cube.s
*/


#include "Cube.h"
#include "CnodeInfo.h"
#include "RegionInfo.h"
#include "Region.h"
#include "helper.h"

namespace cube
{
  using namespace std;
/**
* Creates an object with region information and sets for all related callees 
a type (COM, MPI or OMP).
*/
  CRegionInfo::CRegionInfo(const Cube* cube)
    : InfoObj(cube->get_regv().size()),
      m_cube(cube)
  {
    const vector<Region*>& regions(cube->get_regv());
    CCnodeInfo   cninfo(cube);
    size_t rcnt = regions.size();

    for (size_t i=0; i<rcnt; i++)
      {
	Region* region = regions[i];
	size_t regionId = region->get_id();
	CallpathType& typeref(m_types[regionId]);

        if (region->get_descr() == "MPI") {
          typeref = MPI;
          continue;
        }
        if (region->get_descr() == "OMP") {
          typeref = OMP;
          continue;
        }
        if (region->get_descr() == "EPIK") {
          typeref = EPK;
          continue;
        }

	const vector<Cnode*>& cnodev = region->get_cnodev();
	for (size_t j=0; j<cnodev.size(); j++)
	  {
	    Cnode* cnode = cnodev[j];
	    size_t cnodeId = cnode->get_id();
	    CallpathType cnodeType(cninfo[cnodeId]);
	    if (typeref == COM)
              break;
	    else
              typeref = cnodeType;
	  }
      }
  }
}  

