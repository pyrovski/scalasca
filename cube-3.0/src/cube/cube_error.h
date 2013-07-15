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
* \file cube_error.h
* \brief Provides a hierarchy of errors compatible with C++ thwrow/catch mechanism
*/
#ifndef CUBE_ERROR_H
#define CUBE_ERROR_H

/*
 *----------------------------------------------------------------------------
 *
 * class Error 
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <iostream>
#include <exception>

namespace cube {

/**
* General error: gets a message.
*/
  class Error : public std::exception {
  
  public:
      
    Error(std::string message) : message(message) {} 
    std::string get_msg() const { return "CUBE: " + message; }
    ~Error() throw() {}
  

  protected:
  
    std::string message;
  };
/**
* General error: gets a message.
*/
  class RuntimeError :  public Error {
  
  public:
  
    RuntimeError(std::string message) : Error(message) {}
    
  };
/**
* General error: gets a fatal message.
*/
  class FatalError : public Error {
  
  public:
  
    FatalError(std::string message) : Error(message) {} 
    virtual std::string get_msg() const { return "CUBE Fatal: " + message; } 
  };

   std::ostream& operator<< (std::ostream& out, Error& exception);		 ///< Prints out a message about an error.
   std::ostream& operator<< (std::ostream& out, RuntimeError& exception);	 ///< Prints out a message about a runtime error.
   std::ostream& operator<< (std::ostream& out, FatalError& exception);		 ///< Prints out a message about a fatal error.
}

#endif
