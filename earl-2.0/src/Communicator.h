/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EARL_COMMUNICATOR_H
#define EARL_COMMUNICATOR_H

/*
 *----------------------------------------------------------------------------
 * class Communicator
 *---------------------------------------------------------------------------- 
 */

#include <map>
#include <vector>

namespace earl 
{
  class Location;
  class Process;

  class Communicator
  {
  public:
  
    Communicator(long id, std::vector<Process*>& procv);
  
    long get_id()                 const { return id; }
    long get_rank(const Location* loc)  const;
    long get_nprocs()             const { return procm.size(); }
    long size()                   const { return procm.size(); }
    Process* get_proc(long rank)  const;


  private:

    long id;

    // rank |-> process
    std::map<long, Process*> procm;
    // pid |-> local rank
    std::map<long, long>      pid2rank;
  };
}
#endif










