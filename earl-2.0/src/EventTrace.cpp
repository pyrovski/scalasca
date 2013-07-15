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

#include "EventTrace.h"

#ifdef NO_INLINE
#  define inline
#  include "EventTrace.inl"
#endif

#include <algorithm>
#include <libgen.h>
//#include <iostream>

#include "Error.h"


#include "SeqTrace.h"

using namespace earl;
using namespace std;

EventTrace::EventTrace(string filepath) :
  btrace(filepath),
  state(btrace.get_grid()),
  path(filepath),
  has_mpirma(btrace.get_grid()->get_nwins() > 0),
  has_ompreg(btrace.get_reg("omp_get_thread_num")!=NULL),
  has_shmemreg(btrace.get_reg("shmem_get")!=NULL)
{
  name = basename(const_cast<char*>(filepath.c_str())); 
}

Event   
EventTrace::event(long pos) 
{ 
  if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");
  
  return btrace.get_event(pos); 
}

void 
EventTrace::stack(vector<Event>& out, long pos, long loc_id) 
{
  if ( pos == 0) 
    return;
  else if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();

  Location* loc = btrace.get_grid()->get_loc(loc_id);
  btrace.get_state(pos, state);
  state.stack(out, loc);
}

void 
EventTrace::istack(vector<Event>& out, long pos, long loc_id) 
{ 
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();

  Location* loc = btrace.get_grid()->get_loc(loc_id);

  btrace.get_state(pos, state);
  state.istack(out, loc);
}

void 
EventTrace::queue(vector<Event>& out, long pos, long src_id, long dest_id) 
{ 
  Location *src, *dest;

  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();

  if ( src_id != -1 )
    src = btrace.get_grid()->get_loc(src_id);
  else
    src = NULL; 
  if ( dest_id != -1 )
    dest = btrace.get_grid()->get_loc(dest_id);
  else
    dest = NULL;

  btrace.get_state(pos, state);
  state.messages(out, src, dest);
}

void 
EventTrace::mpicoll(vector<Event>& out, long pos) 
{ 
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();

  btrace.get_state(pos, state);
  state.MPI_CollectiveQueue::get_coll(out);
}

/**/
void 
EventTrace::get_mpisyncccoll(vector<Event>& out, long pos) 
{ 
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();

  btrace.get_state(pos, state);
  state.MPI_RmaCollQueue::get_coll(out);
}

void
EventTrace::get_mpiglobalepoches(vector<CountedPtr<MPIEpoch> >& out, long pos) 
{ 
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  btrace.get_state(pos, state);
  state.get_complete_epoches(out);
}

void
EventTrace::get_mpitransfers(std::vector<Event>& out, long pos, long loc_id)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  Location* loc = get_loc(loc_id);
  btrace.get_state(pos, state);
  state.MPI_TransferQueue::get_rmatransfers(out, loc);

}

void
EventTrace::get_mpigets(std::vector<Event>& out, long pos, long loc_id)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  Location* loc = get_loc(loc_id);
  btrace.get_state(pos, state);
  state.MPI_TransferQueue::get_rmagets(out, loc);

}

void
EventTrace::get_mpiputs(std::vector<Event>& out, long pos, long loc_id)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  Location* loc = get_loc(loc_id);
  btrace.get_state(pos, state);
  state.MPI_TransferQueue::get_rmaputs(out, loc);

}

void
EventTrace::get_mpiorigins(std::vector<Event>& out, long pos, long loc_id)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  Location* loc = get_loc(loc_id);
  btrace.get_state(pos, state);
  state.get_mpiorigins(out, loc);

}

/* SHMEM */
void
EventTrace::get_rmatransfers(std::vector<Event>& out, long pos, long loc_id)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  Location* loc = get_loc(loc_id);
  btrace.get_state(pos, state);
  state.TransferQueue::get_rmatransfers(out, loc);
}

void
EventTrace::get_rmagets(std::vector<Event>& out, long pos, long loc_id)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  Location* loc = get_loc(loc_id);
  btrace.get_state(pos, state);
  state.TransferQueue::get_rmagets(out, loc);
}

void
EventTrace::get_rmaputs(std::vector<Event>& out, long pos, long loc_id)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  Location* loc = get_loc(loc_id);
  btrace.get_state(pos, state);
  state.TransferQueue::get_rmaputs(out, loc);
}

void
EventTrace::get_rmacolls(std::vector<Event>& out, long pos)
{
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  btrace.get_state(pos, state);
  state.CollectiveQueue::get_coll(out);
}

void 
EventTrace::ompcoll(vector<Event>& out, long pos) 
{ 
  if ( pos == 0) 
    return;
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();

  btrace.get_state(pos, state);
  state.OMP_CollectiveQueue::get_coll(out);
}

void 
EventTrace::calltree(vector<Event>& out, long pos) 
{ 
  if ( pos == 0) 
    return;
  else if ( pos == -1 )
    pos = this->get_nevents();
  else if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();
  btrace.get_state(pos, state);
  state.calltree(out);
}

void 
EventTrace::ctroots(vector<Event>& out, long pos) 
{ 
  if ( pos == 0) 
    return;
  else if ( pos == -1 )
    pos = this->get_nevents();
  else if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  out.clear();

  btrace.get_state(pos, state);
  state.ctroots(out);
}

void 
EventTrace::callpath(vector<Event>& out, long cnode) 
{ 
  Event enter = this->event(cnode);

  if ( enter.get_cnodeptr() != enter )
    throw RuntimeError("Invalid call-tree node.");
    
  out.clear();

  out.push_back(enter);
  while ( !enter.get_cedgeptr().null() )
    {
      enter = enter.get_cedgeptr();
      out.push_back(enter);
    }
  reverse(out.begin(), out.end());
}

void 
EventTrace::ctchildren(vector<Event>& out, long cnode) 
{ 
  long pos = this->get_nevents();

  out.clear();

  btrace.get_state(pos, state);
  state.ctchildren(out, cnode);
}

long 
EventTrace::ctvisits(long cnode) 
{ 
  long pos = this->get_nevents();

  btrace.get_state(pos, state);
  return state.ctvisits(cnode);
}

long 
EventTrace::ctsize(long pos) 
{ 
  if ( pos == 0) 
    return 0;
  else if ( pos == -1 )
    pos = this->get_nevents();
  else   if ( pos < 1 || this->get_nevents() < pos )
    throw RuntimeError("Invalid event position.");

  btrace.get_state(pos, state);
  return state.ctsize();  
}

void
EventTrace::get_coords(std::vector<long>& out, long cart_id, long loc_id) const
{
  Cartesian* cart;
  Location* loc;
  
  cart = get_cart(cart_id);
  loc = get_loc(loc_id);
  
  cart->get_coords(out, loc);
}

Location*
EventTrace::get_loc(std::vector<long>& in, long cart_id) const
{
  Cartesian* cart;
 
  cart = get_cart(cart_id);
  
  return(cart->get_loc(in));  
}


















