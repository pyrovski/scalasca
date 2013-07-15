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

#include "RndTrace.h"

#include <cstdio>
//#include <iostream>

#include "Error.h"

using namespace earl;
using namespace std;

RndTrace::RndTrace(std::string path) : 
  SeqTrace(path),
  cur_state(SeqTrace::get_grid())
{
  this->save();
}

void   
RndTrace::save()
{
  if ( !bmarks.save(cur_state.get_pos()) )
    return;
  
  pos2offset.insert( make_pair(cur_state.get_pos(), SeqTrace::get_offset() ));
  pos2state.insert(  make_pair(cur_state.get_pos(), cur_state) );
}

void   
RndTrace::restore(long pos)
{
  cur_state = pos2state.find(pos)->second;
  SeqTrace::set_offset(&pos2offset.find(pos)->second);
}

bool   
RndTrace::move(long pos, EventBuffer* buffer, History& history)
{
  if ( pos < 1 && SeqTrace::get_nevents() < pos)
    throw RuntimeError("Error in RndTrace::move(long, EventBuffer*).");

  if ( pos == cur_state.get_pos() )
    return true;

  // restore nearest bookmark?
  // NOTE: we need to read at least one event, therefore 'pos - 1'  
  if ( cur_state.get_pos() < bmarks.nearest(pos - 1) || pos < cur_state.get_pos() )
    restore(bmarks.nearest(pos - 1));

  while ( cur_state.get_pos() < pos )
    {
      cur_event = SeqTrace::next_event(cur_state, buffer);
      history.insert(cur_event, cur_state);
      this->save();
    }   
  return true;

}

Event
RndTrace::get_event()
{
  return cur_event;
}

void   
RndTrace::get_state(State& state)
{
  state = cur_state;
}
