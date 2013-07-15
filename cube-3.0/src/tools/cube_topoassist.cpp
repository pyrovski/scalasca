/****************************************************************************
 **  SCALASCA    http://www.scalasca.org/                                   **
 *****************************************************************************
 **  Copyright (c) 1998-2013                                                **
 **  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
 **                                                                         **
 **  See the file COPYRIGHT in the package base directory for details       **
 **  Small assistant to include virtual topologies into an existing cube    **
 ****************************************************************************/

#ifdef CUBE_COMPRESSED
#  include "zfstream.h"
#endif

#include <cstdio>
#include <cstdlib>
#include <unistd.h> 
#include <fstream>
#include "cube_topoassist.h"
#include "Question.cpp"
using namespace std;
using namespace cube;

/*-------------------------------------------------------------------------*/
/**
 * @file    cube_topoassist.cpp
 * @brief   Assistant to manually add virtual topologies in cube files
 *
 * With this assistant, one is able to add an arbitrary virtual topology to a cube file.
 * So far, this topology might have up to three dimensions (it's a CUBE limitation, actually)
 * and the total number of possible coordinates must NOT be smaller than the number of processes/threads
 * of the specifed CUBE file.
 *
 * Coordinates are asked for in rank order, and inside every rank, in thread order.
 */
/*-------------------------------------------------------------------------*/






void write_cube(Cube * inCube) {
  /**
   * writes cube object to disk.
   * @param cubeObj The CUBE object to be saved to disk.
   */

#ifndef CUBE_COMPRESSED
  const char *output = "topo.cube";
#else
  const char *output = "topo.cube.gz";
#endif

  cout << "Writing " << output << " ... " << flush;
#ifndef CUBE_COMPRESSED
  ofstream out;
#else
  gzofstream out;
#endif
  out.open(output);
  if (!out.good()) {
    cerr << "Error: open " << output << endl;
    exit(EXIT_FAILURE);
  }

  out << *inCube;
  out.close();
  cout << "done." << endl;


}

void create_topology(Cube * inCube) {

  /**
   * Reads the process vector, sorts it into a map of rank->process and ask for coordinates for the new topology.
   * @param cubeObj The CUBE object to add a topology.
   */
  std::string tempStrQuestion, tempStrAnswer = "";  
  std::string dimperiod;
  size_t num_elements = 1,dimtemp=0;
  unsigned int num_dims=0; 
  std::vector<long> dims;
  std::vector<bool> periods;
  std::vector<std::string> nameDims;

  std::cout << "So far, only cartesian topologies are accepted." << std::endl;

  std::string name;
  question(std::string("Name for new topology?"),&name);
  
  // In a future expansion, I should support Graph topologies too

  question("Number of Dimensions?",&num_dims);
  
  if (num_dims > 3) {
    std::cerr
      << "Currently, Cube will not show more than 3 dimensions."
      << std::endl;
  }

  bool areDimsNamed; 
  bool isDimPeriodic;

  question("Do you want to name the dimensions (axis) of this topology? (Y/N)",&areDimsNamed);

  for (size_t i = 0; i < num_dims; i++) {
    if (areDimsNamed==true){
      question(i,"Name for dimension %i",&tempStrAnswer);
      nameDims.push_back(tempStrAnswer);
    }
      
    do {
      question(i,"Number of elements for dimension %i",&dimtemp);
    } while(dimtemp<=0);
    
    num_elements = num_elements * dimtemp;
    dims.push_back(dimtemp);
    
    question(i,"Is dimension %i periodic?",&isDimPeriodic);
    periods.push_back(isDimPeriodic);
  } // End dimension definition stuff


  Cartesian* topo = inCube->def_cart(dims.size(), dims, periods);

  topo->set_name(name);
  topo->set_namedims(nameDims);
  
  // Define topology
  const vector<Process*>& procv = inCube->get_procv();
  const vector<Thread*>& thrdv = inCube->get_thrdv();

  map<long, long> procv_ranks;
  for (size_t i = 0; i < procv.size(); i++)
    procv_ranks[procv[i]->get_rank()] = i;
  
    // Number of processes/threads should be same or smaller than the number of possible coordinates for them.
    if (num_elements < thrdv.size()) {
      std::cout
          << "The number of possible coordinates with this topology is smaller then the number of threads ("
          << thrdv.size() << "). Cannot continue" << std::endl;
      exit(0);
    }
    if (num_elements > thrdv.size()) {
      std::cout << "Alert: The number of possible coordinates ("
          << num_elements
          << ") is bigger than the number of threads on the specified cube file ("
          << thrdv.size() << "). Some positions will stay empty." << std::endl;
    }
    std::cout << "Topology on THREAD level." << std::endl;
    Thread* thrd;
    Process* proc;

    // It will iterate over the process vector and get all the children

    for (size_t j = 0; j < procv.size(); ++j) {
      proc = procv[procv_ranks[j]];
      for (size_t k = 0; k < proc->num_children(); k++) {
        thrd = proc->get_child(k);
	std::string q=thrd->get_name() + "'s (rank " + intToStr(proc->get_rank()) +  ") coordinates in %i dimensions, separated by spaces";
	question(num_dims,q,&tempStrQuestion);
	std::stringstream ss( tempStrQuestion);
	long l = 0;
	std::vector<long> values;
	for (size_t i = 0; i < num_dims; i++) {
	  ss >> l;
	  if (l + 1 > dims[i]) {
	    std::cerr << "Invalid coordinate. Dimension " << i
		      << " is of size " << dims[i] << ", beginning in zero. You typed " << l
		      << ". Exiting." << std::endl;
	    exit(-1);
	  }
	  values.push_back(l);
	}
	inCube->def_coords(topo, thrd, values);
      }
    }
}

