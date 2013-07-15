/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_BUFFER_H
#define PEARL_BUFFER_H


#include <cstddef>

#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Buffer.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Buffer.
 *
 * This header file provides the declaration of the class Buffer.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Event;
class GlobalDefs;
class RemoteEvent;


/*-------------------------------------------------------------------------*/
/**
 * @class   Buffer
 * @ingroup PEARL_base
 * @brief   Generic memory buffer implementation.
 *
 * The Buffer class provides a generic implementation of a memory buffer
 * which is able to adjust its size dynamically. It can be used, for
 * example, to pass event data between several processes or threads. To
 * allow its use in heterogeneous environments, this class transparently
 * handles the problem of different byte orders.
 */
/*-------------------------------------------------------------------------*/

class Buffer
{
  public:
    /// @name Constructors & destructor
    /// @{

    Buffer(std::size_t capacity=512);
    Buffer(const Buffer& rhs);
    virtual ~Buffer();

    /// @}
    /// @name Assignment operator
    /// @{

    Buffer& operator =(const Buffer& rhs);

    /// @}
    /// @name Accessing buffer properties
    /// @{

    std::size_t capacity() const;
    std::size_t size() const;

    /// @}
    /// @name Storing data values
    /// @{

    void put_id    (const ident_t&     id);
    void put_time  (const timestamp_t& timestamp);
    void put_uint8 (const uint8_t&     value);
    void put_uint32(const uint32_t&    value);
    void put_uint64(const uint64_t&    value);
    void put_double(const double&      value);
    void put_event (const Event&       event);
    void put_buffer(const Buffer&      buffer);

    /// @}
    /// @name Retrieving data values
    /// @{

    ident_t     get_id();
    timestamp_t get_time();
    uint8_t     get_uint8();
    uint32_t    get_uint32();
    uint64_t    get_uint64();
    double      get_double();
    RemoteEvent get_event(const GlobalDefs& defs);
    Buffer*     get_buffer();

    /// @}


  protected:
    /// Current buffer capacity
    std::size_t m_capacity;

    /// Current usage of buffer
    std::size_t m_size;

    /// Current read position
    std::size_t m_position;

    /// Pointer to the actual data
    uint8_t* m_data;


  private:
    /// @name Internal read/write routines
    /// @{

    template<typename T> void put_value(const T& value);
    template<typename T> void get_value(      T& value);

    /// @}
    /// @name Internal buffer handling
    /// @{

    void reserve(std::size_t bytes);

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_BUFFER_H */
