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

#ifndef EARL_OMP_RLOCK_REP_H
#define EARL_OMP_RLOCK_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class OMPRlock_rep
 *
 * Event of acquiring a lock.
 *
 *---------------------------------------------------------------------------- 
 */

#include "OMPSync_rep.h"

namespace earl
{
  class OMPRlock_rep : public OMPSync_rep 
  {
  public:
  
    OMPRlock_rep(State&        state,
	      EventBuffer*  buffer,
	      Location*     loc, 
	      double        time,
	      long          lkid) :
      OMPSync_rep(state, buffer, loc, time, lkid) {}
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return OMPRLOCK; }
    virtual std::string get_typestr() const { return "OMPRLOCK"; }
  };
}
#endif











