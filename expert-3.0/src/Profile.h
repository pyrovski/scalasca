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

#ifndef EXP_PROFILE_H
#define EXP_PROFILE_H

/*
 *----------------------------------------------------------------------------
 *
 * class Profile
 *
 * Mapping (call path, location) onto total execution time and all
 * hardware counters. 
 *                               
 *---------------------------------------------------------------------------- 
 */

#include <earl.h>
#include <vector>

#include "Callback.h"
#include "Severity.h"

class CallTree;
class Analyzer;

struct cmp_event;

class Profile : public Callback
{
 public:

  Profile(earl::EventTrace* trace, Analyzer& analyzer);
  virtual ~Profile() {}

  // callbacks
  struct cmp_event* fork(earl::Event& event);
  struct cmp_event* join(earl::Event& event);
  struct cmp_event* exit(earl::Event& event);
  
  // transform data in exclusive mode with respect to the call tree
  void exclusify(CallTree& ctree);

  // get profile data
  double    get_time(long cnode_id = -1, long loc_id = -1);
  long long get_visits(long cnode_id = -1, long loc_id = -1);
  long long get_metval(long met_id, long cnode_id = -1, long loc_id = -1);
  bool      is_multhrd(long cnode_id = -1, long loc_id = -1);

 private:

  earl::EventTrace* trace;

  Severity<double>    time_sev;
  Severity<long long> visits_sev;
  Severity<int>       multhrd_sev;

  std::vector<Severity<long long> > met_sevv;

  // proc_id |-> executing in a parallel region or not
  std::vector<bool> ismulthrd;
};

#endif






