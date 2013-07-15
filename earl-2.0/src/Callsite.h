/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EARL_CALLSITE_H
#define EARL_CALLSITE_H

/*
 *----------------------------------------------------------------------------
 *
 * class Callsite
 *
 *----------------------------------------------------------------------------
 */

#include <string>

#include "Region.h"

namespace earl 
{
  class Callsite
  {
  public:

    Callsite(long        id,  
	     std::string file,
	     long        line,
	     Region*     callee) :
      id(id), file(file), line(line), callee(callee) {}

    long        get_id()     const { return id; } 
    std::string get_file()   const { return file; } 
    long        get_line()   const { return line; } 
    Region*     get_callee() const { return callee; }
  
  private:
  
    long        id;  
    std::string file;
    long        line;
    Region*     callee;
  };
}
#endif


