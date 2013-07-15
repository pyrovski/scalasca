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
* 
* \file algebra.cpp
* \brief Contains a set of functions allows to operate with a cube and parts of 
it (metrics tree, cnodes tree and so on). 
* Merging, comparison, copying and another operations.
*
************************************************/
/******************************************

  Performance Algebra

*******************************************/

#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "AggrCube.h"
#include "Cnode.h"
#include "Cartesian.h"
#include "Machine.h"
#include "Metric.h"
#include "Node.h"
#include "Predicates.h"
#include "Process.h"
#include "Region.h"
#include "Thread.h"
#include "algebra.h"
#include "Filter.h"

#include "cube_error.h"

using namespace std;
using namespace cube;

#define VERSION "1.4"
#define PATTERNS_URL "@mirror@scalasca_patterns-" VERSION ".html#"
#define REGIONS_URL "@mirror@scalasca_regions-" VERSION ".html#"


namespace cube {
  /* Prototypes */ 
  /* Metric dimension */
/// @cond PROTOTYPES
  bool metric_merge (Cube& newCube, const Cube& input);
  void metric_remap (Cube& newCube, const Cube& input);
  void copy_tree (Cube& newCube, Metric& rhs);
  bool compare_tree (Cube& newCube, Metric& lhs, Metric& rhs);

  /* Program dimension */
  bool cnode_merge (Cube& newCube, const Cube& input);
  bool cnode_reroot (Cube& newCube, const Cube& input, string cn_newrootname, 
                                  vector<string> cn_prunes, CubeMapping& cubeMap);
  void copy_tree (Cube& newCube, Cnode& rhs, Cnode& parent,
                                  vector<string> cn_prunes, CubeMapping& cubeMap);
  void copy_tree (Cube& newCube, Cnode& rhs, Cnode& parent);
  bool compare_tree (Cube& newCube, Cnode& lhs, Cnode& rhs);
  Cnode* def_cnode (Cube& newCube, Cnode& rhs, Cnode *parent);
  
  /* System dimension */
  bool sysres_merge (Cube& newCube, Cube& small, const Cube& big, bool subset, bool collapse);
  bool is_subset  (Cube& lhs, const Cube& rhs, bool& bigger_left, bool& is_equal);
  bool is_subset (const vector<Node*>& mnodes, const vector<Node*>& snodes, bool& is_equal);
  bool is_subset (const vector<Process*>& big, const vector<Process*>& small, bool &is_equal);
  void merge (Cube& newCube, const vector<Machine*>& big, bool subset, bool collapse);
  void plain_merge (Cube& newCube, Cube& lhs, const Cube& rhs);
  void copy_tree (Cube& newCube, Machine& oldMach, Machine& newMach, bool subset, bool collapse);
  void copy_tree (Cube& newCube, Node& oldNode, Node& newNode);
  void def_pro (Cube& newCube, Process& lhs, Node& rhs);
  
  /* Mapping */
  void createMappingMetric(Cube& newCube, Cube& comp, CubeMapping& cubeMap);
  void createMappingCnode(Cube& newCube, Cube& comp, CubeMapping& cubeMapping);
  void createMappingSystem (Cube& newCube, Cube& comp, CubeMapping& cubeMap, bool collapse);

  /* Topologies */
  void add_top(Cube& newCube, Cube& oldCube, CubeMapping& cubeMap);

  /* Data set operations */
  void add_sev (Cube& newCube, Cube& oldCube, CubeMapping& cubeMapw, double fac);
  void set_sev (Cube& newCube, Cube& oldCube, CubeMapping& cubeMap);
  bool check_sev (Cube& newCube, Cube& oldCube, CubeMapping& cubeMap);
  void copy_sev(Cube& newCube, Metric* src, Metric* dest, Cnode* cnode);
  void add_sev(Cube& newCube, Metric* src, Metric* dest, Cnode* cnode);

  /* Filtering operations */
  void _cube_apply_filter(Cube& lhs, const Cube& rhs,
    Cnode* lhs_parent, Cnode* rhs_parent, CubeMapping& mapping,
    Filter& filter);
  void add_sev(Cube& newCube, const Cube& oldCube, CubeMapping& old2new,
    Cnode* oldCnode, bool with_visits = true);

/// @endcond
  /* End of prototypes */
  
  /* Merge metric forest  */
/**
* Merging only metrics from one cube ("input") into another cube ("newCube"). 
It returns whether the new cube allready had an identical  tree of metrics. 
But merging happens only in the case the "newCube" hasn't a tree with same root 
element as "input" has.
********/
  bool metric_merge (Cube& newCube, const Cube& input) {
    bool is_equal = true;
    /* Get roots */
    const vector<Metric*> &newroot  = newCube.get_root_metv ();
    const vector<Metric*> &inroot   = input.get_root_metv ();
    /* Compare root trees */
    for (size_t i = 0; i < inroot.size(); i++) { 
      Metric* rhs  = inroot[i];
      bool    in = false;
      for (size_t j = 0; j < newroot.size(); j++) {
        Metric* lhs = newroot[j];
        if (rhs->get_uniq_name() == lhs->get_uniq_name()) {
          in = true;
          /* If tree already exists then compare whole tree */
          if (!compare_tree(newCube, *lhs, *rhs))
            is_equal = false;
        }
      }
      if (!in) {
        /* If tree does not exist then copy whole tree */
        newCube.def_met(rhs->get_disp_name(), rhs->get_uniq_name(), 
                        rhs->get_dtype(), rhs->get_uom(), rhs->get_val(), rhs->get_url() , 
                        rhs->get_descr(), NULL); 
        copy_tree(newCube, *rhs);
        is_equal = false;
      }  
    }
    return is_equal;
  }

  /* Re-map metric forrest */
/**
* Setting a values of metrics in "newCube" to values of metrics in "input".  
If necessary, the new metric in "newCube" is created.
********/
  void metric_remap (Cube& newCube, const Cube& input) {
    const vector<Metric*> &inmetv = input.get_metv();
    for (size_t i = 0; i < inmetv.size(); i++) { 
      Metric* rhs = inmetv[i];
      Metric* lhs = newCube.get_met(rhs->get_uniq_name());

      if (lhs)
        lhs->set_val(rhs->get_val());
      else
        newCube.def_met(rhs->get_disp_name(), rhs->get_uniq_name(), 
                        rhs->get_dtype(), rhs->get_uom(), rhs->get_val(),
                        rhs->get_url(), rhs->get_descr(), NULL);
    }
  }

  /* Compare two metric trees */
/**
* Compares "lhs"-tree with "rhs"-tree using recursive deep search method. 
In the case "newCube" doesn't have a metric tree it will be copied.
*/
  bool compare_tree (Cube& newCube, Metric& lhs,  Metric& rhs) {
    bool is_equal = true;
    for (unsigned int i = 0; i < rhs.num_children(); i++) {
      Metric* it = rhs.get_child(i);
      bool in = false;
      for (unsigned int j = 0; j < lhs.num_children(); j++) {
        if (it->get_uniq_name() == lhs.get_child(j)->get_uniq_name()) {/* Tree already exists? */
          in = true;
          if (it->num_children() != 0) { /* Has metric children? */
            if ( !compare_tree(newCube, *lhs.get_child(j), *it)) {
              is_equal = false;
              break;
            }
          }
        } 
      }
      if (!in) {
        /* Create metric and copy tree */
        newCube.def_met(it->get_disp_name(), it->get_uniq_name(), 
                        it->get_dtype(), it->get_uom(), it->get_val(), it->get_url(), 
                        it->get_descr(), newCube.get_met(rhs.get_uniq_name())); 
        copy_tree(newCube, *it);
        is_equal = false;
      }
    }
    return is_equal;
  }  
  
  /* Copy metric tree */
/**
* Copy a  "rhs"-tree into  "newCube" using recursive deep search method. 
*/
  void copy_tree (Cube& newCube, Metric& rhs) {
    Metric* met = newCube.get_met(rhs.get_uniq_name());
    for (unsigned int i = 0; i < rhs.num_children(); i++) {
      Metric *it = rhs.get_child(i);
      newCube.def_met(it->get_disp_name(), it->get_uniq_name(), 
                      it->get_dtype(), it->get_uom(), it->get_val(),
                      it->get_url(), it->get_descr(), met); 
      if (rhs.num_children() != 0)
        copy_tree(newCube, *rhs.get_child(i));
    }
  }
  
  /* Merge cnode forest  */
/**
*Merging a cnodes tree of "input" cube into the "newCube" cube. 
It returns whether the new cube allready has an identical  tree of cnodes. 
But merging happens only in the case, the "newCube" hasn't a tree with same 
root element as "input" has.
*/
  bool cnode_merge (Cube& newCube, const Cube& input) {
    bool is_equal = true;
    /* Get roots */
    const vector<Cnode*> &newroot  = newCube.get_root_cnodev ();
    const vector<Cnode*> &inroot   = input.get_root_cnodev ();
    /* Compare root trees */
    for (size_t i = 0; i < inroot.size(); i++) { 
      Cnode* rhs  = inroot[i];
      bool    in = false;
      for (size_t j = 0; j < newroot.size(); j++) {
        Cnode* lhs = newroot[j];
        if ( *rhs == *lhs ) {  /* Cnode found => compare tree */
          in = true;
          if (!compare_tree(newCube, *lhs, *rhs)) {
            is_equal = false;
            break;
          }
        }
      }
      if (!in) { /* Not found => create cnode and copy tree */
        Cnode* newCnode = def_cnode(newCube, *rhs, NULL);
        copy_tree(newCube, *rhs, *newCnode);
        is_equal = false;
      }  
    }
    return is_equal;
  }

  /* Copy input Cnode-tree (only from newroot if specified) without pruned Cnodes */
/**
* Copy all (if "newrootname" isn't specified) or some (having callee with a name
 "newrootname") cnodes from "input" into "newCube". Returns if some elements were copyied.
*/
  bool cnode_reroot (Cube& newCube, const Cube& input, string cn_newrootname,
                     vector<string> cn_prunes, CubeMapping& cubeMap) {
    // if newrootname specified, search entire input cnodev for it
    // otherwise retain vector of existing roots
    const vector<Cnode*> &incnodev = cn_newrootname.empty() ?
                                input.get_root_cnodev () : input.get_cnodev ();
    bool got_root = false;

    /* Get roots */
    for (size_t i = 0; i < incnodev.size(); i++) { 
      Cnode* rhs  = incnodev[i];
      if ( cn_newrootname.empty() || *rhs->get_callee() == cn_newrootname) {
        /* matching Cnode found => copy tree */
        Cnode* newCnode = def_cnode(newCube, *rhs, NULL);
        cubeMap.cnodem[rhs]=newCnode;
        copy_tree(newCube, *rhs, *newCnode, cn_prunes, cubeMap);
        got_root = true;
      }
    }
    return got_root;
  }
/**
* Comparing of two "rhs"-tree with "lhs"-tree. In the case "newCube" doesn't 
have some elements of "lhs" they will be copied in to the "newCube" cube.
*/
  bool compare_tree (Cube& newCube, Cnode& lhs,  Cnode& rhs) {
    bool is_equal = true;
    for (unsigned int i = 0; i < rhs.num_children(); i++) {
      Cnode* it = rhs.get_child(i);
      bool in = false;
      for (unsigned int j = 0; j < lhs.num_children(); j++) {
        if (*it == *lhs.get_child(j)) {
          in = true;
          if (it->num_children() != 0)
            if (!compare_tree(newCube, *lhs.get_child(j), *it)) {
              is_equal = false;
              break;
            }
        } 
      }
      if (!in) {
        copy_tree(newCube, *it, *def_cnode(newCube, *it, &lhs));
        is_equal = false;
      }
    }
    return is_equal;
  }

  /* Copy Cnode-tree */
/**
* Recursive copying of a "rhs"-tree into "newCube" without any conditions.
*/
  void copy_tree (Cube& newCube, Cnode& rhs, Cnode& parent) {
    for (unsigned int i = 0; i < rhs.num_children(); i++) {
      Cnode* newCnode = def_cnode(newCube, *rhs.get_child(i), &parent);
      if (rhs.num_children() != 0)
        copy_tree(newCube, *rhs.get_child(i), *newCnode);
    }
  }

