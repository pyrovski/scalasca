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

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "earl.h"

#include "earl_dump.h"

#include "MPILocEpoch.h"
#include "MPIRma_rep.h"

using namespace std;
using namespace earl;


ostream& operator<<(ostream& out, const Location& loc)
{
  //out.width(13);
  out << "LOCATION " << "[" 
      << loc.get_id() << "] ( "
      << loc.get_mach()->get_id() << ", "
      << loc.get_node()->get_node_id() << ", "
      << loc.get_proc()->get_id() << ", " 
      << loc.get_thrd()->get_thrd_id() << " )\n"; 
  return out;
}

ostream& operator<<(ostream& out, const Machine& mach)
{
  //out.width(13);
  out << "MACHINE " << "[" 
      << mach.get_id() << "] ( "
      << mach.get_name() << ", #nodes: " 
      << mach.get_nnodes() << " )\n";
  return out;
}

ostream& operator<<(ostream& out, const Node& node)
{
  //out.width(13);
  out << "NODE " << "[" 
      << node.get_mach_id() << ", "
      << node.get_node_id() << "] ( "
      << node.get_name() << ", #cpus: " 
      << node.get_ncpus() << ", clock rate: "
      << node.get_clckrt() << ", #procs: "
      << node.get_nprocs() << " )\n";
  return out;
}

ostream& operator<<(ostream& out, const Process& proc)
{
  //out.width(13);
  out << "PROCESS " << "[" 
      << proc.get_id() << "] ( "
      << proc.get_name() << ", #thrds: " 
      << proc.get_nthrds() << " )\n";
  return out;
}

ostream& operator<<(ostream& out, const Thread& thrd)
{
  //out.width(13);
  out << "THREAD " << "[" 
      << thrd.get_proc_id() << ", "
      << thrd.get_thrd_id() << "] ( "
      << thrd.get_name() << " )\n";
  return out;
}

ostream& operator<<(ostream& out, const Callsite& csite)
{
  out << "CALLSITE " << "[" 
      << csite.get_id() << "] ( "
      << csite.get_file() << ":" 
      << csite.get_line() << " -> "
      << csite.get_callee()->get_name() << " )\n";
  return out;
}

ostream& operator<<(ostream& out, const Region& reg)
{
  //out.width(13);
  out << "REGION " << "[" 
      << reg.get_id() << "] ( "
      << reg.get_name() << ", " 
      << reg.get_file() << ":" 
      << reg.get_begln() << "/"
      << reg.get_endln() << ", "
      << reg.get_rtype() << ", "
      << reg.get_descr() << " )\n";
  return out;
}

ostream& operator<<(ostream& out, const Metric& met)
{
  //out.width(13);
  out << "METRIC " << "[" 
      << met.get_id()  << "] ( "
      << met.get_name() << ", "
      << met.get_descr() << ", "
      << met.get_type() << ", "
      << met.get_mode() << ", "     
      << met.get_ival() << " )\n";
  return out;
}


ostream& operator<<(ostream& out, const Communicator& com)
{
  //out.width(13);
  out << "COMMUNICATOR " << "[" 
      << com.get_id()  << "] ( ";   
  for ( long i = 0; i < com.size(); i++)
    out << com.get_proc(i)->get_id() << " ";
  out << " )\n";
  return out;
}

ostream& operator<<(ostream& out, const MPIWindow& win)
{
  //out.width(13);
  out << "MPIWindow " << "[" 
      << win.get_id()  << "]: ";   
  out << "  " << *win.get_com();
  return out;
}

ostream& operator<<(ostream& out, const Event& event)
{
  out << setprecision(6);

  //out.width(14);
  out << event.get_typestr() << "[" 
      << event.get_pos() << "] (" 
      << event.get_loc()->get_mach()->get_id() << ", "
      << event.get_loc()->get_node()->get_node_id() << ", "
      << event.get_loc()->get_proc()->get_id() << ", "
      << event.get_loc()->get_thrd()->get_thrd_id() << ") "
      << setw(7) << event.get_time() << " ";
  if ( !event.get_enterptr().null() )
    out << " enter=" << event.get_enterptr().get_pos() << "\n";
  if ( event.get_type() == MPIGET1TS )
    out << " origin=" << event->get_TO_event().get_pos() << "\n";
  if ( !event.get_cnodeptr().null() )
    out << " cnodeptr       " << event.get_cnodeptr().get_pos() << "\n";
  if ( !event.get_cedgeptr().null() )
    out << " cedgeptr       " << event.get_cedgeptr().get_pos() << "\n";
  if ( event.get_reg() != NULL )
    out << " region         " << event.get_reg()->get_name() << "\n";
  if ( event.get_csite() != NULL )
    out << " callsite       " << event.get_csite()->get_callee()->get_name() << "\n";
  if ( event.get_type() == SEND || event.get_type() == RECV || event.get_type() == MPICEXIT )
      out << " communicator   " << event.get_com()->get_id() << "\n";    
  if ( event.get_type() == SEND || event.get_type() == RECV )
    {
      out << " length         " << event.get_length() << "\n";
      out << " tag            " << event.get_tag() << "\n";
    }
  if ( event.get_src() != NULL ) 
      out << " src          " << event.get_src()->get_id() << "\n";
  if ( event.get_dest() != NULL ) 
      out << " dest         " << event.get_dest()->get_id() << "\n";
  if ( !event.get_sendptr().null() )
    out << " sendptr        " << event.get_sendptr().get_pos() << "\n";
  if ( event.get_root() != NULL ) 
      out << " root         " << event.get_root()->get_id() << "\n";
  if ( event.get_type() == MPICEXIT )
    {
      out << " sent            " << event.get_sent() << "\n";
      out << " recvd           " << event.get_recvd() << "\n";
    }
  if ( !event.get_forkptr().null() )
    out << " forkptr        " << event.get_forkptr().get_pos() << "\n";
  if ( event.is_type(OMPSYNC) || event.is_type(SYNC) ) {
    if ( event.get_type()==OMPRLOCK || event.get_type() == RLOCK ) {
      if ( !event.get_lockptr().null() )
        out << " lockptr        " << event.get_lockptr().get_pos() << "\n";
      else
        out << " !!! missing lock ptr\n";
    }
    out << " lock_id        " << event.get_lock_id() << "\n";
  }
  for ( int i = 0; i < event.get_nmets(); i++ )
    out << event.get_metname(i) << " " << event.get_metval(i) << "\n";
  return out;
}  
 
