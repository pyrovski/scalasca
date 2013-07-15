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

#ifndef EARL_MPI_WLOCK_REP_H
#define EARL_MPI_WLOCK_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIWLock_rep
 *
 *
 *---------------------------------------------------------------------------- 
 */

#include "MPIRmaSync_rep.h"

namespace earl
{
  class MPIWLock_rep : public MPISync_rep 
  {
  public:
  
    MPIWLock_rep(State&        state,
                 EventBuffer*  buffer,
                 Location*     loc, 
                 double        time,
                 Location*     lloc,
                 MPIWindow*    win,
                 long          ltype
                ) :
    MPISync_rep(state, buffer, loc, time, lloc, win), ltype(ltype)
    {}
    
    // accessor functions
    long        get_ltype()    const  { return ltype;    }
    
    virtual bool is_type(etype type) const
      { return MPISync_rep::is_type(type) || type == MPIWLOCK; }
    virtual etype       get_type()    const { return MPIWLOCK; }
    virtual std::string get_typestr() const { return "MPIWLOCK"; }

    virtual void trans(State& state);
  
  private:
    // destination of data transfer
    long      ltype;
  };
}
#endif






