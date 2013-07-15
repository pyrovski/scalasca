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
*
*	\file Vertex.cpp
*	\brief Defines a general element of a tree. Many elements (metrics, machine, calltree) in CUBE have a treelike sturkture. 
*
********************************************/

#include "cube_error.h"
#include "Vertex.h"

using namespace std;
using namespace cube;


Vertex::Vertex(Vertex* parent, uint32_t id)
  : IdentObject(id), parent(parent) {
  if (parent != NULL) {
    parent->add_child(this);
  }
}

int Vertex::get_level() const {

  if ( parent == NULL )
     return 0;
  else
     return parent->get_level() + 1;
}

Vertex* Vertex::get_child(unsigned int i) const {

  if (i >= childv.size()) {
    throw RuntimeError("Vertex::get_child(i): out of range");
  }
  return childv[i];
}

void Vertex::set_parent(Vertex* vertex) { 

  if (vertex == NULL) return;
  parent = vertex; 
  parent->add_child(this); 
}
