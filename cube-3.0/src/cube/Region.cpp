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
*	\file Region.cpp
*	\brief  Defines a xml-representation of a region in a .cube file.
********************************************/

#include <iostream>

#include "Region.h"
#include "services.h"

using namespace std;
using namespace cube;
using namespace services;


void Region::add_cnode(Cnode* cnode)  {
  for (unsigned int i = 0; i < cnodev.size(); i++) {
    if (cnode == cnodev[i]) return;
  }
  cnodev.push_back(cnode);
}

void Region::writeXML(ostream& out) const {
  out << "    <region id=\"" << get_id() << "\" " << "mod=\"" 
      << escapeToXML(get_mod())  << "\" "  << "begin=\"" << get_begn_ln() << "\" " 
      << "end=\"" << get_end_ln() <<  "\">" << endl;
  out << "      <name>"
      << escapeToXML(get_name())
      << "</name>" << endl;
  out << "      <url>" << escapeToXML(get_url()) << "</url>" << endl;
  out << "      <descr>" 
      << escapeToXML(get_descr())
      << "</descr>" << endl;
  out << "    </region>" << endl;

}

namespace cube {
  bool operator== (const Region& a, const Region& b) {
    return a.get_name() == b.get_name() && a.get_mod() == b.get_mod();
  }
}
