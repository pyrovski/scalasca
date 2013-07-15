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

#ifndef EARL_LOCATION_H
#define EARL_LOCATION_H

/*
 *----------------------------------------------------------------------------
 *
 * class Location
 *
 * Hierarchical location.
 *
 *----------------------------------------------------------------------------
 */

namespace earl
{
  class Machine;
  class Node;
  class Process;
  class Thread;

  class Location
  {
  public:

    Location(long        id,  
	     Machine*    mach,
	     Node*       node,
	     Process*    proc,
	     Thread*     thrd) :
      id(id), mach(mach), node(node), proc(proc), thrd(thrd) {}

    long        get_id()    const { return id; }
    Machine*    get_mach()  const { return mach; }
    Node*       get_node()  const { return node; }
    Process*    get_proc()  const { return proc; }
    Thread*     get_thrd()  const { return thrd; }
  
  private:
  
    long        id;
    Machine*    mach;
    Node*       node;
    Process*    proc;
    Thread*     thrd;
  };
}
#endif


