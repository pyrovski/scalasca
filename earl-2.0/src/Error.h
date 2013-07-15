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

#ifndef EARL_ERROR_H
#define EARL_ERROR_H

/*
 *----------------------------------------------------------------------------
 *
 * class Error 
 *
 *----------------------------------------------------------------------------
 */

#include <string>

namespace earl 
{
  class Error
  {
  public:
      
    Error(std::string message) : message(message) {} 
    virtual std::string get_msg() const { return "EARL: " + message; }
    virtual ~Error() {}
  
  protected:
  
    std::string message;
  };

  class RuntimeError : public Error 
  {
  public:
  
    RuntimeError(std::string message) : Error(message) {} 
  };

  class FatalError : public Error
  {
  public:
  
    FatalError(std::string message) : Error(message) {} 
    virtual std::string get_msg() const { return "EARL Fatal: " + message; } 
  };
}
#endif
