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

#ifndef EARL_OMPCEXIT_REP_H
#define EARL_OMPCEXIT_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class OMPCExit_rep
 *
 * Event_rep of leaving an OMP collective operation.
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <vector>

#include "Exit_rep.h"

namespace earl
{
  class OMPCExit_rep : public Exit_rep
  {
  public:
    
    OMPCExit_rep(State&        state,
		 EventBuffer*  buffer,
		 Location*     loc, 
		 double        time,
		 std::map<long, Metric*>&  metm,
		 std::vector<elg_ui8>& metv) :
      Exit_rep(state, buffer, loc, time, metm, metv) {} 

    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return OMPCEXIT; }
    virtual std::string get_typestr() const { return "OMPCEXIT"; }
  
    virtual void trans(State& state);
  };
}  
#endif








