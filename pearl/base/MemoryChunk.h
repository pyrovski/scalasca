/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MEMORYCHUNK_H
#define PEARL_MEMORYCHUNK_H


#include <cstddef>


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MemoryChunk.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class MemoryChunk.
 *
 * This header file provides the declaration of the class MemoryChunk.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MemoryChunk
 * @ingroup PEARL_base
 * @brief   Manages a chunk of memory used to store a set of objects of a
 *          fixed size.
 */
/*-------------------------------------------------------------------------*/

class MemoryChunk
{
  public:
    /// @name Constructors & destructor
    /// @{

    MemoryChunk(std::size_t objectSize);
    ~MemoryChunk();

    /// @}
    /// @name Memory allocation routines
    /// @{

    void* alloc(std::size_t objectSize);
    void  dealloc(void* deadObject, std::size_t objectSize);

    /// @}
    /// @name Query functions
    /// @{

    bool isEmpty() const { return m_avail == NUM_ENTRIES; }
    bool isFull()  const { return m_avail == 0; }

    bool isInside(void* objectPtr, std::size_t objectSize) const
    {
      unsigned char* tmp = static_cast<unsigned char*>(objectPtr);

      return (tmp >= m_data &&
              tmp <  m_data + (NUM_ENTRIES * objectSize));
    }

    static unsigned short capacity() { return NUM_ENTRIES; }

    /// @}


  private:
    /// Number of objects a single memory chunk handles
    enum {
      NUM_ENTRIES = 256
    };


    /// Pointer to the actual memory
    unsigned char* m_data;

    /// Number of unused entries
    unsigned short m_avail;

    /// Index of first unused entry
    unsigned char m_first;
};


}   /* namespace pearl */


#endif   /* !PEARL_MEMORYCHUNK_H */
