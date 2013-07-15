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

#include "MPILocEpoch.h"
#include "MPIRma_rep.h"

#include "earl_dump.h"

using namespace earl;
using namespace std;

void dump_rma_transfers(EventTrace& trace, Event event, long loc_id)
{
  vector<Event> transfers;
  trace.get_mpitransfers(transfers, event.get_pos(), loc_id );
  if (transfers.size())
    {
       cout << "loc=" << loc_id << ":\n";
        for (size_t i=0; i < transfers.size(); i++)
          cout << transfers[i];
        cout << "-------------------\n";
    }
}
 
void test_rma_data(EventTrace& trace)
{
  cout << endl;  
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "+ MPI RMA DATA " << endl;
  cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

  int nepoches=0;

  /* check for missing EXIT events (i.e., whether stack empty)
  long last_pos = trace.get_nevents();
  vector<Event> outv;
  for ( long loc_id = 0; loc_id < trace.get_nlocs(); loc_id++ )
  {
    trace.stack(outv, last_pos, loc_id);
    if ( ! outv.empty() )
      {
        cerr << "EXPERT: Error in analysis. Trace file incomplete because of missing EXIT events." << endl;
        for (size_t i=0; i<outv.size(); i++)
            cerr << outv[i];
        exit(EXIT_FAILURE);
      }
  }
  cout << "EXIT_SUCCESS\n";
  exit(EXIT_SUCCESS);
  */ 
  for ( int i = 1; i <= trace.get_nevents(); i++ )
      {
	Event event = trace.event(i);
	//cout << event << "\n";
	//cout << "pos=" << i << ", t=" << event.get_typestr() <<  "\n";
        
         
        vector<CountedPtr<MPIEpoch> > glbEps;
        trace.get_mpiglobalepoches( glbEps, event.get_pos() );

	if ( glbEps.size() )
          cout << "pos=" << i << ", t=" << event.get_typestr() <<  "\n";
        
        for (size_t j=0; j < glbEps.size(); j++)
          {
            if ( glbEps.size() > 0 ) 
              {
                //cout << "---" << glbEps.size() << " epoches complete ---\n";
                CountedPtr<MPIEpoch> glbEp =  glbEps[j];
                cout << *glbEp << "\n\n";
                vector<CountedPtr<MPILocEpoch> > acc_eps;
                glbEp->get_acc_epoches(acc_eps);
                for (size_t k=0; k<acc_eps.size() ; k++)
                  dump_rma_transfers(trace, event, acc_eps[k]->get_loc()->get_id());
              }
            nepoches++;
          }
        
      }
      /*
  for ( int i = trace.get_nevents(); i > 0; i++ )
      {
	Event event = trace.event(i);
	//cout << event << "\n";
	cout << "pos= " << i << ", t=" << event.get_typestr() <<  "\n";
        
        CountedPtr<MPIEpoch> glbEp =  trace.get_global_epoch( i );
        if ( glbEp ) 
          {
            const MPIEpoch& ep = *glbEp;
            cout << ep << "\n\n";
          }
        
      }
 
    */     
  cout << "\n" << nepoches << " completed exposure epoches\n"; 
  cout << "\n";

  // dump all transfers
  Event event = trace.event(trace.get_nevents());
  for (long i=0; i < trace.get_nlocs() ; i++)
    {
      dump_rma_transfers(trace, event, i);
    }
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
    cout << "trace created: " << trace.get_nevents() << "events" <<endl; 

    test_rma_data(trace);
    
    cout << "ready" << endl; 
  }
  catch ( Error error ) {

    cerr << error.get_msg() << "\n";
    exit(EXIT_FAILURE);
  } 
} 


