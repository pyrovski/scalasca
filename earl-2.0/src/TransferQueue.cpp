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

#include "TransferQueue.h"

#include <algorithm>

#include "Communicator.h"
#include "Error.h"
#include "Grid.h"
#include "RmaComm_rep.h"

#include "Get1TS_rep.h"
#include "Get1TE_rep.h"

using namespace earl;
using namespace std;

TransferQueue::TransferQueue(const Grid* grid) : 
  transfers(grid->get_nlocs())
{
}

void 
TransferQueue::queue_rmatransfer(Event start)
{
    if ( start->get_type() != PUT1TS && 
         start->get_type() != GET1TS ) 
      throw RuntimeError("queue_rmatransfer(): invalid input parameter");

    // pack transfer start to origin epoch location 
    long locid = start->get_origin()->get_id();
    int rmaid = start->get_rmaid();

    transfers[locid][rmaid] = Event(start);  

}

void 
TransferQueue::dequeue_rmatransfer(Event end)
{
    if ( end->get_type() != PUT1TE && 
         end->get_type() != GET1TE ) 
      throw RuntimeError("queue_rmatransfer(): invalid input parameter");

    // pack transfer start to origin epoch location 
    long locid = end->get_origin()->get_id();
    int rmaid  = end->get_rmaid();
    
    transfers[locid].erase(rmaid);
    
}

Event
TransferQueue::get_rmatransfer(const Location* origin, int rmaid ) const
{
    int locid = origin->get_id();
    map<int,Event>::const_iterator it = transfers.at(locid).find(rmaid);
    if ( it == transfers[locid].end() )
      return NULL;

    return it->second;
}

void
TransferQueue::get_rmatransfers(vector<Event>& out, Location* origin ) const
{
  if (!origin) return;
  
  rmamap_t::const_iterator it;
  const rmamap_t& rmamap = transfers.at(origin->get_id());

  out.clear();
  out.reserve(rmamap.size());
  
  for ( it=rmamap.begin() ;it!=rmamap.end(); it++ )
    out.push_back( it->second );
}

void
TransferQueue::get_rmagets(std::vector<Event>& out, Location* origin ) const
{
  if (!origin) return;
  
  rmamap_t::const_iterator it;
  const rmamap_t& rmamap = transfers.at(origin->get_id());

  out.clear();
  out.reserve(rmamap.size());
  
  for ( it=rmamap.begin() ;it!=rmamap.end(); it++ )
    if ( it->second->get_type() == GET1TS )
      out.push_back( it->second );
}

void
TransferQueue::get_rmaputs(std::vector<Event>& out, Location* origin ) const
{
  if (!origin) return;
  
  rmamap_t::const_iterator it;
  const rmamap_t& rmamap = transfers.at(origin->get_id());

  out.clear();
  out.reserve(rmamap.size());
  
  for ( it=rmamap.begin() ;it!=rmamap.end(); it++ )
    if ( it->second->get_type() == PUT1TS )
      out.push_back( it->second );
}

