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

#include "Compensator.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

#include <elg_error.h>

#include "ApproxTimesMgr.h"

using namespace earl;
using namespace std;

/*
 *---------------------------------------------------------------------------------
 * Helper functions
 *---------------------------------------------------------------------------------
 */


string lower(string str)
{
  string result = str;

  for ( unsigned int i = 0; i < result.length(); i++ )
    result[i] = tolower(result[i]);

  return result;
}

bool is_mpi_api(const Region* region)
{
  if ( region->get_name().length() < 4 )
    return false;
  
  string prefix(region->get_name(), 0, 3);

  return lower(prefix) == "mpi";
}

bool is_mpi_n2n(const Region* region)
{
  if ( ! is_mpi_api(region) )
    return false;

  string suffix = lower(region->get_name().substr(3));

  if ( suffix.find("_all") != string::npos || suffix == "_reduce_scatter" || suffix == "_barrier")
    return true;
  else
    return false;  
}

bool is_mpi_12n(const Region* region)
{
   if ( ! is_mpi_api(region) )
    return false;

  string suffix = lower(region->get_name().substr(4));

  if ( suffix == "bcast" ||
       suffix == "scatter" ||
       suffix == "scatterv" )
    return true;
  else
    return false;  
}

bool is_mpi_n21(const Region* region)
{
  if ( ! is_mpi_api(region) )
    return false;
  
  string suffix = lower(region->get_name().substr(4));

  if ( suffix == "gather" ||
       suffix == "gatherv" ||
       suffix == "reduce" )
    return true;
  else
    return false;  
}

/*
 *---------------------------------------------------------------------------------
 * Class methods
 *---------------------------------------------------------------------------------
 */

Compensator::Compensator(char* infile_name) : infile_name(infile_name)
{  
  // open input file (earl)
  trace = new EventTrace(infile_name);  
  predv.resize(trace->get_nlocs());

  // check whether trace has at least one process
  if ( trace->get_nprocs() <= 1 )
      elg_error_msg("Trace must at least have one process");
    
  // check whether application is pure MPI
  for ( long i = 0; i < trace->get_nprocs(); i++ )
    if ( trace->get_proc(i)->get_nthrds() > 1 )
      elg_error_msg("Trace must be from pure MPI application");
}

Compensator::~Compensator()
{
  delete trace;
}

void 
Compensator::check_missing_exits()
{
  // check for missing EXIT events (i.e., whether stack empty)
  long last_pos = trace->get_nevents();
  vector<Event> outv;
  for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
    {
      trace->stack(outv, last_pos, loc_id);
      if ( ! outv.empty() )
	elg_error_msg("Trace file incomplete because of missing EXIT events");
    }
}

void 
Compensator::dismiss_last_events()
{
  Location* loc;
  long pred_pos;
  for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
    {
      loc = trace->get_loc(loc_id);
      pred_pos = this->get_pred_pos(loc);
      if ( pred_pos != 0 )
	atime->dismiss(pred_pos);
    }
}

double 
Compensator::approx_recv_cm(double send_m, double recv_m, 
			    double send_a, double enter_recv_a, 
			    size_t msg_size)
{
  double recv_a, c_m;

  // yes, communication time can be measured
  c_m = recv_m - send_m;
  
  // two cases
  if ( send_a + c_m > enter_recv_a )
    recv_a = send_a + c_m;
  else
    recv_a = enter_recv_a + ov.get_m2mcpy_time(msg_size);		

  if ( recv_a <= send_a )
    elg_error_msg("Inconsistent message order in approximated execution");

  return recv_a;
}

