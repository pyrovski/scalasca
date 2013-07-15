/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
* \file AggrCube.cpp
* \brief Defines a cube, which can "aggregate" the date over some dimension.
*/
#include "AggrCube.h"

#include <algorithm>
#include <cfloat>

#include "Cnode.h"
#include "Cube.h"
#include "cube_error.h"
#include "Machine.h"
#include "Metric.h"
#include "Region.h"
#include "Sysres.h"

using namespace std;
using namespace cube;

AggrCube::AggrCube() : Cube() { }
AggrCube::AggrCube(Cube const& cube) : Cube(cube) { }

//------------- call-tree view ---------------------
/**
* \details Calculates incusive and exclusive values for every metric.
*/
void AggrCube::get_met_tree(vector<double>& excl_sevv,
		            vector<double>& incl_sevv,  
		            inclmode cnode_mode,
                            inclmode sys_mode,
		            Cnode* cnode,
                            Sysres* sys) const
{
  const vector<Metric*>& metrics = get_metv();
  size_t num_metrics = metrics.size();

  excl_sevv.resize(num_metrics);
  incl_sevv.resize(num_metrics);

  #pragma omp parallel
  {
    #pragma omp for
    for (size_t i = 0; i < num_metrics; ++i)
      incl_sevv[i] = get_vcsev(INCL, cnode_mode, sys_mode, metrics[i], cnode, sys);

    #pragma omp for
    for (size_t i = 0; i < num_metrics; ++i) {
      Metric* met = metrics[i];

      double result = incl_sevv[i];
      for (unsigned int j = 0; j < met->num_children(); ++j)
        result -= incl_sevv[met->get_child(j)->get_id()];

      excl_sevv[i] = result;
    }
  }
}


/**
* \details Calculates incusive and exclusive values for every cnode.
*/
void AggrCube::get_cnode_tree(vector<double>& excl_sevv,
			      vector<double>& incl_sevv,  
			      inclmode met_mode,
                              inclmode sys_mode,
			      Metric* met,
                              Sysres* sys) const
{
  const vector<Cnode*>& cnodes = get_cnodev();
  int num_cnodes = cnodes.size();

  excl_sevv.resize(num_cnodes);
  incl_sevv.resize(num_cnodes);

  #pragma omp parallel for
  for (int i = 0; i < num_cnodes; ++i) {
    Cnode* cnode = cnodes[i];

    double result = get_vcsev(met_mode, EXCL, sys_mode, met, cnode, sys);

    excl_sevv[i] = result;
    incl_sevv[i] = result;
  }

  for (int i = num_cnodes; i > 0; --i) {
    int id = i - 1;
    Cnode* parent = cnodes[id]->get_parent();

    if (parent) {
      int p_id = parent->get_id();
      incl_sevv[p_id] += incl_sevv[id];
    }
  }
}


/**
* \details Calculates incusive and exclusive values for every thread.
*/
void AggrCube::get_sys_tree(vector<double>& excl_sevv,
		            vector<double>& incl_sevv,  
		            inclmode met_mode,
                            inclmode cnode_mode,
		            Metric* met,
                            Cnode* cnode) const
{
  const vector<Sysres*>& sysres = get_sysv();
  int num_sysres = sysres.size();

  excl_sevv.resize(num_sysres);
  incl_sevv.resize(num_sysres);

  #pragma omp parallel for
  for (int i = 0; i < num_sysres; ++i) {
    Sysres* sys = sysres[i];

    double result = get_vcsev(met_mode, cnode_mode, EXCL, met, cnode, sys);

    excl_sevv[i] = result;
    incl_sevv[i] = result;
  }

  for (int i = num_sysres; i > 0; --i) {
    int id = i - 1;
    Sysres* parent = sysres[id]->get_parent();

    if(parent) {
      int p_id = parent->get_sys_id();
      incl_sevv[p_id] += incl_sevv[id];
    }
  }
}


//------------- flat-profile view ------------------

