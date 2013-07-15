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

#ifndef EARL_MPI_PUT1TS_REP_H
#define EARL_MPI_PUT1TS_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIPut1TS_rep
 *
 * transfer start of an 'PUT' operation. 
 * Occurs only on source location.
 * Corresponding MPIPut1TE_rep has the same 'rmaid' value
 *
 *---------------------------------------------------------------------------- 
 */

#include "MPIRmaComm_rep.h"

namespace earl
{
  class MPIPut1TS_rep : public MPIRmaComm_rep 
  {
  public:
  
    MPIPut1TS_rep(State&        state,
                  EventBuffer*  buffer,
                  Location*     loc, 
                  double        time,
                  Location*     dst,
                  MPIWindow*    win,
                  long          rmaid,
                  long          length
             ) :
    MPIRmaComm_rep(state, buffer, loc, time, win, rmaid, length), dst(dst)
    {}
    
    // accessor functions
    Location* get_dest()   const { return dst;       }
    Location* get_src()    const { return get_loc(); }
    Location* get_origin() const { return get_loc(); }
    Location* get_target() const { return dst;       }
    
    virtual bool is_type(etype type) const
           { return MPIRmaComm_rep::is_type(type) || type == MPIPUT1TS; }

    virtual etype       get_type()    const { return MPIPUT1TS; }
    virtual std::string get_typestr() const { return "MPIPUT1TS"; }
    
    virtual void trans(State& state);
  
  private:
    
    Location* dst;
  };
}
#endif