void show_topologies(std::vector<Cartesian *> topologies) {
  /**
   * Displays information of topologies: name, # of dimensions, their names and total threads. 
   * Used to rename a topology or a topology's dimensions at rename_topology and rename_dimensions
   * @param topologies The topology vector from cube file with get_cartv(). 
   */

  std::vector<std::string> names, namedims; size_t num_threads=1;

  std::cout << "This CUBE has " << topologies.size() << " topologie(s)." << std::endl;

  // Iterates over every topology and displays their names, number of dimensions, number of coordinates on each etc.
  for(size_t i=0;i<topologies.size();i++) {  
    names.push_back(topologies[i]->get_name());
    namedims=topologies[i]->get_namedims();
    if(names[i]=="") {
      std::cout << i << ". " << "<Unnamed topology>, ";
    } else {
      std::cout << i << ". " <<  names[i] << ", ";
    }
    std::cout << (topologies[i])->get_ndims() << " dimensions: "; 
    for(int j=0;j<topologies[i]->get_ndims();j++) { 
      if(namedims.size()!=0) { std::cout << namedims[j] << ": "; }
      std::cout << topologies[i]->get_dimv()[j] << ((j+1<topologies[i]->get_ndims())?", ":".");
      num_threads=num_threads * topologies[i]->get_dimv()[j]; 
    }
    std::cout << " Total = " << num_threads << " threads. ";
    if(namedims.size()==0) { std::cout << "<Dimensions are not named>";}
    std::cout << std::endl;
    num_threads=1; // reset counter.
  }
}

void rename_topology(Cube * inCube) {
  /**
   * Traverses cube's topologies, shows the current names of them and offer to name/rename them
   * @param cubeObj The CUBE object to be changed.
   */
  std::vector<Cartesian *> topologies = inCube->get_cartv();
  show_topologies(topologies);
  unsigned int index_topo;
  std::string new_name;
  question("\nTopology to [re]name?",&index_topo);
  question("New name: ",&new_name);
  topologies[index_topo]->set_name(new_name);
  std::cout << "Topology successfully [re]named." << std::endl << std::endl;
}

void rename_dimensions(Cube * inCube) {

  /**
   * Traverses cube's topologies, shows the current names of them and offer to name/rename dimensions of one of them
   * @param cubeObj The CUBE object to be changed.
   */
  // GET_NAMEDIMS
  std::vector<Cartesian *> topologies = inCube->get_cartv();
  show_topologies(topologies);
  unsigned int index_topo;
  std::vector<std::string> new_namedims;
  std::string new_name="";
  question("\nIn which topology do you want to [re]name dimensions?",&index_topo);
  for(int dim=0;dim<topologies[index_topo]->get_ndims();dim++) {
    question(dim, "New name for dimension %i : ",&new_name); 
    new_namedims.push_back(new_name);
    new_name="";
  }
  if (!topologies[index_topo]->set_namedims(new_namedims)) {
    std::cout << "Could not rename dimensions."; 
    exit(EXIT_FAILURE);
  } else { std::cout << "Dimensions successfully [re]named" << std::endl << std::endl; } 

}

int main(int argc, char ** argv) {

  int rename_topology_flag = 0, rename_dimensions_flag = 0, create_topology_flag=0;
  string cube_name="";
  int c; // option to name existing topology or dimensions
  opterr=1;
  int index;

  string usage("Usage: cube3_topoassist [opts] <cube file>\n \
Command-line switches:\n \
-c : create a new topology in the existing CUBE file.\n \
-n : [re]name an existing topology\n \
-d : [re]name dimensions of a topology \n \
\nThe output is a topo.cube[.gz] file in the current directory.");

  // Parses arguments for an existing cube file.
  for ( index=1 ; index < argc; index++) {
    if(!access(argv[index], F_OK))
      cube_name=argv[index];
  }

  if(cube_name== "") {	cerr << "ERROR: Missing file name. " << std::endl << usage;
    exit(EXIT_FAILURE);
  }


  // Parses command-line options
  while ((c = getopt (argc, argv, "ndc")) != -1) {
    switch (c)
      {
      case 'n':
	rename_topology_flag++;
	break;
      case 'd':
	rename_dimensions_flag++;
	break;
      case 'c':
	create_topology_flag++;
	break;
      case '?':
	  cerr << usage;
	  exit(EXIT_FAILURE);
      default:
	exit(EXIT_FAILURE);
      }
  }
  // No options
  if(rename_topology_flag==0 &&	    \
     rename_dimensions_flag==0 && \
     create_topology_flag==0) {
    std::cout <<  usage << std::endl;
    exit(EXIT_FAILURE);
  }
  // Read CUBE file
  Cube *inCube = new Cube();
  cout << "Reading " << cube_name << " . Please wait... " << flush;
#ifndef CUBE_COMPRESSED
  ifstream in(cube_name.c_str());
#else
  gzifstream in(cube_name.c_str(), ios_base::in|ios_base::binary);
#endif
  if (!in) {
    cerr << "Error: open " << cube_name << endl;
    exit(1);
  }
  try { 
    in >> *inCube;
  } 
  catch (...)
    {
      exit(-1);
    }

  std::cout << "Done." << std::endl << "Processes are ordered by rank. \
For more information about this file, use cube3_info -S <cube experiment>\n" << std::endl;


  if(rename_topology_flag) {
    rename_topology(inCube);
}
  if(rename_dimensions_flag) {
    rename_dimensions(inCube);
}
  if(create_topology_flag) {
    create_topology(inCube);
  }



  write_cube(inCube);
  delete inCube; 

}

