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

#include "BufTrace.h"


#include "Error.h"
#include "State.h"

using namespace earl;
using namespace std;

BufTrace::BufTrace(std::string path) :  
  RndTrace(path),
  history(SeqTrace::get_grid())
{}

Event
BufTrace::get_event(long pos)
{
  Event event;

  event = buffer.retrieve(pos);
  if ( !event.null() )
    return event;

  RndTrace::move(pos, &buffer, history);
  return RndTrace::get_event();
}

void   
BufTrace::get_state(long pos, State& state)
{
  if ( pos < 1 && SeqTrace::get_nevents() < pos)
    throw RuntimeError("Error in BufTrace::(long, State&).");
  
  else if ( pos == RndTrace::get_pos() )
    RndTrace::get_state(state);
  else if ( history.is_buffered(pos) )
    history.get_state(pos, state);
  else
    {
      RndTrace::move(pos, &buffer, history);
      RndTrace::get_state(state);
    }
}
