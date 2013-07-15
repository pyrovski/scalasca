/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "MemoryChunk.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MemoryChunk
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

MemoryChunk::MemoryChunk(std::size_t objectSize)
  : m_avail(NUM_ENTRIES),
    m_first(0)
{
  // Allocate chunk of memory
  m_data = new unsigned char[NUM_ENTRIES * objectSize];

  // Initialize each entry with index of next unused entry
  unsigned char* entry = m_data;
  for (unsigned int index = 1; index < NUM_ENTRIES; ++index) {
    *entry = static_cast<unsigned char>(index);
    entry += objectSize;
  }
}


MemoryChunk::~MemoryChunk()
{
  // Release memory
  delete[] m_data;
}


//--- Memory allocation routines --------------------------------------------

void* MemoryChunk::alloc(std::size_t objectSize)
{
  // Calculate pointer to first unused entry
  unsigned char* entry = m_data + (m_first * objectSize);

  // Bookkeeping
  m_first = *entry;
  --m_avail;

  return entry;
}


void  MemoryChunk::dealloc(void* deadObject, std::size_t objectSize)
{
  unsigned char* entry = static_cast<unsigned char*>(deadObject);

  // Bookkeeping
  *entry  = m_first;
  m_first = static_cast<unsigned char>((entry - m_data) / objectSize);
  ++m_avail;
}
