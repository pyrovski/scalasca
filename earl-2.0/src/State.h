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

#ifndef EARL_STATE_H
#define EARL_STATE_H

/*
 *----------------------------------------------------------------------------
 *
 * State
 *
 *----------------------------------------------------------------------------
 */

#include "Calltree.h"
#include "CollectiveQueue.h"
#include "RegionStack.h"
#include "MPI_EpochesQueue.h"
#include "MPI_MessageQueue.h"
#include "MPI_OriginQueue.h"
#include "SyncQueue.h"
#include "TransferQueue.h"

namespace earl
{
  class State : public Calltree,
                public CollectiveQueue,
                public RegionStack,
                public MPI_CollectiveQueue,
                public MPI_MessageQueue,
                public OMP_CollectiveQueue,
                public OMP_SyncQueue,
                public MPI_TransferQueue,
                public MPI_OriginQueue,
                public MPI_RmaCollQueue,
                public MPI_SyncQueue,
                public MPI_EpocheQueue,
                public TransferQueue,
                public SyncQueue
  {
  public:
  
    State(const Grid* grid) : 
      CollectiveQueue(grid),
      RegionStack(grid),
      MPI_CollectiveQueue(grid), 
      MPI_MessageQueue(),
      OMP_CollectiveQueue(grid), 
      OMP_SyncQueue(), 
      MPI_TransferQueue(grid),
      MPI_OriginQueue(grid),
      MPI_RmaCollQueue(grid), 
      MPI_SyncQueue(), 
      MPI_EpocheQueue(grid),
      TransferQueue(grid),
      SyncQueue(),
      pos(0)
    {}

    long get_pos() const { return pos; }
    void inc_pos()       { pos++; } 
    Event get_mostrcnt(const Location* loc, etype type) const
      { return MPI_TransferQueue::get_mostrcnt(loc, type); }

  private:

    long pos;
  };
}
#endif