double 
Compensator::approx_recv_ca(double send_m, double recv_m, 
			    double send_a, double enter_recv_a, 
			    size_t msg_size)
{
  double recv_a, c_a;

  // no, communication time cannot be measured

  if ( amode == UPPER_BOUND )
    {
      // estimate based on upper bound
      c_a = recv_m - send_m;
    }
  else
    {
      // estimate based on lower bound
      c_a = 2.0 * ov.get_m2mcpy_time(msg_size);
    } 
      
  // determine maximum
  recv_a = max(send_a + c_a, enter_recv_a + ov.get_m2mcpy_time(msg_size));

  if ( recv_a <= send_a )
    elg_error_msg("Inconsistent message order in approximated execution");

  return recv_a;
}

bool
Compensator::compensate_default(Event event, long pred_pos)
{
  double pred_a, pred_m, event_a, event_m; 

  event_m = event.get_time();
  if ( pred_pos != 0 )
    {
      if ( !atime->has_approx_time(pred_pos) )
	return false;
      pred_a  = atime->get_approx_time(pred_pos);
      pred_m  = trace->event(pred_pos).get_time();
      event_a = pred_a + ( event_m - pred_m ) - ov.get_event_ovrhd(); 
    }
  else
    {
      event_a = event_m;
    }
  long ndis;
  // determine the number of events whose approximated time depends on this one
  // note: send to self is treated like ordinary event
  if ( event.get_type() == SEND && event.get_dest() != event.get_loc() )
    ndis = 2;
  else
    ndis = 1;

  atime->set_approx_time(event.get_pos(), pred_pos, event.get_loc()->get_id(), ndis, event_a);
  if ( pred_pos != 0 )
    atime->dismiss(pred_pos);
  return true;
}

bool
Compensator::compensate_recv(Event recv)
{
  double enter_recv_m, enter_recv_a, send_m, send_a, recv_m, recv_a; 
  Event send, enter_send, enter_recv;

  // can communication time be measured?
  // implemented as: has the send operation been completed at the time of enter-receive?
  
  // events involved
  send       = recv.get_sendptr();
  enter_send = recv.get_sendptr().get_enterptr();
  enter_recv = recv.get_enterptr();
  
  // times involved
  recv_m       = recv.get_time();
  enter_recv_m = enter_recv.get_time();
  send_m       = send.get_time();
  if ( !atime->has_approx_time(send.get_pos()) )
    return false;
  send_a       = atime->get_approx_time(send.get_pos());
  if ( !atime->has_approx_time(enter_recv.get_pos()) )
    return false;
  enter_recv_a = atime->get_approx_time(enter_recv.get_pos());

  size_t msg_size = recv.get_length();

  // sender's stack at the time of the recv
  vector<Event> sender_stackv;
  trace->stack(sender_stackv, enter_recv.get_pos(), send.get_loc()->get_id());
  
  int size = sender_stackv.size();
  // can communication time be measured?
  if ( size > 0 && sender_stackv[size - 1] == enter_send )
    recv_a = this->approx_recv_cm(send_m, recv_m, 
				  send_a, enter_recv_a, 
				  msg_size); // yes		
  else 
    recv_a = this->approx_recv_ca(send_m, recv_m, 
				  send_a, enter_recv_a, 
				  msg_size); // no		

  atime->set_approx_time(recv.get_pos(), enter_recv.get_pos(), recv.get_loc()->get_id(), 1, recv_a);	
  atime->dismiss(send.get_pos());
  atime->dismiss(enter_recv.get_pos());
  return true;
}


