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

#ifndef EARL_ALOCK_REP_H
#define EARL_ALOCK_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Alock_rep
 *
 * Event of acquiring a lock.
 *
 *---------------------------------------------------------------------------- 
 */

#include "Sync_rep.h"

namespace earl 
{  
  class Alock_rep : public Sync_rep 
  {
  public:
    
    Alock_rep(State&        state,
	      EventBuffer*  buffer,
	      Location*     loc, 
	      double        time,
	      long          lkid) :
      Sync_rep(state, buffer, loc, time, lkid) {}
    
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return ALOCK; }
    virtual std::string get_typestr() const { return "ALOCK"; }
  };
}

#endif






