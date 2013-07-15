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
*	\file Thread.cpp
*	\brief Defines a xml-output of a thread description in a .cube file.
*
********************************************/

#include <iostream>

#include "Thread.h"
#include "services.h"
using namespace std;
using namespace cube;
using namespace services;


void Thread::writeXML(ostream& out) const {

  out << indent() << "    <thread Id=\"" << get_id() << "\">" << endl; 
  out << indent() << "      <name>" << escapeToXML(get_name()) << "</name>\n";
  out << indent() << "      <rank>" << get_rank() << "</rank>\n";
  out << indent() << "    </thread>\n";
}
