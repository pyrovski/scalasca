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

#ifndef EARL_MPI_WUNLOCK_REP_H
#define EARL_MPI_WUNLOCK_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIWUnlock_rep
 *
 *
 *---------------------------------------------------------------------------- 
 */

#include "Event_rep.h"
#include "MPIRmaSync_rep.h"

namespace earl
{
  class MPIWUnlock_rep : public MPISync_rep 
  {
  public:
    MPIWUnlock_rep(State&        state,
                   EventBuffer*  buffer,
                   Location*     loc, 
                   double        time,
                   Location*     lloc,
                   MPIWindow*    win );
    
    virtual bool is_type(etype type) const
      { return MPISync_rep::is_type(type) || type == MPIWUNLOCK; }
    virtual etype       get_type()    const { return MPIWUNLOCK; }
    virtual std::string get_typestr() const { return "MPIWUNLOCK"; }

    virtual void trans(State& state);

  private:
    Event lockptr;
  
  };
}
#endif

