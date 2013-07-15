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
* \file RegionInfo.h
* \brief Extends a general class InfoObj to get ability to carry a cube.
*/

#ifndef CUBE_REGIONINFO
#define CUBE_REGIONINFO

#include "enums.h"
#include "InfoObj.h"

namespace cube

{
  class Cube;

  class CRegionInfo : public InfoObj
  {
  private:
    const Cube*  m_cube;

  public:
    CRegionInfo(const Cube*);
  };
}
#endif
