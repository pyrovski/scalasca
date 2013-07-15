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

#include <algorithm>

#include "EventBuffer.h"
#include "Event_rep.h"
#include "Error.h"

using namespace earl;
using namespace std;

EventBuffer::~EventBuffer()
{
  map<long, Event_rep*>::const_iterator cit = buffer.begin();
  while ( cit != buffer.end() )
    {
      cit->second->detach_buffer();
      cit ++;
    }
}

void 
EventBuffer::insert(Event_rep* event) 
{
  map<long, Event_rep*>::iterator it;

  it = buffer.find(event->get_pos());
  if (it == buffer.end()) 
    buffer[event->get_pos()] = event;
  else 
    throw FatalError("Error in EventBuffer::insert(Event_rep*): event already present in buffer.");
  
}

void 
EventBuffer::remove(long pos)
{
  map<long, Event_rep*>::iterator it;

  it = buffer.find(pos);
  if (it == buffer.end())
    throw FatalError("Error in EventBuffer::remove(long).");
  else 
    buffer.erase(it);
}

Event 
EventBuffer::retrieve(long pos)
{
  map<long, Event_rep*>::const_iterator it;

  it = buffer.find(pos);
  if (it == buffer.end())
    return NULL;
  else
    return it->second;
}













