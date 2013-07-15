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
* \file Process.h
* \brief Describes a node of a process.
*/
#ifndef CUBE_PROCESS_H
#define CUBE_PROCESS_H   


#include <iosfwd>

#include "Sysres.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Process
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  class Node;
  class Thread;
/**
* Machine containts nodes, nodes contains processes, process contains threads. Heterogen treelike structure.
*/
  class Process : public Sysres {
    
  public:

    Process(const std::string& name, int rank, Node* node, uint32_t id=0, uint32_t sysid=0)
      : Sysres((Sysres*)node, name, id, sysid), rank(rank) {}
    
    int get_rank() const { return rank; } ///< Returns a rank of the process.

/**
* Get i-th Thread of this process.
*/
    Thread* get_child(unsigned int i) const {
      return (Thread*) Vertex::get_child(i);
    }
/**
* On what node does this Process run?
*/
    Node* get_parent() const {
      return (Node*) Vertex::get_parent();
    }
    void writeXML(std::ostream& out) const;
    
  private:
    
    int rank;

  };

  inline bool operator== (const Process& a, const Process& b) {
    return a.get_rank() == b.get_rank();
  }
  inline bool operator< (const Process& a, const Process& b) {
    return a.get_rank() < b.get_rank();
  }

}

#endif
