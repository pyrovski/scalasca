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

#include "PatternHWC.h"

#include "earl.h"

#include <algorithm>
#include <cstdlib>
#include <string>

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "epk_conf.h"

#include "Analyzer.h"
#include "CallTree.h"
#include "ExpertOpt.h"
#include "Profile.h"

using namespace earl;
using namespace std;

/****************************************************************************
* Declaration of auxiliary functions
****************************************************************************/


static int  get_met_id (std::string met_name, const earl::EventTrace* trace);
//static bool is_met_avail (std::string met_name, const earl::EventTrace* trace);


static long get_metric_id (std::string name, std::vector<ComputedCounter*>& pv);
static long get_parent_id (std::string name, std::vector<ComputedCounter*>& pv);
static std::string get_parent_name (std::string child_name, std::vector<ComputedCounter*>& pv);
static std::string get_metric_alias (std::string name, const earl::EventTrace* trace);
/*
static int ept_env_int (std::string envvar, int def)
{
    char* tmp = getenv(envvar.c_str());
    if (tmp == NULL) return def;
    return atoi(tmp);
}

static bool ept_env_bool (std::string envvar, bool def)
{
    return (ept_env_int(envvar, def) != 0);
}
*/
static int ept_verbosity = ExpertOpt::env_int("EPT_VERBOSE",0);
static int ept_measured_metrics = ExpertOpt::env_int("EPT_MEASURED_METRICS",1);
static bool ept_incomplete_computation = ExpertOpt::env_bool("EPT_INCOMPLETE_COMPUTATION",false);

static void Dprintf (const char* fmt, ...)
{
    if (ept_verbosity>1) {
        va_list vargs;
        va_start(vargs, fmt);
        vfprintf(stderr, fmt, vargs);
        va_end(vargs);
    }
    return;
}

static epk_metricmap_t* mmapv = NULL;

void init_patternv_hwc (std::vector<Pattern*>& pv, earl::EventTrace* trace, ExpertOpt* opt)
{
  // Hardware-counter measured and computed metrics
  std::vector<ComputedCounter*> cpv;

  ept_verbosity = opt->get_verbose();

  putenv(strdup("ELG_VERBOSE=0")); // ignore commentary from Epilog library
  if (ept_verbosity>0) {
    cerr << "EPT_VERBOSE=" << ept_verbosity << endl;
    cerr << "EPT_MEASURED_METRICS=" << ept_measured_metrics << endl;
    cerr << "EPT_INCOMPLETE_COMPUTATION=" << ept_incomplete_computation << endl;
    cerr << "EPK_METRICS_SPEC=" << epk_get(EPK_METRICS_SPEC) << endl;
  }

  // read MEASURE mappings to be used as aliases for measured metrics
  mmapv = epk_metricmap_init(EPK_METMAP_MEASURE);

  if (ept_verbosity>0) epk_metricmap_dump(mmapv);

  // read COMPOSE & COMPUTE mappings to construct composition hierarchy
  epk_metricmap_t* cmapv = epk_metricmap_init((epk_metmap_t)(EPK_METMAP_COMPOSE|EPK_METMAP_COMPUTE));
  if (ept_verbosity>0) epk_metricmap_dump(cmapv);

  epk_metricmap_t* map = cmapv;
  while (map != NULL) {
      cpv.push_back(new ComputedCounter(trace, cpv, 
                                map->type, map->event_name, map->alias_name));
      map = map->next;
  }

  epk_metricmap_free(cmapv);

  for (unsigned i=0; i<cpv.size(); i++) {
    Dprintf("Pattern %ld -> %3ld: %s = %s\n", 
        cpv[i]->get_id(), cpv[i]->get_parent_id(),
        cpv[i]->get_name().c_str(), cpv[i]->get_derivation().c_str());
    //cpv[i]->isenabled();
    pv.push_back(cpv[i]);
  }

  Dprintf("\nTrace has %ld measured counter metrics\n", trace->get_nmets());
  for ( int i = 0; i < trace->get_nmets(); i++ )
    {
      string name = trace->get_met(i)->get_name();
      long metric_id = get_metric_id(name, cpv);
      long parent_id = get_parent_id(name, cpv);

      Dprintf("  metric %d: %s metric %ld parent %ld\n", i, name.c_str(),
            metric_id, parent_id);

      if (ept_measured_metrics > 1) { // include all measured metrics separately
          metric_id = EXP_PAT_NONE;
          parent_id = EXP_PAT_NONE;
      }
      // only add "measured" if a "computed" metric class for it is not defined
      // (and separate listing of orphan measured metrics is not disabled)
      if (metric_id == EXP_PAT_NONE) {
          if ((parent_id != EXP_PAT_NONE) || ept_measured_metrics) {
              pv.push_back(new MeasuredCounter(trace, name, parent_id));
              Dprintf("  metric %d: %s -> %s (%ld) => %ld\n", 
                  i, name.c_str(), get_parent_name(name,cpv).c_str(),
                  pv.back()->get_parent_id(), pv.back()->get_id());
          }
      } else { // already defined as a computed metric
          Dprintf("  metric %d: %s -> %s (%ld) == %ld\n", 
                  i, name.c_str(), get_parent_name(name,cpv).c_str(),
                  parent_id, metric_id);
      }
    }
  Dprintf("Pattern specification complete!\n\n");

  epk_metricmap_free(mmapv);
}



