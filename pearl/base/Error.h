/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_ERROR_H
#define PEARL_ERROR_H


#include <exception>
#include <new>
#include <string>


/*-------------------------------------------------------------------------*/
/**
 * @file    Error.h
 * @ingroup PEARL_base
 * @brief   Declaration of the exception classes Error, RuntimeError, and
 *          FatalError.
 *
 * This header file provides the declaration of the three exception classes
 * Error, RuntimeError, and FatalError that are used by the PEARL library.
 */
/*-------------------------------------------------------------------------*/


namespace pearl 
{

/*-------------------------------------------------------------------------*/
/**
 * @class   Error
 * @ingroup PEARL_base
 * @brief   Base class for exceptions related to the PEARL library.
 *
 * The class Error serves as a base class for all exceptions related to
 * the PEARL library. It provides a method to obtain an error message,
 * which in most cases will deliver the name of the internal operation
 * that failed.
 */
/*-------------------------------------------------------------------------*/

class Error : public std::exception
{
  public:
    /// @name Constructors & destructor
    /// @{

    Error(const std::string& message);
    virtual ~Error() throw();

    /// @}
    /// @name Get error message
    /// @{

    virtual const char* what() const throw();

    /// @}


  protected:
    /// %Error message
    std::string m_message;
};


/*-------------------------------------------------------------------------*/
/**
 * @class   RuntimeError
 * @ingroup PEARL_base
 * @brief   Exception class related to the PEARL library for recoverable
 *          errors.
 *
 * The class RuntimeError provides an exception class related to the PEARL
 * library. Runtime errors are thrown whenever a method is used the wrong
 * way, for example by supplying undefined parameters. In this case, the
 * operation has failed, but no data has been corrupted so that the
 * application might be able to recover from this error.
 */
/*-------------------------------------------------------------------------*/

class RuntimeError : public Error 
{
  public:
    /// @name Constructors & destructor
    /// @{

    RuntimeError(const std::string& message);

    /// @}
};


/*-------------------------------------------------------------------------*/
/**
 * @class   FatalError
 * @ingroup PEARL_base
 * @brief   Exception class related to the PEARL library for unrecoverable
 *          errors.
 *
 * The class FatalError provides an exception class related to the PEARL
 * library. Fatal errors are thrown for severe error conditions that make
 * it impossible to continue the execution of the application. This is
 * usually the case if data has been corrupted in some way.
 */
/*-------------------------------------------------------------------------*/

class FatalError : public Error
{
  public:
    /// @name Constructors & destructor
    /// @{

    FatalError(const std::string& message);

    /// @}
};


/*-------------------------------------------------------------------------*/
/**
 * @class   MemoryError
 * @ingroup PEARL_base
 * @brief   Exception class related to the PEARL library that is used to
 *          signal out of memory situations.
 *
 * The class MemoryError provides an exception class related to the PEARL
 * library. Memory errors are thrown whenever there is not enough memory
 * available to fulfill a memory allocation request (unless you have
 * registered a custom new handler after calling PEARL_init() or
 * PEARL_mpi_init()).
 */
/*-------------------------------------------------------------------------*/

class MemoryError : public std::bad_alloc
{
  public:
    /// @name Get error message
    /// @{

    virtual const char* what() const throw();

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_ERROR_H */