  /* Copy Cnode-tree */
/**
* Recursive copying of a "rhs"-tree into "parent" cnode of "newCube" with pruning (??????).
*/
  void copy_tree (Cube& newCube, Cnode& rhs, Cnode& parent,
                  vector<string> prunes, CubeMapping& cubeMap) {
    const Region* par=parent.get_callee(); // newCnode in newCube for rhs
    const Region* reg=rhs.get_callee();
      
    for (unsigned int i = 0; i < rhs.num_children(); i++) {
      bool prune=(par->get_name() != reg->get_name());
      const Region* child=rhs.get_child(i)->get_callee();

      for (size_t p = 0; p < prunes.size(); p++)
        if (child->get_name() == prunes[p]) prune=true;

      // copy to parent when pruning, otherwise create matching Cnode in newCube
      Cnode* newCnode = prune ? &parent : def_cnode(newCube, *rhs.get_child(i), &parent);
      cubeMap.cnodem[rhs.get_child(i)]=newCnode;
      copy_tree(newCube, *rhs.get_child(i), *newCnode, prunes, cubeMap);
    }
  }
/**
* Define and return a cnode. Returns directly if the "newCube" has already the 
right "region" or the region will be created.
*/
  Cnode* def_cnode (Cube& newCube, Cnode& rhs, Cnode* parent) {
    const vector<Region*> &newRegv  = newCube.get_regv();
    int num = newRegv.size();

    for (int i = 0; i < num; i++ ) {
      if (*newRegv[i] == *rhs.get_callee())
        return newCube.def_cnode(newRegv[i], rhs.get_mod(),rhs.get_line(),parent);
    }
    
    Region* newregion;
    newregion = newCube.def_region(rhs.get_callee()->get_name(),rhs.get_callee()->get_begn_ln(),
                                   rhs.get_callee()->get_end_ln(),rhs.get_callee()->get_url(),
                                   rhs.get_callee()->get_descr(),rhs.get_callee()->get_mod() );
    return newCube.def_cnode(newregion, rhs.get_mod(),rhs.get_line(),parent);
  }

  /* set undefined URLs according to descr field */
  void set_region_urls (Cube& cube) {
    const vector<Region*> &regv = cube.get_regv();
    for (size_t r = 0; r < regv.size(); r++) {
      Region* reg = regv[r];
      if ((reg->get_url() == "") && (reg->get_descr() != ""))
          reg->set_url(REGIONS_URL + reg->get_descr());
    }
  }

  /* Merge system dimension */
/**
* Merging of two ("lhs" and "rhs") system resources tree into third one "newCube".
 If "collapse" set on "false" -> only bigger tree will be merged. Returns whether a "lhs" and "rhs" are equal.
*/
  bool sysres_merge (Cube& newCube, Cube& lhs, const Cube& rhs, bool subset, bool collapse) {
    /* Get roots */
    bool is_equal = true;
    const vector<Machine*> &mroot  = lhs.get_machv();
    const vector<Machine*> &sroot  = rhs.get_machv();
    bool bigger_left;
    if( (is_subset(lhs, rhs, bigger_left, is_equal) == true) && !collapse) { /* Are system trees compatible? */
      if (bigger_left) { 
        merge(newCube, mroot, subset, collapse);      
      } else {
        merge(newCube, sroot, subset, collapse);      
      }
    } else {
      is_equal = false;
      if (!subset && !collapse) {
        if (bigger_left) { 
          merge(newCube, mroot, subset, collapse);
        } else {
          merge(newCube, sroot, subset, collapse);      
        }
      } else {
        plain_merge(newCube, lhs, rhs);
      }
    }
    return is_equal;
  }
/**
* Merging "big" into "newCube". If the "big" not a subset of machines in "newCube",
 the needed "machines" will be created.
*/ 
  void merge (Cube& newCube, const vector<Machine*>& big, bool subset, bool collapse) {
    int num = big.size();
    for (int i = 0; i < num; i++) { 
      Machine* rhs  = big[i];
      stringstream num;
      num << i;
      if ( subset && !collapse)
        copy_tree(newCube, *rhs, *newCube.def_mach(rhs->get_name(),""), subset, collapse);
      else
        copy_tree(newCube, *rhs, *newCube.def_mach(("Machine " + num.str()),""), subset, collapse);
    }
  }    
/**
* Recursive copying of "oldMach" tree into "newMach" tree using a deep search algorithm.
*/
  void copy_tree (Cube& newCube, Machine& oldMach, Machine& newMach, bool subset, bool collapse) {
    int num = oldMach.num_children();
    for (int i = 0; i < num; i++) {
      stringstream num;
      num << i;
      if ( subset && !collapse)
        copy_tree(newCube, *oldMach.get_child(i),*newCube.def_node(oldMach.get_child(i)->get_name(), &newMach));
      else
        copy_tree(newCube, *oldMach.get_child(i),*newCube.def_node(("Node " + num.str()), &newMach));
    }
  }
 
/**
* Copyying of "nodes" of a machine.
*/
  void copy_tree (Cube& newCube, Node& oldNode, Node& newNode) {
    int num = oldNode.num_children();
    for (int i = 0; i < num; i++) {
      def_pro(newCube, *oldNode.get_child(i), newNode);
    }
  }
/**
* Creating of a "system" dimension in "newCube" using bigger number of processes 
and threads of "rhs" and "lhs". Number of processes and threads have to have a common divider. 
Otherwise it stops the program.
*/
  void plain_merge (Cube& newCube, Cube& lhs, const Cube& rhs) {
    int num = (lhs.get_procv().size() > rhs.get_procv().size()) 
      ? lhs.get_procv().size() : rhs.get_procv().size();
    int thrd = (lhs.get_thrdv().size() > rhs.get_thrdv().size()) 
      ? lhs.get_thrdv().size() : rhs.get_thrdv().size();

    int thrds_per_pro = thrd / num;
    if ( thrd % num != 0) {
      cerr<<"No valid cube experiment!"<<endl;
      exit(0);
    }
      
    Machine* mach = newCube.def_mach("Virtual machine", "");
    
    Node* node = newCube.def_node("Virtual node", mach);
    for ( int i = 0; i < num; i++) { 
      stringstream num;
      num << i;
      Process* newproc = newCube.def_proc(("Process " + num.str()),i, node); 
      for ( int j = 0; j < thrds_per_pro; j++) { 
        stringstream mynum;
        mynum << j;
        newCube.def_thrd(("Thread " +mynum.str()), j, newproc);
      }
    } 
  }

/**
* Create "process" and threads just like in "lhs" Process.
*/
  void def_pro (Cube& newCube, Process& lhs, Node& rhs) {
    Process* newproc = newCube.def_proc(lhs.get_name(), lhs.get_rank(), &rhs); 
    int num = lhs.num_children();
#if 1
    /* possibly copy a minimal number of threads, even if they are VOID */
    /* when only a single process on node and XT_NODE_CORES is set */
    char *env = getenv("XT_NODE_CORES");
    int min_thrds = (env && (atoi(env) > 0)) ? atoi(env) : 1;
    if ((lhs.get_parent()->num_children() == 1) && (min_thrds > 1)) {
      for ( int i = 0; i < num; i++) {
        string thrd_name = lhs.get_child(i)->get_name();
        if ((thrd_name != "VOID") || (i < min_thrds))
            newCube.def_thrd(thrd_name, i, newproc);
      }
      if ((min_thrds > 1) &&
                (newproc->get_child(newproc->num_children()-1)->get_name() == "VOID"))
          cout << "Retained minimum of " << min_thrds << " threads for process "
               << lhs.get_rank() << endl;
    } else
#endif
    for ( int i = 0; i < num; i++) { /* copy non-VOID threads */
      string thrd_name = lhs.get_child(i)->get_name();
      if (thrd_name != "VOID")
          newCube.def_thrd(thrd_name, i, newproc);
    }
  }

/**
* Testing is a "lhs" and "rhs" Machines are subsets of eachother (result saved 
in "bigger_left") or are equal.
*/
  bool is_subset (Cube& lhs, const Cube& rhs, bool& bigger_left, bool& is_equal) {
    const vector<Machine*> &lroot    = lhs.get_machv();
    const vector<Machine*> &inroot  = rhs.get_machv();
    if (lroot.size() != inroot.size())
      is_equal = false;
    
    const vector<Node*>    &lnode    = lhs.get_nodev();
    const vector<Node*>    &innode  = rhs.get_nodev();
    const vector<Process*> &lproc    = lhs.get_procv();
    const vector<Process*> &inproc  = rhs.get_procv();
    const vector<Thread*> &lthrd    = lhs.get_thrdv();
    const vector<Thread*> &inthrd  = rhs.get_thrdv();

    if ( lproc.size() < inproc.size() &&  lthrd.size() < inthrd.size()) {
      bigger_left = false;
      is_equal    = false;
      return (is_subset(innode, lnode, is_equal) && is_subset(inproc, lproc, is_equal));
    } else if ( lproc.size() >= inproc.size() &&  lthrd.size() >= inthrd.size()){

      bigger_left = true;

      if ( lproc.size() != inproc.size() &&  lthrd.size() != inthrd.size())
        is_equal = false;

      if ((is_subset(lnode, innode, is_equal) && is_subset(lproc, inproc, is_equal))) {
        return true;
      } else {
        return false;
      }
    } else {
      bigger_left = false;
      is_equal = false;
      return false;
    }
  }

/**
* Testing is a "big" and "small" Processes are subsets of eachother  or are equal. 
*/
  bool is_subset (const vector<Process*>& big, const vector<Process*>& small, bool &is_equal) {
    bool subset = true;  
    size_t num = big.size();
    for ( size_t i = 0; i < num; i++) {
      if ( small.size() == i ) { /* Has the bigger experiment more nodes then break! */
        break;
      }
      int big_num   = big[i]->num_children();
      int small_num = small[i]->num_children();
      if ( big_num < small_num ) {
        subset = false;
        is_equal = false;
      } else if ( big_num > small_num ) {
        is_equal = false;
      }
    }
    return subset;
  }

/**
* Testing is the "big" and the "small" nodes of a machine are subsets of eachother  or are equal. 
*/
  bool is_subset (const vector<Node*>& big, const vector<Node*>& small, bool &is_equal) {
    bool subset = true;  
    size_t num = big.size();
    for ( size_t i = 0; i < num; i++) {
      if ( small.size() == i ) { /* Has the bigger experiment more nodes then break! */
        break;
      }
      int big_num   = big[i]->num_children();
      int small_num = small[i]->num_children();
      if ( big_num < small_num ) {
        subset = false;
        is_equal = false;
      } else if ( big_num > small_num ) {
        is_equal = false;
      }
    }
    return subset;
  }


/**
* Makes a connection ("mapping") between identical metrics, cnodes and threades 
and saves it in "cubeMap". The connection is allways map[metric1]-> metric2 . 
(-> stands for "points at"). Mapping of threads is tricky.
*/
  void createMapping (Cube& newCube, Cube& comp, CubeMapping& cubeMap, bool collapse) {
    createMappingMetric(newCube, comp, cubeMap);
    createMappingCnode(newCube, comp, cubeMap);
    createMappingSystem(newCube, comp, cubeMap, collapse);
  }

  void createMappingMetric(Cube& newCube, Cube& comp, CubeMapping& cubeMap)
  {
    /* Mapping of the metric dimension */
    const vector<Metric*> &newCubemet  = newCube.get_metv ();
    const vector<Metric*> &compmet  = comp.get_metv ();
    int num_met_new = newCubemet.size();
    for (int i = 0; i < num_met_new; i++) {
      Metric* it = newCubemet[i];
      int num_met_old = compmet.size();
      for (int j = 0; j < num_met_old; j++) {
        Metric* cp = compmet[j];
        if ( *it == *cp ){
          cubeMap.metm[cp]=it;
          break;
        }
      }
    }
  }

  void createMappingCnode(Cube& newCube, Cube& comp, CubeMapping& cubeMap)
  {
    /* Mapping of the program dimension */
    const vector<Cnode*> &newCubecnode  = newCube.get_cnodev ();
    const vector<Cnode*> &compcnode  = comp.get_cnodev ();
    int num_cnode_new = newCubecnode.size();
    for (int i = 0; i < num_cnode_new; i++) {
      Cnode* it = newCubecnode[i];
      int num_cnode_old = compcnode.size();
      for (int j = 0; j < num_cnode_old; j++) {
        Cnode* cp = compcnode[j];
        if ( *it == *cp ) {
          cubeMap.cnodem[cp]=it;
          break;
        }
      }
    }
  }
    
