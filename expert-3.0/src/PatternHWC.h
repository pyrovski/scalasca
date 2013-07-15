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

#ifndef EXP_PATTERN_HWC_H
#define EXP_PATTERN_HWC_H


#include <earl.h>

#include "epk_metric.h"

#include "Callback.h"
#include "Severity.h"
#include "Pattern.h"

class CallTree;
class Profile; 

class EventTrace; 
class ExpertOpt; 
        
void init_patternv_hwc(std::vector<Pattern*>& pv, earl::EventTrace* trace, ExpertOpt* opt );

/****************************************************************************
* Measured counter metric
****************************************************************************/

class MeasuredCounter : public Pattern
{
 public:

  MeasuredCounter (earl::EventTrace* trace, std::string name, int parent_id);

  void set_parent_id(long id)           { _parent_id = id; return; }
  void set_name(std::string name)       { _name = name; return; }
  void set_alias(std::string alias)     { _alias = alias; return; }
  void set_descr(std::string descr)     { _descr = descr; return; }
  void set_type(epk_metmap_t type)      { _type = type; return; }

  long get_id()                  { return _id; }
  long get_metric_id()           { return _metric_id; }
  long get_parent_id()           { return _parent_id; }
  std::string get_name()         { return _name; }
  std::string get_unique_name()  { return _name; }
  std::string get_alias()        { return _alias; }
  std::string get_uom()          { return "occ"; }
  std::string get_url()          { return EXP_PATTERNS_URL + _name; }
  epk_metmap_t get_type()        { return _type; }

  bool has_alias()               { return (_name != _alias); }
  bool has_data()                { return (_metric_id != -1); }

  std::string get_val();
  std::string get_descr();
  bool isenabled();
  void end(Profile& profile, CallTree& ctree);

 private:
  int _id;              // identifier of counter metric
  int _metric_id;       // index of metric in current trace
  int _parent_id;       // identifier of parent metric/pattern
  std::string _name;    // name of counter metric (in analysis)
  std::string _alias;   // name of counter metric (in measurement)
  std::string _descr;   // descriptor of counter metric
  epk_metmap_t _type;   // type of metric
};

class ComputedCounter : public MeasuredCounter
{
 public:

  ComputedCounter (earl::EventTrace* trace, std::vector<ComputedCounter*> cpv,
                   epk_metmap_t type, std::string name, std::string derivation);

  std::string get_derivation()   { return _derivation; }
  std::string get_url()          {
        std::string name = get_name();
        return EXP_PATTERNS_URL + ((name[0]=='~') ? name.substr(1) : name); }

  std::string get_val();
  std::string get_descr();
  bool isenabled();
  void end(Profile& profile, CallTree& ctree);

 private:
  typedef struct { int id; char op; } met_t;
  std::vector<met_t> metv; // measurement metrics incorporated in derivation
  std::string _derivation; // specification of derived metric
  int missing; // number of constituent metrics missing from derivation
};

#endif