bool
Compensator::compensate_n2n(Event mpicexit)
{
  double pred_m, exit_a, exit_m; 
  long pred_pos;

  // collect exit and enter events
  vector<Event> exitv, enterv;
  trace->mpicoll(exitv, mpicexit.get_pos());
  for ( unsigned long i = 0; i < exitv.size(); i++ )
    enterv.push_back(exitv[i].get_enterptr());
  
  // determine latest measured / approximated entry
  vector<double> enter_mv, enter_av;

  for ( unsigned long i = 0; i < enterv.size(); i++ )
    {
      enter_mv.push_back(enterv[i].get_time());
      if ( ! atime->has_approx_time(enterv[i].get_pos()) )
	return false;      
      enter_av.push_back(atime->get_approx_time(enterv[i].get_pos()));
    }
  double latest_enter_m = *max_element(enter_mv.begin(), enter_mv.end());
  double latest_enter_a = *max_element(enter_av.begin(), enter_av.end());

  // calculate approximated time
  for ( unsigned long i = 0; i < exitv.size(); i++ )
    {
      exit_m = exitv[i].get_time();
      pred_pos = exitv[i].get_enterptr().get_pos();
      pred_m = trace->event(pred_pos).get_time();
      exit_a = latest_enter_a + ( exit_m - latest_enter_m );
      double enter_a = atime->get_approx_time(pred_pos);
      if ( exit_a <= enter_a )
	elg_error_msg("Inconsistent event order in approximated execution of n->n operation");
      atime->set_approx_time(exitv[i].get_pos(), pred_pos, exitv[i].get_loc()->get_id(), 1, exit_a);	
      atime->dismiss(pred_pos);
    }
  return true;
}

bool
Compensator::compensate_12n(Event mpicexit)
{
  Event enter_root, exit_root, enter, exit;
  double enter_root_m, enter_root_a, exit_root_m, exit_root_a;
  double enter_recv_m, enter_recv_a, exit_recv_m, exit_recv_a;

  // collect exit events
  vector<Event> exitv;
  trace->mpicoll(exitv, mpicexit.get_pos());

  // determine enter_root and exit_root
  for ( unsigned long i = 0; i < exitv.size(); i++ )  
    if ( exitv[i].get_loc() == exitv[i].get_root() )
      {
	exit_root  = exitv[i];
	enter_root = exitv[i].get_enterptr(); 
      }
  // root times
  enter_root_m = enter_root.get_time();
  if ( ! atime->has_approx_time(enter_root.get_pos()) )
    return false;      
  enter_root_a = atime->get_approx_time(enter_root.get_pos());

  exit_root_m = exit_root.get_time();
  exit_root_a = enter_root_a + ( exit_root_m - enter_root_m ) - ov.get_event_ovrhd();

  atime->set_approx_time(exit_root.get_pos(), enter_root.get_pos(), exit_root.get_loc()->get_id(), 1, exit_root_a);	
  atime->dismiss(enter_root.get_pos());

  for ( unsigned long i = 0; i < exitv.size(); i++ )  
    {
      // ignore root process
      if ( exitv[i].get_loc() == exitv[i].get_root() )
	continue;

      // receiver events
      Event exit_recv  = exitv[i];
      Event enter_recv = exitv[i].get_enterptr();
      
      // receiver times
      enter_recv_m = enter_recv.get_time();
      if ( ! atime->has_approx_time(enter_recv.get_pos()) )
	return false;      
      enter_recv_a = atime->get_approx_time(enter_recv.get_pos());
      exit_recv_m = exit_recv.get_time();     

      size_t msg_size = exit_recv.get_recvd();
  
      // can communication time be measured ? 
      if ( enter_recv_m <= exit_root_m )
	exit_recv_a = this->approx_recv_cm(enter_root_m, exit_recv_m, 
					   enter_root_a, enter_recv_a, 
					   msg_size); // yes		
      else 
	exit_recv_a = this->approx_recv_ca(enter_root_m, exit_recv_m, 
					   enter_root_a, enter_recv_a, 
					   msg_size); // no		

      atime->set_approx_time(exit_recv.get_pos(), enter_recv.get_pos(), exit_recv.get_loc()->get_id(), 1, exit_recv_a);	
      atime->dismiss(enter_recv.get_pos());
    }
  return true;
}

bool
Compensator::compensate_n21(Event mpicexit)
{
  elg_error_msg("MPI n-to-1 colective operations not yet supported");
  return false;
}

