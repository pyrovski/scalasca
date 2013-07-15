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

#ifndef EARL_FLOW_REP_H
#define EARL_FLOW_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Flow_rep
 *
 * Contol-flow related event (e.g, entering or leaving a region).
 *
 *---------------------------------------------------------------------------- 
 */

#include <map>
#include <string>
#include <vector>

#include "elg_defs.h"

#include "Event_rep.h"

namespace earl
{
  class Metric;

  class Flow_rep : public Event_rep 
  {
  public:
  
    Flow_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time,
	     Region*       reg,
	     std::map<long, Metric*>&  metm,
	     std::vector<elg_ui8>& metv) :
      Event_rep(state, buffer, loc, time), reg(reg), metm(metm), metv(metv) {}
  
    virtual bool is_type(etype type) const;

    Region* get_reg()   const { return reg; }
  
    long           get_nmets()         const { return metv.size(); } 
    long long      get_metval(long i)  const;
    std::string    get_metname(long i) const;
    
  protected:

    void set_reg(Region* region) { reg = region; }

  private:
      
    Region*  reg;

    std::map<long, Metric*>&  metm;
    std::vector<elg_ui8> metv;
  };
}
#endif






