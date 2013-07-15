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

#ifndef EARL_MPI_RMACOMM_REP_H
#define EARL_MPI_RMACOMM_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIRmaComm_rep
 *
 * Lock synchronization event (e.g, acquiring or releasing a lock).
 *
 *---------------------------------------------------------------------------- 
 */

#include "MPIRma_rep.h"

namespace earl
{
  class MPIRmaComm_rep : public MPIRma_rep 
  {
  public:
  
    MPIRmaComm_rep(State&        state,
                   EventBuffer*  buffer,
                   Location*     loc, 
                   double        time,
                   MPIWindow*    win,
                   long          rmaid,
                   long          length
                  ) :
               MPIRma_rep(state, buffer, loc, time, win), rmaid(rmaid)
    {}
    
    // accessor functions
    long   get_rmaid()  const { return rmaid;  }
    long   get_length() const { return length; }

    
    virtual bool is_type(etype type) const
      { return MPIRma_rep::is_type(type) || type == MPIRMACOMM; }
    virtual etype       get_type()    const { return MPIRMACOMM; }
    
    virtual std::string get_typestr() const { return "MPIRMACOMM"; }

    virtual void trans(State& state);

  private:
    
    long rmaid;
    long length;
    
  };
}
#endif






