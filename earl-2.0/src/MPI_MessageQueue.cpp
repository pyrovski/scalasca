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

#include "MPI_MessageQueue.h"

#include <algorithm>

#include "Error.h"
#include "Location.h"

using namespace earl;
using namespace std;

void
MPI_MessageQueue::messages(vector<Event>& out, Location* src, Location* dest) 
{
  map<long, Event>::iterator first = sendm.begin();
  map<long, Event>::iterator last = sendm.end();

  if ( src == NULL && dest == NULL ) 
    {
      while (first != last)
	{
	  out.push_back(first->second);
	  first++;
	}
    }
  else if ( src == NULL )
    {
      while (first != last)
	{
	  if ( first->second.get_dest() == dest )
	    out.push_back(first->second);
	  first++;
	}
    }
  else if ( dest == NULL )
    {
      while (first != last)
	{
	  if ( first->second.get_loc() == src )
	      out.push_back(first->second);
	  first++;
	}
    }
  else
    {
      while ( first != last )
	{
	  if ( first->second.get_loc()  == src && first->second.get_dest() == dest )
	    out.push_back(first->second);
	  first++;
	}
    }
}

void 
MPI_MessageQueue::queue_msg(Event send) 
{
  sendm[send.get_pos()] = send;
}

void 
MPI_MessageQueue::dequeue_msg(long pos) 
{
  map<long, Event>::iterator it = sendm.find(pos);

  if (it == sendm.end())
    throw FatalError("Error in MPI_MessageQueue::dequeue_msg(long).");
  sendm.erase(it);
}


Event 
MPI_MessageQueue::get_match_msg(Location* src, Location* dest, long tag, Communicator* com) const 
{
  map<long, Event>::const_iterator first = sendm.begin();
  map<long, Event>::const_iterator last  = sendm.end();

  while (first != last)
    {
      if (first->second.get_loc()  == src  && 
	  first->second.get_dest() == dest &&
	  first->second.get_tag()  == tag  &&
	  first->second.get_com()  == com)
	{
	  return first->second;
	}
      first++;
    }
  throw FatalError("Error in MPI_MessageQueue::get_match_msg(Location*, Location*, long, Communicator*).\n>>>>>> Trace file has probably incorrectly synchronized time stamps.");
}