  void createMappingSystem (Cube& newCube, Cube& comp, CubeMapping& cubeMap, bool collapse) {
    /* Mapping of the system dimension */
    const vector<Machine*> &newCubemach     = newCube.get_machv ();
    for ( size_t i = 0; i < newCubemach.size(); i++ ) {
      string oldDesc = newCubemach[i]->get_desc();
      newCubemach[i]->set_desc(oldDesc);
    }
    const vector<Node*> &newCubenode     = newCube.get_nodev ();
    const vector<Node*> &compnode        = comp.get_nodev ();
    const vector<Process*> &newCubeproc  = newCube.get_procv ();
    const vector<Process*> &compproc     = comp.get_procv ();
    int num_nodes_new = newCubenode.size();
    int num_nodes_old =  compnode.size();
    int num_procs_new = newCubeproc.size();
    int num_procs_old = compproc.size();
    bool tmp;
    if ( num_nodes_new >= num_nodes_old 
         && is_subset(newCubenode, compnode, tmp) 
         && is_subset(newCubeproc, compproc, tmp)
         && !collapse ) {
      int num = (num_nodes_new == num_nodes_old) ? num_nodes_new : num_nodes_old;
      for (int i = 0; i < num; i++) {
        Node* cp_it = compnode[i];
        Node* newCube_it = newCubenode[i];
        int num_children = cp_it->num_children();
        for ( int k = 0; k < num_children ; k++) {
          Process* newCubep = newCube_it->get_child(k);
          Process* cp = cp_it->get_child(k);
          if ( *newCubep == *cp ) {
            int cp_thrds = cp->num_children();
            for ( int m = 0; m < cp_thrds; m++)
              cubeMap.sysm[cp->get_child(m)]=newCubep->get_child(m);
          }
        }
      }
    } else if ( num_procs_new == num_procs_old ) { // map threads process-to-process
      for (int k = 0; k < num_procs_old ; k++) {
        Process* newCubep = newCubeproc[k];
        Process* cp = compproc[k];
        int num_thrds = min(newCubep->num_children(), cp->num_children());
        for (int m = 0; m < num_thrds ; m++)
          cubeMap.sysm[cp->get_child(m)]=newCubep->get_child(m);
      }
    } else { // map threads in the order they are globally defined
      const vector<Thread*> newCubethrd  = newCube.get_thrdv ();
      const vector<Thread*> &compthrd   = comp.get_thrdv ();
      int num_thrd_old = compthrd.size();
      for (int i = 0; i < num_thrd_old ; i++) {
        cubeMap.sysm[const_cast<Thread*>(compthrd[i])]=const_cast<Thread*>(newCubethrd[i]);

        ostringstream id_map;
        id_map << "(" << compthrd[i]->get_parent()->get_rank() 
               << "," << compthrd[i]->get_rank() << ") to "
               << "(" << newCubethrd[i]->get_parent()->get_rank() 
               << "," << newCubethrd[i]->get_rank() << ");";
        string oldDesc = newCubethrd[i]->get_parent()->get_parent()->get_parent()->get_desc();

        newCubethrd[i]->get_parent()->get_parent()
                      ->get_parent()->set_desc(oldDesc + "\n" + id_map.str()); 
      }
    }
  }

/**
* Copies the severities of "oldCube" in to matrix of severities in "newCube" 
according to the cube mapping "cubeMap" multiplying every value with factor "fac".
*/
  void add_sev (Cube& newCube, Cube& oldCube, CubeMapping& cubeMap, double fac) {
    map<Metric*, Metric*>::iterator  miter;
    map<Cnode*, Cnode*>::iterator  citer;
    map<Thread*, Thread*>::iterator  siter;
    for ( miter = cubeMap.metm.begin(); miter != cubeMap.metm.end(); miter++) {
      for ( citer = cubeMap.cnodem.begin(); citer != cubeMap.cnodem.end(); citer++) {
        for ( siter = cubeMap.sysm.begin(); siter != cubeMap.sysm.end(); siter++) {
          double val = oldCube.get_sev( miter->first, citer->first, siter->first);
          val *= fac;
          if ( val != 0.0)
            newCube.add_sev (miter->second, citer->second, siter->second, val);
        }
      }
    }
  }
/**
* Copies the severities of "oldCube" in to matrix of severities in "newCube" 
according to the cube mapping "cubeMap".
*/
  void set_sev (Cube& newCube, Cube& oldCube, CubeMapping& cubeMap) {
    map<Metric*, Metric*>::iterator  miter;
    map<Cnode*, Cnode*>::iterator  citer;
    map<Thread*, Thread*>::iterator  siter;
    for ( miter = cubeMap.metm.begin(); miter != cubeMap.metm.end(); miter++) {
      for ( citer = cubeMap.cnodem.begin(); citer != cubeMap.cnodem.end(); citer++) {
        for ( siter = cubeMap.sysm.begin(); siter != cubeMap.sysm.end(); siter++) {
          double val = oldCube.get_sev( miter->first, citer->first, siter->first);
          if ( val != 0.0) {
            newCube.set_sev (miter->second, citer->second, siter->second, val);
          }
        }
      }
    }
  }
/**
* Compares  the severities of "oldCube" with severities in "newCube" according 
to the cube mapping "cubeMap" and returns whether all are equal.
*/
  bool check_sev (Cube& newCube, Cube& oldCube, CubeMapping& cubeMap) {
    bool is_equal = true;
    map<Metric*, Metric*>::iterator  miter;
    map<Cnode*, Cnode*>::iterator  citer;
    map<Thread*, Thread*>::iterator  siter;
    for ( miter = cubeMap.metm.begin(); miter != cubeMap.metm.end(); miter++) {
      for ( citer = cubeMap.cnodem.begin(); citer != cubeMap.cnodem.end(); citer++) {
        for ( siter = cubeMap.sysm.begin(); siter != cubeMap.sysm.end(); siter++) {
          double oldval = oldCube.get_sev( miter->first, citer->first, siter->first);
          double newval = newCube.get_sev (miter->second, citer->second, siter->second);
          if (oldval != newval) {
            is_equal = false;
            break;
          }
        }
      }
    }
    return is_equal;
  }
/**
* Adds in to "newCube" topology from the "oldCube" according the cube mapping "cubeMap".
*/
  void add_top(Cube& newCube, Cube& oldCube, CubeMapping& cubeMap) {
    const vector<Cartesian*>& cartv = oldCube.get_cartv();
#ifdef VERBOSE
    if (!cartv.empty())
        {
          cout<< endl << "\tTopology retained in experiment."<<endl;
        }
#endif
    int num = cartv.size();
    for (int i = 0; i < num; i++) {
      Cartesian *newCart = newCube.def_cart (cartv[i]->get_ndims(), 
                                             cartv[i]->get_dimv(), 
                                             cartv[i]->get_periodv());
      newCart->set_name(cartv[i]->get_name());
      newCart->set_namedims(cartv[i]->get_namedims());
      const multimap<const Sysres*, vector<long> >&  coordv = cartv[i]->get_cart_sys();
      map<Thread*, Thread*>::iterator th_it;
      for ( th_it = cubeMap.sysm.begin(); th_it != cubeMap.sysm.end(); ++th_it) {
	multimap<const Sysres*, vector<long> >::const_iterator  it;
	    pair<multimap<const Sysres*, vector<long> >::const_iterator, multimap<const Sysres*, vector<long> >::const_iterator> ret;
	    ret = coordv.equal_range( ( Thread* )( th_it->first ) );
	    if ( ret.first  != ret.second ){
		for ( it = ret.first; it != ret.second; ++it ) {
		    newCube.def_coords( newCart, cubeMap.sysm[ ( Thread* )( th_it->first ) ], ( *it ).second );
		}
	    }
      }
    }
  }

/**
* Merges two lists of topologies
*/
  void merge_top(Cube& newCube, Cube& cube1, Cube& cube2) {
    const vector<Cartesian*>& old_top1 = cube1.get_cartv();
    const vector<Cartesian*>& old_top2 = cube2.get_cartv();
    vector<Cartesian*> new_top;

    for (unsigned i=0; i<old_top1.size(); i++)
    {
        if ( new_top.empty() ) 
        {  
            new_top.push_back((old_top1[i])->clone(newCube.get_thrdv()));  //
        }
        else
        {
            bool present = true;
            for (unsigned j=0; j<new_top.size(); j++)
            {
                 present = present && (  *(new_top[j]) == *(old_top1[i]) );
            }
             if (!present) {  
                new_top.push_back((old_top1[i])->clone(newCube.get_thrdv()));  }
        }
    }
    for (unsigned i=0; i<old_top2.size(); i++)
    {
        if ( new_top.empty() ) 
        {
            new_top.push_back((old_top2[i])->clone(newCube.get_thrdv()));
        }
        else
        {
            bool present = true;
            for (unsigned j=0; j<new_top.size(); j++)
            {
                 present = present && (*(new_top[j]) == *(old_top2[i]));
            }
             if (!present) {  
                    new_top.push_back((old_top2[i])->clone(newCube.get_thrdv()));  }
        }
    }
     newCube.add_cart(new_top);
  }


/**
* Set all severities (for every thread) in "newCube" for given Metric "src" and 
Cnode "cnode" into Metric "dest" and same "cnode". 
*/
  void copy_sev(Cube& newCube, Metric* src, Metric* dest, Cnode* cnode) {
    const vector<Thread*>& threadv = newCube.get_thrdv();

    vector<Thread*>::const_iterator it = threadv.begin();
    while (it != threadv.end()) {
      newCube.set_sev(dest, cnode, *it, newCube.get_sev(src, cnode, *it));
      ++it;
    }
  }
/**
* Add all severities (for every thread) in "newCube" for given Metric "src" and 
Cnode "cnode" into Metric "dest" and same "cnode". 
*/
  void add_sev(Cube& newCube, Metric* src, Metric* dest, Cnode* cnode) {
    const vector<Thread*>& threadv = newCube.get_thrdv();

    vector<Thread*>::const_iterator it = threadv.begin();
    while (it != threadv.end()) {
      newCube.add_sev(dest, cnode, *it, newCube.get_sev(src, cnode, *it));
      ++it;
    }
  }

/**
* Applys a filter to a cube file.
*/
  void cube_apply_filter(Cube& newCube, Cube& rhs, Filter& filter,
                         CubeMapping& mapping)
  {
    Cube tmp = Cube();
    const vector<string>& mirrors = rhs.get_mirrors();
    for (unsigned i=0; i<mirrors.size(); i++) newCube.def_mirror(mirrors[i]);
    newCube.def_attr("CUBE_CT_AGGR", "SUM");
    metric_merge(newCube, rhs);
    sysres_merge(newCube, tmp, rhs, true, false);
    createMappingMetric(newCube, rhs, mapping);
    createMappingSystem(newCube, rhs, mapping, false);
    add_top (newCube, rhs, mapping);
    const vector<Cnode*> root_nodes = rhs.get_root_cnodev();
    for (vector<Cnode*>::const_iterator it = root_nodes.begin();
          it != root_nodes.end(); ++it)
    {
      Cnode* new_root = def_cnode(newCube, **it, NULL);
      mapping.cnodem.insert(pair<Cnode*,Cnode*>(*it, new_root));
      add_sev(newCube, rhs, mapping, *it);
      _cube_apply_filter(newCube,rhs,new_root,*it,mapping,filter);
    }
  }
  
  void _cube_apply_filter(Cube& lhs, const Cube& rhs,
    Cnode* lhs_parent, Cnode* rhs_parent, CubeMapping& mapping,
    Filter& filter)
  {
    for (unsigned int i = 0; i < rhs_parent->num_children(); ++i)
    {
      bool found = false;
      Cnode* rhs_child = rhs_parent->get_child(i);
      Cnode* lhs_child = NULL;
  
      if (filter.matches(rhs_child))
        continue;
  
      for (unsigned int j = 0; j < lhs_parent->num_children(); ++j)
      {
        lhs_child = lhs_parent->get_child(j);
        if (*(lhs_child->get_callee()) == *(rhs_child->get_callee()))
        {
          found = true;
          break;
        }
      }
      if (found == false)
        lhs_child = def_cnode(lhs, *rhs_child, lhs_parent);
      mapping.cnodem.insert(pair<Cnode*, Cnode*>(rhs_child, lhs_child));
      add_sev(lhs, rhs, mapping, rhs_child);
    }
  
    // Now go into depth:
    for (unsigned int i = 0; i < rhs_parent->num_children(); ++i)
    {
      Cnode* rhs_child = rhs_parent->get_child(i);
  
      if (filter.matches(rhs_child) == true)
      {
        mapping.cnodem.insert(pair<Cnode*, Cnode*>(rhs_child, lhs_parent));
        add_sev(lhs, rhs, mapping, rhs_child, false);
        _cube_apply_filter(lhs, rhs, lhs_parent, rhs_child,
          mapping, filter);
      }
      else
      {
        _cube_apply_filter(lhs, rhs, mapping.cnodem[rhs_child], rhs_child,
          mapping, filter);
      }
    }
  }
  
