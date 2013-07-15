/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2006-2013                                                **
**  TU Dresden, Zentrum fuer Informationsdienste und Hochleistungsrechnen  **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_LIST_EVENT_H
#define PEARL_LIST_EVENT_H


#include <iosfwd>
#include <iterator>
#include <string>

#include "Event_rep.h"
#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Event.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Event.
 *
 * This header file provides the declaration of the class Event.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Buffer;
class CNode;
class LocalTrace;
class Location;


/*-------------------------------------------------------------------------*/
/**
 * @class   Event
 * @ingroup PEARL_base
 * @brief   Generic representation for local events with iterator
 *          functionality.
 */
/*-------------------------------------------------------------------------*/

class Event : public std::iterator<std::bidirectional_iterator_tag, Event>
{
  public:
    /// @name Construcor

    Event();

    /// @name Comparison operators

    bool operator==(const Event& rhs) const;
    bool operator!=(const Event& rhs) const;

    bool operator<(const Event& rhs) const;
    bool operator>(const Event& rhs) const;

    /// @}
    /// @name Forward iterator interface
    /// @{

    Event& operator++();
    Event  operator++(int);

    /// @}
    /// @name Backward iterator interface
    /// @{

    Event& operator--();
    Event  operator--(int);

    /// @}
    /// @name Check iterator validity
    /// @{

    bool is_valid() const;

    /// @}
    /// @name Check identity
    /// @{

    uint32_t get_id() const { return m_index; }

    /// @}
    /// @name Access related events
    /// @{

    Event next() const { return ++Event(*this); }
    Event prev() const { return --Event(*this); }

    Event enterptr() const;
    Event exitptr() const;

    Event request() const;
    Event completion() const;

    Event next_request_event() const;
    Event prev_request_event() const;

    /// @}
    /// @name Access local event information
    /// @{

    CNode*    get_cnode() const;
    Location* get_location() const;

    /// @}
    /// @name Access event representation
    /// @{

    Event_rep& operator*() const;
    Event_rep* operator->() const;

    /// @}


  private:
    /// Associated local event trace
    const LocalTrace* m_trace;

    /// Index of current event
    uint32_t m_index;


    /* Private methods */
    Event(const LocalTrace* trace, bool isEnd=false);
    Event(const LocalTrace* trace, uint32_t index);

    void pack(Buffer& buffer) const;
    std::ostream& output(std::ostream& stream) const;


    /* Declare friends */
    friend class Buffer;
    friend class LocalTrace;

    friend std::ostream& operator<<(std::ostream& stream, const Event& event);
};


//--- Related functions -----------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const Event& event);

std::string event_typestr(event_t type);


}   /* namespace pearl */


#endif   /* !PEARL_LIST_EVENT_H */
