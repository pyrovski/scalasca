/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
* \file cube_part.cpp
* \brief Parts the system-tree of nodes/processes/threads of a cube.
*
*/
/******************************************

  Performance Algebra Operation: PART (System-tree)

*******************************************/

#include <cstdlib>
#include <iostream>
#include <string>
#ifndef CUBE_COMPRESSED
#  include <fstream>
#else
#  include "zfstream.h"
#endif
#include <sstream>
#include <unistd.h>

#include <string.h>

#include "Cube.h"
#include "Cnode.h"
#include "Filter.h"
#include "Machine.h"
#include "Node.h"
#include "Process.h"
#include "Thread.h"
#include "Metric.h"
#include "Region.h"
#include "algebra.h"
#include "services.h"
#include "cube_error.h"

using namespace std;
using namespace cube;
using namespace services;

/**
* Main program. 
* - Check calling arguments
* - Read the .cube input file.
* - Calls cube_part(...) to part the systemtree.
* - Saves the result in "-o outputfile" or "part.cube|.gz" file.
* - end.
*/
int main(int argc, char* argv[]) {
  int ch;
  bool invert = false;
  string ranks;
  vector <string> inputs;
#ifndef CUBE_COMPRESSED
  const char *output = "part.cube";
#else
  const char *output = "part.cube.gz";
#endif
       
  const string USAGE =
    "Partition some of system tree (processes) and void the remainder\n"
    "Usage: " + string(argv[0]) +
    " [-h] [-I] [-R ranks] [-o output] <cubefile>\n"
    "  -I     Invert sense of partition\n"
    "  -R     List of process ranks for partition (e.g., \"0-3,7,13-\")\n"
    "  -o     Name of the output file (default: " + output +")\n"
    "  -h     Help; Show this brief help message and exit.\n" 
    ;
  
  while ((ch = getopt(argc, argv, "IR:o:h")) != -1) {
    switch (ch)
      {
      case 'I':
        invert = true;
        break;
      case 'R':
        ranks = optarg;
        break;
      case 'o':
	output = optarg;
	break;
      case 'h':
      case '?':
	cerr << USAGE << endl; 
	exit(0);
	break;
      default:
	cerr<<USAGE<<"\nError: Wrong arguments.\n";
	exit(1);
      }
  }
  
  if ( argc - optind != 1) {
    cerr<<USAGE<<"\nError: Wrong arguments.\n";
    exit(1);
  }

  for (int i = optind; i < argc; i++) {
    inputs.push_back(argv[i]);
  }
  
  if (!cube_stat(inputs[0])) exit(2);

  if (ranks == "") {
    cerr <<"Must specify partition criteria" << endl;
    exit(2);
  }

  Cube *theCube = NULL;
  try 
  {
    theCube = new Cube();
    cout << "Reading " << inputs[0] << " ... " << flush;
    if (check_file(inputs[0].c_str()) != 0 ) exit(-1);
    #ifndef CUBE_COMPRESSED
    ifstream in(inputs[0].c_str());
    #else
    gzifstream in(inputs[0].c_str(), ios_base::in|ios_base::binary);
    #endif
    if (!in) {
        cerr << "Error: open " << inputs[0] << endl;
        exit(1);
    }
    in >> *theCube;
    cout << "done." << endl;
    
    cout << "++++++++++++ Part operation begins ++++++++++++++++++++++++++" << endl;
    
    const vector<Process*>& procv = theCube->get_procv();
    vector<bool> partproc(procv.size());
    char *token = strtok(strdup(ranks.c_str()),",");
    while (token) {
        char *sep = strchr(token,'-');
        size_t start=atoi(token), end=start;
        if (sep==token) start=0;
        if (sep) { sep++; end=atoi(sep); }
        if (end==0) end=procv.size()-1;
        if (end<start) cerr << "Invalid rank range: " << token << endl;
        else for (size_t p=start; p<=end; p++) partproc[p]=true;
        token = strtok(NULL,",");
    }

    size_t void_procs = 0;
    for (size_t p = 0; p<procv.size(); p++) {
        Process* proc = procv[p];
        if (partproc[p]==invert) proc->set_name(proc->get_name() + " <VOID>");
        void_procs += (partproc[p]==invert);
    }

    if (void_procs == procv.size()) {
        cerr << "Error: partition contains none of the " << procv.size() << " processes." << endl;
        exit(2);
    } else if (void_procs == 0) {
        cerr << "Error: partition contains all of the " << procv.size() << " processes." << endl;
        exit(2);
    }
    cout << "Voiding " << void_procs << " of " << procv.size() << " processes." << endl;

    const vector<Thread*>& thrdv = theCube->get_thrdv();
    vector<bool> partthrd(thrdv.size());
    vector<Thread*>::const_iterator tit = thrdv.begin();
    for (tit=thrdv.begin(); tit!= thrdv.end(); ++tit) {
        int tid = (*tit)->get_id();
        int process_rank = (*tit)->get_parent()->get_rank();
        partthrd[tid]=(partproc[process_rank]!=invert);
    }

    const vector<Metric*>& metv = theCube->get_metv();
    const vector<Cnode*>& cnodev = theCube->get_cnodev();
    vector<Metric*>::const_iterator mit;
    for (mit=metv.begin(); mit != metv.end(); ++mit) {
        if ((*mit)->get_val() == "VOID") continue; // no values to modify
        if ((*mit)->get_uniq_name() == "visits") continue; // retain visit counts
        if ((*mit)->get_uniq_name() == "imbalance") { // non-VOID!
            cerr << "Warning: Voiding metrics for " << (*mit)->get_disp_name() << "!" << endl;
            (*mit)->set_val("VOID");
        }
        vector<Cnode*>::const_iterator cit;
        for (cit = cnodev.begin(); cit != cnodev.end(); ++cit) {
            //cerr << "Cnode: " << (*cit)->get_callee()->get_name() << endl;
            for (tit=thrdv.begin(); tit!= thrdv.end(); ++tit) {
                int tid = (*tit)->get_id();
                if (partthrd[tid]) continue;
                theCube->set_sev(*mit, *cit, *tit, 0.0);
            }
        }
    }

    cout << "++++++++++++ Part operation ends successfully ++++++++++++++++" << endl;
    
    cout << "Writing " << output << " ... " << flush;
    #ifndef CUBE_COMPRESSED
    ofstream out;
    #else
    gzofstream out;
    #endif
    out.open(output);
    if (!out.good()) {
        cerr << "Error: open " << output << endl;
        exit(1);
    }
    
    out << *theCube;
    out.close();
    cout << "done." << endl;
  }
  catch (RuntimeError& err)
  {
    cerr << err.get_msg() << endl;
  }
  return 0;
}
