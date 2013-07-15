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

#include "SyncQueue.h"

#include "Location.h"

using namespace earl;
using namespace std;

void      
SyncQueue::queue_sync(Event sync)
{
  syncm[sync.get_lock_id()] = sync;
} 

Event 
SyncQueue::last_sync(long lkid)
{
  map<long, Event>::const_iterator it = 
    syncm.find(lkid);
   
   if (it != syncm.end())
     return it->second;
   else
     return NULL;
}

