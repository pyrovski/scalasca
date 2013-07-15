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
*  \file CnodeInfo.cpp
*  \brief Defines a CNodoInfo class.
*/


#include "CnodeInfo.h"
#include "Cube.h"
#include "Cnode.h"
#include "Region.h"

using namespace std;

namespace cube
{
  CCnodeInfo::CCnodeInfo(const Cube* cube)
    : InfoObj(cube->get_cnodev().size()),
      m_cnodes(cube->get_cnodev()),
      m_cube(cube)
  {
    size_t ccnt(m_cnodes.size());
    for (size_t i=0; i<ccnt; i++)
      {
	/* don't forget to iterate from back to front
	   or the algorithm may not work */
	classify_cnode(m_cnodes[ccnt-i-1]);
      }
  }

  const vector<Cnode*>& CCnodeInfo::get_cnodev()
  {
    return m_cnodes;
  }
/**
* Marks for "cnode", whether it is a general COM or MPI (OMP).
*/
  void CCnodeInfo::classify_cnode(Cnode* cnode)
  {
    size_t nodeId=cnode->get_id();
    CallpathType& typeref(m_types[nodeId]);
    if (typeref == COM)
      return;
    else
      {
	const Region* region = cnode->get_callee();
	if (region->get_descr() == "MPI")
	  {
	    typeref=MPI;
	    propagate_com(cnode);
	    return;
	  }
        else if (region->get_descr() == "OMP")
          {
            typeref=OMP;
            propagate_com(cnode);
            return;
          } 
      }
  }
/**
* Set type of cnode for all parents of "cnode" as "COM".
*/
  void CCnodeInfo::propagate_com(Cnode* cnode)
  {
    Cnode* parent = cnode->get_parent();
    while (parent)
      {
	size_t parentId = parent->get_id();
	m_types[parentId]=COM;

        parent = parent->get_parent();
      }
  }
}
