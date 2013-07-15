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

#ifndef EARL_EXIT_REP_H
#define EARL_EXIT_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Exit_rep
 *
 * Event of leaving a region.
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <vector>

#include "Flow_rep.h"
#include "Enter_rep.h"

namespace earl
{
  class Exit_rep : public Flow_rep
  {
  public:
    
    Exit_rep(State& state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time,
	     std::map<long, Metric*>&  metm,
	     std::vector<elg_ui8>& metv);
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return EXIT; }
    virtual std::string get_typestr() const { return "EXIT"; }
  
    virtual void trans(State& state);
  };
}  
#endif

