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

#include "PatternTree.h"

#include <earl.h>
#include <vector>

#include "Analyzer.h"
#include "Pattern.h"

using namespace earl;
using namespace std;

PatternTree::PatternTree(earl::EventTrace* trace, Analyzer& analyzer) 
{
  vector<Pattern*> pv;

  get_patternv(pv, trace, analyzer.get_opt() );
  
  for ( size_t i = 0; i < pv.size(); i++ )
    if ( pv[i]->isenabled() )
      this->add_node(pv[i]->get_id(), pv[i], pv[i]->get_parent_id());
    else
      delete pv[i];

  // register callbacks
  this->regcb(analyzer);
}

PatternTree::~PatternTree() 
{
  vector<long> keyv;
  this->get_keyv(keyv);

  for ( size_t i = 0; i < keyv.size(); i++ )
    delete this->get_data(keyv[i]);
}

void 
PatternTree::regcb(Analyzer& analyzer)
{
  vector<long> keyv;
  this->get_keyv(keyv);

  for ( size_t i = 0; i < keyv.size(); i++ )
    this->get_data(keyv[i])->regcb(analyzer);
}
