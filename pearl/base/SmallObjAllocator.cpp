/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstdlib>

#include "BlockAllocator.h"
#include "Error.h"
#include "SmallObjAllocator.h"

using namespace std;
using namespace pearl;


//--- Static member variables -----------------------------------------------

const SmallObjAllocator* SmallObjAllocator::m_instance = NULL;
SmallObjAllocator::alloc_container SmallObjAllocator::m_allocators;


//--- Utility functions -----------------------------------------------------

namespace {

extern "C" void free_ObjAllocatorInstance()
{
  delete SmallObjAllocator::instance();
}

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class SmallObjAllocator
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

SmallObjAllocator::~SmallObjAllocator()
{
  // Release allocators
  alloc_container::iterator it = m_allocators.begin();
  while (it != m_allocators.end())
  {
    delete it->second;
    ++it;
  }
}


//--- Memory allocation routines --------------------------------------------

void* SmallObjAllocator::alloc(std::size_t objectSize) const throw(bad_alloc)
{
  // Retrieve memory for object
  return alloc(m_allocators, objectSize);
}


void SmallObjAllocator::dealloc(void* deadObject, std::size_t objectSize) const
{
  // Release memory for object
  dealloc(m_allocators, deadObject, objectSize);
}


//--- Singleton interface ---------------------------------------------------

void SmallObjAllocator::registerAllocator(const SmallObjAllocator* allocator)
{
  assert(allocator);

  if (m_instance)
    throw FatalError("SmallObjAllocator::registerAllocator(const SmallObjAllocator*) -- "
                     "Allocator already registered.");

  m_instance = allocator;

  // Register clean-up function
  atexit(free_ObjAllocatorInstance);
}


//--- Internal memory allocation routines (protected) -----------------------

void* SmallObjAllocator::alloc(alloc_container& container,
                               std::size_t      objectSize) const
                                                            throw(bad_alloc)
{
  // Search allocator for given size; create new one if necessary
  alloc_container::iterator it = container.lower_bound(objectSize);
  if (it == container.end() ||
      container.key_comp()(objectSize, it->first))
    it = container.insert(it,
                          alloc_container::value_type(objectSize,
                                                      new BlockAllocator(objectSize)));

  // Retrieve memory for object
  return it->second->alloc();
}


void SmallObjAllocator::dealloc(alloc_container& container,
                                void*            deadObject,
                                std::size_t      objectSize) const
{
  // Search allocator for given size
  alloc_container::iterator it = container.find(objectSize);

  // If allocator was not found, we are in trouble...
  assert(it != container.end());

  // Release memory for object
  it->second->dealloc(deadObject);
}