void AggrCube::get_met_tree(vector<double>& excl_sevv,
		            vector<double>& incl_sevv,  
		            inclmode reg_mode,
                            inclmode sys_mode,
		            Region* reg,
                            Sysres* sys) const
{
  const vector<Metric*>& metrics = get_metv();
  size_t num_metrics = metrics.size();

  excl_sevv.resize(num_metrics);
  incl_sevv.resize(num_metrics);

  #pragma omp parallel
  {
    #pragma omp for
    for (size_t i = 0; i < num_metrics; ++i)
      incl_sevv[i] = this->get_vrsev(INCL, reg_mode, sys_mode, metrics[i], reg, sys);

    #pragma omp for
    for (size_t i = 0; i < num_metrics; ++i) {
      Metric* met = metrics[i];

      double result = incl_sevv[i];
      for (unsigned int j = 0; j < met->num_children(); ++j)
        result -= incl_sevv[met->get_child(j)->get_id()];
      excl_sevv[i] = result;
    }
  }
}

void AggrCube::get_met_tree(std::map<Metric*, double> & excl_sevv,
		            std::map<Metric*, double> & incl_sevv,  
		            inclmode reg_mode,
                            inclmode sys_mode,
		            Cnode*  cnode,
                            Sysres* sys) const
{
  const vector<Metric*>& metrics = get_metv();
  size_t num_metrics = metrics.size();

//   excl_sevv.resize(num_metrics);
//   incl_sevv.resize(num_metrics);

  #pragma omp parallel
  {
    #pragma omp for
    for (size_t i = 0; i < num_metrics; ++i)
      incl_sevv[metrics[i]] = this->get_vcsev(INCL, reg_mode, sys_mode, metrics[i], cnode, sys);

    #pragma omp for
    for (size_t i = 0; i < num_metrics; ++i) {
      Metric* met = metrics[i];

      double result = incl_sevv[met];
      for (unsigned int j = 0; j < met->num_children(); ++j)
        result -= incl_sevv[met->get_child(j)];
        excl_sevv[met] = result;
    }
  }

  
}


void AggrCube::get_reg_tree(vector<double>& excl_sevv,
		            vector<double>& incl_sevv,  
		            vector<double>& diff_sevv,  
		            inclmode met_mode,
                            inclmode sys_mode,
		            Metric* met,
                            Sysres* sys) const
{
  const vector<Region*>& regions = get_regv();
  int num_regions = regions.size();

  excl_sevv.resize(num_regions);
  incl_sevv.resize(num_regions);
  diff_sevv.resize(num_regions);

  #pragma omp parallel for
  for (int i = 0; i < num_regions; ++i) {
    Region* reg = regions[i];

    double excl = get_vrsev(met_mode, EXCL, sys_mode, met, reg, sys);
    double incl = get_vrsev(met_mode, INCL, sys_mode, met, reg, sys);

    excl_sevv[i] = excl;
    incl_sevv[i] = incl;
    diff_sevv[i] = incl - excl;
  } 
}



void AggrCube::get_reg_tree(std::map<Region*, double> & excl_sevv,
		            std::map<Region*, double> & incl_sevv,  
		            std::map<Region*, double> & diff_sevv,  
		            inclmode met_mode,
                            inclmode sys_mode,
		            Metric* met,
                            Sysres* sys) const
{
  const vector<Region*>& regions = get_regv();
  int num_regions = regions.size();

// //   excl_sevv.resize(num_regions);
//   incl_sevv.resize(num_regions);
//   diff_sevv.resize(num_regions);

  #pragma omp parallel for
  for (int i = 0; i < num_regions; ++i) {
    Region* reg = regions[i];



    double excl = get_vrsev(met_mode, EXCL, sys_mode, met, reg, sys);
    double incl = get_vrsev(met_mode, INCL, sys_mode, met, reg, sys);

    excl_sevv[reg] = excl;
    incl_sevv[reg] = incl;
    diff_sevv[reg] = incl - excl;
  } 


}





void AggrCube::get_sys_tree(vector<double>& excl_sevv,
		            vector<double>& incl_sevv,  
		            inclmode met_mode, inclmode reg_mode,
		            Metric* met, Region* reg) const
{
  const vector<Sysres*>& sysres = get_sysv();
  int num_sysres = sysres.size();

  excl_sevv.resize(num_sysres);
  incl_sevv.resize(num_sysres);

  #pragma omp parallel for
  for (int i = 0; i < num_sysres; ++i) {
    Sysres* sys = sysres[i];

    double result = get_vrsev(met_mode, reg_mode, EXCL, met, reg, sys);

    excl_sevv[i] = result;
    incl_sevv[i] = result;
  }

  for(int i = num_sysres; i > 0; --i) {
    int id = i - 1;
    Sysres* parent = sysres[id]->get_parent();

    if(parent) {
      int p_id = parent->get_sys_id();
      incl_sevv[p_id] += incl_sevv[id];
    }
  }
}


