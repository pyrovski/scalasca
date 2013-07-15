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

#include "Analyzer.h"

#include <cstdlib>
#include <iostream>

#include "CallTree.h"
#include "ExpertOpt.h"
#include "Pattern.h"
#include "PatternTree.h"
#include "Profile.h"
#include "Report.h"

#include "earl_dump.h"
#include "PatternTrace.h"

using namespace earl;
using namespace std;
using namespace cube;

Analyzer::Analyzer(EventTrace* trace, ExpertOpt *opt) : trace(trace) , expert_opt(opt)
{
  ptree = new PatternTree(trace, *this);
  profile = new Profile(trace, *this);
}

Analyzer::~Analyzer() 
{
  delete ptree;
  delete profile;
}

void
Analyzer::subscribe_event(etype type, Callback* cbobj)
{ 
  eventcbm[type].push_back(cbobj);
  if ( type == EXIT )
    {
      eventcbm[COLLEXIT].push_back(cbobj);
      eventcbm[MPICEXIT].push_back(cbobj);
      eventcbm[MPIWCEXIT].push_back(cbobj);
      eventcbm[MPIWEXIT].push_back(cbobj);
      eventcbm[OMPCEXIT].push_back(cbobj);
    }
}

void 
Analyzer::subscribe_cmpnd(cetype type, Callback* cbobj)
{
  ceventcbm[type].push_back(cbobj);
}
  
void 
Analyzer::run()
{
  int new_percent, percent = 0;

  for ( long pos = 1; pos <= trace->get_nevents(); pos++ )
    {
      this->step(pos);
      if ( expert_opt->get_verbose() >= 0 ) {
        new_percent = ( 100 * pos ) / trace->get_nevents();
        if ( new_percent > percent )
          {
            percent = new_percent;
            cerr <<  '\r' << percent << " %"; 
          }
      }
    }
  if ( expert_opt->get_verbose() >= 0 )
    cerr << endl;

  // check for missing EXIT events (i.e., whether stack empty)
  long last_pos = trace->get_nevents();
  vector<Event> outv;
  for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
    {
      trace->stack(outv, last_pos, loc_id);
      if ( ! outv.empty() )
        {
          cerr << "EXPERT: Error in analysis. Trace file incomplete because of missing EXIT events." << endl;
          if ( expert_opt->get_verbose() >= 1 )
            for (size_t i=0; i<outv.size(); i++)
              cerr << outv[i];
          exit(EXIT_FAILURE);
        }
    }

  CallTree ctree(trace);

  // postprocess profile
  profile->exclusify(ctree);
  
  // end() callbacks
  vector<long> keyv;
  ptree->get_keyv(keyv);
  for ( size_t i = 0; i < keyv.size(); i++ )
    ptree->get_data(keyv[i])->end(*profile, ctree);

  // generate report
  string cube_name;
  if (trace->get_name() == "epik.elg") {
    cube_name = trace->get_path();
    cube_name.resize(cube_name.size() + 3);
#ifndef CUBE_COMPRESSED
    cube_name.replace(cube_name.size() - 11, 11, "expert.cube");
#else
    cube_name.replace(cube_name.size() - 11, 11, "expert.cube.gz");
#endif
  } else {
    cube_name = trace->get_name();
    cube_name.resize(cube_name.size() + 1);
#ifndef CUBE_COMPRESSED
    cube_name.replace(cube_name.size() - 4, 4, "cube");
#else
    cube_name.replace(cube_name.size() - 4, 4, "cube.gz");
#endif
  }
  Report(*ptree, ctree, *trace).write(cube_name);
  if ( expert_opt->get_verbose() >= 0 ) {
    cout << "EXPERT: Report produced in " << cube_name << endl;
  }

  // check for missing RMA transfers (i.e., whether transfer queue is empty)
  if (trace->get_nwins() > 0 ) 
    {
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        {
          trace->get_mpitransfers(outv, last_pos, loc_id);
          if ( ! outv.empty() )
            {
              cerr << "EXPERT: Error in analysis. "
                      "Trace file incomplete because of missing "
                      "RMA Transfer End events." << endl;
              if ( expert_opt->get_verbose() >= 2 )
                for (size_t i=0; i<outv.size(); i++)
                  cerr << outv[i];
            }
          trace->get_mpiorigins(outv, last_pos, loc_id);
          if ( ! outv.empty() )
            {
              cerr << "EXPERT: Error in analysis. "
                      "Trace file incomplete because of missing "
                      "RMA Transfer Origin events." << endl;
              if ( expert_opt->get_verbose() >= 2 )
                for (size_t i=0; i<outv.size(); i++)
                  cerr << outv[i];
            }
        }
    }
  return;
}

