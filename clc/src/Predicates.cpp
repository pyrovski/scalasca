/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <cctype>

#include <mpi.h>

#include <pearl.h>
#include <Event.h>
#include <Region.h>
#include <Location.h>
#include <MpiComm.h>
#include <MpiGroup.h>

#include <Predicates.h>

using namespace std;
using namespace pearl;

namespace {

  struct fo_tolower : public std::unary_function<int,int> {
    int operator()(int x) const {
      return std::tolower(x);
    }
  };

}

/**
 ** Helper function for isNx1() and is1xN()
 **/

string lowercase(const string& str) {
  string result(str);
  transform(str.begin(), str.end(), result.begin(), fo_tolower());
  return result;
}

/**
 ** Returns TRUE if data needs to be sent
 **/

bool isSend(const Event& event, int rank) {
  return (
	  (event->is_typeof(MPI_SEND))
	  ||
	  (
	   (event.exitptr()->get_type() == MPI_COLLEXIT) 
	   && 
	   ( (isNxN(event.exitptr())) 
	     ||
	     (isNx1(event) && !isOnRoot(event.exitptr(), rank))
	     || 
	     (is1xN(event) && isOnRoot(event.exitptr(), rank))
	     || 
	     (is_mpi_scan(event) || is_mpi_exscan(event))
	   )
	  )
	  || 
	  (
	   (event.exitptr()->get_type() == OMP_COLLEXIT)
	   &&
	   (isNxN(event.exitptr()))
	  )
	 );
}

/**
 ** Returns TRUE if data needs to be received
 **/

bool isRecv(const Event& event, int rank) {
  return (
	  (event->is_typeof(MPI_RECV))
	  ||
	  (
	   (event->get_type() == MPI_COLLEXIT) 
	   && 
	   ( (isNxN(event)) 
	     ||
	     (isNx1(event.enterptr()) && isOnRoot(event, rank))
	     || 
	     (is1xN(event.enterptr()) && !isOnRoot(event, rank))
	     ||
	     (is_mpi_scan(event.enterptr()) || is_mpi_exscan(event.enterptr()))
	   )
	  )
	  || 
	  (
	   (event->get_type() == OMP_COLLEXIT)
	   &&
	   (isNxN(event))
	  )
	 );
}

/**
 ** Returns TRUE if the EXIT event is an NxN event.
 **/

bool isNxN(const Event& event) {

  string name = lowercase(event.enterptr()->get_region()->get_name());
  if (event->get_type() == MPI_COLLEXIT) {
    if (event->get_root() == PEARL_NO_ID) {

      // MPI_Barrier
      if (!name.compare(0, 11, "mpi_barrier")) {
	return true;
      }

      // Partial reduction case is not a NxN situation
      if (!name.compare(0, 8, "mpi_scan")) {
	return false;
      }
      if (!name.compare(0, 10, "mpi_exscan")) {
	return false;
      }
      
      // Ignore variable lenght all-ato-all operations
      if (!name.compare(0, 13, "mpi_alltoallv")  ||
	  !name.compare(0, 13, "mpi_alltoallw")) {
	return false;
      }

      if (!name.compare(0, 18, "mpi_reduce_scatter")) {
	return true;
      }

      // Handle MPI_Allgather, MPI_Allgatherv, MPI_Alltoall, MPI_Allreduce if data received
      if (event->get_received() != 0){
	return true;
      }
    }
  } else if (event->get_type() == OMP_COLLEXIT) {
    if (name.compare(0, 5, "!$omp") != 0)
      return false;
    string suffix = name.substr(7);
    if (suffix.compare(0, 7, "barrier") == 0)
      return true;
  }
  return false;
}

/**
 ** Returns TRUE if the ENTER event is a 1xN event.
 */

bool is1xN(const Event& event) {

  string name = lowercase(event->get_region()->get_name());
  if (name.compare(0, 4, "mpi_") != 0)
    return false;
 
  string suffix = name.substr(4);
  
    if (suffix == "bcast" ||
	suffix == "scatter" ||
	suffix == "scatterv") 
      return true;

  return false;
}

/**
 ** Returns TRUE if the ENTER event is a Nx1 event.
 **/

bool isNx1(const Event& event) {

  string name = lowercase(event->get_region()->get_name());
  if (name.compare(0, 4, "mpi_") != 0)
    return false;
 
  string suffix = name.substr(4);

  if (suffix == "gather" ||
      suffix == "gatherv" ||
      suffix == "reduce")
    return true;
  
  return false;
}

/**
 ** Returns TRUE for entering and exiting a OMP parallel region
 **/
bool isParallel(const Event& event) {
  string name;
  if (event->get_type() == ENTER)
    name = lowercase(event->get_region()->get_name());
  else if (event->get_type() == OMP_COLLEXIT)
    name = lowercase(event.enterptr()->get_region()->get_name());
  else 
    return false;

  if (name.compare(0, 5, "!$omp") != 0)
    return false;
 
  string suffix = name.substr(6);

  if (suffix.compare(0, 8, "parallel") == 0)
      return true;
 
  return false;
}

/**
 ** Returns TRUE if the MPI_COLLEXIT event is on the
 ** root process of a collective opperation.
 */

bool isOnRoot(const Event& event, int rank) {
  return (event->get_root() == static_cast<uint32_t>(rank));
}

/**
 ** Returns TRUE if clc_root
 **/
bool isclcRoot(clc_location loc) {
  if (loc.machine == 0 && loc.node == 0 && loc.process == 0 && loc.thread == 0)
    return true;
  return false;  
}

/**
 ** Returns TRUE if MPI_Exscan
 **/
bool is_mpi_scan (const pearl::Event& event) {

  string name = lowercase(event->get_region()->get_name());
  if (name.compare(0, 4, "mpi_") != 0)
    return false;

  string suffix = name.substr(4);

  if (suffix == "scan")
    return true;

  return false;

}

/**
 ** Returns TRUE if MPI_Exscan
 **/
bool is_mpi_exscan (const pearl::Event& event) {

  string name = lowercase(event->get_region()->get_name());
  if (name.compare(0, 4, "mpi_") != 0)
    return false;

  string suffix = name.substr(4);

  if (suffix == "exscan")
    return true;

  return false;

}
