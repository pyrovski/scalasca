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

#ifndef EARL_HISTORY_H
#define EARL_HISTORY_H

/*
 *----------------------------------------------------------------------------
 *
 * class History
 * 
 * This class is responsible for the history buffer. The most recently read
 * events are buffered for later retrieval. The buffer allways contains a 
 * contiguous subsequence of the event trace.
 *
 *----------------------------------------------------------------------------
 */

#include <list>

#include "Event.h"
#include "State.h"
#include "Queue.h"

namespace earl
{
  class Grid;

  class History
  {
  public:
  
    History(const Grid* grid);
    ~History();
  
    //  check whether S(pos) can be reconstructed from history first before using get_state()
    bool is_buffered(long pos);

    void get_state(long pos, State& state);
  
    // in order to insert event = E(i), state = S(i) has to be supplied
    // as argument
    void insert(Event event, const State& state);
  
  private:
  
    // maximum size of history buffer
    // must be >= 1
    long max_len;    
  
    State hstate; 
    State out; 
  
    Queue<Event>* histq;
  };
}
#endif











