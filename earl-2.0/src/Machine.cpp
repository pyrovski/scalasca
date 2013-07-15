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

#include "Machine.h"

#include "Error.h"
#include "Node.h"

using namespace earl;
using namespace std;

Node*    
Machine::get_node(long node_id)  const
{
  map<long, Node*>::const_iterator it = nodem.find(node_id);
  
  if (it == nodem.end())
    throw RuntimeError("Error in Machine::get_node(long).");
  else
    return it->second;
}

void 
Machine::add_node(Node* node) 
{ 
  nodem[node->get_node_id()] = node; 
}

