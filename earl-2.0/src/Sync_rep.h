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

#ifndef EARL__SYNC_REP_H
#define EARL__SYNC_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Sync_rep
 *
 * Lock synchronization event (e.g, acquiring or releasing a lock).
 *
 *---------------------------------------------------------------------------- 
 */

#include "Event_rep.h"
//#include "State.h"

namespace earl
{
  class Sync_rep : public Event_rep 
  {
  public:
  
    Sync_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time,
	     long          lkid);
  
    virtual bool is_type(etype type) const
      { return Event_rep::is_type(type) || type == SYNC; }
    virtual etype       get_type()    const { return SYNC; }
    virtual std::string get_typestr() const { return "SYNC"; }


    virtual long   get_lock_id()  const { return lkid; }
    virtual Event  get_lockptr()  const { return lockptr; }
  
    virtual void trans(State& state);
  
  private:
    long lkid;
    Event lockptr;
  };
}
#endif

