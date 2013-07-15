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

#ifndef EARL_MPI_GET1TO_REP_H
#define EARL_MPI_GET1TO_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIGet1TO_rep
 *
 * transfer start of an 'PUT' operation. 
 * Occurs only on origin location.
 *
 *---------------------------------------------------------------------------- 
 */

#include "MPIRmaComm_rep.h"

namespace earl
{
  class MPIGet1TO_rep : public MPIRmaComm_rep 
  {
  public:
  
    MPIGet1TO_rep(State&     state,
                  EventBuffer*  buffer,
                  Location*     loc, 
                  double        time,
                  long          rmaid
                 ) :
    MPIRmaComm_rep(state, buffer, loc, time, NULL, rmaid, 0)
    {}
    // accessor functions
    Location* get_dest()     const { return get_loc();  }
    Location* get_origin()   const { return get_loc();  }
             
    virtual bool is_type(etype type) const
           { return MPIRmaComm_rep::is_type(type) || type == MPIGET1TO; }
    virtual etype       get_type()    const { return MPIGET1TO; }
    virtual std::string get_typestr() const { return "MPIGET1TO"; }
    
    virtual void trans(State& state);
    
    void set_win(MPIWindow* win){ this->win=win; };
  };
}
#endif






