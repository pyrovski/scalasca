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
* \file cube_error.cpp
* \brief Define the output functions for every kind of error.
*/
#include "cube_error.h"

using namespace std;
using namespace cube;

namespace cube {

 ostream& operator <<(ostream& out, Error& exception) {
   out << exception.get_msg() << endl;
   return out;
 }

 ostream& operator <<(ostream& out, RuntimeError& exception) {
   //out << "CUBE RuntimeError: " << exception.get_msg() << endl;
   out << exception.get_msg() << endl;
   return out;
 }

 ostream& operator <<(ostream& out, FatalError& exception) {
   out << exception.get_msg() << endl;
   return out;
 }

}
