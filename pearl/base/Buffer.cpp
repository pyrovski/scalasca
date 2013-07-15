/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>
#include <cstring>

#include <elg_defs.h>

#include "Buffer.h"
#include "Event.h"
#include "RemoteEvent.h"


using namespace std;
using namespace pearl;


//--- Utility functions -----------------------------------------------------

namespace
{

void swap_bytes(void* buffer, size_t length)
{
  if (length > 1) {
    uint8_t* data = static_cast<uint8_t*>(buffer);

    for (size_t i = 0; i < length / 2; ++i) {
      char tmp = data[i];
      data[i] = data[length - 1 - i];
      data[length - 1 - i] = tmp;
    }
  }
}

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class Buffer
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new buffer instance with the given capacity, which has to be
 * at least 1 byte (default: 512).
 *
 * @param capacity Initial capacity
 */
Buffer::Buffer(std::size_t capacity)
  : m_capacity(capacity),
    m_size(1),
    m_position(1)
{
  assert(capacity > 0);

  // Allocate data & store byte order
  m_data = new uint8_t[m_capacity];
  m_data[0] = ELG_BYTE_ORDER;
}


/**
 * Copy constructor. Creates a new instance and initializes its data with
 * the contents of the given buffer @a rhs (deep copy).
 *
 * @param rhs Source operand
 */
Buffer::Buffer(const Buffer& rhs)
  : m_data(NULL)
{
  *this = rhs;
}


/**
 * Destructor. Destroys the instance and releases all occupied resources.
 */
Buffer::~Buffer()
{
  delete[] m_data;
}


//--- Assignment operator ---------------------------------------------------

/**
 * Assignment operator. Creates a deep copy of the given argument @a rhs.
 *
 * @param rhs Source operand
 *
 * @return Reference to the instance
 */
Buffer& Buffer::operator =(const Buffer& rhs)
{
  // Check for self-assignment
  if (this == &rhs)
    return *this;

  // Release memory
  delete[] m_data;

  // Copy data
  m_capacity = rhs.m_capacity;
  m_size     = rhs.m_size;
  m_position = rhs.m_position;
  m_data     = new uint8_t[m_capacity];
  memcpy(m_data, rhs.m_data, m_size);

  return *this;
}


//--- Accessing buffer properties -------------------------------------------

/**
 * Returns the current capacity of the buffer instance in bytes.
 *
 * @return Buffer capacity
 */
size_t Buffer::capacity() const
{
  return m_capacity;
}


/**
 * Returns the current usage of the buffer instance in bytes.
 *
 * @return Buffer usage
 */
size_t Buffer::size() const
{
  return m_size;
}


//--- Storing data values ---------------------------------------------------

/**
 * Appends the given @a id to the buffer. If not enough buffer space is
 * available, the buffer will be automatically resized.
 *
 * @param id Identifier to be appended
 */
void Buffer::put_id(const ident_t& id)
{
  put_value(id);
}


/**
 * Appends the given @a timestamp to the buffer. If not enough buffer space
 * is available, the buffer will be automatically resized.
 *
 * @param timestamp Timestamp to be appended
 */
void Buffer::put_time(const timestamp_t& timestamp)
{
  put_value(timestamp);
}


/**
 * Appends the given unsigned 8-bit integer @a value to the buffer. If not
 * enough buffer space is available, the buffer will be automatically resized.
 *
 * @param value Value to be appended
 */
void Buffer::put_uint8(const uint8_t& value)
{
  put_value(value);
}


/**
 * Appends the given unsigned 32-bit integer @a value to the buffer. If not
 * enough buffer space is available, the buffer will be automatically resized.
 *
 * @param value Value to be appended
 */
void Buffer::put_uint32(const uint32_t& value)
{
  put_value(value);
}


/**
 * Appends the given unsigned 64-bit integer @a value to the buffer. If not
 * enough buffer space is available, the buffer will be automatically resized.
 *
 * @param value Value to be appended
 */
void Buffer::put_uint64(const uint64_t& value)
{
  put_value(value);
}


/**
 * Appends the given double-precision floating-point @a value to the buffer.
 * If not enough buffer space is available, the buffer will be automatically
 * resized.
 *
 * @param value Value to be appended
 */
void Buffer::put_double(const double& value)
{
  put_value(value);
}


/**
 * Appends the data of the given @a event to the buffer. If not enough
 * buffer space is available, the buffer will be automatically resized.
 *
 * @param event Event to be written to the buffer
 */
void Buffer::put_event(const Event& event)
{
  event.pack(*this);
}

/**
 * Appends the contents of the given @a buffer to this buffer. If not enough
 * buffer space is available, the buffer will be automatically resized.
 *
 * @param buffer Buffer to be appended to the current buffer
 */
void Buffer::put_buffer(const Buffer& buffer)
{
  // self-assignment currently not allowed
  assert(m_data != buffer.m_data);

  put_value(buffer.m_size);

  // Increase capacity if necessary
  reserve(buffer.m_size);

  memcpy(m_data + m_size, buffer.m_data, buffer.m_size);
  m_size += buffer.m_size;
}


//--- Retrieving data values ------------------------------------------------

/**
 * Returns the identifier stored in the buffer at the current read position.
 *
 * @return Identifier
 */
ident_t Buffer::get_id()
{
  ident_t id;
  get_value(id);
  return id;
}


/**
 * Returns the timestamp stored in the buffer at the current read position.
 *
 * @return Timestamp
 */
timestamp_t Buffer::get_time()
{
  timestamp_t time;
  get_value(time);
  return time;
}


/**
 * Returns the 8-bit unsigned integer value stored in the buffer at the
 * current read position.
 *
 * @return 8-bit unsigned integer value
 */
uint8_t Buffer::get_uint8()
{
  uint8_t value;
  get_value(value);
  return value;
}

/**
 * Returns the 32-bit unsigned integer value stored in the buffer at the
 * current read position.
 *
 * @return 32-bit unsigned integer value
 */
uint32_t Buffer::get_uint32()
{
  uint32_t value;
  get_value(value);
  return value;
}

/**
 * Returns the 64-bit unsigned integer value stored in the buffer at the
 * current read position.
 *
 * @return 64-bit unsigned integer value
 */
uint64_t Buffer::get_uint64()
{
  uint64_t value;
  get_value(value);
  return value;
}


/**
 * Returns the double-precision floating-point value store in the buffer at
 * the current read position.
 *
 * @return Double-precision floating-point value
 */
double Buffer::get_double()
{
  double value;
  get_value(value);
  return value;
}


/**
 * Returns a new instance of RemoteEvent, initialized with the data stored
 * in the buffer at the current read position.
 *
 * @param defs %GlobalDefs instance providing the corresponding global
 *             definition data.
 * @return %RemoteEvent instance
 */
RemoteEvent Buffer::get_event(const GlobalDefs& defs)
{
  return RemoteEvent(defs, *this);
}


/**
 * Returns a pointer to a new buffer, initialized with data of an embedded
 * buffer at the current read position. The caller is responsible for 
 * @em delete'ing the retrieved buffer object.
 *
 * @return Pointer to new Buffer instance.
 */
Buffer* Buffer::get_buffer()
{
  size_t size = 0;

  get_value(size);

  assert(size > 0);
  assert(m_size - m_position >= size);

  Buffer* buffer = new Buffer(size);

  buffer->m_size = size;
  memcpy(buffer->m_data, m_data + m_position, size);
  m_position    += size;

  return buffer;
}


//--- Private methods -------------------------------------------------------

/**
 * Appends the given @a value to the buffer. If not enough buffer space is
 * available, the buffer will be automatically resized.
 *
 * @param value Value that should be appended
 */
template<typename T>
void Buffer::put_value(const T& value)
{
  // If necessary, resize buffer
  reserve(sizeof(T));

  // Append value
  memcpy(m_data + m_size, &value, sizeof(T));
  m_size += sizeof(T);
}


/**
 * Reads an item from the buffer at the current read position and stores it
 * in the given argument @a value.
 *
 * @param value Reference to result location
 */
template<typename T>
void Buffer::get_value(T& value)
{
  assert(m_size - m_position >= sizeof(T));

  // Copy value
  memcpy(&value, m_data + m_position, sizeof(T));
  m_position += sizeof(T);

  // Adjust byte order
  if (m_data[0] != ELG_BYTE_ORDER)
    swap_bytes(&value, sizeof(T));
}


/**
 * Increases buffer capacity to be able to contain at least @a nbytes more
 * bytes of data.
 *
 * @param nbytes Number of bytes to reserve in addition to current size
 */
void Buffer::reserve(std::size_t nbytes)
{
  if (m_capacity - m_size < nbytes) {
    // Calculate new capacity
    do {
      m_capacity *= 2;
    } while (m_capacity - m_size < nbytes);

    // Allocate new buffer memory and copy data
    uint8_t* tmp = new uint8_t[m_capacity];
    memcpy(tmp, m_data, m_size);
    delete[] m_data;
    m_data = tmp;
  }
}
