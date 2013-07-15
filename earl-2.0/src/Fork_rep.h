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

#ifndef EARL_FORK_REP_H
#define EARL_FORK_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Fork_rep
 *
 * Event of creating a team of threads.
 *
 *---------------------------------------------------------------------------- 
 */

#include "Team_rep.h"

namespace earl
{
  class Fork_rep : public Team_rep 
  {
  public:
  
    Fork_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time) :
      Team_rep(state, buffer, loc, time) {}
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return FORK; }
    virtual std::string get_typestr() const { return "FORK"; }
  
    virtual void trans(State& state);
  };
}
#endif