//------------- single severity values ------------------

double AggrCube::get_vcsev(inclmode met_mode, 
			   inclmode cnode_mode, 
			   inclmode sys_mode,
			   Metric* met, 
			   Cnode* cnode, 
			   Sysres* sys) const  {
  double result = 0.0; 
  
  if ( met_mode == DIFF || cnode_mode == DIFF || sys_mode == DIFF )
      throw RuntimeError("AggrCube::get_vcsev(inclmode, inclmode cnode_mode, sys_mode, Metric*, Cnode*, Sysres*): Inclusion mode DIFF not allowed here.");
  
  // cases where cnode or sys is NULL
  
  if ( cnode == NULL ) {
    if ( ct_aggr_mode == MAX ) {
      result = -DBL_MAX;
      unsigned int count = this->get_root_cnodev().size();
      for (unsigned int i = 0; i < count; i++) {
        double value = this->get_vcsev(met_mode, INCL, sys_mode, met, this->get_root_cnodev()[i], sys);
        if (value > result)
          result = value;
      }
      return result;
    } else {
      for (unsigned int i = 0; i < this->get_cnodev().size(); i++) 
	result += this->get_vcsev(met_mode, EXCL, sys_mode, 
				  met, this->get_cnodev()[i], sys);
      return result;
    } 
  }
  
  if ( sys == NULL ) {
    for (unsigned int i = 0; i < this->get_machv().size(); i++) 
      result += this->get_vcsev(met_mode, cnode_mode, INCL, met, cnode, this->get_machv()[i]);
    return result;
  }
  
  // calculating non-standard inclusion modes

  if ( met_mode == EXCL ) {
    result = this->get_vcsev(INCL, cnode_mode, sys_mode, met, cnode, sys);
    for (unsigned int i = 0; i < met->num_children(); i++) 
      result -= this->get_vcsev(INCL, cnode_mode, sys_mode, met->get_child(i), cnode, sys);
    return result;
  }
  
  if ( cnode_mode == INCL ) {
    result = this->get_vcsev(met_mode, EXCL, sys_mode, met, cnode, sys);
    for (unsigned int i = 0; i < cnode->num_children(); i++) 
      result += this->get_vcsev(met_mode, INCL, sys_mode, met, cnode->get_child(i), sys);
    return result;
  }
  
  if ( sys_mode == INCL ) {
    result = this->get_vcsev(met_mode, cnode_mode, EXCL, met, cnode, sys);
    for (unsigned int i = 0; i < sys->num_children(); i++) 
      result += this->get_vcsev(met_mode, cnode_mode, INCL, met, cnode, sys->get_child(i));
    return result;
  }

  // default case

  return this->get_esev(met, cnode, sys);
}
    
double AggrCube::get_vrsev(inclmode met_mode, 
			   inclmode reg_mode, 
			   inclmode sys_mode,  
			   Metric* met, 
			   Region* reg, 
			   Sysres* sys) const {
  double result = 0.0;

  switch (reg_mode) {
      case EXCL:
	  for (unsigned int i = 0; i < reg->get_excl_cnodev().size(); i++) 
	      result += this->get_vcsev(met_mode, EXCL, sys_mode, met, reg->get_excl_cnodev()[i], sys);
	  break;
      case INCL:
	  for (unsigned int i = 0; i < reg->get_incl_cnodev().size(); i++) 
	      result += this->get_vcsev(met_mode, INCL, sys_mode, met, reg->get_incl_cnodev()[i], sys);
	  break;
      case DIFF:
	  result = this->get_vrsev(met_mode, INCL, sys_mode, met, reg, sys) - this->get_vrsev(met_mode, EXCL, sys_mode, met, reg, sys);
	  break;
  }

  return result;
}

double AggrCube::get_esev(Metric* met, Cnode* cnode, Sysres* sys) const {
  
  // if sys is not a thread, return zero
  if ( sys->num_children() != 0 )
    return 0.0;
  else
    return this->get_sev(met, cnode, (Thread*)sys);
}

double AggrCube::get_rmet_sev(Metric* met) {
  if (met == NULL) return 0.0;
  met = get_root_met(met);

  map<Metric*, double>::const_iterator it;
  it = rmet_sevm.find(met);
  if (it != rmet_sevm.end()) 
    return it->second; 
  double res = this->get_vcsev(INCL, INCL, INCL, met, NULL, NULL);
  rmet_sevm[met] = res;
  return res;
}
