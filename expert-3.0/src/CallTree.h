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

#ifndef EXP_CALLTREE_H
#define EXP_CALLTREE_H

/*
 *----------------------------------------------------------------------------
 *
 * class CallTree
 *
 *---------------------------------------------------------------------------- 
 */

#include <earl.h>
#include <string>

#include "Tree.h"

struct CallPath
{
  earl::Callsite* csite;
  earl::Region*   callee;
};

class CallTree : public Tree<CallPath*>
{
 public:

  CallTree(earl::EventTrace* trace);
  ~CallTree();

  const earl::Callsite*  get_csite(long cnode_id)  const { return this->get_data(cnode_id)->csite; }
  const earl::Region*    get_callee(long cnode_id) const { return this->get_data(cnode_id)->callee; }
};

#endif






