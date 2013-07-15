/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <algorithm>
#include <cstdlib>
#include <new>

#include "Buffer.h"

using namespace silas;

//--- Constructors & destructor ---------------------------------------------

/**
 * Create a buffer with a certain capacity
 * @param capacity Required minimal capacity of the buffer
 */
Buffer::Buffer(std::size_t capacity)
{
    if (capacity <= 0)
    {
        m_buffer = NULL;
        m_size   = 0;
    }
    else
    {
        m_buffer = new char[capacity];
        m_size = capacity;

        // zero buffer
        std::fill_n(m_buffer, m_size, 0);
    }
}

/**
 * Free allocated memory
 */
Buffer::~Buffer()
{
    delete[] m_buffer;
}

//--- Querying the buffer ---------------------------------------------------

/**
 * Return the current buffer address
 * @return Pointer to the allocated buffer space
 */
void *Buffer::get_buffer()
{
    return m_buffer;
}

/**
 * Return the pointer to a buffer of at least 'required_capacity' size
 * @param required_capacity Minimal capacity of the buffer that is requested
 * @return Pointer to the allocated buffer space
 */
void *Buffer::get_buffer(std::size_t required_capacity)
{
    if (required_capacity > m_size)
    {
        delete[] m_buffer;

        m_buffer = new char[required_capacity];
        m_size   = required_capacity;

        // zero buffer
        std::fill_n(m_buffer, m_size, 0);
    }
    
    return m_buffer;
}

/**
 * Return the current buffer size
 * @return Size of of currently allocated buffer
 */
std::size_t Buffer::get_size()
{
    return m_size;
}
