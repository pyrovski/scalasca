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
* \file CnodeInfo.h
* \brief Extends a InfoObj class to be able to carry information about call tree.
*/


#ifndef CUBE_CNODEINFO
#define CUBE_CNODEINFO

#include <vector>

#include "InfoObj.h"

namespace cube
{
  class Cube;
  class InfoObj;
  class Cnode;

  using std::vector;

/**
* Extension of InfoObj.
*/
  class CCnodeInfo : public InfoObj
  {
  private:
    const vector<Cnode*>& m_cnodes;///< A set of call nodes.
    const Cube* m_cube;///< CUBE, where the information is saved.
  public:
    CCnodeInfo(const Cube*); ///< A constructor.
    const vector<Cnode*>& get_cnodev();///< Just gives a corresponding "cnode" for this CNodeInfo object.
  private:
    void classify_cnode(Cnode*);
    void propagate_com(Cnode*);
  };
}
  
#endif
