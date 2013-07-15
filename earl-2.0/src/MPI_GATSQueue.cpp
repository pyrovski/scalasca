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

#include "MPI_GATSQueue.h"

#include <algorithm>
#include <cctype>
#include <string>

#include "Communicator.h"
#include "Error.h"
#include "Grid.h"
#include "MPIWindow.h"
#include "Region.h"
#include "MPILocEpoch.h"


using namespace earl;
using namespace std;

MPI_GATSQueue::MPI_GATSQueue(const Grid* grid) : 
  acc_epoches(grid->get_nwins()),
  exp_epoches(grid->get_nwins())
{
  for ( int i = 0; i < grid->get_nwins(); i++ ) {
    MPIWindow* win = grid->get_win(i);
    int com_size = win->get_com()->size();  
    acc_epoches[i].resize(com_size);
    exp_epoches[i].resize(com_size);
  }
}

MPI_GATSQueue::~MPI_GATSQueue() 
{
}

void    
MPI_GATSQueue::get_access_epoch(MPILocEpoch& out) 
{
  if ( !completed_epoch || ! completed_epoch->is_access_epoch()  ) 
    return;

  out = *completed_epoch;
}

void 
MPI_GATSQueue::get_access_epoch(MPILocEpoch& out,  MPIWindow* win, Location* loc )
{
  Communicator* com;
  if ( !win || ! (com=win->get_com()) )
    throw RuntimeError("Error in MPI_GATSQueue::get_access_epoch(...): invalud window");
  
  long rank = com->get_rank(loc);
  if ( rank < 0  || rank > win->get_com()->get_nprocs() ) 
    throw RuntimeError("Error in MPI_GATSQueue::get_access_epoch(...): invalud rank");
  
  out = *acc_epoches[win->get_id()][rank];
}
    
 
void   
MPI_GATSQueue::get_exposure_epoch(MPILocEpoch& out ) 
{
  if ( !completed_epoch || !completed_epoch->is_exposure_epoch()  ) 
    return;

  out = *completed_epoch;
} 

void 
MPI_GATSQueue::get_exposure_epoch(MPILocEpoch& out,  MPIWindow* win, Location* loc )
{
  Communicator* com;
  if ( !win || !(com=win->get_com()) )
    throw RuntimeError("Error in MPI_GATSQueue::get_exposure_epoch(...): invalud window");

  long rank = com->get_rank(loc);
  if ( rank < 0  || rank > win->get_com()->get_nprocs() ) 
    throw RuntimeError("Error in MPI_GATSQueue::get_exposure_epoch(...): invalud rank");
  
  out = *exp_epoches[win->get_id()][rank];
}
    
void   
MPI_GATSQueue::queue(Event event)
{
  
  Communicator* com;
  Location*     loc;
  MPIWindow*    win = event->get_win();
  // handle only window related events
  if ( !win ) return;
  
  // lookup event data
  if ( !(loc=event.get_loc()) )
      throw RuntimeError("MPI_GATSQueue::queue_gats_event(): Location not found");
  if ( !(com=win->get_com()) )
      throw RuntimeError("MPI_GATSQueue::queue_gats_event(): Communicator not found");
  
  CountedPtr<MPILocEpoch> ep;
  
  long win_id = win->get_id();
  long rank   = com->get_rank(loc);

  if ( isAccessEpochEvent(event) ) 
    {
      if (  !! (ep = acc_epoches.at(win_id).at(rank)) )
          ep->push_back(event);
      else
          acc_epoches[win_id][rank] = ep = CountedPtr<MPILocEpoch>( new MPILocEpoch(event) );
    } else if ( isExposureEpochEvent(event) ) 
    {
      if ( !! (ep = exp_epoches.at(win_id).at(rank)) ) 
          ep->push_back(event);
      else
          exp_epoches[win_id][rank] = ep =  CountedPtr<MPILocEpoch>(new MPILocEpoch(event)); 
    }
  
    if ( ep && ep->is_complete() ) 
      completed_epoch = ep;
  
}

