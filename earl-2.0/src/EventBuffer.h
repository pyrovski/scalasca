/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EARL_EVENTBUFFER_H
#define EARL_EVENTBUFFER_H

/*
 *----------------------------------------------------------------------------
 *
 * class EventBuffer
 *
 *----------------------------------------------------------------------------
 */

#include <map> 

#include "Event.h"
  
namespace earl 
{
  class EventBuffer
  {
  public:

    ~EventBuffer();

    Event retrieve(long pos);
  
  private:

    friend class Event_rep;

    void insert(Event_rep* event);
    void remove(long pos);
  
    std::map<long, Event_rep*> buffer;
  };
}
#endif




