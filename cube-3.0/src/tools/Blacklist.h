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
* \file Blacklist.h
* \brief Declares and provides a class to keep blacklistet Regions together.
*/
#ifndef CUBE_CBLACKLIST
#define CUBE_CBLACKLIST

#include <string>
#include <vector>

#include "helper.h"

namespace cube
{
  class Cube;
  class Region;
  class CRegionInfo;
  class CBlacklist
  {
  private:
    Cube* m_cube;
    std::vector<unsigned long> vBlacklist;
    const CRegionInfo* m_reginfo;
    void init(std::string);
    void initUSR();
    bool good;
    
  public:

    /* construct from file with or without CRegionInfo
       errors about filtered EPK/MPI/OMP or COM regions are
       only possible with a CRegionInfo */
    CBlacklist(Cube*, std::string);
    CBlacklist(Cube*, std::string, const CRegionInfo*);
    /* construct with all USR regions */
    CBlacklist(Cube*, const CRegionInfo*);

    /* query whether a certain region is on the list */
    bool operator() (size_t);     /* by region id */
    bool operator() (std::string);/* by region name */
    bool operator() (Region*);    /* by region's address in memory */

    /* return id of some region from the list */
    size_t operator[] (size_t) const;
    size_t size() { return vBlacklist.size(); };
    void set_regioninfo(CRegionInfo*);
  };
}

#endif
