/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>

#include <elg_error.h>

#include "BlockAllocator.h"
#include "MemoryChunk.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class BlockAllocator
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

BlockAllocator::BlockAllocator(std::size_t objectSize)
  : m_objectSize(objectSize),
    m_allocCache(NULL),
    m_deallocCache(0),
    m_unused(0)
{
}


BlockAllocator::~BlockAllocator()
{
  // Release memory chunks
  chunk_container::iterator it = m_chunks.begin();
  while (it != m_chunks.end()) {
    if (!(*it)->isEmpty())
      elg_error_msg("Memory leak detected!");

    delete *it;

    ++it;
  }
}


//--- Memory allocation routines --------------------------------------------

void* BlockAllocator::alloc()
{
  if (!m_allocCache || m_allocCache->isFull()) {
    if (m_unused > 0) {
      // Search a chunk with unused entries
      chunk_container::reverse_iterator it = m_chunks.rbegin();
      while (it != m_chunks.rend()) {
        if (!(*it)->isFull()) {
          m_allocCache = *it;
          break;
        }

        ++it;
      }
      assert(it != m_chunks.rend());
    } else {
      // All chunks full ==> create a new one
      m_allocCache = new MemoryChunk(m_objectSize);
      m_chunks.push_back(m_allocCache);

      m_unused += MemoryChunk::capacity();
    }
  }

  // Adjust unused object entry counter
  --m_unused;

  // Retrieve memory for a new object
  return m_allocCache->alloc(m_objectSize);
}


void BlockAllocator::dealloc(void* deadObject)
{
  assert(!m_chunks.empty());

  if (!m_chunks[m_deallocCache]->isInside(deadObject, m_objectSize)) {
    // Search correct chunk for object
    chunk_container::reverse_iterator it = m_chunks.rbegin();
    while (it != m_chunks.rend()) {
      if ((*it)->isInside(deadObject, m_objectSize)) {
        // See "S. Meyers: Effective STL, Item 28" why we need (it + 1).base()
        m_deallocCache = (it + 1).base() - m_chunks.begin();
        break;
      }

      ++it;
    }
    assert(it != m_chunks.rend());
  }

  // Release memory for object
  MemoryChunk*& chunk = m_chunks[m_deallocCache];
  chunk->dealloc(deadObject, m_objectSize);
  ++m_unused;

  // If chunk is now empty, check whether we can free some memory
  if (chunk->isEmpty() && m_chunks.size() > 1) {
    // Empty chunks are always at the end of the list...
    MemoryChunk*& lastChunk = m_chunks.back();

    if (chunk != lastChunk) {
      // If last chunk is already empty, discard it
      if (lastChunk->isEmpty()) {
        // If necessary, let allocation cache point to remaining empty chunk
        if (m_allocCache == lastChunk)
          m_allocCache = chunk;

        delete lastChunk;
        m_chunks.pop_back();

        // Adjust unused object entry counter
        m_unused -= MemoryChunk::capacity();
      }

      // Move empty chunk to the end of the list
      swap(m_chunks.back(), chunk);
    }

    // If we assume a LIFO deallocation strategy, the last non-empty
    // chunk seems appropriate for the dealloc cache
    m_deallocCache = 0;
    if (m_chunks.size() > 2)
      m_deallocCache = m_chunks.size() - 2;
  }
}
