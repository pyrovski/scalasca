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
* \file AggrCubeMaps.h
* \brief Defines a map for from "dimension value" -> "aggregate severity value" 
*/


#include "AggrCubeMaps.h"

using namespace std;
using namespace cube;


//------------- call-tree view ---------------------
/**
* \details Get a aggregate severites for all metrics for given cnode and thread.
*/
void AggrCubeMaps::get_met_tree(map<Metric*, double>& excl_sevm,
                                map<Metric*, double>& incl_sevm,  
                                inclmode cnode_mode,
                                inclmode sys_mode,
                                Cnode* cnode,
                                Sysres* sys) const {
  const vector<Metric*>& metrics = get_metv();
  unsigned int num_metrics = metrics.size();

  vector<double> excl_sevv(num_metrics);
  vector<double> incl_sevv(num_metrics);

  get_met_tree(excl_sevv, incl_sevv, cnode_mode, sys_mode, cnode, sys);

  for(unsigned long i = 0; i < num_metrics; ++i) {
    Metric* met = metrics[i];

    excl_sevm[met] = excl_sevv[i];
    incl_sevm[met] = incl_sevv[i];
  }

}

/**
* \details Get a aggregate severites for all cnodes for given metric and thread.
*/
void AggrCubeMaps::get_cnode_tree(map<Cnode*, double>& excl_sevm,
                                  map<Cnode*, double>& incl_sevm,  
                                  inclmode met_mode,
                                  inclmode sys_mode,
                                  Metric* met,
                                  Sysres* sys,
                                  Cnode* /*cur_cnode*/) const {
  const vector<Cnode*>& cnodes = get_cnodev();
  unsigned int num_cnodes = cnodes.size();

  vector<double> excl_sevv(num_cnodes);
  vector<double> incl_sevv(num_cnodes);

  get_cnode_tree(excl_sevv, incl_sevv, met_mode, sys_mode, met, sys);

  for(unsigned long i = 0; i < num_cnodes; ++i) {
    Cnode* cnode = cnodes[i];

    excl_sevm[cnode] = excl_sevv[i];
    incl_sevm[cnode] = incl_sevv[i];
  }

}

/**
* \details Get a aggregate severites for all threads for given metric and cnode.
*/
void AggrCubeMaps::get_sys_tree(map<Sysres*, double>& excl_sevm,
                                map<Sysres*, double>& incl_sevm,  
                                inclmode met_mode,
                                inclmode cnode_mode,
                                Metric* met,
                                Cnode* cnode, 
                                Sysres* /*cur_sys*/) const {
  const vector<Sysres*>& sysres = get_sysv();
  unsigned int num_sysres = sysres.size();

  vector<double> excl_sevv(num_sysres);
  vector<double> incl_sevv(num_sysres);

  get_sys_tree(excl_sevv, incl_sevv, met_mode, cnode_mode, met, cnode);

  for(unsigned long i = 0; i < num_sysres; ++i) {
    Sysres* sys = sysres[i];

    excl_sevm[sys] = excl_sevv[i];
    incl_sevm[sys] = incl_sevv[i];
  }
}


//------------- flat-profile view ------------------
/**
* \details Get a aggregate severites for all metrics for given region and thread.
*/
void AggrCubeMaps::get_met_tree(std::map<Metric*, double>& excl_sevm,
                                std::map<Metric*, double>& incl_sevm,  
                                inclmode reg_mode,
                                inclmode sys_mode,
                                Region* reg,
                                Sysres* sys) const {
  const vector<Metric*>& metrics = get_metv();
  unsigned int num_metrics = metrics.size();

  vector<double> excl_sevv(num_metrics);
  vector<double> incl_sevv(num_metrics);

  get_met_tree(excl_sevv, incl_sevv, reg_mode, sys_mode, reg, sys);

  for(unsigned long i = 0; i < num_metrics; ++i) {
    Metric* met = metrics[i];

    excl_sevm[met] = excl_sevv[i];
    incl_sevm[met] = incl_sevv[i];
  }
}


/**
* \details Get a aggregate severites for all regions for given metric and thread.
*/
void AggrCubeMaps::get_reg_tree(map<Region*, double>& excl_sevm,
                                map<Region*, double>& incl_sevm,  
                                map<Region*, double>& diff_sevm,  
                                inclmode met_mode,
                                inclmode sys_mode,
                                Metric* met,
                                Sysres* sys) const {
  const vector<Region*>& regions = get_regv();
  unsigned int num_regions = regions.size();

  vector<double> excl_sevv(num_regions);
  vector<double> incl_sevv(num_regions);
  vector<double> diff_sevv(num_regions);

  get_reg_tree(excl_sevv, incl_sevv, diff_sevv, met_mode, sys_mode, met, sys);

  for (unsigned int i = 0; i < num_regions; ++i) {
    Region* reg  = regions[i];

    excl_sevm[reg] = excl_sevv[i];
    incl_sevm[reg] = incl_sevv[i];
    diff_sevm[reg] = diff_sevv[i];
  } 
}


/**
* \details Get a aggregate severites for all threads for given metric and region.
*/
void AggrCubeMaps::get_sys_tree(map<Sysres*, double>& excl_sevm,
                                map<Sysres*, double>& incl_sevm,  
                                inclmode met_mode,
                                inclmode reg_mode,
                                Metric* met,
                                Region* reg, 
                                Sysres* /*cur_sys*/) const {
  const vector<Sysres*>& sysres = get_sysv();
  unsigned int num_sysres = sysres.size();

  vector<double> excl_sevv(num_sysres);
  vector<double> incl_sevv(num_sysres);
 
  get_sys_tree(excl_sevv, incl_sevv, met_mode, reg_mode, met, reg);

  for (unsigned int i = 0; i < num_sysres; ++i) {
    Sysres* sys  = sysres[i];

    excl_sevm[sys] = excl_sevv[i];
    incl_sevm[sys] = incl_sevv[i];
  }
}
