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

#ifndef EARL_OMP_SYNC_REP_H
#define EARL_OMP_SYNC_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class OMPSync_rep
 *
 * Lock synchronization event (e.g, acquiring or releasing a lock).
 *
 *---------------------------------------------------------------------------- 
 */

#include "Event_rep.h"

namespace earl
{
  class OMPSync_rep : public Event_rep 
  {
  public:
  
    OMPSync_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time,
	     long          lkid);
  
    virtual bool is_type(etype type) const;

    long   get_lock_id()  const { return lkid; }
    Event  get_lockptr()  const { return lockptr; }
  
    virtual void trans(State& state);
  
  private:
    
    long lkid;
    Event lockptr;
  };
}
#endif






