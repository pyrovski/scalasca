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

#include "CallTree.h"

#include <earl.h>
#include <vector>

using namespace earl;
using namespace std;

CallTree::CallTree(EventTrace* trace) 
{
  long cnode_id, parent_id;
  CallPath* cpath;
  vector<Event> out;
  
  trace->calltree(out);
  for ( size_t i = 0; i < out.size(); i++ )
    {
      cpath = new CallPath();
      cpath->csite  = out[i].get_csite();
      cpath->callee = out[i].get_reg();
      
      cnode_id = out[i].get_pos();
      if ( out[i].get_cedgeptr().null() )
	parent_id = -1;
      else
	parent_id = out[i].get_cedgeptr().get_pos(); 
      
      this->add_node(cnode_id, cpath, parent_id);
    }
}

CallTree::~CallTree() 
{
  vector<long> keyv;
  this->get_keyv(keyv);

  for ( size_t i = 0; i < keyv.size(); i++ )
    delete this->get_data(keyv[i]);
}
