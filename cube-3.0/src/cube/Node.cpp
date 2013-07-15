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

	\file  Node.cpp
	\brief  Defines a method to save a xml-representation of a node.

********************************************/

#include <iostream>

#include "Node.h"
#include "Process.h"
#include "services.h"
using namespace std;
using namespace cube;
using namespace services;


void Node::writeXML(ostream& out) const {

  out << indent() << "    <node Id=\"" << get_id() << "\">" << endl; 
  out << indent() << "      <name>" << escapeToXML(get_name()) << "</name>\n";
  for( unsigned int i = 0; i < num_children() ; i++ ) {
    const Process* p = get_child(i);
    p->writeXML(out);
  }
  out << indent() << "    </node>\n";
}