  void add_sev(Cube& newCube, const Cube& oldCube, CubeMapping& old2new,
    Cnode* oldCnode, bool with_visits)
  {
    map<Metric*, Metric*>::iterator miter;
    map<Cnode*,  Cnode* >::iterator citer;
    map<Thread*, Thread*>::iterator siter;
  
    citer = old2new.cnodem.find(oldCnode);
    if (citer == old2new.cnodem.end())
      throw RuntimeError("Could not find the node "
        + oldCnode->get_callee()->get_name() + " in mapping.");
  
    for ( miter = old2new.metm.begin(); miter != old2new.metm.end(); miter++) {
      if (with_visits == false && miter->first->get_uniq_name() == "visits")
        continue;
      for ( siter = old2new.sysm.begin(); siter != old2new.sysm.end(); siter++) {
        double val = oldCube.get_sev( miter->first, citer->first, siter->first);
        if ( val != 0.0)
          newCube.add_sev (miter->second, citer->second, siter->second, val);
      }
    }
  }

/**
*An attempt to use xtprocadmin mapfile to remap Cray XT nodenames
* and generate topologies (based on cabinets and mesh)
*/
  void node_remap(Cube& newCube, string dirname) {
    // verify first node has (unmapped) name in expected format "node<NID>"
    const vector<Node*>& nodev = newCube.get_nodev();
    vector<Node*>::const_iterator nit = nodev.begin();
    string node_name = (*nit)->get_name();
    if (node_name.substr(0,4) != "node") {
        //cerr << "First node has unexpected name " << node_name << endl;
        return;
    }

    string mapname = "./xtnids";
    ifstream in(mapname.c_str());
    if (!in) { // no map found in current directory
       if (dirname.empty()) return;
       mapname.erase(0,1);
       mapname.insert(0,dirname);
       in.clear();
       in.open(mapname.c_str());
       if (!in) return; // no map found in given (epik) directory
       // XXXX should also check installdir
    }
    cout << "Remapping " << nodev.size() << " nodes using " << mapname << endl;

    const string header1="   NID    (HEX)    NODENAME     TYPE ARCH        OS CORES";
    const string header2="   NID    (HEX)    NODENAME     TYPE ARCH        OS CPUS";
    char linebuf[1024];

    in.getline(linebuf,1024);
    if ((strncmp(linebuf, header1.c_str(), header1.length()) != 0) && 
        (strncmp(linebuf, header2.c_str(), header2.length()) != 0)) {
        cerr << "Error: Map header mismatch:" << endl << linebuf << endl;
        return;
    }
    
    unsigned CoresPerNode = 0;
    unsigned minCABX=1000;
    unsigned maxCABX=0, maxCABY=0;
    vector<char*> xtnidnames(1024,'\0');
    char nodetype[16], nodearch[16], nodeos[16];

    // parse each map entry for nid & nodename to store in vector of xtnidnames
    // additionally parse each nodename to determine cabinet ranges

    while (!in.eof()) {
        in.getline(linebuf,1024);
        if (in.gcount() == 0) continue;

        unsigned nid, hex, cores;
        char* nodename = (char*)calloc(16, 1);
        if (7 != sscanf(linebuf, "%u %x %16s %s %s %s %u", &nid, &hex,
                        nodename, nodetype, nodearch, nodeos, &cores)) {
            // down compute nodes ('X' in xtstat) have incomplete data
            if (4 == sscanf(linebuf, "%u %x %16s %s", &nid, &hex,
                        nodename, nodetype))
                cerr << "Info: skipping down compute node " << nodename << endl;
            else
                cerr << "Error: Invalid map entry:" << endl << linebuf << endl;
        } else if (nid != hex) {
            cerr << "Error: Inconsistent nid " << nid << "<>" << hex << endl;
        } else {
            //cout << "Parsed: nid=" << nid << " name=" << (nodename ? nodename : "(nil)") 
            //     << " cores=" << cores << endl;
            while (nid >= xtnidnames.size())
                xtnidnames.resize(xtnidnames.size()+1024,'\0');
            xtnidnames[nid] = nodename;
            if (cores > CoresPerNode) CoresPerNode = cores;
            unsigned CABX, CABY, CAGE, SLOT, NODE;
            if (nodename[0] == 'r') { // x2 compute node?
                //cerr << "Ignoring unexpected nodename " << nodename << " for nid " << nid << endl;
            } else
            if (5 != sscanf(nodename, "c%u-%uc%us%un%u", &CABX, &CABY, &CAGE, &SLOT, &NODE)) {
                cerr << "Error: Unparsable nodename " << nodename << " for nid " << nid << endl;
            } else {
                if (CABY>maxCABY) maxCABY=CABY;
                if (CABX>maxCABX) maxCABX=CABX;
                if (CABX<minCABX) minCABX=CABX;
            }
        }
    }

    // Define new topologies
    vector<bool> periodv(3);
    vector<long> dimv(3), coordv(3);
    const unsigned CagesPerCbnt = 3;
    const unsigned SlotsPerCage = 8;
    const unsigned NodesPerSlot = 4;

    char *env = getenv("XT_NODE_CORES");
    if (env) {
        int cores = atoi(env);
        if (cores > 0) {
            CoresPerNode = cores;
            cout << "Using XT_NODE_CORES=" << CoresPerNode << endl;
        } else {
            cerr << "Ignoring invalid XT_NODE_CORES=" << env << endl;
        }
    }
    cout << "Using " << CoresPerNode << " cores per XT node" << endl;

    periodv[0] = false; dimv[0] = ((maxCABY+1)*SlotsPerCage)*CoresPerNode;
    periodv[1] = false; dimv[1] = CagesPerCbnt*NodesPerSlot;
    periodv[2] = false; dimv[2] = maxCABX+1; // (max) number of cabinets
    Cartesian *xtCabsCart = newCube.def_cart (3, dimv, periodv);
    cout << "Generating " << dimv[0] << "*" << dimv[1] << "*" << dimv[2]
         << " topology for " << maxCABX+1 << "*" << maxCABY+1 
         << " Cray XT cabinets" << endl;
#if 0 // extra topologies
    periodv[0] = true; dimv[0] = maxCABX-minCABX+1;
    periodv[1] = true; dimv[1] = (maxCABY+1)*NodesPerSlot*CoresPerNode;
    periodv[2] = true; dimv[2] = CagesPerCbnt*SlotsPerCage;
    Cartesian *xtMeshCart = newCube.def_cart (3, dimv, periodv);
    cout << "Generating " << dimv[0] << "*" << dimv[1] << "*" << dimv[2]
         << " xtshowmesh-like topology for mesh" << endl;

    periodv[0] = true; dimv[0] = CagesPerCbnt*SlotsPerCage*CoresPerNode;
    periodv[1] = true; dimv[1] = (maxCABY+1)*NodesPerSlot;
    periodv[2] = true; dimv[2] = maxCABX-minCABX+1;
    Cartesian *xtMaltCart = newCube.def_cart (3, dimv, periodv);
    cout << "Generating " << dimv[0] << "*" << dimv[1] << "*" << dimv[2]
         << " alternative topology for mesh" << endl;
#endif
    // Remap nodes
    while (nit != nodev.end()) {
      string node_name = (*nit)->get_name();
      unsigned nid = atoi(node_name.substr(4).c_str());
      if (node_name.substr(0,4) != "node") {
          cerr << "Leaving node with unexpected name " << node_name << endl;
      } else if ((nid >= xtnidnames.size()) || !xtnidnames[nid]) {
          cerr << "Skipping " << node_name << " without nid mapping" << endl; 
      } else {
          unsigned CABX, CABY, CAGE, SLOT, NODE;
          sscanf(xtnidnames[nid], "c%u-%uc%us%un%u", &CABX, &CABY, &CAGE, &SLOT, &NODE);
          (*nit)->set_name(xtnidnames[nid]);

          unsigned cabsX = CABY*SlotsPerCage + SLOT;
          unsigned cabsY = CAGE*NodesPerSlot + NODE;
          unsigned cabsZ = CABX;

#if 0 // extra topologies
          unsigned meshX = 0;
          unsigned meshY = 0;
          unsigned meshZ = 0;
          if (CABX%2) meshX = (maxCABX-minCABX) - (CABX-minCABX)/2;
          else        meshX =                     (CABX-minCABX)/2;
#if 0 // old cabinet map
          switch (CABY) {
              case 0: meshY = NODE; break;
              case 1: meshY = NODE + 12; break;
              case 2: meshY = NODE + 4; break;
              case 3: meshY = NODE + 8; break;
              default:
                  cerr << "Unexpected CABY=" << CABY << endl;
          }
#endif
          meshY = NODE + (4*CABY); // simple cabinet map
          if ((CABY==0) || (CABY==3)) switch (CAGE) {
              case 0: meshZ = SLOT; break;
              case 1: meshZ = 15 - SLOT; break;
              case 2: meshZ = SLOT + 16; break;
              default:
                  cerr << "Unexpected CAGE=" << CAGE << endl;
          } else switch (CAGE) {
              case 0: meshZ = SLOT + 16; break;
              case 1: meshZ = 15 - SLOT; break;
              case 2: meshZ = SLOT; break;
              default:
                  cerr << "Unexpected CAGE=" << CAGE << endl;
          }

          unsigned maltX = meshZ;
          unsigned maltY = ((maxCABY+1)*4)-1-meshY;
          unsigned maltZ = (maxCABX-minCABX)-meshX;
#endif

          unsigned processesOnNode = (*nit)->num_children();
          for (unsigned p=0; p<processesOnNode; p++) {
              Process* proc = (*nit)->get_child(p);
              unsigned threads = proc->num_children();
              for (unsigned t=0; t<threads; t++) {
                  unsigned core = (t*processesOnNode) + p;
                  if (core >= CoresPerNode) {
                      cout << core << ">=" << CoresPerNode
                           << ": Skipping coordinate mapping for over-subscribed compute node!" << endl;
                      continue;
                  }
                  Thread* thrd = proc->get_child(t);
                  coordv[0] = cabsX*CoresPerNode + core; 
                  coordv[1] = cabsY; coordv[2] = cabsZ;
                  xtCabsCart->def_coords(thrd, coordv);
#if 0 // extra topologies
                  coordv[1] = meshY*CoresPerNode + core;
                  coordv[0] = meshX; coordv[2] = meshZ;
                  xtMeshCart->def_coords(thrd, coordv);
                  coordv[0] = maltX*CoresPerNode + core;
                  coordv[1] = maltY; coordv[2] = maltZ;
                  xtMaltCart->def_coords(thrd, coordv);
#endif
              }
          }
#if DEBUG
          cout << node_name << " : " << xtnidnames[nid]
               << " : " << CABX << "/" << CABY << "/" << CAGE << "/" << SLOT << "/" << NODE
               << " (" << cabsX << "," << cabsY << "," << cabsZ << ")"
#if 0 // extra topologies
               << " (" << meshX << "," << meshY << "," << meshZ << ")"
               << " (" << maltX << "," << maltY << "," << maltZ << ")"
#endif
               << endl;
#endif
      }
      ++nit;
    }
  }


} /* End of unnamed namespace */
/**
* Get a status of the .cube file.
*/
bool cube::cube_stat (const std::string& filename)
{
    if ((filename.length()<5) || (filename.substr(filename.length()-5,5) != ".cube")) {
#ifdef CUBE_COMPRESSED
        if ((filename.length()<8) || (filename.substr(filename.length()-8,8) != ".cube.gz"))
#endif
        cerr << "WARNING: "<< filename << ": Missing .cube suffix" << endl;
    }

    struct stat statbuf;
    if (stat(filename.c_str(), &statbuf)) {
        cerr << "ERROR: "<< filename << ": " << strerror(errno) << endl;
        return false;
    }
    if (!S_ISREG(statbuf.st_mode)) {
        cerr << "ERROR: "<< filename << ": Not a regular file" << endl;
        return false;
    }
    if (!(S_IRUSR&statbuf.st_mode)) {
        cerr << "ERROR: "<< filename << ": Not a readable file" << endl;
        return false;
    }
    return true;
}

