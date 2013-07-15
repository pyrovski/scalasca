/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "BlockAllocator.h"
#include "OmpSmallObjAllocator.h"

using namespace std;
using namespace pearl;


//--- Static member variables -----------------------------------------------

OmpSmallObjAllocator::thread_alloc_container OmpSmallObjAllocator::m_thread_allocators;
omp_lock_t OmpSmallObjAllocator::m_thread_lock;
int OmpSmallObjAllocator::m_num_threads = 0;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpSmallObjAllocator
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

OmpSmallObjAllocator::OmpSmallObjAllocator()
{
  // Create allocator for master thread
  m_thread_allocators.push_back(new alloc_container);
  m_num_threads = 1;

  // Create thread lock
  omp_init_lock(&m_thread_lock);
}


OmpSmallObjAllocator::~OmpSmallObjAllocator()
{
  // Release allocators
  thread_alloc_container::iterator tit = m_thread_allocators.begin();
  while (tit != m_thread_allocators.end())
  {
    alloc_container::iterator it = (*tit)->begin();
    while (it != (*tit)->end()) {
      delete it->second;
      ++it;
    }

    delete *tit;
    ++tit;
  }

#if 0 // OpenMP runtime system may already be shut down, making lock operations unsafe
  // Destroy thread lock
  omp_destroy_lock(&m_thread_lock);
#endif
}


//--- Memory allocation routines --------------------------------------------

void* OmpSmallObjAllocator::alloc(std::size_t objectSize) const throw(bad_alloc)
{
  int num_threads = omp_get_num_threads();

  // Check whether all thread-local allocators exist
  if (m_num_threads < num_threads)
  {
    bool error = false;

    // We need to catch 'bad_alloc' exceptions that might be thrown by
    // the push_back() and operator new() below and re-throw them outside
    // the critical section.
    omp_set_lock(&m_thread_lock);
    try {
      // For performance, the first check was not protected by a critical
      // section. However, we should double-check here before touching the
      // container object.
      while (m_num_threads < num_threads) {
        m_thread_allocators.push_back(new alloc_container);
        ++m_num_threads;
      }
    }
    catch (bad_alloc&) {
      error = true;
    }
    omp_unset_lock(&m_thread_lock);

    // Re-throw exception
    if (error)
      throw bad_alloc();
  }

  // Retrieve thread-local allocator
  int tid = omp_get_thread_num();
  omp_set_lock(&m_thread_lock);
  alloc_container* container = m_thread_allocators[tid];
  omp_unset_lock(&m_thread_lock);

  return SmallObjAllocator::alloc(*container, objectSize);
}


void OmpSmallObjAllocator::dealloc(void*       deadObject,
                                   std::size_t objectSize) const
{
  int tid = omp_get_thread_num();

  // Retrieve thread-local allocator
  omp_set_lock(&m_thread_lock);
  assert(static_cast<size_t>(tid) < m_num_threads);
  alloc_container* container = m_thread_allocators[tid];
  omp_unset_lock(&m_thread_lock);

  // At this point, we know that the thread-local allocator is in place
  SmallObjAllocator::dealloc(*container, deadObject, objectSize);
}
