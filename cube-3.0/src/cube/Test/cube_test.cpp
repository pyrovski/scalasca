/****************************************************************************
**  Copyright (C) 2005                                                     **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
* \file cube_test.cpp
* \brief An example how to create and save a cube.
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Cube.h"
#include "Metric.h"
#include "Cnode.h"
#include "Process.h"
#include "Node.h"
#include "Thread.h"
#include "Cartesian.h"

using namespace std;
using namespace cube;

/** Debug: only used for Metric, Cnode */
template<class T>
void traverse_print(T root) {
  cout << root->get_id() << endl;
  for(int i = 0; i < root->num_children(); i++) {
    traverse_print<T>((T) root->get_child(i));
  }
}

// example code.
/**
* Main program. 
* - Create an object "cube"
* - Specify mirrors for onilne help.
* - Specify information related to the file (optional)
* - Build metric tree 
* - Build call tree
* - Build location tree
* - Severity mapping
* - Building a topology
*    - create 1st cartesian.
*    - create 2nd cartesian
* - Output to a cube file
*
* - For a test read the cube again and save it in another file.
* - end.
*/
int main(int argc, char* argv[]) {
  Metric *met0, *met1, *met2;
  Region *regn0, *regn1, *regn2;
  Cnode  *cnode0, *cnode1, *cnode2;
  Machine* mach;
  Node* node;
  Process* proc0, *proc1;
  Thread *thrd0, *thrd1;

  Cube cube;

  // Specify mirrors (optional)
  cube.def_mirror("http://icl.cs.utk.edu/software/kojak/");

  // Specify information related to the file (optional)
  cube.def_attr("experiment time", "November 1st, 2004");
  cube.def_attr("description", "a simple example");

  // Build metric tree 
  met0 = cube.def_met("Time", "Uniq_name1", "", "sec", "", 
		      "@mirror@patterns-2.1.html#execution", 
		      "root node", NULL); // using mirror
  met1 = cube.def_met("User time", "Uniq_name2", "", "sec", "",  
                      "http://www.cs.utk.edu/usr.html", 
                      "2nd level", met0); // without using mirror
  met2 = cube.def_met("System time", "Uniq_name3", "", "sec", "", 
                       "http://www.cs.utk.edu/sys.html", 
                      "2nd level", met0); // without using mirror
 
  // Build call tree
  string mod    = "/ICL/CUBE/example.c";
  regn0  = cube.def_region("main", 21, 100, "", "1st level", mod);
  regn1  = cube.def_region("foo", 1, 10, "", "2nd level", mod);
  regn2  = cube.def_region("bar", 11, 20, "", "2nd level", mod);

  cnode0 = cube.def_cnode(regn0, mod, 21, NULL);
  cnode1 = cube.def_cnode(regn1, mod, 60, cnode0);
  cnode2 = cube.def_cnode(regn2, mod, 80, cnode0);

  // Build location tree
  mach  = cube.def_mach("MSC", "");
  node  = cube.def_node("Athena", mach);
  proc0 = cube.def_proc("Process 0", 0, node);
  proc1 = cube.def_proc("Process 1", 1, node);
  thrd0 = cube.def_thrd("Thread 0", 0, proc0);
  thrd1 = cube.def_thrd("Thread 1", 1, proc1);


  // Severity mapping

  cube.set_sev(met0, cnode0, thrd0, 4);
  cube.set_sev(met0, cnode0, thrd1, 0);
  cube.set_sev(met0, cnode1, thrd0, 5);
  cube.set_sev(met0, cnode1, thrd1, 9);
  cube.set_sev(met1, cnode0, thrd0, 2);
  cube.set_sev(met1, cnode0, thrd1, 1);
  // unset severities default to zero
  cube.set_sev(met1, cnode1, thrd1, 3);

  // building a topology
  // create 1st cartesian.
  int ndims = 2;
  vector<long> dimv;
  vector<bool> periodv;
  for (int i = 0; i < ndims; i++) {
    dimv.push_back(5);
    if (i % 2 == 0)
      periodv.push_back(true);
    else 
      periodv.push_back(false);
  }
  std::vector<std::string> namedims;
  namedims.push_back("first");
  namedims.push_back("second");     // comment this and no names will be recorded. The vector must have the
                                    // exact number of dimensions present in the current topology.
  //  namedims.push_back("third");  // uncomment this and no names at all will be recorded
  
  
  Cartesian* cart = cube.def_cart(ndims, dimv, periodv);
  cart->set_namedims(namedims);
  vector<long> p[2];
  p[0].push_back(0);
  p[0].push_back(0);
  p[1].push_back(2);
  p[1].push_back(2);
  cube.def_coords(cart, thrd1, p[0]);
  // create 2nd cartesian
  ndims = 2;
  vector<long> dimv2;
  vector<bool> periodv2;
  for (int i = 0; i < ndims; i++) {
    dimv2.push_back(3);
    if (i % 2 == 0)
      periodv2.push_back(true);
    else 
      periodv2.push_back(false);
  }

  cart->set_name("test1");
  Cartesian* cart2 = cube.def_cart(ndims, dimv2, periodv2);
  vector<long> p2[2];
  p2[0].push_back(0);
  p2[0].push_back(1);
  p2[1].push_back(1);
  p2[1].push_back(0);
  cube.def_coords(cart2, thrd0, p2[0]);
  cube.def_coords(cart2, thrd1, p2[1]);
  cart2->set_name("Test2");
  // Output to a cube file
  ofstream out;
  out.open("example.cube");
  out << cube;

  // Read it (example.cube) in and write it to another file (example2.cube)
  ifstream in("example.cube");
  Cube cube2;
  in >> cube2;

  ofstream out2;
  out2.open("example2.cube");
  out2 << cube2;

  Cube cube3(cube2); // Tests the cube's copy constructor.
  ofstream out3;
  out3.open("example3.cube");
  out3 << cube3;


}
