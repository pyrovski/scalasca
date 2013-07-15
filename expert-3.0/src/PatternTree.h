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

#ifndef EXP_PATTERNTREE_H
#define EXP_PATTERNTREE_H

/*
 *----------------------------------------------------------------------------
 *
 * class PatternTree
 *
 * Tree of performance properties (i.e., execution patterns)
 *
 *---------------------------------------------------------------------------- 
 */

#include <earl.h>
#include <string>

#include "Tree.h"

class Analyzer;
class Pattern;

class PatternTree : public Tree<Pattern*>
{
 public:

  PatternTree(earl::EventTrace* trace, Analyzer& analyzer);
  ~PatternTree();

 private:

  void regcb(Analyzer& analyzer);
};

#endif






