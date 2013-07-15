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

#ifndef EARL_MPIRMA_SYNC_REP_H
#define EARL_MPIRMA_SYNC_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Sync_rep
 *
 * Lock synchronization event (e.g, acquiring or releasing a lock).
 *
 *---------------------------------------------------------------------------- 
 */

#include "MPIRma_rep.h"

namespace earl
{
  class MPISync_rep : public MPIRma_rep 
  {
  public:
  
    MPISync_rep(State&        state,
                EventBuffer*  buffer,
                Location*     loc, 
                double        time,
                Location*     lloc,
                MPIWindow*    win
               ) :
               MPIRma_rep(state, buffer, loc, time, win),lock_loc(lloc)
    {}
  
    virtual bool is_type(etype type) const
      { return MPIRma_rep::is_type(type) || type == MPIRMASYNC; }
    virtual etype       get_type()    const { return MPIRMASYNC; }
    virtual std::string get_typestr() const { return "MPIRMASYNC"; }

    Location* get_lock_loc()  const { return lock_loc; }
  
    virtual void trans(State& state);
  
  private:
    Location* lock_loc;
  };
}
#endif

