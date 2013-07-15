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
* \file Machine.cpp
* \brief Defines a function for xml-representation of the machine.
********************************************/


#include <iostream>

#include "Machine.h"
#include "Node.h"
#include "services.h"

using namespace std;
using namespace cube;
using namespace services;


void Machine::writeXML(std::ostream& out) const {

  out << indent() << "    <machine Id=\"" << get_id() << "\">" << endl; 
  out << indent() << "      <name>" << escapeToXML(get_name()) << "</name>" << endl;
  if (!get_desc().empty() || get_desc() != "") out << indent() << "      <descr>" << escapeToXML(get_desc()) << "</descr>" << endl;
  for( unsigned int i = 0; i < num_children(); i++ ) {
    const Node* node = get_child(i);
    node->writeXML(out);
  }
  out << indent() << "    </machine>" << endl;

}

