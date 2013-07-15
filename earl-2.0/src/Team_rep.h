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

#ifndef EARL_TEAM_REP_H
#define EARL_TEAM_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Team_rep
 *
 * Event of creating a team of threads.
 *
 *---------------------------------------------------------------------------- 
 */

#include "Event_rep.h"

namespace earl
{
  class Team_rep : public Event_rep 
  {
  public:
  
    Team_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time) :
      Event_rep(state, buffer, loc, time) {}
  
    virtual bool is_type(etype type) const;
  };
}
#endif






