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

#ifndef EARL_JOIN_REP_H
#define EARL_JOIN_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Join_rep
 *
 * Event of creating a team of threads.
 *
 *---------------------------------------------------------------------------- 
 */

#include "Team_rep.h"

namespace earl
{
  class Join_rep : public Team_rep 
  {
  public:
  
    Join_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time);
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return JOIN; }
    virtual std::string get_typestr() const { return "JOIN"; }
  
    Event get_forkptr()  const { return forkptr; }
  
    virtual void trans(State& state);
  
  private:
  
    Event forkptr;
  };
}
#endif






