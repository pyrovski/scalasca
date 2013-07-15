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
* \file Vertex.h
* \brief Provides an element of a tree. 
*/
#ifndef CUBE_VERTEX_H
#define CUBE_VERTEX_H   


#include <string>
#include <vector>

#include "IdentObject.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Vertex 
 *
 *----------------------------------------------------------------------------
 */

namespace cube {
/**
* Every element of a tree has an ID. Therefore is Vertex a kind of IdenObject.
*/
  class Vertex : public IdentObject {
    
  public:

    Vertex(Vertex* parent, uint32_t id=0);
    Vertex(uint32_t id=0) : IdentObject(id) { parent = NULL; }
    
    std::string indent() const { return std::string(2 * get_level(), ' '); }///< By printing is a proper level of indent
    
    unsigned int num_children() const { return childv.size(); } ///< How many undertrees ?
    Vertex* get_parent() const { return parent; }///< Root element?
    Vertex* get_child(unsigned int i) const;  
    int get_level() const; /// Recursiv calculation of a level in the tree hierarchy

  protected:
    
    void set_parent(Vertex* vertex); 
    void add_child(Vertex* vertex)  { childv.push_back(vertex); } 

  private:
    
    std::vector<Vertex*> childv;
    Vertex* parent;

  };
}

#endif
