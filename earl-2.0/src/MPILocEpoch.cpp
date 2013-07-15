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

#include "MPILocEpoch.h"

#include <cctype>

#include "Error.h"
#include "Region.h"
#include "State.h"
#include "MPIWindow.h"

using namespace earl;
using namespace std;

MPILocEpoch::MPILocEpoch() : 
    first_transfer(NULL),last_transfer(NULL),
    partners(0),loc(0),win(0)
{
}

MPILocEpoch::MPILocEpoch(Event e) : 
    first_transfer(NULL),last_transfer(NULL),
    partners(e.get_com()), loc(e.get_loc()), win(e->get_win())
{
  if (!loc)
    throw RuntimeError("MPILocEpoch::MPILocEpoch(const Event&): invalid location");
  if (!win)
    throw RuntimeError("MPILocEpoch::MPILocEpoch(const Event&): invalid RMA window");
  
  events.push_back(e);
}

MPILocEpoch::MPILocEpoch(const MPILocEpoch& ep) :
    events(ep.events),
    first_transfer(ep.first_transfer),last_transfer(ep.last_transfer),
    partners(ep.partners), loc(ep.loc), win(ep.win)
{
  if (!loc)
    throw RuntimeError("MPILocEpoch::MPILocEpoch(): invalid location");
  if (!win)
    throw RuntimeError("MPILocEpoch::MPILocEpoch(): invalid RMA window");
}

const MPILocEpoch& MPILocEpoch::operator= (const MPILocEpoch& ep)
{
  if (this==&ep) return *this;
  
  if (!ep.loc)
    throw RuntimeError("MPILocEpoch::operator=(): invalid location");
  if (!ep.win)
    throw RuntimeError("MPILocEpoch::operator=(): invalid RMA window");

  events   = ep.events; 
  partners = ep.partners;
  loc      = ep.loc;
  win      = ep.win;
  first_transfer= ep.first_transfer; 
  last_transfer = ep.last_transfer;
 
  return *this;
}
/*
CountedPtr<MPILocEpoch> 
MPILocEpoch::clone () const
{
  //MPILocEpoch* p  = new MPILocEpoch(*this);  
  MPILocEpoch* p  = new MPILocEpoch();  
  return CountedPtr<MPILocEpoch>(p);
  //CountedPtr<MPILocEpoch> clone = CountedPtr<MPILocEpoch>( new MPILocEpoch(*this) );  
  //return clone;
}
*/

void MPILocEpoch::get_all_events(std::vector<Event>& out ) const
{
#if 1
  events_t:: const_iterator it, itEnd;

  out.reserve(events.size());
  for (it=events.begin(),itEnd=events.end(); it!=itEnd; it++)
    out.push_back(*it);
#else
  out.resize(events.size());
  copy( events.begin(), events.end(), out.begin() );
#endif
}

void
MPILocEpoch::push_back(Event e)
{
  events.push_back(e);
 
  // only MPI_Put() and MPI_Get() EXITs are queued
  if (e.get_type() == EXIT ) {
    if ( first_transfer.null() )
      first_transfer = e;
    last_transfer = e;
  }
}

bool MPILocEpoch::is_access_epoch() const
{
  if ( events.empty() )  
    return false;
  Region* reg = events.front().get_reg();
  if (!reg) 
    return false;
  return lower(reg->get_name()) == "mpi_win_start";
}
 
bool MPILocEpoch::is_exposure_epoch() const
{
  if ( events.empty() )  
    return false;
  Region* reg = events.front().get_reg();
  if (!reg) 
    return false;
  return lower(reg->get_name()) == "mpi_win_post";
}

bool 
MPILocEpoch::is_lock_epoch() const
{
  if ( events.empty() )  
    return false;
  Region* reg = events.front().get_reg();
  if (!reg) 
    return false;
  return lower(reg->get_name()) == "mpi_win_lock";
}

bool 
MPILocEpoch::is_complete() const
{
  if ( events.empty() )  
    return false;
  Region* reg = events.back().get_reg();
  if (!reg) 
    return false;
  string name = lower(reg->get_name());

  return name == "mpi_win_unlock"   ||
         name == "mpi_win_wait"     || 
         name == "mpi_win_complete" ||
        (name == "mpi_win_test" && (events.back()->get_synex()!=0) );
}

bool MPILocEpoch::is_empty() const
{
  return events.empty();
}

Event 
MPILocEpoch::get_mostrcnt(const etype type, const Region* reg) const
{
 if ( events.empty() ) 
   return NULL;

  events_t::const_reverse_iterator it;
  for ( it=events.rbegin() ;it!=events.rend(); it++ ) {
    if ( it->is_type(type) && !reg )
      {
        if (!reg)
          return *it;
        if ( reg && it->get_reg() && it->get_reg()->get_id() == reg->get_id() )
          return *it;
      }
  }
 return NULL;
}

Event
MPILocEpoch::get_epoch_begin() const
{
 if ( events.empty() ) 
   return NULL;
 
 return events.front(); 
}

Event
MPILocEpoch::get_epoch_end() const
{
 if ( events.empty() ) 
   return NULL;
 
 return events.back(); 
}


Event 
MPILocEpoch::get_reg_exit(double time) const
{
  events_t::const_iterator it;

  for (it=events.begin(); it!=events.end(); it++)
    {
      if ( it->is_type(EXIT) ) 
        {
          if ( it->get_enterptr().get_time() <= time && time <= it->get_time() )
            return *it;
        }
    }
  return NULL;
}



/****************************************************************************
*
* Auxiliary functions                                                                                       
*
****************************************************************************/

string 
MPILocEpoch::lower(string str)
{
  string result = str;

  for (size_t i = 0; i < result.length(); i++ )
    result[i] = tolower(result[i]);

  return result;
}


