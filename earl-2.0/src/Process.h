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

#ifndef EARL_PROCESS_H
#define EARL_PROCESS_H

/*
 *----------------------------------------------------------------------------
 *
 * class Process
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <map>

namespace earl
{
  class Grid;
  class Location;
  class Node;
  class Thread;

  class Process
  {
  public:

    Process(long        id,  
	    std::string name) :
      id(id), name(name) { }

    long        get_id()             const { return id; }
    std::string get_name()           const { return name; }
    Node*       get_node()           const { return node; }
    long        get_nthrds()         const { return thrdm.size() ; }
    Thread*     get_thrd(long thrd_id) const;
    Location*   get_loc()            const;     

  private:
  
    friend class Grid;
    void set_name(std::string str)  { name = str; } 
    void set_node(Node* n)          { this->node = n; }
    void add_thrd(Thread* thrd);
  
    long                    id;
    std::string             name;
    Node*                   node;
    std::map<long, Thread*> thrdm;
  };
}
#endif


