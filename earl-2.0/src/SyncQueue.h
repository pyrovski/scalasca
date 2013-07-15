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

#ifndef EARL_SYNCQUEUE_H
#define EARL_SYNCQUEUE_H

/*
 *----------------------------------------------------------------------------
 * class SyncQueue
 *---------------------------------------------------------------------------- 
 */

#include <map>

#include "Event.h"

#include "Sync_rep.h"
#include "Location.h"

#include "Sync_rep.h"
#include "MPIRmaSync_rep.h"
#include "OMPSync_rep.h"

namespace earl
{
    
  template<class T>
  class SyncQueueT
  {
  public:
    virtual ~SyncQueueT(){};
    void  queue_sync(T* sync, long loc_id);
    Event last_sync(long lkid, long loc_id);

  private:
    // (lock_id, loc_id/proc_id) |-> event (Sync_rep) 
    std::map<std::pair<long, long>, Event> syncm; 
  };



template<class T> inline void      
SyncQueueT<T>::queue_sync(T* pSync, long loc_id)
{
  using namespace std;
  // construct smart pointer objeckt
  Event sync(pSync);
  syncm[make_pair(sync->get_lock_id(), loc_id)] = sync;
} 

template<class T> inline Event     
SyncQueueT<T>::last_sync(long lkid, long loc_id)
{
  using namespace std;
  map<pair<long, long>, Event>::const_iterator it = 
    syncm.find(make_pair(lkid, loc_id));
   
   if (it != syncm.end())
     return it->second;
   else
     return NULL;
}

typedef SyncQueueT<MPISync_rep>   MPI_SyncQueue;
typedef SyncQueueT<OMPSync_rep>   OMP_SyncQueue;
typedef SyncQueueT<Sync_rep>      SyncQueue;

}

#endif