std::string get_metric_alias (std::string name, const earl::EventTrace* trace)
{
    for ( int i = 0; i < trace->get_nmets(); i++ ) {
        std::string trace_met_name = trace->get_met(i)->get_name();
        if ( name == trace_met_name ) {
            //Dprintf("GOT direct match for %s\n", name.c_str());
            return name;
        }
        epk_metricmap_t* map = mmapv;
        while ( map != NULL ) {
            if ((name == map->event_name) && (map->alias_name == trace_met_name)) {
                //Dprintf("GOT alias match for %s = %s\n", name.c_str(), trace_met_name.c_str());
                return trace_met_name;
            }
            map = map->next;
        }
    }

    //Dprintf("NO alias match for %s\n", name.c_str());
    return name;
}

long get_metric_id (std::string name, std::vector<ComputedCounter*>& pv)
{
  // scan pv looking for match in the metric name
  for (unsigned i=0; i<pv.size(); i++) {
    ComputedCounter* p=pv[i];
    if (p->get_name() == name) {
          //Dprintf("GOT direct match for metric %s\n", name.c_str());
          return p->get_id();
    } else if (p->get_alias() == name) {
          //Dprintf("GOT alias %s match %s\n", p->get_alias().c_str(), p->get_name().c_str());
          return p->get_id();
    }
  }
  //Dprintf("NO direct match for metric %s\n", name.c_str());
  return EXP_PAT_NONE;
}

long get_parent_id (std::string name, std::vector<ComputedCounter*>& pv)
{
  // scan pv looking for name match in the description of a (derived) pattern
  for (unsigned i=0; i<pv.size(); i++) {
    ComputedCounter* p = pv[i];
    std::string derivation = p->get_derivation();

    // only composed metrics are visible parents
    if (p->get_type() != EPK_METMAP_COMPOSE) continue;

    bool match = false;
    std::string::size_type sep, base=0;
    while ((sep = derivation.find(" + ",base)) != string::npos) {
        std::string met (derivation,base,sep-base);
        match |= (name == met);
        base=sep+3;
    }
    std::string met (derivation,base,string::npos);  
    match |= (name == met);

    if (match) return p->get_id();
  }
  return EXP_PAT_NONE;
}

std::string get_parent_name (std::string name, std::vector<ComputedCounter*>& pv)
{
  // scan pv looking for name match in the description of a (derived) pattern
  for (unsigned i=0; i<pv.size(); i++) {
    ComputedCounter* p = pv[i];
    std::string derivation = p->get_derivation();

    // only composed metrics are visible parents
    if (p->get_type() != EPK_METMAP_COMPOSE) continue;

    bool match = false;
    std::string::size_type sep, base=0;
    while ((sep = derivation.find(" + ",base)) != string::npos) {
        std::string met (derivation,base,sep-base);
        match |= (name == met);
        base=sep+3;
    }
    std::string met (derivation,base,string::npos);  
    match |= (name == met);

    if (match) return p->get_name();
  }
  return "NONE";
}

/****************************************************************************
* Measured counter metric
****************************************************************************/