/**
* Create a difference of two cube and returns the result as a new cube. 
*
* It is done  step-by-step by creating of an empty cube, merging dimensions and addtion in to the new cube old values for severities multiplied with a +/- 1.
*/
Cube* cube::cube_diff (Cube* minCube, Cube* subCube, bool subset, bool collapse) {
  Cube* myCube;
  myCube = new Cube();
  CubeMapping lhsmap, rhsmap;

  cout<<"INFO::Merging metric dimension...";  
  metric_merge (*myCube, *minCube);
  metric_merge (*myCube, *subCube);
  cout<<" done."<<endl;

  cout<<"INFO::Merging program dimension...";
  cnode_merge (*myCube, *minCube);
  cnode_merge (*myCube, *subCube);
  cout<<" done."<<endl  ;

  cout<<"INFO::Merging system dimension...";
  sysres_merge (*myCube, *minCube, *subCube, subset, collapse);
  cout<<" done."<<endl;

  cout<<"INFO::Mapping severities...";
  createMapping (*myCube,*minCube, lhsmap, collapse);
  createMapping (*myCube,*subCube, rhsmap, collapse);
  cout<<" done."<<endl;

  cout<<"INFO::Merging topologies...";
  merge_top (*myCube, *minCube, *subCube);
  cout<<" done."<<endl;

  cout<<"INFO::Diff operation...";
  add_sev (*myCube, *subCube, rhsmap, -1);
  add_sev (*myCube, *minCube, lhsmap, 1);
  cout<<" done."<<endl;
  
  return myCube;
}

/**
* Create a n average of two cubes and returns the result as a new cube. 
* 
* It is done step-by-step by creating of an empty cube, merging dimensions and 
addtion in to the new cube old values for severities multiplied with a factor. 
Severities from "lhsCube" will be just copied, from "rhsCube" divided by "num" and added. 
*
* This nonsymmetrical way (first 1, then 1/num) coused by the usage of this 
function in "cube_mean.cpp". First call of this function has as lhsCube an empty cube. 
On such way the sum will be real "mean" or "average".
*/
Cube* cube::cube_mean (Cube* lhsCube, Cube* rhsCube, bool subset, double num, bool collapse) {
  Cube* myCube;
  myCube = new Cube();
  CubeMapping lhsmap, rhsmap;

  cout<<"INFO::Merging metric dimension...";  
  metric_merge (*myCube, *lhsCube);
  metric_merge (*myCube, *rhsCube);
  cout<<" done."<<endl;
  
  cout<<"INFO::Merging program dimension...";
  cnode_merge (*myCube, *lhsCube);
  cnode_merge (*myCube, *rhsCube);
  cout<<" done."<<endl;
  
  cout<<"INFO::Merging system dimension...";
  sysres_merge (*myCube, *lhsCube, *rhsCube, subset, collapse);
  cout<<" done."<<endl;

  cout<<"INFO::Mapping severities...";
  createMapping (*myCube,*lhsCube, lhsmap, collapse);
  createMapping (*myCube,*rhsCube, rhsmap, collapse);
  cout<<" done."<<endl;

  cout<<"INFO::Adding topologies...";
  add_top (*myCube, *lhsCube, lhsmap);
  add_top (*myCube, *rhsCube, rhsmap);
  cout<<" done."<<endl;


  cout<<"INFO::Mean operation...";
  add_sev (*myCube, *lhsCube, lhsmap, 1);
  add_sev (*myCube, *rhsCube, rhsmap, (1/num));
  cout<<" done."<<endl;

  return myCube;
}
/**
* Merging of two cubes and returns the result as a new cube. 
* 
* It is done step-by-step by creating of an empty cube, merging dimensions and 
addtion in to the new cube old values for severities. If machines are equal 
the topology is copied too.
*/
Cube* cube::cube_merge (Cube* lhsCube, Cube* rhsCube, bool subset, bool collapse) {
  Cube* myCube;
  myCube = new Cube();
  CubeMapping lhsmap, rhsmap;

  cout<<"INFO::Merging metric dimension...";  
  metric_merge (*myCube, *lhsCube);
  metric_merge (*myCube, *rhsCube);
  cout<<" done."<<endl;

  cout<<"INFO::Merging program dimension...";
  cnode_merge (*myCube, *lhsCube);
  cnode_merge (*myCube, *rhsCube);
  cout<<" done."<<endl;
  
  cout<<"INFO::Merging system dimension...";
  sysres_merge (*myCube, *lhsCube, *rhsCube, subset, collapse);
  cout<<" done."<<endl;

  cout<<"INFO::Mapping severities...";
  createMapping (*myCube,*lhsCube, lhsmap, collapse);
  createMapping (*myCube,*rhsCube, rhsmap, collapse);
  cout<<" done."<<endl;
  
  cout<<"INFO::Merge operation...";
  set_sev (*myCube, *lhsCube, lhsmap);
  set_sev (*myCube, *rhsCube, rhsmap);
  cout<<" done."<<endl;

  cout<<"INFO::Merging topologies...";
  merge_top (*myCube, *lhsCube, *rhsCube);
/*
  if (is_equal) {
    add_top (*myCube, *lhsCube, lhsmap);
    add_top (*myCube, *rhsCube, rhsmap);
 }*/
  cout<<" done."<<endl;
  return myCube;
}

/**
* Compares "lhsCube" and "rhsCube".
* 
* First the metric dimension. If not equal -> return "not equal", "false".
* Then cnodes dimension, then the machine and at least the vales for severities.
*/
bool cube::cube_cmp (Cube* lhsCube, Cube* rhsCube) {
  Cube* myCube;
  myCube = new Cube();
  CubeMapping lhsmap, rhsmap;

  metric_merge (*myCube, *lhsCube);
  if (!metric_merge (*myCube, *rhsCube)) {
    delete myCube;  
    return false;
  }

  cnode_merge (*myCube, *lhsCube);
  if (!cnode_merge (*myCube, *rhsCube)) {
    delete myCube;  
    return false;
  }
  
  if (!sysres_merge (*myCube, *lhsCube, *rhsCube, true, false)) {
    delete myCube;  
    return false;
  }

  createMapping (*myCube,*lhsCube, lhsmap, false);
  createMapping (*myCube,*rhsCube, rhsmap, false);
  set_sev (*myCube, *lhsCube, lhsmap);
  if (!check_sev (*myCube, *rhsCube, rhsmap)) {
    delete myCube;  
    return false;
  }
  delete myCube;
  return true;
}

/**
* Creates a "cleaned" CUBE object containing only referenced items.
*/
Cube* cube::cube_clean (Cube* lhsCube, bool subset, bool collapse) {
  Cube* myCube = new Cube();
  Cube* tmp    = new Cube(); 

  CubeMapping lhsmap;

  metric_merge (*myCube, *lhsCube);

  cnode_merge (*myCube, *lhsCube);
  
  sysres_merge (*myCube, *tmp, *lhsCube, subset, collapse);
  delete tmp;

  createMapping (*myCube,*lhsCube, lhsmap, collapse);

  set_sev (*myCube, *lhsCube, lhsmap);

  add_top (*myCube, *lhsCube, lhsmap);

  return myCube;
}


/**
* Produces extract of call-tree by re-rooting or pruning sub-tree(s).
*/
Cube* cube::cube_cut (Cube* lhsCube, Cube* rhsCube, bool subset, bool collapse,
                      const string& cn_root, const vector<string> cn_prunes) {
  Cube* myCube;
  myCube = new Cube();
  CubeMapping lhsmap, rhsmap, tmp;

  // Define mirror & attributes
  myCube->def_mirror("http://apps.fz-juelich.de/scalasca/releases/scalasca/"
                     VERSION "/help/");
  myCube->def_attr("CUBE_CT_AGGR", "SUM");

  metric_merge (*myCube, *lhsCube);
  metric_merge (*myCube, *rhsCube);

  cnode_merge (*myCube, *lhsCube);
  if (cnode_reroot (*myCube, *rhsCube, cn_root, cn_prunes, rhsmap) == false) {
    delete myCube;
    return NULL;
  }
  
  sysres_merge (*myCube, *lhsCube, *rhsCube, subset, collapse);

  createMapping (*myCube,*lhsCube, lhsmap, collapse);
  createMapping (*myCube,*rhsCube, rhsmap, collapse);

  set_sev (*myCube, *lhsCube, lhsmap);
  add_sev (*myCube, *rhsCube, rhsmap, 1); // aggregate metrics from pruned nodes

  add_top (*myCube, *rhsCube, rhsmap);


  if ( myCube->get_root_cnodev().empty()) {
    delete myCube;
    return NULL;
  }
  else
    return myCube;
}

