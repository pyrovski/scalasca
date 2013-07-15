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
* \file cube_mean.cpp
* \brief Creates a mean (average) cube of many  .cube files.
*
*/
/******************************************

  Performance Algebra Operation: MEAN 

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
* - Check the calling parameters.
* - For every filename it does
* - Read the  .cube file. and saves it on alternate way either in 0-th, 
or 1th place in cube[] array.
* - Calls cube_mean(...) with previous cube (summed) and the current one 
multiplied with a 1/number. 
* - Saves a created mean (average) cube in either "-o output" or "mean.cube|.gz" file.
* - end.
*/
int main(int argc, char* argv[]) {
  int ch;
  bool subset = true;
  bool collapse = false;
  vector <string> inputs;
#ifndef CUBE_COMPRESSED
  const char *output = "mean.cube";
#else
  const char *output = "mean.cube.gz";
#endif
       
  const string USAGE ="Usage: " + string(argv[0]) + " [-o output] [-c] [-C] [-h] <cube experiment> ...\n"
    "  -o     Name of the output file (default: " + output +")\n"
    "  -c     Do not collapse system dimension, if experiments are incompatible\n"
    "  -C     Collapse system dimension!\n"
    "  -h     Help; Output a brief help message.\n" 
    ;

  while ((ch = getopt(argc, argv, "o:cCh?")) != -1) {
    switch (ch)
      {
      case 'o':
	output = optarg;
	break;
      case 'c':
	subset = false;
	break;
      case 'C':
	collapse = true;
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

  if ( argc - optind < 1) {
    cerr<<USAGE<<"\nError: Wrong arguments.\n";
    exit (0);
  }
  
  for (int i = optind; i < argc; i++) {
    inputs.push_back(argv[i]);
  }
  
  Cube *inCube = NULL;
  Cube *outCube = NULL;
  Cube *cube[2];
  cube[0] = NULL;
  cube[1] = NULL;
  try
  {
    cout << "++++++++++++ Mean operation begins ++++++++++++++++++++++++++" << endl;
    int num = inputs.size();
    cube[0] = new Cube ();
    for (int i = 0; i < num; i++) {
        inCube = new Cube();
        cout << "Reading " << inputs[i] << " ... " << flush;
        if (check_file(inputs[i].c_str()) != 0 ) exit(-1);
    #ifndef CUBE_COMPRESSED
        ifstream in(inputs[i].c_str());
    #else
        gzifstream in(inputs[i].c_str(), ios_base::in|ios_base::binary);
    #endif
        if (!in) {
        cerr << "Error: open " << inputs[i] << endl;
        exit(1);
        }
        in >> *inCube;
        cout<<"done."<<endl;
        // Write it 
        cube[(i+1)%2] = cube_mean(cube[i%2], inCube, subset, num, collapse);
        if (cube[i%2]!= NULL) delete cube[i%2];
        if (inCube != NULL) delete inCube;
    }
    outCube = cube[(num)%2];
    cout << "++++++++++++ Mean operation ends successfully ++++++++++++++++" << endl;
    #ifndef CUBE_COMPRESSED
    ofstream out;
    #else
    gzofstream out;
    #endif
    cout << "Writing " << output << " ... " << flush;
    out.open(output);
    if (!out.good()) {
        cerr << "Error: open " << output << endl;
        exit(1);
    }
    out << *outCube;
    out.close();
    cout<<"done."<<endl;
  }
  catch (RuntimeError& err)
  {
    cerr << err.get_msg() << endl;
  }

  if (outCube != NULL) delete outCube;
}