static int counter_metrics = 0; // number of counter-based metrics defined

MeasuredCounter::MeasuredCounter (earl::EventTrace* _trace, std::string name,
                                  int parent_id = EXP_PAT_NONE)
  : Pattern(_trace) 
{
    _type = EPK_METMAP_MEASURE;
    _name = name;
    _alias = get_metric_alias(name, trace);
    _metric_id = get_met_id(_alias, trace);
    _parent_id = parent_id;
    _descr = (_metric_id == -1) ? "" : trace->get_met(_metric_id)->get_descr();
    // generate unique identifier for counter metric
    _id = EXP_PAT_COUNTER_METRIC + counter_metrics;
    Dprintf("MeasuredCounter name=%s alias=%s metric_id=%d\n", 
            _name.c_str(), _alias.c_str(), _metric_id);
    counter_metrics++;
}

std::string
MeasuredCounter::get_descr()
{
    std::string descr = "Measured counter metric " + get_name();
    if (has_alias()) descr += " = " + get_alias();
    descr += " : " + _descr;
    return descr;
}

std::string
MeasuredCounter::get_val()
{
  if (has_data()) return "MEASURED";
  else            return "VOID";
}

bool
MeasuredCounter::isenabled()
{
  // always true when only metrics found in trace are defined
  int met_id = get_metric_id();
  bool avail = has_data();
  Dprintf("MeasuredCounter::isenabled(%s)=%d (id=%d) %s\n", 
        get_name().c_str(), avail, met_id, 
        has_alias() ? get_alias().c_str() : "");
  return avail;
}

void
MeasuredCounter::end (Profile& profile, CallTree& ctree)
{
  int met_id = get_metric_id();
  long long tot_val = profile.get_metval(met_id);
#if 0
  double   tot_time = profile.get_time();
  double avg_rate = tot_val / tot_time;

  Dprintf("%s: tot_time=%f tot_val=%lld avg_rate=%f\n",
        get_name().c_str(), tot_time, tot_val, avg_rate);
#else
  Dprintf("(%d) %s = %lld\n", met_id, get_name().c_str(), tot_val);
#endif
  vector<long> cnode_idv;
  ctree.get_keyv(cnode_idv);

#ifndef DERIVE
  // set severity values directly from metric measurements in trace
  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        severity.set_val(cnode_id, loc_id, profile.get_metval(met_id, cnode_id, loc_id));
    }
#else
  // derive severity times based on threshold (rate>mean)
  Severity<double> rate(trace->get_nlocs());
  for ( long i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        {
          if ( profile.get_time(cnode_id, loc_id) != 0.0 )
            rate.set_val(cnode_id, loc_id, profile.get_metval(met_id, cnode_id, loc_id) / profile.get_time(cnode_id, loc_id));
          else
            rate.set_val(cnode_id, loc_id, 0);
        }
    }
  // leave all matrix elements zero whose rate > avg_rate
  for ( long i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        {
          if ( rate.get(cnode_id, loc_id) > avg_rate )
            severity.set_val(cnode_id, loc_id, profile.get_time(cnode_id, loc_id));
          else
            severity.set_val(cnode_id, loc_id, 0);
        }
    }
#endif
}

/****************************************************************************
* Computed counter metrics (derived from measured counter metrics)
****************************************************************************/

