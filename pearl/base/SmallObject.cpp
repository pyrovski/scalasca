/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>

#include "SmallObject.h"
#include "SmallObjAllocator.h"

using namespace std;
using namespace pearl;


namespace
{

//--- Constants -------------------------------------------------------------

// Maximum object size that is handled by custom memory management
const size_t MAX_OBJECT_SIZE = 64;

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class SmallObject
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

SmallObject::~SmallObject()
{
}


//--- Memory allocation operators -------------------------------------------

void* SmallObject::operator new(std::size_t objectSize) throw(bad_alloc)
{
  // Let default memory management handle "extreme" cases
  if (objectSize == 0 || objectSize > MAX_OBJECT_SIZE)
    return ::operator new(objectSize);

  // Use custom allocator for small objects
  return SmallObjAllocator::instance()->alloc(objectSize);
}


void SmallObject::operator delete(void* deadObject, std::size_t objectSize)
{
  // Ignore NULL pointers
  if (deadObject == NULL)
    return;

  // Let default memory management handle "extreme" cases
  if (objectSize == 0 || objectSize > MAX_OBJECT_SIZE) {
    ::operator delete(deadObject);
    return;
  }

  // Use custom allocator for small objects
  SmallObjAllocator::instance()->dealloc(deadObject, objectSize);
}