void   
MPI_GATSQueue::dequeue()
{
  if ( !completed_epoch )
    return;
  
  // lookup event data
  const MPIWindow* win = completed_epoch->get_win();
  if ( !win ) throw RuntimeError("MPI_GATSQueue::queue_gats_event(): RMA window not found");
  const Location* loc = completed_epoch->get_loc();
  if ( !loc ) throw RuntimeError("MPI_GATSQueue::queue_gats_event(): Location not found");
  const Communicator* com = win->get_com();
  if ( !com ) throw RuntimeError("MPI_GATSQueue::queue_gats_event(): Communicator not found");

  const long rank   = com->get_rank(loc);
  const long win_id = win->get_id();

  // dequeue completed local epoch
  // move dequeued local epoch to global epoch queue
  if ( completed_epoch == acc_epoches.at(win_id).at(rank) ) 
    {
      acc_epoches[win_id][rank] = completed_epoch = CountedPtr<MPILocEpoch>(NULL); 
    } else if (completed_epoch == exp_epoches.at(win_id).at(rank) ) 
    {
      exp_epoches[win_id][rank] = completed_epoch =  CountedPtr<MPILocEpoch>(NULL);
    }
}

/****************************************************************************
*
* Auxiliary functions                                                                                       
*
****************************************************************************/

string 
MPI_GATSQueue::lower(string str)
{
  string result = str;

  for (size_t i = 0; i < result.length(); i++ )
    result[i] = tolower(result[i]);

  return result;
}


bool
MPI_GATSQueue::isAccessEpochEvent(Event event) const
{
   if (event.null() ) throw RuntimeError("MPI_GATSQueue::isAccessEpochEvent() empty event.");
  
   // check event type for non 'Flow' events
   if ( event.is_type(MPIPUT1TS) || 
        event.is_type(MPIGET1TE) || 
        event.is_type(MPIGET1TO) ) 
     return true;
   
   // check event region for 'Flow' events 
   const Region* reg = event.get_reg();
   if (!reg) 
     return false; 

   string name = lower(reg->get_name());
   if ( name == "mpi_get")
     return true;
   if ( name == "mpi_put")
     return true;
   if ( name == "mpi_win_start")
     return true;
   if ( name == "mpi_win_complete")
     return true;
   
   return false; 
}

bool
MPI_GATSQueue::isExposureEpochEvent(Event event) const
{
   if (event.null() ) throw RuntimeError("MPI_GATSQueue::isExposureEpochEvent() empty event.");
  
   // check event type for non 'Flow' events
   if ( event.is_type(MPIPUT1TE) || event.is_type(MPIGET1TS) ) 
     return true;
   
   // check event region for 'Flow' events 
   Region* reg = event.get_reg();
   if (!reg) 
     return false; 

   string name = lower (reg->get_name());
   if ( name == "mpi_win_post")
     return true;
   if ( name == "mpi_win_wait")
     return true;
   if ( name == "mpi_win_test" && event->get_synex()!=0 )
     return true;
   
   return false; 
}

void 
MPI_GATSQueue::queue_exit(Event exit, Event last_rma_ts )
{
  if ( exit.null() || last_rma_ts.null() )
    return;
    
  if ( last_rma_ts.get_enterptr() != exit.get_enterptr() )
    {
      return;  
    }
  
  const MPIWindow*  win = last_rma_ts->get_win();
  if ( !win )
    throw RuntimeError("MPI_GATSQueue::queue_exit(): transfer event without rma window id.");
    
  const long win_id = win->get_id();
  const long rank   = win->get_com()->get_rank(exit.get_loc());
  if ( !acc_epoches[win_id][rank] )
    {
      // ignore exit event from MPI_Put/MPI_Get for lock/unlock synchronisation
      return;
      //throw RuntimeError("MPI_GATSQueue::queue_exit(): invalid exit from rma transfer routine");
    }
  
  acc_epoches[win_id][rank]->push_back(exit);
}


