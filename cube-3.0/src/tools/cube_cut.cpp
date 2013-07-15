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
* \file cube_cut.cpp
* \brief Cuts, reroots or prunes the calltree of a cube.
*
*/
/******************************************

  Performance Algebra Operation: CUT (Calltree)

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

#include "Cube.h"
#include "Cnode.h"
#include "Filter.h"
#include "Machine.h"
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
* - Read the  .cube input file.
* - Calls  cube_cut(...) to cut the calltree.
* - Saves the result in "-o outputfile" or "cut.cube|.gz" file.
* - end.
*/
int main(int argc, char* argv[]) {
  int ch;
  bool subset   = true;
  bool collapse = false;
  vector <string> inputs;
  vector <string> prunes;
  Filter filter = Filter();
  string root;
#ifndef CUBE_COMPRESSED
  const char *output = "cut.cube";
#else
  const char *output = "cut.cube.gz";
#endif
       
  const string USAGE ="Usage: " + string(argv[0]) +
    " [-h] [-r nodename] [-p nodename] [-f file] [-o output] <cube experiment>\n"
    "  -r     Re-root calltree at named node\n"
    "  -p     Prune calltree from named node (== \"inline\")\n"
    "  -f     Apply the filter \"file\" to the file after re-rooting or pruning\n"
    "         it. This operation can also be used alone.\n"
    "  -o     Name of the output file (default: " + output +")\n"
    "  -h     Help; Show this brief help message and exit.\n" 
    ;
  
  while ((ch = getopt(argc, argv, "r:p:f:o:h")) != -1) {
    switch (ch)
      {
      case 'o':
	output = optarg;
	break;
      case 'f':
        filter.add_file(string(optarg));
        break;
      case 'p':
        prunes.push_back(optarg);
        break;
      case 'r':
        root = optarg;
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

  if ((root == "") && (prunes.size() == 0) && (filter.empty())) {
    cerr <<"Must specify new root node and/or node(s) to prune" << endl;
    exit(2);
  }

  for (size_t p=0; p<prunes.size(); p++) if (prunes[p] == root) {
    cerr <<"Can't both reroot and prune node '" << root << "'" << endl;
    exit(2);
  }

  Cube *inCube, *outCube, *tmp;
  inCube = NULL;
  outCube = NULL;
  tmp = NULL;
  try 
  {
    inCube = new Cube();
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
    in >> *inCube;
    cout<<"done."<<endl;
    
    cout << "++++++++++++ Cut operation begins ++++++++++++++++++++++++++" << endl;
    
    tmp = new Cube();
    outCube = cube_cut(tmp, inCube, subset, collapse, root, prunes);
    if (tmp != NULL) delete tmp;
    if (filter.empty() == false)
    {
        Cube* filtered = new Cube();
        CubeMapping mapping;
        cube_apply_filter(*filtered, *outCube, filter, mapping);
        delete outCube;
        outCube = filtered;
    }
    if (inCube!= NULL) delete inCube;
    
    if (outCube == NULL) {
        cerr << "Error: Failed to locate new root node '" << root << "'" << endl;
        exit(2);
    }
    
    cout << "++++++++++++ Cut operation ends successfully ++++++++++++++++" << endl;
    
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
    
    out << *outCube;
    out.close();
    cout << "done." << endl;
  }
  catch (RuntimeError& err)
  {
    cerr << err.get_msg() << endl;
  }
  if (outCube != NULL) delete outCube;
  return 0;
}
