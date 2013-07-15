/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPSMALLOBJALLOCATOR_H
#define PEARL_OMPSMALLOBJALLOCATOR_H


#include <vector>

#include <omp.h>

#include "SmallObjAllocator.h"


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpSmallObjAllocator
 * @ingroup PEARL_omp
 * @brief   Custom memory management class for small objects in
 *          multi-threaded applications.
 */
/*-------------------------------------------------------------------------*/

class OmpSmallObjAllocator : public SmallObjAllocator
{
  public:
    /// @name Constructors & destructor
    /// @{

    OmpSmallObjAllocator();
    virtual ~OmpSmallObjAllocator();

    /// @}
    /// @name Memory allocation routines
    /// @{

    virtual void* alloc  (std::size_t objectSize) const throw(std::bad_alloc);
    virtual void  dealloc(void* deadObject, std::size_t objectSize) const;

    /// @}


  private:
    /// Container type for storing thread-local allocators
    typedef std::vector<alloc_container*> thread_alloc_container;


    /// Allocator containers for each thread
    static thread_alloc_container m_thread_allocators;

    /// Maximum number of threads seen
    static int m_num_threads;

    /// OpenMP lock to protect allocator containers
    static omp_lock_t m_thread_lock;
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPSMALLOBJALLOCATOR_H */
