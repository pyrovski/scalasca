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

#ifndef EARL_MPI_PUT1TE_REP_H
#define EARL_MPI_PUT1TE_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIPut1TE_rep
 *
 * transfer end of an 'PUT' operation. 
 * Occurs only on target location. 
 * Corresponding MPIPut1TS has the same 'rmaid' value
 *
 *---------------------------------------------------------------------------- 
 */

#include "MPIRmaComm_rep.h"


namespace earl
{
  class MPIPut1TE_rep : public MPIRmaComm_rep 
  {
  public:
  
    MPIPut1TE_rep(State&        state,
                  EventBuffer*  buffer,
                  Location*     loc, 
                  double        time,
                  Location*     src,
                  MPIWindow*    win,
                  long          rmaid);
    
    // accessor functions
    Location* get_dest()       const { return get_loc(); }
    Location* get_src()        const { return src;       }
    Location* get_origin()     const { return src;       }
    Location* get_target()     const { return get_loc(); }
    Event get_transfer_start() const { return put1TSptr; }
    
    virtual bool is_type(etype type) const
           { return MPIRmaComm_rep::is_type(type) || type == MPIPUT1TE; }
    virtual etype       get_type()    const { return MPIPUT1TE; }
    virtual std::string get_typestr() const { return "MPIRMAPUT1TE"; }

    virtual void trans(State& state);
  
  private:
    Location* src;
    Event     put1TSptr;
  };
}
#endif






