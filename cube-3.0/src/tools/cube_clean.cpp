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
* \file cube_clean.cpp
* \brief Creates a clean copy of input cube and saves it in "clean.cube|.gz" file.
*
*/
/******************************************

  Performance Algebra Operation: CLEAN 

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
* - Read the .cube input file
* - Calls cube_clean(...) to create a "cleaned" copy of the cube.
* - Saves a new cube in the file "clean.cube|.gz"
* - end.
*/
int main(int argc, char* argv[]) {
  int ch;
  bool subset   = true;
  bool collapse = false;
  vector <string> inputs;
#ifndef CUBE_COMPRESSED
  const char *output = "clean.cube";
#else
  const char *output = "clean.cube.gz";
#endif
       
  const string USAGE ="Usage: " + string(argv[0]) + " [-o output] [-h] <cube experiment>\n"
    "  -o     Name of the output file (default: " + output +")\n"
    "  -h     Help; Output a brief help message.\n" 
    ;
  
  while ((ch = getopt(argc, argv, "o:h?")) != -1) {
    switch (ch)
      {
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
	exit (0);
      }
  }
  
  if ( argc - optind != 1) {
    cerr<<USAGE<<"\nError: Wrong arguments.\n";
    exit (0);
  }

  for (int i = optind; i < argc; i++) {
    inputs.push_back(argv[i]);
  }
  

  Cube *inCube, *outCube;
  inCube = NULL;
  outCube = NULL;
  try
  {

    cout << "++++++++++++ Clean operation begins ++++++++++++++++++++++++++" << endl;
    
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
    cout << "done." << endl;
    // Write it 
    outCube = cube_clean(inCube, subset, collapse);
    if (inCube != NULL) delete inCube;
    
    cout << "++++++++++++ Clean operation ends successfully ++++++++++++++++" << endl;
    
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
}