ComputedCounter::ComputedCounter (earl::EventTrace* _trace, 
                                 std::vector<ComputedCounter*> cpv,
                                 epk_metmap_t type,
                                 std::string name, std::string derivation)
  : MeasuredCounter (_trace, name)
{ 
  missing = 0;
  set_type(type);
  _derivation = derivation; 
  Dprintf("%ld: %s = %s\n", get_id(), name.c_str(), derivation.c_str());
  Dprintf("ComputedCounter name=%s alias=%s metric_id=%ld type=0x%X\n", 
          get_name().c_str(), get_alias().c_str(), get_metric_id(), get_type());

  if (has_data()) {
      // ensure that measured metric has description set from trace
      set_descr(trace->get_met(get_metric_id())->get_descr());
      Dprintf("  direct measurement %ld: %s\n", get_metric_id(),
              get_descr().c_str());
      met_t met;
      met.id = get_metric_id(); met.op = '+';
      metv.push_back(met);
  } else {
      // get metv list from derivation for this metric
      int part=0;
      vector<char> met_opv;
      vector<int>  met_idv;
      string delim(" +-");
      string::size_type last = derivation.find_first_not_of(delim, 0);
      string sep = derivation.substr(0, last);
      if (sep.empty() || 
              (sep.find('+') != string::npos)) met_opv.push_back('+');
      else if (sep.find('-') != string::npos)  met_opv.push_back('-');
      else                                     met_opv.push_back('?');
      string::size_type next = derivation.find_first_of(delim, last);

      while (string::npos != next || string::npos != last) {
          // process new metric token
          string met = derivation.substr(last, next-last);
          int met_id = get_met_id(met, trace);
          met_idv.push_back(met_id); // include all (for now)
          Dprintf("met[%d]=%c%s (%d)\n", part, met_opv.back(), met.c_str(), met_id);
          // scan delimiters
          last = derivation.find_first_not_of(delim, next);
          if (last != string::npos) {
              string sep2 = derivation.substr(next, last-next);
              if      (sep2.find('+') != string::npos) met_opv.push_back('+');
              else if (sep2.find('-') != string::npos) met_opv.push_back('-');
              else                                     met_opv.push_back('?');
          }
          // find next non-delimiter
          next = derivation.find_first_of(delim, last);
          part++;
      }

      // compositions consist only of additions, and may have missing components,
      // while useful computations are not allowed to have any missing components
      bool composition = true;
      for (size_t p=0; p<met_idv.size(); p++) {
          if (met_opv[p] != '+') composition=false;
          if (met_idv[p] == -1)  missing++;
          else {
              met_t met;
              met.id = met_idv[p];
              met.op = met_opv[p];
              metv.push_back(met);
          }
      }
      if (!composition && missing) {
          if (ept_incomplete_computation && metv.size()) {
              fprintf(stderr,"%s computation accepted with %lu/%lu components\n",
                      name.c_str(), (unsigned long) metv.size(),
                      (unsigned long) met_idv.size());
          } else {
              Dprintf("  computation unavailable with %d missing components\n", missing);
              metv.clear(); // erase all metrics
          }
      }
      Dprintf("==> %s: components=%d missing=%d metv.size=%lu\n", 
             name.c_str(), part, missing, metv.size());
  }

  // check derived patterns' derivations for parentage
  for (size_t i=0; i<cpv.size(); i++) {
    int parent_index = -1;
    string drvd_derivation = cpv[i]->get_derivation();
    string::size_type sep, base=0;
    while ((sep = drvd_derivation.find(" + ",base)) != string::npos) {
      string met (drvd_derivation,base,sep-base);
      if (met == name) parent_index=i;
      base=sep+3;
    }
    string met (drvd_derivation,base,string::npos);
    if (met == name) parent_index=i;

    if (parent_index != -1) {
      Dprintf("  parented by %s (%ld)\n", cpv[i]->get_name().c_str(), cpv[i]->get_id());
      set_parent_id(cpv[parent_index]->get_id());
      if (metv.empty()) continue; // no metrics to propagate
      if (cpv[parent_index]->has_data()) continue; // parent measured directly

      // unaliased measured metrics have already been processed by parent
      if (has_data() && !has_alias()) continue;

      if (missing == 0) // no metric components missing from this derivation
          cpv[parent_index]->missing--; // one less component metric required
      // incorporate node's metrics in parent's metv
      for (size_t j=0; j<metv.size(); j++)
          cpv[parent_index]->metv.push_back(metv[j]);
      Dprintf("==> parent metv.size=%lu missing=%d\n", 
          cpv[parent_index]->metv.size(), cpv[parent_index]->missing);

      // propagate metric components up through parents to root (or direct)!
      int grandp_index = i-1; // potential grandparents must precede parents
      while ((parent_index != EXP_PAT_NONE) &&
             (cpv[parent_index]->get_parent_id() != EXP_PAT_NONE)) {
          while (cpv[parent_index]->get_parent_id() != cpv[grandp_index]->get_id())
              grandp_index--;
          Dprintf("  grandparented by %s (%ld) [%ld]\n",
                cpv[grandp_index]->get_name().c_str(),
                cpv[grandp_index]->get_id(), 
                cpv[grandp_index]->get_metric_id());
          // stop with grandparents who have their own direct data
          if (cpv[grandp_index]->has_data()) {
              parent_index = -1;
              continue;
          }
          // incorporate node's metrics in grandparent's metv
          for (size_t j=0; j<metv.size(); j++)
              cpv[grandp_index]->metv.push_back(metv[j]);
          if (!cpv[parent_index]->missing)  // parent definition is complete
              cpv[grandp_index]->missing--; // one less component required
          Dprintf("==> grandparent metv.size=%lu missing=%d\n", 
                cpv[grandp_index]->metv.size(), cpv[grandp_index]->missing);
          parent_index = grandp_index; // move to preceding generation
      }
    }
  }
}