void 
Analyzer::step(long pos)
{
  Event curr = trace->event(pos);
  
  vector<cmp_event> ceventv;

  // callbacks for primitive events
  for ( size_t i = 0; i < eventcbm[curr.get_type()].size(); i++ )
    {
      cmp_event* ce = NULL;

      switch( curr.get_type() )
        {
          case OMPALOCK:
            ce = eventcbm[curr.get_type()][i]->ompalock(curr);
            break;
          case COLLEXIT:
            ce = eventcbm[curr.get_type()][i]->collexit(curr);
            if ( ce != NULL ) cequeue.push(ce);
            ce = eventcbm[curr.get_type()][i]->exit(curr);
            break;
          case ENTER:
            ce = eventcbm[curr.get_type()][i]->enter(curr);
            if( (expert_opt->get_patterntrace()) & (ExpertOpt::PT_PATTERN) ){
              RecordBuffer* buffer = PatternTrace::_pt_ptr->GetBuffer();
              if( (curr.get_reg()->get_rtype() == "OMP_CRITICAL_SBLOCK")  &&
                  (buffer->FindDeleteOpenEnter(curr.get_enterptr().get_pos(),
                                               curr.get_loc()->get_id() ) ) )
                buffer->AddEvent(curr.get_loc()->get_id(),
                                 new ExitRecord(curr.get_time() ) );
            }
            break;
          case EXIT:
            ce = eventcbm[curr.get_type()][i]->exit(curr);
            if( (expert_opt->get_patterntrace()) & (ExpertOpt::PT_PATTERN) ){
              RecordBuffer* buffer = PatternTrace::_pt_ptr->GetBuffer();
              if(buffer->FindDeleteOpenEnter
                    (curr.get_enterptr().get_pos(), curr.get_loc()->get_id() ) )
                buffer->AddEvent(curr.get_loc()->get_id(),
                                 new ExitRecord(curr.get_time() ) );
            }
            break;
          case FORK:
            ce = eventcbm[curr.get_type()][i]->fork(curr);
            break;
          case JOIN:
            ce = eventcbm[curr.get_type()][i]->join(curr);
            break;
          case MPICEXIT:
            ce = eventcbm[curr.get_type()][i]->mpicexit(curr);
            if ( ce != NULL ) cequeue.push(ce);
            ce = eventcbm[curr.get_type()][i]->exit(curr);
            break;
          case MPIWCEXIT:
            ce = eventcbm[curr.get_type()][i]->rmawcexit(curr);
            if ( ce != NULL ) cequeue.push(ce);
            ce = eventcbm[curr.get_type()][i]->exit(curr);
            break;
          case MPIWEXIT:
            ce = eventcbm[curr.get_type()][i]->rmawexit(curr);
            if ( ce != NULL ) cequeue.push(ce);
            ce = eventcbm[curr.get_type()][i]->exit(curr);
            break;
          case OMPCEXIT:
            ce = eventcbm[curr.get_type()][i]->ompcexit(curr);
            if ( ce != NULL ) cequeue.push(ce);
            ce = eventcbm[curr.get_type()][i]->exit(curr);
            break;
          case RECV:
            ce = eventcbm[curr.get_type()][i]->recv(curr);
            break;
          case OMPRLOCK:
            ce = eventcbm[curr.get_type()][i]->omprlock(curr);
            break;
          case SEND:
            ce = eventcbm[curr.get_type()][i]->send(curr);
            break;
          default:
            /* ignore unknown events for now */
            break;
        }
      if ( ce != NULL )
        cequeue.push(ce);
    }
  
  // callbacks for compound events
  while ( cequeue.size() > 0 )
    {
      cmp_event* ce = cequeue.front();
      for ( size_t i = 0; i < ceventcbm[ce->type].size(); i++ )
        {
          cmp_event* new_ce = ceventcbm[ce->type][i]->cmpnd(ce);
          if ( new_ce != NULL )
            cequeue.push(new_ce);
        }
      delete ce;
      cequeue.pop();
    }
}

