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
* \file cube_cmp.cpp
* \brief Compares two .cube files and prints out if they are equal or not.
*
*/
/******************************************

  Performance Algebra Operation: COMPARE

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
* - Read the first .cube input file and the second file.
* - Calls cube_cmp(...) to compare them.
* - Prints the result in human readable form.
* - end.
*/
int main(int argc, char* argv[]) {
  int ch;
  vector <string> inputs;
       
  const string USAGE ="Usage: " + string(argv[0]) + " [-h] <cube experiment 1> <cube experiment 2>\n"
    "  -h     Help; Output a brief help message.\n" 
    ;  

  while ((ch = getopt(argc, argv, "o:ch?")) != -1) {
    switch (ch)
      {
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
  
  if ( argc - optind == 2) {
    string cur;
    for (int i = optind; i < argc; i++) {
      cur = argv[i];
      inputs.push_back(cur);
    }
  } else {
    cerr<<USAGE<<"Error: Two files required.\n\n";
    exit(0);
  }
  
  Cube *cube1, *cube2;
  cube1 = NULL;
  cube2 = NULL;
  try
  {
    cube1 = new Cube();
    cube2 = new Cube();
    
    cout << "Reading " << inputs[0] << " ... " << flush;
    if (check_file(inputs[0].c_str()) != 0 ) exit(-1);
    #ifndef CUBE_COMPRESSED
    ifstream in1(inputs[0].c_str());
    #else
    gzifstream in1(inputs[0].c_str(), ios_base::in|ios_base::binary);
    #endif
    if (!in1) {
        cerr << "Error: open " << inputs[0] << endl;
        exit(1);
    }
    in1 >> *cube1;
    cout<<"done."<<endl;
    
    cout << "Reading " << inputs[1] << " ... " << flush;
    if (check_file(inputs[1].c_str()) != 0 ) exit(-1);
    #ifndef CUBE_COMPRESSED
    ifstream in2(inputs[1].c_str());
    #else
    gzifstream in2(inputs[1].c_str(), ios_base::in|ios_base::binary);
    #endif
    if (!in2) {
        cerr << "Error: open " << inputs[1] << endl;
        exit(1);
    }
    in2 >> *cube2;
    cout << "done." << endl;
    
    // Write it 
    cout << "++++++++++++ Compare operation begins ++++++++++++++++++++++++++" << endl;
    
    if (cube_cmp (cube1, cube2)) {
        cout << "Experiments are equal." << endl;
    } else {
        cout << "Experiments are not equal." << endl;
    }
    
    cout << "+++++++++++++ Compare operation ends successfully ++++++++++++++++" << endl;
    }
    catch (RuntimeError& err)
    {
        cerr << err.get_msg() << endl;
    }
  if (cube1 != NULL) delete cube1;
  if (cube2 != NULL) delete cube2;
}
