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

#ifndef EARL_MPI_GET1TS_REP_H
#define EARL_MPI_GET1TS_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIGet1TS_rep
 *
 * transfer start of an 'GET' operation. 
 * Occurs only on target location.
 * Corresponding MPIGet1TE_rep has the same 'rmaid' value
 *
 *---------------------------------------------------------------------------- 
 */

#include "State.h"
#include "MPIRmaComm_rep.h"

namespace earl
{
  class MPIGet1TS_rep : public MPIRmaComm_rep 
  {
  public:
  
    MPIGet1TS_rep(State&        state,
                  EventBuffer*  buffer,
                  Location*     loc, 
                  double        time,
                  Location*     dst,
                  MPIWindow*    win,
                  long          rmaid,
                  long          length ) :
    MPIRmaComm_rep(state, buffer, loc, time, win, rmaid, length), 
    dst(dst), originptr(state.get_mpiorigin(dst, rmaid) )
    {
      if (originptr.null())
        throw RuntimeError("MPIGet1TS_rep()::MPIGet1TS_rep(): origin pointer not found");
      originptr->set_win(win);
    }
    
    // accessor functions
    Location* get_dest()     const { return dst;  }
    Location* get_src()      const { return get_loc();  }
    Location* get_origin()   const { return dst;  }
    Location* get_target()   const { return get_loc();  }
    Event     get_TO_event() const { return originptr; }
    
    virtual bool is_type(etype type) const
      { return MPIRmaComm_rep::is_type(type) || type == MPIGET1TS; }
    virtual etype       get_type()    const { return MPIGET1TS; }
    virtual std::string get_typestr() const { return "MPIGET1TS"; }

    virtual void trans(State& state);
  
  private:
    // destination of data transfer
    Location* dst;
    // origin of data transfer
    Event originptr;
  };
}
#endif






