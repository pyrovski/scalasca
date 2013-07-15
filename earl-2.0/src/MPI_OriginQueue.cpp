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

#include "MPI_OriginQueue.h"

#include <algorithm>

#include "Communicator.h"
#include "Error.h"
#include "Grid.h"
#include "MPIWindow.h"
#include "MPIRmaComm_rep.h"

using namespace earl;
using namespace std;

MPI_OriginQueue::MPI_OriginQueue(const Grid* grid) : 
  transfers(grid->get_nlocs())
{
}


void 
MPI_OriginQueue::queue_mpiorigin( Event start)
{
    if ( !start.is_type(MPIGET1TO) ) 
      throw RuntimeError("queue_transfer_event(): invalid input event");

    // pack transfer start to access epoch location 
    long loc_id = start->get_loc()->get_id();
    int rma_id = start->get_rmaid();

    transfers[loc_id][rma_id] = start;  

}

void 
MPI_OriginQueue::dequeue_mpiorigin(const Location* loc, int rma_id )
{
    int loc_id = loc->get_id();
    transfers[loc_id].erase(rma_id);
}

Event
MPI_OriginQueue::get_mpiorigin(const Location* loc, int rma_id ) const
{
    int loc_id = loc->get_id();
    map<int,Event>::const_iterator it = transfers.at(loc_id).find(rma_id);
    if ( it == transfers[loc_id].end() )
      return NULL;

    return it->second;
}

void
MPI_OriginQueue::get_mpiorigins(std::vector<Event>& out, const Location* loc ) const
{
  int loc_id = loc->get_id();
  const map<int,Event>& t = transfers.at(loc_id);
  map<int,Event>::const_iterator it;
    
  out.clear();
  out.reserve(t.size());
 
  for ( it=t.begin() ;it!=t.end(); it++ )
    out.push_back( it->second );
}