bool
Compensator::compensate_exit_tracing(Event exit_tracing)
{
  long pred_pos;
  double exit_tracing_a;

  pred_pos = exit_tracing.get_enterptr().get_pos();

  if ( ! atime->has_approx_time(pred_pos) )
    return false;      
  exit_tracing_a = atime->get_approx_time(pred_pos);
  atime->set_approx_time(exit_tracing.get_pos(), pred_pos, exit_tracing.get_loc()->get_id(), 1, exit_tracing_a);	
  atime->dismiss(pred_pos);
  return true;
}

bool
Compensator::compensate_event(Event event, long pred_pos)
{
  bool result = false;

  switch ( event.get_type() )
    {
    case RECV:
      {
	// recv from self is treated like ordinary event
	if ( event.get_src() != event.get_loc() )
	  result = compensate_recv(event);
	else
	  result = compensate_default(event, pred_pos);	  
	break;
      }
    case MPICEXIT:
      {
	// all exit events in a collective call are approximated as a group
	// (in one step) upon the last event of the operation
	
	// check whether operation complete ?
	vector<Event> exitv;
	trace->mpicoll(exitv, event.get_pos());
	if ( exitv.size() == 0 )
	  {
	    result = true;
	    break;
	  }
	
	// if n-to-n?
	if ( is_mpi_n2n(event.get_reg()) )
	  result = compensate_n2n(event);
	
	// if 1-to-n?
	else if ( is_mpi_12n(event.get_reg()) )
	  result = compensate_12n(event);
      
	// if n-to-1?
	else if ( is_mpi_n21(event.get_reg()) )
	  result = compensate_n21(event);

	else
	  elg_error_msg("Unknown MPI collective operation type");
	
	break;
      }
    case EXIT:
      {
	if ( event.get_reg()->get_name() == "TRACING" )
	  result = compensate_exit_tracing(event);
	else
	  result = compensate_default(event, pred_pos);	  
	break;
      }
    default:
      {
	result = compensate_default(event, pred_pos);
	break;
      }
    } 
  return result;
}

struct qitem 
{
  qitem(Event e, long pp) : event(e), pred_pos(pp) {}
  Event event;
  long pred_pos;
};

bool operator==(const qitem& lhs, const qitem& rhs)
{
  return lhs.event == rhs.event && lhs.pred_pos == rhs.pred_pos;
}

void 
Compensator::compensate_trace(char* outfile_name, approx_mode mode)
{
  amode = mode;

  atime   = new ApproxTimesMgr(trace->get_nlocs(), trace->get_nevents(), infile_name, outfile_name);

  list<qitem> unfinished;

  cerr << "Total number of events: " << trace->get_nevents() << endl;

  int new_percent, percent = 0;

  for ( long pos = 1; pos <= trace->get_nevents(); pos++ )
    {
      Event event   = trace->event(pos);
      long pred_pos = this->get_pred_pos(event.get_loc());
      bool result;

      result = compensate_event(event, pred_pos);

      if ( ! result )
	unfinished.push_back(qitem(event, pred_pos));
      
      this->set_pred_pos(event.get_loc(), event.get_pos());

      // progress report
      new_percent = ( 100 * pos ) / trace->get_nevents();
      if ( new_percent > percent )
	{
	  percent = new_percent;
	  cerr <<  '\r' << percent << " %"; 
	}

      if ( ! result )
	continue;

      // try to approximate unfinished events
      list<qitem>::iterator it;
      list<qitem> finished;
     
      for ( it = unfinished.begin(); it != unfinished.end(); it++ )
	if ( compensate_event(it->event, it->pred_pos) )
	  finished.push_back(*it);
      
      // delete finished events from list
      for ( it = finished.begin(); it != finished.end(); it++ )
	unfinished.remove(*it);        
    }
  if ( unfinished.size() > 0 )
    elg_error_msg("Compensation incomplete");
  cerr << endl;
  dismiss_last_events();
  check_missing_exits();

  delete atime;
}

