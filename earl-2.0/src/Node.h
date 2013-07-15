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

#ifndef EARL_NODE_H
#define EARL_NODE_H

/*
 *----------------------------------------------------------------------------
 *
 * class NODE
 *
 *----------------------------------------------------------------------------
 */

#include <map>
#include <string>

namespace earl
{
  class Grid;
  class Machine;
  class Process;

  class Node
  {
  public:

    Node(long        mach_id,
	 long        node_id,
	 std::string name,
	 long        ncpus,
	 double      clckrt) :
      mach_id(mach_id), node_id(node_id), name(name), ncpus(ncpus), clckrt(clckrt), mach(NULL) {}

    long        get_mach_id()      const { return mach_id; }
    long        get_node_id()      const { return node_id; }
    std::string get_name()         const { return name; }
    long        get_ncpus()        const { return ncpus; }
    double      get_clckrt()       const { return clckrt; }
    Machine*    get_mach()         const { return mach; }
    long        get_nprocs()       const { return procm.size(); }
  
  private:
  
    friend class Grid;
    void set_mach(Machine* m)    { this->mach = m; }
    void add_proc(Process* proc);

    long                         mach_id;
    long                         node_id;
    std::string                  name;
    long                         ncpus;
    double                       clckrt;
    Machine*                     mach;
    std::map<long, Process*>     procm;
  };
}
#endif


