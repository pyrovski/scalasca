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

#ifndef EXP_ANALYZER_H
#define EXP_ANALYZER_H

/*
 *----------------------------------------------------------------------------
 *
 * class Analyzer
 *
 * Analysis process
 *
 *---------------------------------------------------------------------------- 
 */

#include <earl.h>
#include <queue>
#include <string>

#include "CompoundEvents.h"

class Callback;
class ExpertOpt;
class PatternTree;
class Profile;

class Analyzer
{
 public:

  Analyzer(earl::EventTrace* trace, ExpertOpt* opt);
  ~Analyzer();

  // subscription
  void subscribe_event(earl::etype type, Callback* cbobj);
  void subscribe_cmpnd(cetype type, Callback* cbobj);
  
  // start analysis
  void run();

  ExpertOpt* get_opt() const { return expert_opt; }

 private:

  // processing of a single event
  void step(long pos);

  earl::EventTrace* trace;
  PatternTree* ptree;
  Profile* profile;

  std::map<earl::etype, std::vector<Callback*> > eventcbm;
  std::map<cetype, std::vector<Callback*> > ceventcbm; 

  std::queue<cmp_event*> cequeue;

  ExpertOpt* expert_opt;
};

#endif






