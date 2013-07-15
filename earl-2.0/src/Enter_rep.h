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

#ifndef EARL_ENTER_REP_H
#define EARL_ENTER_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Enter_rep
 *
 * Event of entering a region.
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <vector>

#include "Flow_rep.h"

namespace earl 
{
  class Enter_rep : public Flow_rep
  {
  public:
    
    Enter_rep(State& state,
	      EventBuffer*  buffer,
	      Location*     loc, 
	      double        time,
	      Region*       region,
	      std::map<long, Metric*>&  metm,
	      std::vector<elg_ui8>& metv,
	      Callsite*     csite);
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return ENTER; }
    virtual std::string get_typestr() const { return "ENTER"; }
  
    Event      get_cnodeptr() const { return cnodeptr; }
    Event      get_cedgeptr() const { return cedgeptr; }
    Callsite*  get_csite()    const { return csite; }

    virtual void trans(State& state);

  private:

    Event cedgeptr;
    Event cnodeptr;
    Callsite*  csite;
  };  
}
#endif










