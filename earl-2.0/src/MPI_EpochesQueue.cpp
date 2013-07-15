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

#include "MPI_EpochesQueue.h"

#include <algorithm>
#include <cctype>

#include "Communicator.h"
#include "Error.h"
#include "Grid.h"
#include "MPIWindow.h"
#include "Region.h"

using namespace earl;
using namespace std;
MPI_EpocheQueue::MPI_EpocheQueue(const Grid* grid) :
    glb_epoches(grid->get_nwins()),
    gats_event_queue(grid)
{
  const long nlocs= grid->get_nlocs();
  for ( int i = 0; i < grid->get_nwins(); i++ )
    glb_epoches[i].resize(nlocs);
 
}

void 
MPI_EpocheQueue::get_complete_epoches( vector<CountedPtr<MPIEpoch> >& out ) const
{
#ifdef __SUNPRO_CC
    out.clear();
    out.reserve(complete_epoches.size());
    for(MPIEpochSet::iterator it=complete_epoches.begin();
	it!=complete_epoches.end(); ++it)
      out.push_back(*it);
#else
    out.assign(complete_epoches.begin(),complete_epoches.end());
#endif
}

void 
MPI_EpocheQueue::queue_gats_event( Event exit )
{
  MPIWindow* win = exit->get_win();
  // extra handle exit from MPI_Put and MPI_Get regions
  if ( !win && exit->get_type() == EXIT )  {
    Event transf;
    if (lower(exit.get_reg()->get_name()) == "mpi_put") {
      transf = get_mostrcnt(exit.get_loc(), MPIPUT1TS);
    }
    if (lower(exit.get_reg()->get_name()) == "mpi_get"){
      transf = get_mostrcnt(exit.get_loc(), MPIGET1TS);
      transf = transf->get_TO_event();
    }
    gats_event_queue.queue_exit(exit, transf);
    return;
  }
  
  // queue GATS event to gats_event_queue
  gats_event_queue.queue(exit);
  
  // check if some epoch is complete
  CountedPtr<MPILocEpoch> ep = gats_event_queue.get_completed_epoch();
  if (!ep)
    return;
  
  // dequeue events of completed epoch from gats_event_queue
  gats_event_queue.dequeue();
 
  // copy completed epoch to epoche queue 
  if ( ep->is_exposure_epoch() )
    queue_exp_epoch(ep);
  else if ( ep->is_access_epoch() )
    queue_acc_epoch(ep);
  else
    throw RuntimeError("MPI_EpocheQueue::queue_gats_event(): invalid epoch type.");
}

void
MPI_EpocheQueue::dequeue_epoch()
{
  set<CountedPtr<MPIEpoch> >::iterator it;
   
  for (it=complete_epoches.begin(); it!=complete_epoches.end(); it++)
    {
  
      const Location*  loc = (*it)->get_loc();
      const MPIWindow* win = (*it)->get_win();
      if (!loc || !win)
        throw RuntimeError("MPI_GATSQueue::dequeue_epoch(): invalid win/loc in completed epoch"); 
      const long loc_id = loc->get_id();
      const long win_id = win->get_id();
      if ( glb_epoches[win_id][loc_id].front() != *it ) 
          throw RuntimeError("MPI_EpocheQueue::dequeue_epoch() invalid completed epoch");
      glb_epoches[win_id][loc_id].pop_front(); 
    }
  complete_epoches.clear();
}

void   
MPI_EpocheQueue::queue_exp_epoch( CountedPtr<MPILocEpoch>& loc_epoch)
{
  // lookup event data
  const MPIWindow* win = loc_epoch->get_win();
  if ( !win ) throw RuntimeError("MPI_GATSQueue::queue_exp_epoch(): RMA window not found");
  const Location* loc = loc_epoch->get_loc();
  if ( !loc ) throw RuntimeError("MPI_GATSQueue::queue_exp_epoch(): Communications partners not found");
  
  const long win_id = win->get_id();
  const long loc_id = loc->get_id();
  
  CountedPtr<MPIEpoch> ep;
  ep = find_matching_epoch(win_id, loc_id, loc_epoch);
  if (!ep)
    throw RuntimeError("MPI_EpocheQueue::queue_exp_epoch(): Matching access epoch not found");
   
  ep->insert(loc_epoch);
  

  if ( ep->is_complete() )
    complete_epoches.insert(ep);
}

void   
MPI_EpocheQueue::queue_acc_epoch( CountedPtr<MPILocEpoch>& loc_epoch)
{
  // lookup event data
  const MPIWindow* win = loc_epoch->get_win();
  if ( !win ) throw RuntimeError("MPI_GATSQueue::queue_acc_epoch(): RMA window not found");
  const Communicator* partners = loc_epoch->get_partners();
  if ( !partners ) throw RuntimeError("MPI_GATSQueue::queue_acc_epoch(): Communications partners not found");
  

  const long win_id = win->get_id();
  CountedPtr<MPIEpoch> ep;
  for (int rank=0; rank<partners->get_nprocs(); rank++)
    {
      const long loc_id = partners->get_proc(rank)->get_loc()->get_id();
      ep = find_matching_epoch(win_id, loc_id, loc_epoch);
      if (!ep) 
        throw RuntimeError("MPI_EpocheQueue::queue_acc_epoch(): Matching exposure epoch not found");
      ep->insert(loc_epoch);
  
      if ( !!ep && ep->is_complete() ) 
          complete_epoches.insert(ep);
    }
}

CountedPtr<MPIEpoch>
MPI_EpocheQueue::find_matching_epoch(long win_id, long loc_id, CountedPtr<MPILocEpoch>& loc_epoch )
{ 
    // result
    CountedPtr<MPIEpoch> ep;
    
    // get epoches at specified location
    list<CountedPtr<MPIEpoch> >& eps = glb_epoches[win_id][loc_id];
    list<CountedPtr<MPIEpoch> >::iterator it;

    // search for appropriate epoch on specified location
    for ( it=eps.begin(); it!=eps.end(); it++ )
      {
        ep = *it;
        if ( ep->match(loc_epoch)  ) 
          return ep;
      }

    // create new empty epoch object if not found 
    ep=CountedPtr<MPIEpoch>( new MPIEpoch() );
    eps.push_back( ep );
    
    return ep;
}


string 
MPI_EpocheQueue::lower(string str)
{
  string result = str;

  for (size_t i = 0; i < result.length(); i++ )
    result[i] = tolower(result[i]);

  return result;
}


