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
*     \file Process.cpp
*     \brief Defines a method to save a xml-representation of a process.
********************************************/

#include <iostream>

#include "Process.h"
#include "Thread.h"
#include "services.h"

using namespace std;
using namespace cube;
using namespace services;


void Process::writeXML(ostream& out) const {

  out << indent() << "    <process Id=\"" << get_id() << "\">" << endl; 
  out << indent() << "      <name>" << escapeToXML(get_name()) << "</name>" << endl;
  out << indent() << "      <rank>" << get_rank() << "</rank>" << endl;
  for( unsigned int i = 0; i < num_children(); i++ ) {
    const Thread* thrd = get_child(i);
    thrd->writeXML(out);
  }
  out << indent() << "    </process>" << endl;
}
