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

using namespace earl;
using namespace std;


void test_calltree(EventTrace& trace)
{
  cout << endl;  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "+ CALLTREE " << endl;
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

  vector<Event> ctv;
    
  trace.calltree(ctv);
  
  for ( size_t i = 0; i < ctv.size(); i++ )
    {
      vector<Event> cpv;
      trace.callpath(cpv, ctv[i].get_pos());

      for ( size_t j = 0; j < cpv.size(); j++ )
	{
	  string region;
	  long line;
	  
	  region = cpv[j].get_reg()->get_name();
	  if ( cpv[j].get_csite() != NULL )
	    line = cpv[j].get_csite()->get_line();
	  else
	    line = -1;
	  
	  cout << region << ":" << line << " ";
	}
      cout << endl;
    }
}

void test_static_data(EventTrace& trace)
{
  cout << endl;  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "+ STATIC DATA " << endl;
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  
  for ( int i = 0; i < trace.get_nmachs(); i++ )
    {
      cout << *trace.get_mach(i);
      for ( int j = 0; j < trace.get_mach(i)->get_nnodes(); j++ )
	cout << *trace.get_node(i,j);
    }
  for ( int i = 0; i < trace.get_nprocs(); i++ )
    {
      cout << *trace.get_proc(i);
      for ( int j = 0; j < trace.get_proc(i)->get_nthrds(); j++ )
	cout << *trace.get_thrd(i,j);
    }
  for ( int i = 0; i < trace.get_nlocs(); i++ )
    cout << *trace.get_loc(i);
  for ( int i = 0; i < trace.get_nfiles(); i++ )
    {
      cout << "FILE " << "[" << i << "] ( " << trace.get_file(i) << " )\n";
    }
  for ( int i = 0; i < trace.get_nregs(); i++ )
    cout << *trace.get_reg(i);
    for ( int i = 0; i < trace.get_ncsites(); i++ )
      cout << *trace.get_csite(i);
    for ( int i = 0; i < trace.get_nmets(); i++ )
      cout << *trace.get_met(i);
    for ( int i = 0; i < trace.get_ncoms(); i++ )
      cout << *trace.get_com(i);
}

