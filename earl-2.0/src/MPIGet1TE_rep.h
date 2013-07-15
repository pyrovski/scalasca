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

#ifndef EARL_MPI_GET1TE_REP_H
#define EARL_MPI_GET1TE_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIGet1TE_rep
 *
 * transfer start of an 'PUT' operation. 
 * Occurs only on origin location.
 * Corresponding MPIGet1TS_rep has the same 'rmaid' value
 *
 *---------------------------------------------------------------------------- 
 */

#include "MPIRmaComm_rep.h"

namespace earl
{
  class MPIGet1TE_rep : public MPIRmaComm_rep 
  {
  public:
  
    MPIGet1TE_rep( State&       state,
                   EventBuffer* buffer,
                   Location*    loc, 
                   double       time,
                   Location*    src,
                   MPIWindow*   win,
                   long         rmaid );
    
    // accessor functions
    Location* get_dest()       const { return get_loc(); }
    Location* get_src()        const { return src;       }
    Location* get_origin()     const { return get_loc(); }
    Location* get_target()     const { return src;       }
    Event get_transfer_start() const  { return get1TSptr;}
    
    virtual bool is_type(etype type) const
           { return MPIRmaComm_rep::is_type(type) || type == MPIGET1TE; }
    virtual etype       get_type()    const { return MPIGET1TE; }
    virtual std::string get_typestr() const { return "MPIGET1TE"; }

    virtual void trans(State& state);
  
  private:
    // data source location. 
    Location* src;

    // transfer start
    Event get1TSptr;
  };
}
#endif






