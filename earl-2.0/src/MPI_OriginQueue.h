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

#ifndef EARL_MPI_ORIG_QUEUE_H
#define EARL_MPI_ORIG_QUEUE_H 

/*
 *----------------------------------------------------------------------------
 *
 *
 *----------------------------------------------------------------------------
 */

#include <map>
#include <vector>

#include "Event.h"


namespace earl
{
  class Grid;
  class MPIRmaComm_rep;

  class MPI_OriginQueue 
  {
  public:
  
    MPI_OriginQueue(const Grid* grid);
    virtual ~MPI_OriginQueue(){};
    
    void queue_mpiorigin( Event start );
    void dequeue_mpiorigin(const Location* loc, int rma_id );
    
    Event get_mpiorigin(const Location* loc, int rma_id ) const;
    void get_mpiorigins (std::vector<Event>& out, const Location* loc ) const;


  private:
    // (loc , rma_id)  |->  Event
    typedef std::vector< std::map<int,Event> > transfers_t;
    transfers_t transfers; 
    
   
  };
}


#endif






