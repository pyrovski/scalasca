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

#ifndef EARL_MACHINE_H
#define EARL_MACHINE_H

/*
 *----------------------------------------------------------------------------
 *
 * class Machine
 *
 *----------------------------------------------------------------------------
 */

#include <map>
#include <string>

namespace earl
{
  class Grid;
  class Node;

  class Machine
  {
  public:

    Machine(long        id,  
	    std::string name) :
      id(id), name(name) {}

    long        get_id()       const { return id; }
    std::string get_name()     const { return name; }
    long        get_nnodes()   const { return nodem.size() ; }
    Node*       get_node(long node_id) const;
  
  private:
  
    friend class Grid;
    void add_node(Node* node); 

    long                     id;
    std::string              name; 
    std::map<long, Node*>    nodem;
  };
}
#endif


