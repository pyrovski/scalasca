/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Error.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Error
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets the error description to the given @a
 * message.
 *
 * @param message %Error description
 */
Error::Error(const string& message)
  : m_message(message)
{
}


/**
 * Destructor. Destroys the instance.
 */
Error::~Error() throw()
{
}


//--- Get error message -----------------------------------------------------

/**
 * Returns a description of the error condition encountered.
 *
 * @return %Error description
 */
const char* Error::what() const throw()
{
  return m_message.c_str();
}


/*
 *---------------------------------------------------------------------------
 *
 * class RuntimeError
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets the error description to the given @a
 * message.
 *
 * @param message %Error description
 */
RuntimeError::RuntimeError(const string& message)
  : Error("PEARL: " + message)
{
}


/*
 *---------------------------------------------------------------------------
 *
 * class FatalError
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets the error description to the given @a
 * message.
 *
 * @param message %Error description
 */
FatalError::FatalError(const string& message)
  : Error("PEARL Fatal: " + message)
{
}


/*
 *---------------------------------------------------------------------------
 *
 * class MemoryError
 *
 *---------------------------------------------------------------------------
 */

//--- Get error message -----------------------------------------------------

/**
 * Returns a description of the error condition encountered.
 *
 * @return %Error description
 */
const char* MemoryError::what() const throw()
{
  return "Not enough memory!";
}


//--- Out-of-memory handler -------------------------------------------------

namespace pearl
{

/**
 * @internal
 *
 * Custom new handler for the PEARL library. Throws an MemoryError exception.
 */
void PEARL_new_handler()
{
  throw MemoryError();
}

}   /* namespace pearl */
