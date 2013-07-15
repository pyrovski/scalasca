/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_REMOTEEVENT_H
#define PEARL_REMOTEEVENT_H


#include <iosfwd>

#include "CountedPtr.h"
#include "Event_rep.h"
#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    RemoteEvent.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class RemoteEvent.
 *
 * This header file provides the declaration of the class RemoteEvent.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Buffer;
class CNode;
class GlobalDefs;
class Location;


/*-------------------------------------------------------------------------*/
/**
 * @class   RemoteEvent
 * @ingroup PEARL_base
 * @brief   Generic representation for remote events without iterator
 *          functionality.
 */
/*-------------------------------------------------------------------------*/

class RemoteEvent
{
  public:
    /// @name Constructors & destructor
    /// @{

    RemoteEvent(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Access remote event information
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
    /// Reference-counted event representation
    CountedPtr<Event_rep> m_event;

    /// %Event location
    Location* m_location;

    /// Associated call tree node
    CNode* m_cnode;


    /* Private methods */
    std::ostream& output(std::ostream& stream) const;


    /* Declare friends */
    friend std::ostream& operator<<(std::ostream&      stream,
                                    const RemoteEvent& event);
};


//--- Related functions -----------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const RemoteEvent& event);


}   /* namespace pearl */


#endif   /* !PEARL_REMOTEEVENT_H */
