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

#ifndef EARL_RNDTRACE_H
#define EARL_RNDTRACE_H

/*
 *----------------------------------------------------------------------------
 *
 * class RndTrace
 *
 * This class adds random access functionality to the SeqTrace class
 *
 *----------------------------------------------------------------------------
 */

#include <map>

#include "Bookmarks.h"
#include "Event.h"
#include "History.h"
#include "SeqTrace.h"

namespace earl
{
  class State;

  class RndTrace : public SeqTrace
  {
  public:
  
    RndTrace(std::string path);
    virtual ~RndTrace(){};

  protected:

    bool   move(long pos, EventBuffer* buffer, History& history);
    long   get_pos() { return cur_state.get_pos(); }
    Event  get_event();
    void   get_state(State& state);

  private:

    void save();
    void restore(long pos);

    Bookmarks   bmarks;

    State      cur_state;
    Event cur_event;

    std::map<long, fpos_t>        pos2offset;     
    std::map<long, State>         pos2state;     
  
  };
}
#endif






