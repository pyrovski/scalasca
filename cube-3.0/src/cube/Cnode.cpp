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
* \file Cnode.cpp
* \brief Defines the methods to handle with the elements of a call tree.
*/
/*******************************************
     Cnode.cpp

********************************************/

#include <iostream>

#include "Cnode.h"
#include "Region.h"
#include "services.h"

using namespace std;
using namespace cube;
using namespace services;
/**

* Created an element of a call tree. 
*
* As a friend class of "Region" is might call "add_excl_cnode".
*/

Cnode::Cnode(Region* callee, const std::string& mod, int line, Cnode* parent, uint32_t id) : 
  Vertex(parent, id), callee(callee), mod(mod), line(line) 
{ 
  //Vertex::set_parent(parent);
  callee->add_excl_cnode(this);
  Cnode* cur_parent = parent;
  bool is_recursive = false;
  while ( cur_parent != NULL ) {
    if ( cur_parent->get_callee() == callee ) {
      is_recursive = true;
      break;
    }
    cur_parent = cur_parent->get_parent();
  }
  if ( is_recursive == false )
    callee->add_incl_cnode(this);
}

Region* Cnode::get_caller() const { 
  if (get_parent() == NULL) return NULL; 
  return get_parent()->get_callee();
}

/**
* \details Writes with deep search algorithm.
*/
void Cnode::writeXML(ostream& out) const {

  out << indent() << "    <cnode id=\"" << get_id() << "\" ";
  if (get_line() != -1) {
      out << "line=\"" << get_line() << "\" "; 
  }
  if (get_mod() != "") {
    out << "mod=\"" << escapeToXML(get_mod()) << "\" ";
  }
  out << "calleeId=\"" << get_callee()->get_id() << "\">" << endl;
  for( unsigned int i = 0; i < num_children() ; i++ ) {
    Cnode* child = get_child(i);
    child->writeXML(out);
  }

  out << indent() << "    </cnode>\n";

}

namespace cube {
  bool operator== (const Cnode& a, const Cnode& b) {
    if ( a.get_mod() != b.get_mod() ||
         !(*a.get_callee() == *b.get_callee()) ||
         a.get_line() != b.get_line())
      return false;

    Cnode* pa = a.get_parent();
    Cnode* pb = b.get_parent();
    if (pa != NULL && pb != NULL)
      return *pa == *pb;
    else
      return pa == pb;
  }
}


