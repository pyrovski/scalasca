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

#ifndef EARL_TRANSFER_QUEUE_H
#define EARL_TRANSFER_QUEUE_H 

/*
 *----------------------------------------------------------------------------
 *
 *
 *----------------------------------------------------------------------------
 */

#include <algorithm>
#include <map>
#include <vector>

#include "Event.h"




#include "MPIRmaComm_rep.h"
#include "RmaComm_rep.h"



#include "Communicator.h"
#include "Error.h"
#include "Grid.h"
#include "RmaComm_rep.h"

#include "Get1TS_rep.h"
#include "Get1TE_rep.h"
namespace earl
{
  class Grid;

  template <typename EventT>
  class TransferQueueT 
  {
  public:
    typedef EventT EventType;
  
    TransferQueueT(const Grid* grid):
      transfers(grid->get_nlocs())
      {}

    virtual ~TransferQueueT(){};

    
    void queue_rmatransfer  ( Event start );
    void dequeue_rmatransfer( Event end   );
    Event get_rmatransfer (const Location* origin, int rmaid ) const;
    
    void get_rmatransfers(std::vector<Event>& out, Location* origin ) const;
    void get_rmagets(std::vector<Event>& out, Location* origin ) const;
    void get_rmaputs(std::vector<Event>& out, Location* origin ) const;
    
    Event get_mostrcnt(const Location* loc, earl::etype type ) const;
  private:
    // (loc , rma_id)  |->  Event
    typedef std::map<int,Event> rmamap_t;
    typedef std::vector< rmamap_t > transfers_t;
    transfers_t transfers; 
   
  };

  typedef TransferQueueT<RmaComm_rep>    TransferQueue;
  typedef TransferQueueT<MPIRmaComm_rep> MPI_TransferQueue;



template<typename EventT> void 
TransferQueueT<EventT>::queue_rmatransfer(Event start)
{
    if ( start->get_type() != PUT1TS    && 
         start->get_type() != GET1TS    &&
         start->get_type() != MPIPUT1TS &&
         start->get_type() != MPIGET1TS   ) 
      throw RuntimeError("queue_rmatransfer(): invalid input parameter");

    // pack transfer start to origin epoch location 
    long locid = start->get_origin()->get_id();
    int rmaid = start->get_rmaid();

    transfers[locid][rmaid] = Event(start);  
}

template<typename EventT> void 
TransferQueueT<EventT>::dequeue_rmatransfer(Event end)
{
    if ( end->get_type() != PUT1TE    && 
         end->get_type() != GET1TE    && 
         end->get_type() != MPIPUT1TE && 
         end->get_type() != MPIGET1TE  ) 
      throw RuntimeError("queue_rmatransfer(): invalid input parameter");

    // pack transfer start to origin epoch location 
    long locid = end->get_origin()->get_id();
    int rmaid  = end->get_rmaid();
    
    transfers[locid].erase(rmaid);
}

template<typename EventT> Event 
TransferQueueT<EventT>::get_rmatransfer(const Location* origin, int rmaid ) const
{
    int locid = origin->get_id();
    std::map<int,Event>::const_iterator it = transfers.at(locid).find(rmaid);
    if ( it == transfers[locid].end() )
      return NULL;

    return it->second;
}

template<typename EventT> Event 
TransferQueueT<EventT>::get_mostrcnt(const Location* loc, earl::etype type ) const
{
  int loc_id = loc->get_id();
  const std::map<int,Event>& t = transfers.at(loc_id);
  std::map<int,Event>::const_reverse_iterator it;
    
  for ( it=t.rbegin() ;it!=t.rend(); it++ )
    if ( it->second.is_type(type) )
      return it->second;
    
  return NULL;
}

template<typename EventT> void
TransferQueueT<EventT>::get_rmatransfers(std::vector<Event>& out, Location* origin ) const
{
  if (!origin) return;
  
  rmamap_t::const_iterator it;
  const rmamap_t& rmamap = transfers.at(origin->get_id());

  out.clear();
  out.reserve(rmamap.size());
  
  for ( it=rmamap.begin() ;it!=rmamap.end(); it++ )
    out.push_back( it->second );
}

template<typename EventT> void
TransferQueueT<EventT>::get_rmagets(std::vector<Event>& out, Location* origin ) const
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

template<typename EventT> void
TransferQueueT<EventT>::get_rmaputs(std::vector<Event>& out, Location* origin ) const
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

}


#endif