ostream& operator<<(ostream& out, const vector<Event>& eventv)
{
  for ( size_t i = 0; i < eventv.size(); i++ )
    out << " " << eventv[i].get_pos();
  out << "\n";
  return out;
}
    
    

void print_enterv(const vector<Event>& eventv)
{
  for ( size_t i = 0; i < eventv.size(); i++ )
    {
      cout << "(" << eventv[i].get_reg()->get_name() << ", ";
      if ( eventv[i].get_csite() )
	cout << eventv[i].get_csite()->get_line();
      else
	cout << "-";
      cout << ") ";
    }
  cout << "\n";
}

void print_sendv(const vector<Event>& eventv)
{
  for ( size_t i = 0; i < eventv.size(); i++ )
    cout << "(" << eventv[i].get_loc()->get_id() << ", " << eventv[i].get_dest()->get_id() << ") ";
  cout << "\n";
}

void print_exitv(const vector<Event>& eventv)
{
  for ( size_t i = 0; i < eventv.size(); i++ )
    cout << "(" << eventv[i].get_reg()->get_name() << ") ";
  cout << "\n";
}

void print_transferv(const std::vector<earl::Event>& eventv)
{
  for ( size_t i = 0; i < eventv.size(); i++ ) {
    long type = eventv[i]->get_type();
    if ( type == MPIGET1TS || type == GET1TS ) {
      cout << "(GETTS["<< eventv[i]->get_pos() <<"]: lid=" << eventv[i].get_loc()->get_id()
           << ",src=" << eventv[i]->get_src()->get_id();
      if ( type == MPIGET1TS ) 
        cout << ",org=" << eventv[i]->get_TO_event().get_pos();
    } 
    else if ( type == MPIPUT1TS || type == PUT1TS)
      cout << "(PUTTS["<< eventv[i]->get_pos() <<"]: lid=" << eventv[i].get_loc()->get_id()
           << ",dst=" << eventv[i]->get_dest()->get_id();
    else if (eventv[i]->get_type() == MPIGET1TO )
      cout << "(GETTO["<< eventv[i]->get_pos() <<"]: lid=" << eventv[i].get_loc()->get_id();
    else 
      cout << "(type=" << eventv[i].get_typestr() << "pos=" << eventv[i].get_pos();
    cout << ") ";
  }
  if ( eventv.empty() )
    cout << "<empty>";
  else
    cout << "qsize=" << eventv.size();
  cout << "\n";
}

ostream& operator<<(ostream& out, const MPIRma_rep& e )
{
    return out <<"MPI Rma event: pos=" << e.get_pos() << ", win=" << e.get_win() << "\n"; 
}

ostream& operator<<(ostream& out, const earl::MPILocEpoch& epoch )
{
  if ( epoch.is_access_epoch() )
      out << "Access epoch: "; 
  else if (epoch.is_exposure_epoch())
      out << "Exposure epoch: ";
  else 
      out << "Undefined epoch: ";
 
  vector<Event> eventv;
  epoch.get_all_events(eventv);
  out << "nevents=" << eventv.size() << ", proc=" << epoch.get_loc()->get_proc()->get_id() << "\n";

  
  for ( size_t i = 0; i < eventv.size(); i++ ) 
    {

        Event event = eventv[i];
        
        cout << setw(13); 
        out << event.get_typestr() + " [";
        cout << left << setw(4) << event.get_pos() << "]" << right;
        cout << " loc=" << event.get_loc()->get_id();
        if ( !event.get_enterptr().null() )
          out << ", enter=" << event.get_enterptr().get_pos() << " ";
        if ( event.get_reg() != NULL )
          out << event.get_reg()->get_name();
      if (eventv[i].get_type() == MPIPUT1TE || eventv[i].get_type() == MPIGET1TE)
        cout <<  "transfer start=" << eventv[i]->get_transfer_start().get_pos(); 

      if (eventv[i].get_type() == MPIGET1TE)
        cout <<  " origin=" << eventv[i]->get_transfer_start()->get_TO_event().get_pos(); 

      cout << "\n";
    }
  out << "\n";
  return out;
}


std::ostream& operator<<(std::ostream& out, const earl::MPIEpoch& epoch )
{
  out << "-----------------------------------\n";
  
  CountedPtr<MPILocEpoch> exp_epoch = epoch.get_exp_epoch();
  std::vector<CountedPtr<MPILocEpoch> > acc_epoches;
  epoch.get_acc_epoches(acc_epoches);
    
  out << "--- exposure epoch (loc=" << epoch.get_loc()->get_id() << ", win=" << epoch.get_win()->get_id() << ")\n";
  out << "-----------------------------------\n";

  out << *exp_epoch;

  out << "\n--- corresponding access epoches ---\n";
  std::vector<CountedPtr<MPILocEpoch> >::const_iterator it;
  for( it=acc_epoches.begin(); it!=acc_epoches.end(); it++ )
      out << **it;

  return out;
}

