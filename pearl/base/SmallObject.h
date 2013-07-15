/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_SMALLOBJECT_H
#define PEARL_SMALLOBJECT_H


#include <new>


/*-------------------------------------------------------------------------*/
/**
 * @file    SmallObject.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class SmallObject.
 *
 * This header file provides the declaration of the class SmallObject.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   SmallObject
 * @ingroup PEARL_base
 * @brief   Provides a base class for small objects using a custom memory
 *          management.
 *
 * Each dynamically allocated memory block usually requires some extra
 * memory for bookkeeping purposes. However, this can be quite space
 * inefficient if a large number of small objects is allocated, as the
 * per-object overhead is significant.
 *
 * The SmallObject class serves as a base class that can be used for these
 * kind of small objects, providing a customized memory management that
 * allocates memory in larger chunks rather than for each object individually.
 * This specialized memory management is restricted to objects of not more
 * than 64 bytes, however, if objects grow larger (e.g., objects of a derived
 * class) the default memory allocation routines are used transparently.
 */
/*-------------------------------------------------------------------------*/

class SmallObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~SmallObject();

    /// @}
    /// @name Memory allocation operators
    /// @{

    static void* operator new(std::size_t objectSize) throw(std::bad_alloc);
    static void  operator delete(void* deadObject, std::size_t objectSize);

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_SMALLOBJECT_H */
