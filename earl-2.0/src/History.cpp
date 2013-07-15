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

#include "History.h"

#include <cstdlib>
//#include <iostream>

#include "Error.h"

using namespace earl;
using namespace std;

History::History(const Grid* grid) :

  hstate(grid),
  out(grid)
{
  char* tmp = getenv("EARL_HISTORY_SIZE");
  if (tmp != NULL)
    {
      max_len = atoi(tmp);
      if (max_len <= 0)
	throw RuntimeError("Bad value for environment variable EARL_HISTORY_SIZE.");
      // cerr << "EARL: History buffer with size " << max_len << "." << endl;
    }
  else
    {
      max_len = 1000 * grid->get_nlocs();     
      // cerr << "EARL: History buffer with dynamic default size" << max_len << "." << endl;
    }
  histq = new Queue<Event>(max_len);
}

History::~History() 
{
  delete histq;
}

bool 
History::is_buffered(long pos)
{
  if ( out.get_pos() == pos )
    return true;
  else if ( histq->empty() )
    return false;
  else if ( hstate.get_pos() <= pos && pos <= histq->back().get_pos() )
    return true;
  else
    return false;
}

void 
History::insert(Event event, const State& state) 
{
  // history is empty
  if ( histq->empty() ) 
    {
      histq->push_back(event);      
      hstate = state; 
    }
  // event is already member of history  
  else if ( histq->front().get_pos() <=  event.get_pos() && event.get_pos() <= histq->back().get_pos() )
    {
      return;
    }
  // event can be appended to history
  else if ( event.get_pos() == histq->back().get_pos() + 1 ) 
    {
      // history is full      
      if ( histq->full() ) 
	{
	  if (hstate.get_pos() == histq->front().get_pos() - 1)
	    histq->front().trans(hstate);
	  histq->pop_front();
	}
      histq->push_back(event);
    }
  // delete old and create new history  
  else 
    {
      histq->clear();      
      histq->push_back(event);
      hstate = state;    
    }
}

void
History::get_state(long pos, State& state) 
{
  // invalid position argument
  if ( !is_buffered(pos) )
    throw RuntimeError("Error in History::get_state(long, State&).");

  // already calculated?
  if (pos == hstate.get_pos())
    state = hstate;
  if (pos == out.get_pos())
    state = out;
  
  // chose the right starting state
  if (! (hstate.get_pos() < out.get_pos()  && out.get_pos() < pos))
    out = hstate;

  // history members are applied to out until event at pos is reached
  for ( size_t i = 0; i < histq->size(); i++ )
    {
      if ( out.get_pos() == pos )
	break;
      Event event = (*histq)[i];
      if ( event.get_pos() == out.get_pos() + 1 )
	event.trans(out);
    }
  state = out;
}












