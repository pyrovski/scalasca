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
* \file cube_remap.cpp
* \brief Remaps a cube and saves it in new "remap.cube|.gz" file.
*/
/******************************************

  Performance Algebra Operation: REMAP

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
* - Read the  .cube input file.
* - Calls cube_remap(...) to create a clone of input cube and calculate exclusive 
and inclusive values.
* - Saves the result in "-o outputfile" or "remap.cube|.gz" file.
* - end.
*/
int main(int argc, char* argv[]) {
  int ch;
#ifndef CUBE_COMPRESSED
  const char *output = "remap.cube";
#else
  const char *output = "remap.cube.gz";
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

  string filename = argv[optind];
  string epikname = filename.substr(0,filename.rfind('/'));
  if (epikname.substr(epikname.rfind('/')+1,5) != "epik_") epikname.clear();

  Cube *inCube, *outCube;
  inCube = NULL;
  outCube= NULL;
  cout << "++++++++++++ Remapping operation begins ++++++++++++++++++++++++++" << endl;
  try
  {
    inCube = new Cube();
    cout << "Reading " << filename << " ... " << flush;
    if (check_file(filename.c_str()) != 0 ) exit(-1);
    #ifndef CUBE_COMPRESSED
    ifstream in(filename.c_str());
    #else
    gzifstream in(filename.c_str(), ios_base::in|ios_base::binary);
    #endif
    if (!in) {
        cerr << "Error: open " << filename << endl;
        exit(1);
    }
    in >> *inCube;
    cout<<" done."<<endl;
    outCube = cube_remap(inCube, epikname, false);
    if (inCube != NULL) delete inCube;
    
    cout << "++++++++++++ Remapping operation ends successfully ++++++++++++++++" << endl;
    
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
