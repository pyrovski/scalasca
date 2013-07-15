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

#ifndef EARL_MPI_GATS_EPOCHES_H
#define EARL_MPI_GATS_EPOCHES_H 

/*
 *----------------------------------------------------------------------------
 *
 *
 *----------------------------------------------------------------------------
 */

#include <list>
#include <vector>

#include "Event.h"
#include "MPIEpoch.h"
#include "MPILocEpoch.h"

#include "CountedPtr.h"


namespace earl
{
  class Communicator;
  class Grid;
  class MPIWindow;

  class MPI_GATSQueue 
  {
  public:
  
    MPI_GATSQueue(const Grid* grid);

    virtual ~MPI_GATSQueue();

  //  TODO perhaps deep copy requered to handle History correctly  
  // uncomment this to see where its is used 
  ///* private:  
  //MPI_GATSQueue(const MPI_GATSQueue& other); 
    
  public:
    void   queue(Event wexit);
    void   dequeue();

    // returns completed access epoch if any
    void get_access_epoch(MPILocEpoch& out );

    // returns incomplete access epoch for specified 
    // RMA-Window and process rank. 
    // Procees ranks are from MPIWindow communicator
    void get_access_epoch(MPILocEpoch& out,  MPIWindow* win, Location* loc );

    // returns completed exposure epoch if any
    void get_exposure_epoch(MPILocEpoch& out );

    // returns completed exposure epoch if any
    CountedPtr<MPILocEpoch> get_completed_epoch() {return completed_epoch;};

    // returns incomplete exposure epoch for specified 
    // RMA-Window and process rank. 
    // Procees ranks are from MPIWindow communicator
    void get_exposure_epoch(MPILocEpoch& out,  MPIWindow* win, Location* loc );

    // queue exit from rma transfer routine 
    void queue_exit(Event exit, Event last_rma_ts);
    
  private:
    // (win_id, rank) |-> (MPILocEpoch)
    typedef std::vector<std::vector< CountedPtr<earl::MPILocEpoch> > > MPILocEpoches;

    // incomplete access and expouse epoche. Accumulate's epoche events.
    MPILocEpoches acc_epoches; 
    MPILocEpoches exp_epoches;
    CountedPtr<MPILocEpoch> completed_epoch; 

    bool isAccessEpochEvent(Event wexit) const;
    bool isExposureEpochEvent(Event wexit) const;

    static std::string lower(std::string str); 
   
  };
}


#endif