std::string
ComputedCounter::get_val()
{
  if (has_data()) return "MEASURED";
  if (metv.empty()) return "VOID";
  switch (get_type()) {
    case EPK_METMAP_UNKNOWN: return "UNKNOWN";
    case EPK_METMAP_MEASURE: return "MEASURED";
    case EPK_METMAP_AGGROUP: return "AGGROUPED";
    case EPK_METMAP_COMPOSE: return "COMPOSED";
    case EPK_METMAP_COMPUTE: return "COMPUTED";
    case EPK_METMAP_INVALID: return "INVALID";
    default:                 return "UNDEFINED";
  }
}

std::string
ComputedCounter::get_descr()
{
  if (has_data()) return MeasuredCounter::get_descr();
  if (get_derivation().find_first_of("/-*") == string::npos)
      return "Composed counter metric " + get_name() + " = " + get_derivation();
  return "Computed counter metric " + get_name() + " = " + get_derivation();
}

bool
ComputedCounter::isenabled()
{
  if (has_data()) return MeasuredCounter::isenabled(); // always true!

  Dprintf("ComputedCounter::isenabled(%s) [%d:%lu] %s\n",
        get_name().c_str(), missing, metv.size(), get_val().c_str());
  return true; // always retained (but marked with val=VOID if without data)
}

void
ComputedCounter::end (Profile& profile, CallTree& ctree)
{
  if (has_data()) return MeasuredCounter::end(profile, ctree);

  long long tot_val = 0;
  for (unsigned i=0; i<metv.size(); i++) {
      if (metv[i].op == '+')
          tot_val += profile.get_metval(metv[i].id);
      else if (metv[i].op == '-')
          tot_val -= profile.get_metval(metv[i].id);
      // else ignore for now XXXX
  }
  // missing components indicate (potentially) incomplete derivation
  if (missing != 0) set_name("~" + get_name());
  Dprintf("%s = %s = %lld\n", get_name().c_str(),
          get_derivation().c_str(), tot_val);

  vector<long> cnode_idv;
  ctree.get_keyv(cnode_idv);

  // aggregate from children for derivation
  for ( size_t i = 0; i < cnode_idv.size(); i++ )
    {
      long cnode_id =  cnode_idv[i];
      for ( long loc_id = 0; loc_id < trace->get_nlocs(); loc_id++ )
        {
          long long value = 0;
          for ( unsigned met_i = 0; met_i < metv.size(); met_i++)
              if (metv[met_i].op == '+')
                  value += profile.get_metval(metv[met_i].id, cnode_id, loc_id);
              else if (metv[met_i].op == '-')
                  value -= profile.get_metval(metv[met_i].id, cnode_id, loc_id);
              // else ignore for now XXXX
          severity.set_val(cnode_id, loc_id, value);
        }
    }
}

/****************************************************************************
*
* Auxiliary functions
*
****************************************************************************/
int get_met_id (std::string met_name, const earl::EventTrace* trace)
{
    for ( int i = 0; i < trace->get_nmets(); i++ ) {
        if ( met_name == trace->get_met(i)->get_name() ) {
            //Dprintf("GOT direct trace met match for %s\n", met_name.c_str());
            return i;
        }
    }
    //Dprintf("NO direct trace met match for %s\n", met_name.c_str());
    return -1;
}

//bool is_met_avail (std::string met_name, const earl::EventTrace* trace)
//{
//  if ( get_met_id(met_name, trace) == -1 )
//    return false;
//  else
//    return true;
//}

