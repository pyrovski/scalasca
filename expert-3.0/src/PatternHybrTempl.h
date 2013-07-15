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

#ifndef EXP_PATTERN_HYBR_TEMPL_H
#define EXP_PATTERN_HYBR_TEMPL_H

#include "Pattern.h"


#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <string>
 
// earl
#include "earl.h"
#include "earl_dump.h"

// expert
#include "Analyzer.h"
#include "CallTree.h"
#include "Profile.h"
#include "StatisticController.h"

/****************************************************************************
*
* Profiling pattern template class
*
* All profiling classes shares the implementation of end callback function.
* 'isappropriate' function differs on each Profiling class, but is known 
* to compile time
*
****************************************************************************/


// 
// 
// 
template <long id> 
struct ProfilingPatternTraits
{
};

template <long id> 
struct WaitAtPatternTraits
{
};

template <long id, typename T = ProfilingPatternTraits<id> >
class ProfilingPatternT : public Pattern
{
 public:
  ProfilingPatternT(earl::EventTrace* trace) : Pattern(trace) {}

  long get_id()                  { return id;                  }
  long get_parent_id()           { return T::get_parent_id();  }
  std::string get_name()         { return T::get_name();       }
  std::string get_unique_name()  { return T::get_unique_name();}
  std::string get_descr()        { return T::get_descr();      }
  std::string get_uom()          { return T::get_uom();        }
  std::string get_url()          { return T::get_url();        }
  bool isenabled()               { return T::isenabled(trace); }
  
  void end(Profile& profile, CallTree& ctree)
    {
      std::vector<long> cnode_idv;
      ctree.get_keyv(cnode_idv);
      for ( size_t i = 0; i < cnode_idv.size(); i++ )
         {
           long cnode_id =  cnode_idv[i];
           if ( T::isappropriate(ctree.get_callee(cnode_id)) )
             for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
               severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
         }
    }
};

/****************************************************************************
* Wait at TypeT 
****************************************************************************/
template <long id, typename T = WaitAtPatternTraits<id> >
class WaitAtPattern : public Pattern
{
  public:
    WaitAtPattern(earl::EventTrace* trace) : Pattern(trace) {}

    // delegate getter functions to Trait class
    long get_id()                  { return id;                  }
    long get_parent_id()           { return T::get_parent_id();  }
    std::string get_name()         { return T::get_name();       }
    std::string get_unique_name()  { return T::get_unique_name();}
    std::string get_descr()        { return T::get_descr();      }
    std::string get_uom()          { return T::get_uom();        }
    std::string get_url()          { return T::get_url();        }
    bool isenabled()               { return T::isenabled(trace); }
    void regcb(Analyzer& analyzer) { analyzer.subscribe_cmpnd(T::get_cmpnd(), this);}
 
    // generic implementation of WaitAt pattern
    cmp_event* cmpnd(const cmp_event* ce)
      {
        const typename T::CollT* col = (const typename T::CollT*) ce;
        const earl::Region* reg = col->enterv[0].get_reg();
        if ( ! T::isappropriate(reg) )
          return NULL;
          
      // events are compared by its position.
      // so event with maximal position is  event with maximal time.
        earl::Event max_start = *std::max_element(col->enterv.begin(), col->enterv.end());
        double wastedTime = 0.0;  
        for ( size_t i = 0; i < col->enterv.size(); i++ )
          {
            long loc_id       = col->enterv[i].get_loc()->get_id();
            long cnode_id     = col->enterv[i].get_cnodeptr().get_pos();
            double idle_time  = max_start->get_time() - col->enterv[i].get_time();

            severity.add_val(cnode_id, loc_id, idle_time);
            wastedTime += idle_time;
          }
        if(StatisticController::theController->IsActive(get_unique_name(),
                                                        wastedTime))
          {
             earl::Event earliestStart = *min_element(col->enterv.begin(),
                                        col->enterv.end(), CompareEventsByTime);
             earl::Event latestEnd = *max_element(col->exitv.begin(),
                                         col->exitv.end(), CompareEventsByTime);
            StatisticController::severePatternInstance spi(
                earliestStart.get_time(), latestEnd.get_time(), wastedTime,
                col->enterv[0].get_cnodeptr().get_pos(),
                earliestStart.get_time(), earliestStart.get_loc()->get_id());
             StatisticController::theController->AddSeverity(id,
                                get_unique_name(), spi);
             if(PatternTrace::_pt_ptr)
               WriteWaitAt<id, WaitAtPatternTraits<id> >(ce, max_start.get_time());
             if(id == EXP_PAT_MPI_WAIT_AT_BARRIER)
               StatisticController::theController->
                       SetWaitAtBarrierWritten(true, col->enterv[0].get_pos());
          }
        else if(id == EXP_PAT_MPI_WAIT_AT_BARRIER) 
          {
              StatisticController::theController->
                      SetWaitAtBarrierWritten(false, col->enterv[0].get_pos());
          }
        /*
        earl::Event min_exit = *std::min_element(col->exitv.begin(), col->exitv.end());
        if ( min_exit < max_start )
          {
            std::cerr << "inconsistent collective N x N operation:\n"
                         " 'exit before enter'\n"
                         "--- early exit event ---\n"
                      << min_exit 
                      << "--- late enter event ---\n"
                      << max_start
                      << "------------------------\n";
          }
        */ 
        return NULL;
      }
};

  
#endif

