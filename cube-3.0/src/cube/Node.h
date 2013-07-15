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
* \file Node.h
* \brief Describes a node of a machine.
*/

#ifndef CUBE_NODE_H
#define CUBE_NODE_H   


#include <iosfwd>

#include "Sysres.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Node 
 *
 *----------------------------------------------------------------------------
 */


namespace cube {

  class Machine;
  class Process;

/**
* Machine containts nodes, nodes contains processes, process contains threads. Heterogen treelike structure.
*/
  class Node : public Sysres {

  public:
    
    Node(const std::string& name, Machine* mach, uint32_t id=0, uint32_t sysid=0)
      :  Sysres((Sysres*)mach, name, id, sysid) {}

/**
* Returns i-th process on this node.
*/    
    Process* get_child(unsigned int i) const {
      return (Process*) Vertex::get_child(i);
    } 
/**
* Get a machine 
*/
    Machine* get_parent() const {
      return (Machine*) Vertex::get_parent();
    }
    void writeXML(std::ostream& out) const; ///< Writes a xml-description of this node.

  };
  
  inline bool operator== (const Node& a, const Node& b) {
    return a.get_name() == b.get_name();
  }
  
}

#endif
