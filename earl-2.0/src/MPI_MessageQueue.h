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

#ifndef EARL_MPI_MESSAGEQUEUE_H
#define EARL_MPI_MESSAGEQUEUE_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPI_MessageQueue
 *
 *----------------------------------------------------------------------------
 */

#include <map>
#include <vector>

#include "Event.h"

namespace earl
{
  class Communicator;
  class Location;

  class MPI_MessageQueue
  {
  public:
     
    virtual ~MPI_MessageQueue(){};
    void   messages(std::vector<Event>& out, Location* src, Location* dest); 
    void   queue_msg(Event send);   
    void   dequeue_msg(long pos);
    Event  get_match_msg(Location* src, Location* dest, long tag, Communicator* com) const;
   
  private:
  
    std::map<long, Event> sendm; 
  };
}
#endif