void test_dynamic_data(EventTrace& trace)
{
  cout << endl;  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "+ DYNAMIC DATA " << endl;
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  

  for ( int i = 1; i <= trace.get_nevents(); i++ )
      {
	Event event = trace.event(i);
	cout << event << "\n";
	
	vector<Event> ev;
	if (  event.is_type(COLLEXIT) )
	  {
	    cout << "SHMEM COLL";
	    trace.get_rmacolls(ev, i);
	    print_exitv(ev);
	  }
	if (  event.is_type(FLOW) )
	  {
	    cout << "STACK ";
	    trace.stack(ev, i, event.get_loc()->get_id()); 
	    print_enterv(ev); 
	  }
	if (  event.is_type(ENTER) )
	  {
	    cout << "CTREE ";
	    trace.calltree(ev, i);
	    print_enterv(ev);
	    cout << "CPATH ";
	    trace.callpath(ev, event.get_cnodeptr().get_pos());
	    print_enterv(ev);
	  }
	if (  event.is_type(MPICEXIT) )
	  {
	    cout << "MPICOLL ";
	    trace.mpicoll(ev, i);
	    print_exitv(ev);
	  }
	if (  event.is_type(MPIRMACOMM) )
	  {
            int loc_id = event->get_origin()->get_id();
            // Transfer Queue
            if (loc_id >=0 ) 
              {
                // transfer queue
	        trace.get_mpitransfers(ev, i, loc_id);
	        cout << "TRANSF QUEUE[" << setw(4) << event.get_pos() << "] ";
                print_transferv(ev);
                // Origin Queue
                if (event.is_type(MPIGET1TS) || event.is_type(MPIGET1TO) )
                  { 
                    trace.get_mpiorigins(ev, i, loc_id);
	            cout << "ORIG QUEUE[" << setw(4) << event.get_pos() << "] ";
                    print_transferv(ev);
                  }
              } 
	  }
	if (  event.is_type(OMPCEXIT) )
	  {
	    cout << "OMPCOLL ";
	    trace.ompcoll(ev, i);
	    print_exitv(ev);
	  }
        
	if (  event.is_type(SEND) )
	  {
	    cout << "MSG QUEUE(S) ";
	    trace.queue(ev, i, event.get_loc()->get_id(), event.get_dest()->get_id());
	    print_sendv(ev);
	  }
	if (  event.is_type(RECV) )
	  {
	    trace.queue(ev, i, event.get_src()->get_id(), event.get_loc()->get_id());
	    if (!ev.empty())
              {
                cout << "MSG QUEUE(R) ";
	        print_sendv(ev);
              }
	  }
	if (  event.is_type(RMACOMM) )
	  {
            cout << "RMACOMM: " << event.get_typestr() << "[" << event.get_pos() <<"]" <<endl;
            int loc_id = event->get_origin()->get_id();
            // Transfer Queue
            if (loc_id >=0 ) 
              {
                // transfer queue
	        trace.get_rmatransfers(ev, i, loc_id);
	        cout << "SHMEM TRANSF QUEUE[" << setw(4) << event.get_pos() << "] ";
                print_transferv(ev);
              } 
	  }
	cout << "\n";
      }
  cout << "--------------\n";
  for ( int i = trace.get_nevents(); i > 0; i-- )
      {
	Event event = trace.event(i);
	cout << event << "\n";
	
	vector<Event> ev;
	if (  event.is_type(FLOW) )
	  {
	    cout << "STACK ";
	    trace.stack(ev, i, event.get_loc()->get_id()); 
	    print_enterv(ev); 
	  }
	if (  event.is_type(ENTER) )
	  {
	    cout << "CTREE ";
	    trace.calltree(ev, i);
	    print_enterv(ev);
	    cout << "CPATH ";
	    trace.callpath(ev, event.get_cnodeptr().get_pos());
	    print_enterv(ev);
	  }
	if (  event.is_type(MPICEXIT) )
	  {
	    cout << "MPICOLL ";
	    trace.mpicoll(ev, i);
	    print_exitv(ev);
	  }
	if (  event.is_type(OMPCEXIT) )
	  {
	    cout << "OMPCOLL ";
	    trace.ompcoll(ev, i);
	    print_exitv(ev);
	  }
	if (  event.is_type(SEND) )
	  {
	    cout << "MSG QUEUE(S) ";
	    trace.queue(ev, i, event.get_loc()->get_id(), event.get_dest()->get_id());
	    print_sendv(ev);
	  }
	if (  event.is_type(RECV) )
	  {
	    cout << "MSG QUEUE(R) ";
	    trace.queue(ev, i, event.get_src()->get_id(), event.get_loc()->get_id());
	    print_sendv(ev);
	  }
	if (  event.is_type(MPIRMACOMM) )
	  {
            //cout << "MPIRMACOMM: " << event.get_typestr() << "[" << event.get_pos() <<"]" <<endl;
            int loc_id = -1;
            loc_id = event.is_type(MPIPUT1TS)? event.get_loc()->get_id() : loc_id;
            loc_id = event.is_type(MPIPUT1TE) ? event.get_src()->get_id() : loc_id;
            loc_id = event.is_type(MPIGET1TS)? event.get_src()->get_id() : loc_id;
            loc_id = event.is_type(MPIGET1TE) ? event.get_loc()->get_id() : loc_id;
            loc_id = event.is_type(MPIGET1TO) ? event.get_loc()->get_id() : loc_id;
            if (loc_id >=0 ) 
              {
                // transfer queue
	        trace.get_mpitransfers(ev, i, loc_id);
	        cout << "TRANSF QUEUE[" << setw(4) << event.get_pos() << "] ";
                print_transferv(ev);
                // Origin Queue
                if (event.is_type(MPIGET1TS) || event.is_type(MPIGET1TO) )
                  { 
                    trace.get_mpiorigins(ev, i, loc_id);
	            cout << "ORIG QUEUE[" << setw(4) << event.get_pos() << "] ";
                    print_transferv(ev);
                  }
              } 
	  }
	cout << "\n";
      }
}

// check for missing EXIT events (i.e., whether stack empty)
void test_stack(EventTrace& trace)
{
  long last_pos = trace.get_nevents();
  vector<Event> outv;
  for ( long loc_id = 0; loc_id < trace.get_nlocs(); loc_id++ )
  {
    trace.stack(outv, last_pos, loc_id);
    if ( ! outv.empty() )
      {
        cerr << "Trace file incomplete because of missing EXIT events. LAST POS=" << last_pos <<endl;
        //for (size_t i=0; i<outv.size(); i++)
            //cerr << outv[i];
        cout << "STACK[" << loc_id << "]: "; 
        print_enterv(outv);
        exit(EXIT_FAILURE);
      }
  }
  cout << "Test Stack EXIT_SUCCESS\n";
}

int main(int argc, char* argv[])
{
  if ( argc < 2 )
    {
      cerr << "Wrong # arguments.\n";
      exit(EXIT_FAILURE);
    }

  try {

    // open trace file

    EventTrace trace(argv[1]);

    test_calltree(trace);
    test_static_data(trace);
    test_stack(trace);
    test_dynamic_data(trace);
  }
  catch ( FatalError error ) {
    cerr << error.get_msg() << "\n";
    exit(EXIT_FAILURE);
  } 
  catch ( Error error ) {
    cerr << error.get_msg() << "\n";
    exit(EXIT_FAILURE);
  } 
  cout << "EXIT_SUCCESS\n";
} 

