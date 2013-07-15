/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_BLOCKALLOCATOR_H
#define PEARL_BLOCKALLOCATOR_H


#include <cstddef>
#include <vector>


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    BlockAllocator.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class BlockAllocator.
 *
 * This header file provides the declaration of the class BlockAllocator.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class MemoryChunk;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   BlockAllocator
 * @ingroup PEARL_base
 * @brief   Provides an customized memory allocator for objects of a fixed
 *          size, allocating memory in blocks.
 */
/*-------------------------------------------------------------------------*/

class BlockAllocator
{
  public:
    /// @name Constructors & destructor
    /// @{

    BlockAllocator(std::size_t objectSize);
    ~BlockAllocator();

    /// @}
    /// @name Memory allocation routines
    /// @{

    void* alloc();
    void  dealloc(void* deadObject);

    /// @}


  private:
    /// Container type for memory chunks
    typedef std::vector<MemoryChunk*> chunk_container;


    /// Size of the objects stored in the pool
    std::size_t m_objectSize;

    /// Stores memory chunks
    chunk_container m_chunks;

    /// Memory chunk used for last allocation
    MemoryChunk* m_allocCache;

    /// Index of memory chunk used for last deallocation
    std::size_t m_deallocCache;

    /// Total number of unused object entries
    std::size_t m_unused;
};


}   /* namespace pearl */


#endif   /* !PEARL_BLOCKALLOCATOR_H */
