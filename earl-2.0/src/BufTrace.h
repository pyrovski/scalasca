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

#ifndef EARL_BUFTRACE_H
#define EARL_BUFTRACE_H

/*
 *----------------------------------------------------------------------------
 *
 * class BufTrace
 *
 * This class adds a history buffer to the RndTrace class
 *
 *----------------------------------------------------------------------------
 */

#include <map>

#include "History.h"
#include "Event.h"
#include "RndTrace.h"

namespace earl 
{
  class State;
  
  class BufTrace : public RndTrace
  {
  public:
    
    BufTrace(std::string path);
    virtual ~BufTrace(){};
    
    Event get_event(long pos);
    void  get_state(long pos, State& state);
    
  private:
    
    EventBuffer       buffer;
    History           history;
  };
}
#endif






