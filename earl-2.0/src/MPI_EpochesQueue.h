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

#ifndef EARL_MPI_EPOCHES_QUEUE_H
#define EARL_MPI_EPOCHES_QUEUE_H 

/*
 *----------------------------------------------------------------------------
 *
 *
 *----------------------------------------------------------------------------
 */

#include <list>
#include <vector>
#include <set>

#include "Event.h"
#include "MPIEpoch.h"
#include "MPILocEpoch.h"
#include "MPI_GATSQueue.h"


namespace earl
{
  //class Communicator;
  class Grid;

  class MPI_EpocheQueue 
  {
  public:
    MPI_EpocheQueue(const Grid* grid);
    virtual ~MPI_EpocheQueue(){};
        
    void queue_gats_event( Event exit );
    void dequeue_epoch();
  
    // epoches which are completed with current event
    void get_complete_epoches( std::vector<CountedPtr<MPIEpoch> >& out ) const ;

  private:
    virtual Event get_mostrcnt(const Location* loc, etype type ) const = 0;
      
    void queue_exp_epoch( CountedPtr<MPILocEpoch>& complete_epoch);
    void queue_acc_epoch( CountedPtr<MPILocEpoch>& complete_epoch);
    CountedPtr<MPIEpoch> find_matching_epoch(long win_id, long loc_id, CountedPtr<MPILocEpoch>& loc_epoch );
        

    // Globale completed epoches
    // (win_id, loc_id) |-> (MPILocEpoch [exposure epoches] )
    typedef std::vector<std::vector< std::list<CountedPtr<earl::MPIEpoch> > > > MPIEpoches;
    MPIEpoches glb_epoches;

    MPI_GATSQueue gats_event_queue;
   
    typedef std::set<CountedPtr<MPIEpoch> > MPIEpochSet;
    MPIEpochSet complete_epoches; 
    
    static std::string lower(std::string str); 
  };
}


#endif