/**
* Creates a new cube with manually specified list of metrics. Calculates 
exclusive and inclusive values for metrics and fills severities.
*/
Cube* cube::cube_remap(Cube* inCube, const std::string& dirname, bool skip_omp)
{
  // Create new CUBE object
  AggrCube* outCube = new AggrCube();

  // Define mirror & attributes
  outCube->def_mirror("http://apps.fz-juelich.de/scalasca/releases/scalasca/"
                      VERSION "/help/");
  outCube->def_attr("CUBE_CT_AGGR", "SUM");

  // Define metric hierarchy (time-based metrics)
  Metric* time   = outCube->def_met("Time", "time",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "time",
                                    "Total CPU allocation time (includes time allocated for idle threads)",
                                    NULL);
  Metric* visits = outCube->def_met("Visits", "visits",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "visits",
                                    "Number of visits",
                                    NULL);
  Metric* exec   = outCube->def_met("Execution", "execution",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "execution",
                                    "Execution time (does not include time allocated for idle threads)",
                                    time);
  Metric* over   = outCube->def_met("Overhead", "overhead",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "overhead",
                                    "Time spent performing tasks related to trace generation",
                                    time);
  Metric* mpi    = outCube->def_met("MPI", "mpi",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi",
                                    "Time spent in MPI calls",
                                    exec);
  Metric* sync   = outCube->def_met("Synchronization", "mpi_synchronization",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_synchronization",
                                    "Time spent in MPI synchronization calls",
                                    mpi);
  Metric* comm   = outCube->def_met("Communication", "mpi_communication",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_communication",
                                    "Time spent in MPI communication calls",
                                    mpi);
  Metric* io     = outCube->def_met("File I/O", "mpi_io",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_io",
                                    "Time spent in MPI file I/O calls",
                                    mpi);
  Metric* collio = outCube->def_met("Collective", "mpi_io_collective",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_io_collective",
                                    "Time spent in collective MPI file I/O calls",
                                    io);
  Metric* inex   = outCube->def_met("Init/Exit", "mpi_init_exit",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_init_exit",
                                    "Time spent in MPI initialization calls",
                                    mpi);
  Metric* scoll  = outCube->def_met("Collective", "mpi_sync_collective",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_sync_collective",
                                    "Time spent in MPI barriers",
                                    sync);
  Metric* srma   = outCube->def_met("Remote Memory Access", "mpi_rma_synchronization",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_rma_synchronization",
                                    "Time spent in MPI RMA synchronization calls",
                                    sync);
  Metric* srmaact = outCube->def_met("Active Target", "mpi_rma_sync_active",
                                     "FLOAT", "sec", "",
                                     PATTERNS_URL "mpi_rma_sync_active",
                                     "Time spent in MPI RMA active target synchronization calls",
                                     srma);
  /* Metric* srmaact_lp = */ outCube->def_met("Late Post", "mpi_rma_late_post",
                                        "FLOAT", "sec", "VOID",
                                        PATTERNS_URL "mpi_rma_late_post",
                                        "Waiting time in MPI RMA general active target synchronization due to a late post call",
                                        srmaact);
  Metric* srmaact_wf = outCube->def_met("Wait at Fence", "mpi_rma_wait_at_fence",
                                        "FLOAT", "sec", "VOID",
                                        PATTERNS_URL "mpi_rma_wait_at_fence",
                                        "Waiting time in MPI RMA due to inherent synchronization in MPI_Win_fence",
                                        srmaact);
  /* Metric* srmaact_ef = */ outCube->def_met("Early Fence", "mpi_rma_early_fence",
                                        "FLOAT", "sec", "VOID",
                                        PATTERNS_URL "mpi_rma_early_fence",
                                        "Waiting time in MPI RMA synchronization due to pending RMA ops",
                                        srmaact_wf);
  Metric* srmaact_ew = outCube->def_met("Early Wait", "mpi_rma_early_wait",
                                        "FLOAT", "sec", "VOID",
                                        PATTERNS_URL "mpi_rma_early_wait",
                                        "Waiting time in MPI RMA general active target synchronization due to an early wait call",
                                        srmaact);
  /* Metric* srmaact_lc = */ outCube->def_met("Late Complete", "mpi_rma_late_complete",
                                        "FLOAT", "sec", "VOID",
                                        PATTERNS_URL "mpi_rma_late_complete",
                                        "Waiting time in MPI RMA general active target synchronization due to unnecessary time between last RMA op and synchronization call",
                                        srmaact_ew);
  Metric* srmapas = outCube->def_met("Passive Target", "mpi_rma_sync_passive",
                                     "FLOAT", "sec", "",
                                     PATTERNS_URL "mpi_rma_sync_passive",
                                     "Time spent in MPI RMA passive target synchronization calls",
                                     srma);
  Metric* p2p    = outCube->def_met("Point-to-point", "mpi_point2point",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_point2point",
                                    "MPI point-to-point communication",
                                    comm);
  Metric* coll   = outCube->def_met("Collective", "mpi_collective",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_collective",
                                    "MPI collective communication",
                                    comm);
  Metric* rma    = outCube->def_met("Remote Memory Access", "mpi_rma_communication",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "mpi_rma_communication",
                                    "MPI remote memory access communication",
                                    comm);
  /* Metric* rma_et = */ outCube->def_met("Early Transfer", "mpi_rma_early_transfer",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_rma_early_transfer",
                                    "Waiting time due to an early transfer to target location in MPI RMA operations",
                                    rma);
  /* Metric* er     = */ outCube->def_met("Early Reduce", "mpi_earlyreduce",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_earlyreduce",
                                    "Waiting time due to an early receiver in MPI n-to-1 operations",
                                    coll);
  /* Metric* es     = */ outCube->def_met("Early Scan", "mpi_earlyscan",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_earlyscan",
                                    "Waiting time due to an early receiver in an MPI scan operation",
                                    coll);
  /* Metric* lb     = */ outCube->def_met("Late Broadcast", "mpi_latebroadcast",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_latebroadcast",
                                    "Waiting time due to a late sender in MPI 1-to-n operations",
                                    coll);
  /* Metric* wait   = */ outCube->def_met("Wait at N x N", "mpi_wait_nxn",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_wait_nxn",
                                    "Waiting time due to inherent synchronization in MPI n-to-n operations",
                                    coll);
  /* Metric* comp   = */ outCube->def_met("N x N Completion", "mpi_nxn_completion",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_nxn_completion",
                                    "Time needed to finish a N-to-N collective operation",
                                    coll);
  Metric* ls     = outCube->def_met("Late Sender", "mpi_latesender",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_latesender",
                                    "Time a receiving process is waiting for a message",
                                    p2p);
  /* Metric* lr     = */ outCube->def_met("Late Receiver", "mpi_latereceiver",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_latereceiver",
                                    "Time a receiving process is waiting for a message",
                                    p2p);
  Metric* lswo   = outCube->def_met("Messages in Wrong Order", "mpi_latesender_wo",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_latesender_wo",
                                    "Late Sender situation due to messages received in the wrong order",
                                    ls);
  /* Metric* lswod  = */ outCube->def_met("From different sources", "mpi_lswo_different",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_lswo_different",
                                    "Wrong order situation due to messages received from different sources",
                                    lswo);
  /* Metric* lswos  = */ outCube->def_met("From same source", "mpi_lswo_same",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_lswo_same",
                                    "Wrong order situation due to messages received from the same source",
                                    lswo);
  /* Metric* bwait  = */ outCube->def_met("Wait at Barrier", "mpi_barrier_wait",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_barrier_wait",
                                    "Waiting time in front of MPI barriers",
                                    scoll);
  /* Metric* bcompl = */ outCube->def_met("Barrier Completion", "mpi_barrier_completion",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "mpi_barrier_completion",
                                    "Time needed to finish an MPI barrier",
                                    scoll);

  // Define metric hierarchy (message statistics)
  Metric* syncs  = outCube->def_met("Synchronizations", "syncs",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs",
                                    "Number of synchronization operations",
                                    NULL);
  Metric* psyncs = outCube->def_met("Point-to-point", "syncs_p2p",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_p2p",
                                    "Number of point-to-point synchronization operations",
                                    syncs);
  Metric* csyncs = outCube->def_met("Collective", "syncs_coll",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_coll",
                                    "Number of collective synchronization operations",
                                    syncs);
  Metric* rsyncs = outCube->def_met("Remote Memory Access", "syncs_rma",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_rma", 
                                    "Number of Remote Memory Access synchronizations",
                                    syncs);
  Metric* rfence = outCube->def_met("Fences", "syncs_fence",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_fence",
                                    "Number of fence synchronizations",
                                    rsyncs);
  Metric* rgats  = outCube->def_met("GATS Epochs", "syncs_gats",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_gats",
                                    "Number of GATS epochs",
                                    rsyncs);
  Metric* raccess = outCube->def_met("Access Epochs", "syncs_gats_access",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_gats_access",
                                    "Number of access epochs",
                                    rgats);
  Metric* rexposure = outCube->def_met("Exposure Epochs", "syncs_gats_exposure",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_gats_exposure",
                                    "Number of exposure epochs",
                                    rgats);
  Metric* rlocks = outCube->def_met("Locks", "syncs_locks",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_locks",
                                    "Number of locks",
                                    rsyncs);
  Metric* pwsyncs = outCube->def_met("Pairwise synchronizations for RMA", "mpi_rma_pairsync_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "pwsyncs",
                                    "Number of pairwise synchronizations using MPI RMA",
                                    NULL);
  /* Metric* pwsyncs_unneeded = */ outCube->def_met("Unneeded synchronizations", "mpi_rma_pairsync_unneeded_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "pwsyncs_unneeded",
                                    "Number of unneeded pairwise synchronizations using MPI RMA",
                                    pwsyncs);
  Metric* ssend  = outCube->def_met("Sends", "syncs_send",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_send",
                                    "Number of point-to-point send synchronization operations",
                                    psyncs);
  Metric* srecv  = outCube->def_met("Receives", "syncs_recv",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "syncs_recv",
                                    "Number of point-to-point receive synchronization operations",
                                    psyncs);

  Metric* comms  = outCube->def_met("Communications", "comms",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms",
                                    "Number of communication operations",
                                    NULL);
  Metric* pcomms = outCube->def_met("Point-to-point", "comms_p2p",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_p2p",
                                    "Number of point-to-point communication operations",
                                    comms);
  Metric* ccomms = outCube->def_met("Collective", "comms_coll",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_coll",
                                    "Number of collective communication operations",
                                    comms);
  Metric* rcomms = outCube->def_met("Remote Memory Access", "comms_rma",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_rma",
                                    "Number of remote memory access operations",
                                    comms);
  Metric* rputs  = outCube->def_met("Puts", "comms_rma_puts",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_rma_puts",
                                    "Number of RMA put and accumulate operations",
                                    rcomms);
  Metric* rgets  = outCube->def_met("Gets", "comms_rma_gets",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_rma_gets",
                                    "Number of RMA get operations",
                                    rcomms);
  Metric* cexch  = outCube->def_met("Exchange", "comms_cxch",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_cxch",
                                    "Number of collective communications as source and destination",
                                    ccomms);
  Metric* csrc   = outCube->def_met("As source", "comms_csrc",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_csrc",
                                    "Number of collective communications as source",
                                    ccomms);
  Metric* cdst   = outCube->def_met("As destination", "comms_cdst",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_cdst",
                                    "Number of collective communications as destination",
                                    ccomms);
  Metric* csend  = outCube->def_met("Sends", "comms_send",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_send",
                                    "Number of point-to-point send communication operations",
                                    pcomms);
  Metric* crecv  = outCube->def_met("Receives", "comms_recv",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "comms_recv",
                                    "Number of point-to-point receive communication operations",
                                    pcomms);

  Metric* bytes  = outCube->def_met("Bytes transferred", "bytes",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes",
                                    "Number of bytes transferred",
                                    NULL);
  Metric* bp2p   = outCube->def_met("Point-to-point", "bytes_p2p",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_p2p",
                                    "Number of bytes transferred in point-to-point communication operations",
                                    bytes);
  Metric* bcoll  = outCube->def_met("Collective", "bytes_coll",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_coll",
                                    "Number of bytes transferred in collective communication operations",
                                    bytes);
  Metric* brma   = outCube->def_met("Remote Memory Access", "bytes_rma",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_rma",
                                    "Number of bytes transferred in remote memory access operations",
                                    bytes);
  Metric* bcout  = outCube->def_met("Outgoing", "bytes_cout",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_cout",
                                    "Number of bytes outgoing in collective communication operations",
                                    bcoll);
  Metric* bcin   = outCube->def_met("Incoming", "bytes_cin",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_cin",
                                    "Number of bytes incoming in collective communication operations",
                                    bcoll);
  Metric* bsent  = outCube->def_met("Sent", "bytes_sent",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_sent",
                                    "Number of bytes sent in point-to-point communication operations",
                                    bp2p);
  Metric* brcvd  = outCube->def_met("Received", "bytes_rcvd",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_rcvd",
                                    "Number of bytes received in point-to-point communication operations",
                                    bp2p);
  Metric* bput   = outCube->def_met("Sent", "bytes_put",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_put",
                                    "Number of bytes sent in remote memory access operations",
                                    brma);
  Metric* bgot   = outCube->def_met("Received", "bytes_get",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "bytes_get",
                                    "Number of bytes received in remote memory access operations",
                                    brma);

  // MPI File operations
  Metric* file_ops = outCube->def_met("MPI file operations", "mpi_file_ops",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "mpi_file_ops",
                                    "Number of MPI file operations",
                                    NULL);
  Metric* file_iops = outCube->def_met("Individual", "mpi_file_iops",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "mpi_file_iops",
                                    "Number of individual MPI file operations",
                                    file_ops);
  Metric* file_irops = outCube->def_met("Reads", "mpi_file_irops",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "mpi_file_irops",
                                    "Number of individual MPI file read operations",
                                    file_iops);
  Metric* file_iwops = outCube->def_met("Writes", "mpi_file_iwops",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "mpi_file_iwops",
                                    "Number of individual MPI file write operations",
                                    file_iops);
  Metric* file_cops = outCube->def_met("Collective", "mpi_file_cops",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "mpi_file_cops",
                                    "Number of collective MPI file operations",
                                    file_ops);
  Metric* file_crops = outCube->def_met("Reads", "mpi_file_crops",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "mpi_file_crops",
                                    "Number of collective MPI file read operations",
                                    file_cops);
  Metric* file_cwops = outCube->def_met("Writes", "mpi_file_cwops",
                                    "INTEGER", "occ", "",
                                    PATTERNS_URL "mpi_file_cwops",
                                    "Number of collective MPI file write operations",
                                    file_cops);

  // MPI File bytes
  Metric* file_bytes = outCube->def_met("MPI file bytes transferred", "mpi_file_bytes",
                                    "INTEGER", "bytes", "VOID",
                                    PATTERNS_URL "mpi_file_bytes",
                                    "Number of bytes transferred in MPI file operations",
                                    NULL);
  Metric* file_ibytes = outCube->def_met("Individual", "mpi_file_ibytes",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "mpi_file_ibytes",
                                    "Number of bytes transferred in individual MPI file operations",
                                    file_bytes);
  Metric* file_irbytes = outCube->def_met("Read", "mpi_file_irbytes",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "mpi_file_irbytes",
                                    "Number of bytes read in individual MPI file operations",
                                    file_ibytes);
  Metric* file_iwbytes = outCube->def_met("Written", "mpi_file_iwbytes",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "mpi_file_iwbytes",
                                    "Number of bytes written in individual MPI file operations",
                                    file_ibytes);
  Metric* file_cbytes = outCube->def_met("Collective", "mpi_file_cbytes",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "mpi_file_cbytes",
                                    "Number of bytes transferred in collective MPI file operations",
                                    file_bytes);
  Metric* file_crbytes = outCube->def_met("Read", "mpi_file_crbytes",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "mpi_file_crbytes",
                                    "Number of bytes read in collective MPI file operations",
                                    file_cbytes);
  Metric* file_cwbytes = outCube->def_met("Written", "mpi_file_cwbytes",
                                    "INTEGER", "bytes", "",
                                    PATTERNS_URL "mpi_file_cwbytes",
                                    "Number of bytes written in collective MPI file operations",
                                    file_cbytes);

  // Trace-based pattern statistics
  Metric* sls    = outCube->def_met("Late Senders", "mpi_sls_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "mpi_sls_count",
                                    "Number of Late Sender instances in synchronizations",
                                    srecv);
  /* Metric* slswo  = */ outCube->def_met("Messages in Wrong Order", "mpi_slswo_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "mpi_slswo_count",
                                    "Number of Late Sender instances in synchronizations were messages are received in wrong order",
                                    sls);
  /* Metric* slr    = */ outCube->def_met("Late Receivers", "mpi_slr_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "mpi_slr_count",
                                    "Number of Late Receiver instances in synchronizations",
                                    ssend);

  Metric* cls    = outCube->def_met("Late Senders", "mpi_cls_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "mpi_cls_count",
                                    "Number of Late Sender instances in communications",
                                    crecv);
  /* Metric* clswo  = */ outCube->def_met("Messages in Wrong Order", "mpi_clswo_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "mpi_clswo_count",
                                    "Number of Late Sender instances in communications were messages are received in wrong order",
                                    cls);
  /* Metric* clr    = */ outCube->def_met("Late Receivers", "mpi_clr_count",
                                    "INTEGER", "occ", "VOID",
                                    PATTERNS_URL "mpi_clr_count",
                                    "Number of Late Receiver instances in communications",
                                    csend);

  // Load imbalance heuristic
  Metric* imbal  = outCube->def_met("Computational imbalance", "imbalance",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "imbalance",
                                    "Computational load imbalance heuristic (see Online Description for details)",
                                    NULL);
  Metric* above  = outCube->def_met("Overload", "imbalance_above",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "imbalance_above",
                                    "Computational load imbalance heuristic (overload)",
                                    imbal);
  Metric* single = outCube->def_met("Single participant", "imbalance_above_single",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "imbalance_above_single",
                                    "Computational load imbalance heuristic (single participant)",
                                    above);
  Metric* below  = outCube->def_met("Underload", "imbalance_below",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "imbalance_below",
                                    "Computational load imbalance heuristic (underload)",
                                    imbal);
  Metric* bypass = outCube->def_met("Non-participation", "imbalance_below_bypass",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "imbalance_below_bypass",
                                    "Computational load imbalance heuristic (non-participation)",
                                    below);
  Metric* bysing = outCube->def_met("Singularity", "imbalance_below_singularity",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "imbalance_below_singularity",
                                    "Computational load imbalance heuristic (non-participation in singularity)",
                                    bypass);

  // Idle threads heuristics
  Metric* idleth = outCube->def_met("Idle threads", "omp_idle_threads",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "omp_idle_threads",
                                    "Unused CPU reservation time",
                                    time);
  Metric* limpar = outCube->def_met("Limited parallelism", "omp_limited_parallelism",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_limited_parallelism",
                                    "Unused CPU reservation time in parallel regions due to limited parallelism",
                                    idleth);

  // OpenMP time
  Metric* omp_time  = outCube->def_met("OMP", "omp_time",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "omp_time",
                                    "Time spent in the OpenMP run-time system and API",
                                    exec);
  Metric* omp_flush = outCube->def_met("Flush", "omp_flush",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_flush",
                                    "Time spent in the OpenMP flush directives",
                                    omp_time);
  Metric* omp_mgmt = outCube->def_met("Management", "omp_management",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_management",
                                    "Time needed to start up and shut down team of threads",
                                    omp_time);
  /* Metric* omp_fork = */ outCube->def_met("Fork", "omp_fork",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_fork",
                                    "Time needed to create team of threads",
                                    omp_mgmt);
  Metric* omp_sync = outCube->def_met("Synchronization", "omp_synchronization",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_synchronization",
                                    "Time spent on OpenMP synchronization",
                                    omp_time);
  Metric* omp_barr = outCube->def_met("Barrier", "omp_barrier",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_barrier",
                                    "OpenMP barrier synchronization",
                                    omp_sync);
  Metric* omp_ebarr = outCube->def_met("Explicit", "omp_ebarrier",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_ebarrier",
                                    "Time spent in explicit OpenMP barriers",
                                    omp_barr);
  /* Metric* omp_eb_wait = */ outCube->def_met("Wait at Barrier", "omp_ebarrier_wait",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "omp_ebarrier_wait",
                                    "Waiting time in front of explicit OpenMP barriers",
                                    omp_ebarr);
  Metric* omp_ibarr = outCube->def_met("Implicit", "omp_ibarrier",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_ibarrier",
                                    "Time spent in implicit OpenMP barriers",
                                    omp_barr);
  /* Metric* omp_ib_wait = */ outCube->def_met("Wait at Barrier", "omp_ibarrier_wait",
                                    "FLOAT", "sec", "VOID",
                                    PATTERNS_URL "omp_ibarrier_wait",
                                    "Time spent in explicit OpenMP barriers",
                                    omp_ibarr);
#if 0
  Metric* omp_lock = outCube->def_met("Lock Competition", "omp_lock_competition",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_lock",
                                    "Time spent waiting for a lock possessed by another thread",
                                    omp_sync);
  Metric* omp_lock_api = outCube->def_met("API", "omp_lock_api",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_lock_api",
                                    "Time spent in OpenMP API calls waiting for a lock",
                                    omp_lock);
  Metric* omp_crit = outCube->def_met("Critical", "omp_lock_critical",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_lock_critical",
                                    "Time spent in front of a critical section waiting for a lock",
                                    omp_lock);
#else
  Metric* omp_crit = outCube->def_met("Critical", "omp_critical",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_critical",
                                    "Time spent in front of a critical section",
                                    omp_sync);
  Metric* omp_lock_api = outCube->def_met("Lock API", "omp_lock_api",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_lock_api",
                                    "Time spent in OpenMP API calls dealing with locks",
                                    omp_sync);
  Metric* omp_ord = outCube->def_met("Ordered", "omp_ordered",
                                    "FLOAT", "sec", "",
                                    PATTERNS_URL "omp_ordered",
                                    "Time spent in front of an ordered region",
                                    omp_sync);
#endif

  // Re-map or merge hierarchies
  metric_remap(*outCube, *inCube);
  cnode_merge (*outCube, *inCube);
  set_region_urls(*outCube);
  sysres_merge(*outCube, *inCube, Cube(), true, false);

  node_remap(*outCube, dirname); // update nodes and topologies

  // Copy severities & topology
  CubeMapping inMap;
  createMapping(*outCube, *inCube, inMap, false);
  set_sev(*outCube, *inCube, inMap);
  add_top(*outCube, *inCube, inMap);

  bool includes_mpi = false, includes_rma = false, includes_io = false, includes_omp = false;
  skip_omp = (inCube->get_met("omp_management") == NULL);

  // Divide "Time" metric
  const vector<Cnode*>& cnodev = outCube->get_cnodev();

  vector<Cnode*>::const_iterator it = cnodev.begin();
  while (it != cnodev.end()) {
    const Region* region = (*it)->get_callee();

    if (is_epk(region)) {                  // EPIK overhead
      copy_sev(*outCube, time, over, *it);
    } else {                               // Execution
      copy_sev(*outCube, time, exec, *it);

      if (is_omp(region)) {                // OMP
        includes_omp = true;
        string name = region->get_name();
        if (is_omp_flush(name)) { // Explicit flush
          copy_sev(*outCube, time, omp_time, *it);
          copy_sev(*outCube, time, omp_flush, *it);
        } else if (is_omp_ebarrier(name)) { // Explicit barrier synch
          copy_sev(*outCube, time, omp_time, *it);
          copy_sev(*outCube, time, omp_sync, *it);
          copy_sev(*outCube, time, omp_barr, *it);
          copy_sev(*outCube, time, omp_ebarr, *it);
        } else if (is_omp_ibarrier(name)) { // Implicit barrier synch
          copy_sev(*outCube, time, omp_time, *it);
          copy_sev(*outCube, time, omp_sync, *it);
          copy_sev(*outCube, time, omp_barr, *it);
          copy_sev(*outCube, time, omp_ibarr, *it);
        } else if (is_omp_critical(name)) { // Critical synch
          copy_sev(*outCube, time, omp_time, *it);
          copy_sev(*outCube, time, omp_sync, *it);
          copy_sev(*outCube, time, omp_crit, *it);
        } else if (is_omp_lock_api(name)) { // Explicit lock synch
          copy_sev(*outCube, time, omp_time, *it);
          copy_sev(*outCube, time, omp_sync, *it);
          copy_sev(*outCube, time, omp_lock_api, *it);
        } else if (is_omp_ordered(name)) { // Ordered synch
          copy_sev(*outCube, time, omp_time, *it);
          copy_sev(*outCube, time, omp_sync, *it);
          copy_sev(*outCube, time, omp_ord, *it);
        } else if (is_omp_parallel(name)) { // Management
          add_sev(*outCube, omp_mgmt, omp_time, *it);
        }
      }

      if (is_mpi(region)) {                // MPI
        includes_mpi = true;
        string name = lowercase(region->get_name());
        copy_sev(*outCube, time, mpi, *it);

        bool is_p2p  = is_mpi_p2p(name);
        bool is_coll = is_mpi_coll(name);
        bool is_rma  = is_mpi_rma(name);
        bool is_rma_sync  = is_mpi_sync_rma(name);
        bool is_coll_sync = is_mpi_sync_coll(name);

        if (is_p2p || is_coll || is_rma) {           // Communication
          copy_sev(*outCube, time, comm, *it);

          if (is_p2p)                      // Point-to-point communication
            copy_sev(*outCube, time, p2p, *it);
          else if (is_coll)                            // Collective communication
            copy_sev(*outCube, time, coll, *it);
          else if (is_rma)
          {
            copy_sev(*outCube, time, rma, *it);
            if (is_mpi_comm_rma_put(name))
              copy_sev(*outCube, visits, rputs, *it);
            else if (is_mpi_comm_rma_get(name))
              copy_sev(*outCube, visits, rgets, *it);
          }
        } else if (is_coll_sync || is_rma_sync) {    // Collective synchronization
          copy_sev(*outCube, time, sync, *it);

          if (is_coll_sync)
            copy_sev(*outCube, time, scoll, *it);
          else if (is_rma_sync) {          // RMA synchronization
            includes_rma = true;
            // copy time
            copy_sev(*outCube, time, srma,  *it);
            
            // count occurences
            if (is_mpi_sync_rma_fence(name))
              copy_sev(*outCube, visits, rfence, *it);
            else if (is_mpi_sync_rma_gats_access(name))
              copy_sev(*outCube, visits, raccess, *it);
            else if (is_mpi_sync_rma_gats_exposure(name))
              copy_sev(*outCube, visits, rexposure, *it);

            if (is_mpi_sync_rma_active(name))
              copy_sev(*outCube, time, srmaact, *it);
            else if (is_mpi_sync_rma_passive(name))
              copy_sev(*outCube, time, srmapas, *it);
          }
        } else if (is_mpi_io(name)) {      // File I/O
          includes_io = true;
          copy_sev(*outCube, time, io, *it);
          copy_sev(*outCube, visits, file_ops, *it);
          if (is_mpi_coll_io(name)) {      // Collective File I/O
            copy_sev(*outCube, time, collio, *it);
            copy_sev(*outCube, visits, file_cops, *it);
            copy_sev(*outCube, file_bytes, file_cbytes, *it);
            if (is_mpi_io_read(name)) {
              copy_sev(*outCube, visits, file_crops, *it);
              copy_sev(*outCube, file_bytes, file_crbytes, *it);
            }
            if (is_mpi_io_write(name)) {
              copy_sev(*outCube, visits, file_cwops, *it);
              copy_sev(*outCube, file_bytes, file_cwbytes, *it);
            }
          } else {                         // Individual File I/O
            copy_sev(*outCube, visits, file_iops, *it);
            copy_sev(*outCube, file_bytes, file_ibytes, *it);
            if (is_mpi_io_read(name)) {
              copy_sev(*outCube, visits, file_irops, *it);
              copy_sev(*outCube, file_bytes, file_irbytes, *it);
            }
            if (is_mpi_io_write(name)) {
              copy_sev(*outCube, visits, file_iwops, *it);
              copy_sev(*outCube, file_bytes, file_iwbytes, *it);
            }
          }
        } else if (is_mpi_init(name)) {    // Init/Exit
          copy_sev(*outCube, time, inex, *it);
        }
      }
    }

    // Accumulate message statistics
    add_sev(*outCube, cexch,  ccomms, *it);
    add_sev(*outCube, csrc,   ccomms, *it);
    add_sev(*outCube, cdst,   ccomms, *it);
    add_sev(*outCube, csend,  pcomms, *it);
    add_sev(*outCube, crecv,  pcomms, *it);
    add_sev(*outCube, ccomms, comms,  *it);
    add_sev(*outCube, pcomms, comms,  *it);
    add_sev(*outCube, rputs,  rcomms, *it);
    add_sev(*outCube, rgets,  rcomms, *it);
    add_sev(*outCube, rcomms, comms,  *it);
    // MPI P2P synchronizations
    add_sev(*outCube, ssend,  psyncs, *it);
    add_sev(*outCube, srecv,  psyncs, *it);
    // MPI RMA synchronizations
    add_sev(*outCube, rfence, rsyncs, *it);
    add_sev(*outCube, raccess, rgats, *it);
    add_sev(*outCube, rexposure, rgats, *it);
    add_sev(*outCube, rgats,  rsyncs, *it);
    add_sev(*outCube, rlocks, rsyncs, *it);
    // Sum up all synchronizations
    add_sev(*outCube, csyncs, syncs,  *it);
    add_sev(*outCube, psyncs, syncs,  *it);
    add_sev(*outCube, rsyncs, syncs,  *it);
    add_sev(*outCube, bcout,  bcoll,  *it);
    add_sev(*outCube, bcin,   bcoll,  *it);
    add_sev(*outCube, bsent,  bp2p,   *it);
    add_sev(*outCube, brcvd,  bp2p,   *it);
    add_sev(*outCube, bput,   brma,  *it);
    add_sev(*outCube, bgot,   brma,  *it);
    add_sev(*outCube, bcoll,  bytes,  *it);
    add_sev(*outCube, bp2p,   bytes,  *it);
    add_sev(*outCube, brma,   bytes,  *it);

    ++it;
  }

  if (!includes_mpi) { /* hide unused MPI metrics */
    mpi->set_val("VOID");
    syncs->set_val("VOID");
    comms->set_val("VOID");
    bytes->set_val("VOID");
  }

  if (!includes_rma) { /* hide unused MPI RMA metrics */
    rma->set_val("VOID");
    srma->set_val("VOID");
    brma->set_val("VOID");
    rcomms->set_val("VOID");
    rsyncs->set_val("VOID");
    pwsyncs->set_val("VOID");
  }

  if (!includes_io) { /* hide unused MPI File I/O metrics */
    io->set_val("VOID");
    file_ops->set_val("VOID");
    file_bytes->set_val("VOID");
  }

  if (includes_omp && skip_omp)
    cout << "Skipping OpenMP metrics." << endl;
  else if (includes_omp) { // Calculate idle threads heuristic
    const vector<Process*> &procv = outCube->get_procv();
    size_t num_procs = procv.size();
#if 0
    printf("%d process(es)\n", num_procs);
    for (size_t p = 0; p < num_procs; ++p) {
      const size_t proc_thrds = procv[p]->num_children();
      printf("  %d threads\n", proc_thrds);
      for (size_t t = 0; t < proc_thrds; ++t) {
        Thread* thread = procv[p]->get_child(t);
        bool master = (thread->get_rank() == 0);
        printf("    %s(%d)%s\n", thread->get_name().c_str(), thread->get_id(),
                          master ? "*" : "");
      }
    }
#endif

    // scan call-tree for OpenMP parallel regions
    bool in_parallel = false;
    size_t preg_level = UINT_MAX;
    size_t num_thrds = outCube->get_thrdv().size();
    vector<double> tvalues(num_thrds);
    Cnode* preg = NULL;
    it = cnodev.begin();
    while (it != cnodev.end()) {
      const Region* region = (*it)->get_callee();
      const size_t level = (*it)->indent().length()/2;
      if (level <= preg_level) {
        if (is_omp(region)) preg_level = level;
        else preg_level = UINT_MAX;
        if (preg) {
            //const Region* region = preg->get_callee();
            //printf("%s inclusive times:\n", region->get_name().c_str());
            for (size_t t = 0; t < num_thrds; ++t) {
                Thread* thread = outCube->get_thrdv()[t];
                Thread* master = thread->get_parent()->get_child(0);
                double mvalue = tvalues[master->get_id()];
                double idled = mvalue - tvalues[t];
                if (idled < 1e-10) continue; // ignore values below measurement threshold
                //printf("    %s %.6f %+.6f\n", thread->get_name().c_str(), tvalues[t], idled);
                outCube->set_sev(limpar, preg, thread, idled);
                outCube->add_sev(idleth, preg, thread, idled);
                outCube->add_sev(time, preg, thread, idled);
            }
            preg = NULL; // no longer in parallel region
        }
      }
      in_parallel = (preg_level != UINT_MAX);
      //printf("%d-%s:%d %s%s\n", level, region->get_descr().c_str(), in_parallel,
      //         (*it)->indent().c_str(), region->get_name().c_str());

      if (in_parallel) { // accumulate inclusive execution time in parallel region
        if (!preg) {
            preg = *it; // stash current parallel region
            //const Region* region = preg->get_callee();
            //printf("%s stashed at level %d\n", region->get_name().c_str(), preg_level);
            for (size_t t = 0; t < num_thrds; ++t) tvalues[t] = 0.0;
        }
        for (size_t t = 0; t < num_thrds; ++t) {
          Thread* thread = outCube->get_thrdv()[t];
          double value = outCube->get_sev(time, *it, thread);
          tvalues[t] += value;
        }
      } else { // master's serial time is idle time for workers
        for (size_t p = 0; p < num_procs; ++p) {
          Process *proc = procv[p];
          const size_t proc_thrds = proc->num_children();
          Thread* master = proc->get_child(0);
          double value = outCube->get_sev(time, *it, master);
          //printf("    %s %.6f\n", master->get_name().c_str(), value);
          for (size_t t = 1; t < proc_thrds; ++t) {
            Thread* worker = proc->get_child(t);
            outCube->set_sev(idleth, *it, worker, value);
            outCube->add_sev(time, *it, worker, value);
            //printf("    %s -> %.6f\n", worker->get_name().c_str(), value);
          }
        }
      }

      ++it;
    }

    if (preg) { // still have last parallel region stashed
      //const Region* region = preg->get_callee();
      //printf("%s inclusive times:\n", region->get_name().c_str());
      for (size_t t = 0; t < num_thrds; ++t) {
        Thread* thread = outCube->get_thrdv()[t];
        Thread* master = thread->get_parent()->get_child(0);
        double mvalue = tvalues[master->get_id()];
        double idled = mvalue - tvalues[t];
        if (idled < 1e-10) continue; // ignore values below measurement threshold
        //printf("    %s %.6f %+.6f\n", thread->get_name().c_str(), tvalues[t], idled);
        outCube->set_sev(limpar, preg, thread, idled);
        outCube->add_sev(idleth, preg, thread, idled);
        outCube->add_sev(time, preg, thread, idled);
      }
    }

    // remove VOID tags to make metrics visible
    idleth->set_val("");
    omp_time->set_val("");
  }

  // Calculate load imbalance heuristic
  vector<double> average;
  vector<double> dummy;
  size_t num_cnodes  = outCube->get_cnodev().size();
  size_t num_threads = outCube->get_thrdv().size();
  size_t num_procs   = outCube->get_procv().size();

  // Determine numbers of void processes and threads to ignore
  size_t void_procs  = 0, void_thrds = 0;
  for (size_t p = 0; p < outCube->get_procv().size(); p++) {
      Process *proc = outCube->get_procv()[p];
      if (proc->get_name().find("VOID") != string::npos) {
	  void_procs++;
          void_thrds+=proc->num_children();
      }
  }

  if (num_threads == 1) imbal->set_val("VOID");

  // Calculate average exclusive execution time
  outCube->get_cnode_tree(average, dummy, EXCL, INCL, exec, NULL);
  for (size_t cid = 0; cid < num_cnodes; ++cid) {
    Cnode* cnode = cnodev[cid];

    /* Inside parallel region? */
    bool is_parallel = false;
    if (includes_omp && !skip_omp) {
      while (cnode && !is_parallel) {
        is_parallel = is_omp_parallel(cnode->get_callee()->get_name());
        cnode = cnode->get_parent();
      }
    }

    if (is_parallel)
      average[cid] = average[cid] / (num_threads - void_thrds);
    else
      average[cid] = average[cid] / (num_procs - void_procs);
  }

  // Calculate difference to average value
  for (size_t tid = 0; tid < num_threads; ++tid) {
    Thread* thread = outCube->get_thrdv()[tid];

    /* Skip threads on void processes */
    if (thread->get_parent()->get_name().find("VOID") != string::npos) continue;

    /* Skip worker threads for pure MPI */
    if ((!includes_omp || skip_omp) && thread->get_rank() != 0)
      continue;

    for (size_t cid = 0; cid < num_cnodes; ++cid) {
      Cnode* cnode = cnodev[cid];

      /* Inside parallel region? */
      bool   is_parallel = false;
      Cnode* tmp_cnode   = cnode;
      while (tmp_cnode && !is_parallel) {
        is_parallel = is_omp_parallel(tmp_cnode->get_callee()->get_name());
        tmp_cnode = tmp_cnode->get_parent();
      }

      /* Ignore worker threads when outside parallel region */
      if (!is_parallel && thread->get_rank() != 0)
        continue;

      double sev  = outCube->get_vcsev(EXCL, EXCL, EXCL, exec, cnode, (Sysres*)thread);
      double diff = sev - average[cid];

      if (diff > 0) {
        outCube->set_sev(imbal, cnode, thread, diff);
        outCube->set_sev(above, cnode, thread, diff);
      } else if (diff < 0) {
        outCube->set_sev(imbal, cnode, thread, -diff);
        outCube->set_sev(below, cnode, thread, -diff);
      }
    }
  }

  // resolve computational imbalance due to singularities and imperfect parallelization
  for (size_t cid = 0; cid < num_cnodes; ++cid) {
    unsigned visitors=0;
    Cnode* cnode = cnodev[cid];
    double imbsev = average[cid];

    if (imbsev == 0.0) continue;

    if (includes_omp) {
      /* Inside parallel region? */
      bool   is_parallel = false;
      Cnode* tmp_cnode   = cnode;
      while (tmp_cnode && !is_parallel) {
        is_parallel = is_omp_parallel(tmp_cnode->get_callee()->get_name());
        tmp_cnode = tmp_cnode->get_parent();
      }

      /* Ignore OpenMP threads when outside parallel region */
      if (!is_parallel)
        continue;
    }

    for (size_t tid = 0; tid < num_threads; ++tid) {
      Thread* thread = outCube->get_thrdv()[tid];
      if (thread->get_parent()->get_name().find("VOID") != string::npos) continue;
      int nvisits = (int)outCube->get_sev(visits, cnode, thread);
      if (nvisits) visitors++;
    }

    if (visitors == (num_threads - void_thrds)) continue;

    for (size_t tid = 0; tid < num_threads; ++tid) {
      Thread* thread = outCube->get_thrdv()[tid];
      if (thread->get_parent()->get_name().find("VOID") != string::npos) continue;
      int nvisits = (int)outCube->get_sev(visits, cnode, thread);
      if (nvisits == 0)   outCube->set_sev(bypass, cnode, thread, imbsev);
      if (visitors == 1) { // executed by a single participant
        if (nvisits == 0) outCube->set_sev(bysing, cnode, thread, imbsev);
        else              outCube->set_sev(single, cnode, thread, imbsev*(num_threads-void_thrds-1));
      }
    }
  }

  return outCube;
}
