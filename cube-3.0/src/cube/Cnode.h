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
* \file Cnode.h
* \brief Provides a class with an element of call tree.
*/
#ifndef CUBE_CNODE_H
#define CUBE_CNODE_H


#include <iosfwd>

#include "Vertex.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Cnode
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  class Region;
  /**
* An element of the call tree. As child of general element of a tree (Vertex).
*/  
  class Cnode : public Vertex {

  public:

    Cnode(Region* callee, const std::string& mod, int line, Cnode* parent, uint32_t id=0);
    
    int get_line() const { return line; } ///< Returns a start line of a region.
    std::string get_mod() const { return mod; } ///< Returns a modus of a call node.
    Region* get_callee() const { return callee; }///< Returns a region, from where entered here.
    Region* get_caller() const;
    Cnode* get_parent() const {
      return static_cast<Cnode*>(Vertex::get_parent());///< Get a call three from where enetered.
    } 
    Cnode* get_child(unsigned int i) const {
      return static_cast<Cnode*>(Vertex::get_child(i)); ///< Get a further calls of from this call.
    } 
    
    void writeXML(std::ostream& out) const;  ///< Writes a xml-representation of the call node.
    
 private:
  
    Region* callee;///< Region in a source code, where the call node was entered from.
    std::string mod;///< Modus.
    int line;///< Start line in a region.

  };
  
  bool operator== (const Cnode& a, const Cnode& b);
  
}

#endif
