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
* \file helper.cpp
* \brief Defines a set of helping functions. 
*
*/
#include "helper.h"
#include "Cube.h"
#include "Cnode.h"
#include "Predicates.h"
#include "Region.h"
#include "Thread.h"

#include <map>

using namespace std;

namespace cube
{
/**
* Transforms a callpathtype \f$ \longrightarrow \f$ string.
*/
  string Callpathtype2String(CallpathType ct)
  {
    switch (ct)
    {
    case MPI:
      return "MPI";
    case OMP:
      return "OMP";
    case USR:
      return "USR";
    case COM:
      return "COM";
    case REC:
      return "REC";
    case SEN:
      return "SEN";
    case SENREC:
      return "SENREC";
    case COL:
      return "COL";
    case FORK:
      return "FORK";
    case LOCK:
      return "LOCK";
    case CRIT:
      return "CRIT";
    case EPK:
      return "EPK";
    case NUL:
      return "NUL";
    case MPI_RMA_OP:
      return "MPI_RMA_OP";
    case MPI_RMA_GATS:
      return "MPI_RMA_GATS";
    case MPI_RMA_COLL:
      return "MPI_RMA_COLL";
    case MPI_RMA_LOCK:
      return "MPI_RMA_LOCK";
    case MPI_RMA_UNLOCK:
      return "MPI_RMA_UNLOCK";
    default:
      break;
    }
    return "UNDEF";
  }
/**
* Transforms a string \f$ \longrightarrow \f$ callpathtype.
*/
  CallpathType String2Callpathtype(string name)
  {
    /*
      classify a functionname into two classes:
      MPI (collective, send, receive, MPI) and USR
    */
    string region = lowercase(name);

    if (region.length()<=4)
    {
      return USR;
    }
    else
    {
      string suffix = name.substr(4);

      if (is_mpi_api(region))
      {
        if (is_mpi_coll(region) || is_mpi_sync_coll(region))  
        {
	  return COL;
        }
        else if (is_mpi_rma(region))
        {
          return MPI_RMA_OP;
        }
        else if (region == "mpi_win_fence"  ||
                 region == "mpi_win_create" ||
                 region == "mpi_win_free")
        {
          return MPI_RMA_COLL;
        }
        else if (is_mpi_sync_rma_active(region))
        {
          // Fence is already handled above
          return MPI_RMA_GATS;
        }
        else if (region == "mpi_win_lock")
        {
          return MPI_RMA_LOCK;
        }
        else if (region == "mpi_win_unlock")
        {
          return MPI_RMA_UNLOCK;
        }
        else if (region.substr(0,12) == "mpi_sendrecv")
        {
          return SENREC;
        }
        else if (region.find("send") != string::npos    ||
	         region.find("start") != string::npos)
        {
	  return SEN;
        }
        else if (region.find("recv") != string::npos)
        {
	  return REC;
        }
        else
        {
	  return MPI;
        }
      }
      else if (name.compare(0,4,"omp_")== 0 || name.compare(0,5,"!$omp")==0)
      {
        if (name.compare(0,4,"omp_")==0 && suffix.find("lock") != string::npos)
        {
          return LOCK;
        }
        else if (name.compare(0,5,"!$omp")==0)
        {
          string construct = name.substr(5, name.find_first_of('@', 5) - 5);

          if (construct.find("parallel") != string::npos)
          {
            return FORK;
          }
          if (construct.find("critical") != string::npos)
          {
            return CRIT;
          }
        }
        return OMP;
      }

      return USR;
    }
  }
/**
* Get a callpath in "directory notation" (like func1/func2/func3/../cnode1).
*/
  string get_callpath_for_cnode(const Cnode& cn)
  {
    string callpath;
    Cnode* ptr = cn.get_parent();

    while (true)
    {
      if (ptr == 0) break;
      else
      {
	const Region* region = ptr->get_callee();
	string nname = region->get_name();
	callpath = "/" + nname + callpath;
	ptr = ptr->get_parent();
      }
    }

    const Region* region = cn.get_callee();
    callpath = callpath + "/" + region->get_name();
    return callpath;
  }
/**
* Does sum all values for given metric in a CUBE. 
*/
  double get_atotalt(Cube* input, string metricname)
  {
    const vector<Cnode*>& cnodes = input->get_cnodev();
    const vector<Thread*>& threads= input->get_thrdv();
    Metric* metric = input->get_met(metricname);

    if (metric==0)
      return -1.0;
    
    double total(0.0);
    
    for (size_t i=0; i<cnodes.size(); i++)
    {
      double max(0.0);
      for (size_t j=0; j<threads.size(); j++)
      {
	double nval=input->get_sev(metric, cnodes[i], threads[j]);
	if (nval>max)
	  max=nval;
      }
      total+=max;
    }
    
    return total;
  }
  
/**
* Gets to every  calltype a factor.
*/   
  unsigned long long TypeFactor(std::string name)
  {
    const CallpathType cpType = String2Callpathtype(name);
    unsigned long long d;
    switch (cpType)
    {
    case USR:
      d=24;
      break;
    case COL:
      d=40;
      break;
    case SEN:
      d=50;
      break;
    case REC:
      d=46;
      break;
    case SENREC:
      d=72;
      break;
    case MPI:
      d=24;
      break;
    case OMP:
      d=24;
      break;
    case FORK:
      d=44;
      break;
    case LOCK:
      d=34;
      break;
    case CRIT:
      d=44;
      break;
    case MPI_RMA_OP:
      d=72;
      break;
    case MPI_RMA_GATS:
      d=33;
      break;
    case MPI_RMA_COLL:
      d=28;
      break;
    case MPI_RMA_LOCK:
      d=45;
      break;
    case MPI_RMA_UNLOCK:
      d=44;
      break;
    default:
      d=0;
      break;
    }
    return d;
  }
}
