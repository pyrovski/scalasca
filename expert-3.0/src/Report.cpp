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

#include "Report.h"

#include "CallTree.h"
#include "Pattern.h"
#include "PatternTree.h"

#include "StatisticController.h"

using namespace std;

#include "Location.h"
#include "Thread.h"
#include "Process.h"

#include "Cnode.h"
#include "Metric.h"

#include <map>
#include <sstream>

using namespace earl;
using namespace cube;


Report::Report(const PatternTree& ptree, 
	       const CallTree&    ctree,
	       const earl::EventTrace& trace)
{
  // mapping of EXPERT IDs onto CUBE objects

  map<long, cube::Metric*> patm;
  map<const earl::Region*, cube::Region*> regm;
  map<long, cube::Cnode*> cnodem;
  map<const earl::Machine*, cube::Machine*> machm;
  map<const earl::Node*, cube::Node*> nodem;
  map<const earl::Process*, cube::Process*> procm;
  map<const earl::Thread*, cube::Thread*> thrdm;
  map<const earl::Location*, cube::Thread*> locm;
  map<const earl::Cartesian*, cube::Cartesian*> cartm;

  vector<long> keyv;
  int unique = 0;

  // define mirror sites

  cb.def_mirror("http://icl.cs.utk.edu/projectsfiles/kojak/software/kojak/");

  // define CUBE aggregation mode

  cb.def_attr("CUBE_CT_AGGR", "SUM");

  // specify mapping of absent information

  patm[-1] = NULL;
  cnodem[-1] = NULL;

  // define CUBE metrics

  ptree.get_dfkeyv(keyv);
  for ( size_t i = 0; i < keyv.size(); i++ )
    {
      Pattern* pat = ptree.get_data(keyv[i]);
      stringstream num;
      num << unique++;
      string dtype;
      if (pat->get_uom() == "occ") dtype = "INTEGER";
      else dtype = "FLOAT";
      patm[pat->get_id()] = cb.def_met(pat->get_name(), pat->get_unique_name(),
                        dtype, pat->get_uom(), pat->get_val(), pat->get_url(),
                        pat->get_descr(), patm[pat->get_parent_id()]);
    }

  // define CUBE regions

  for ( long reg_id = 0; reg_id < trace.get_nregs(); reg_id++ )
    {
      const earl::Region* reg = trace.get_reg(reg_id);
      regm[reg] = cb.def_region(reg->get_name(), 
				reg->get_begln(), 
				reg->get_endln(),
				"",
				reg->get_descr(), 
				reg->get_file()); 
    }

  // define CUBE call paths (i.e., call-tree nodes)

  ctree.get_dfkeyv(keyv);
  for ( size_t i = 0; i < keyv.size(); i++ )
  {
      const earl::Callsite* csite  = ctree.get_csite(keyv[i]);
      const earl::Region*   callee = ctree.get_callee(keyv[i]);

      if(csite == NULL)
      {
          cnodem[keyv[i]] = cb.def_cnode(regm[callee],
                                         cnodem[ctree.get_parent(keyv[i])]);
      }
      else
      {
          cnodem[keyv[i]] = cb.def_cnode(regm[callee],
                                         csite->get_file(), csite->get_line(),
                                         cnodem[ctree.get_parent(keyv[i])]);
      }
      StatisticController::theController->AddCubeCNodeID(keyv[i],
                                                     cnodem[keyv[i]]->get_id());
  }  


  // define CUBE machines and nodes

  long node_id = 0;
  for ( long mach_id = 0; mach_id < trace.get_nmachs(); mach_id++ )
    {
      const earl::Machine* mach = trace.get_mach(mach_id);
      machm[mach] = cb.def_mach(mach->get_name(), "");
				     
      for ( long n = 0; n < mach->get_nnodes(); n++, node_id++ )
	{
	  const earl::Node* node = trace.get_node(mach_id, node_id);
	  nodem[node] = cb.def_node(node->get_name(), machm[mach]);
	}				       
    }
  
  // define CUBE processes and threads

  for ( long proc_id = 0; proc_id < trace.get_nprocs(); proc_id++ )
    {
      const earl::Process* proc = trace.get_proc(proc_id);
      procm[proc] = cb.def_proc(proc->get_name(),
				proc->get_id(),
				nodem[proc->get_node()]);
      
      for ( long thrd_id = 0; thrd_id < proc->get_nthrds(); thrd_id++ )
	{
	  const earl::Thread* thrd = trace.get_thrd(proc_id, thrd_id);
	  thrdm[thrd] = cb.def_thrd(thrd->get_name(), 
				    thrd->get_thrd_id(), 
				    procm[proc]);
	  
	  locm[thrd->get_loc()] = thrdm[thrd];
	}				       
    }

  // define CUBE severity
  
  vector<long> pat_idv, cnode_idv;

  ptree.get_keyv(pat_idv);
  ctree.get_keyv(cnode_idv);
  
  for ( size_t i = 0; i < pat_idv.size(); i++ )
    {
      long pat_id = pat_idv[i];

      for ( size_t j = 0; j < cnode_idv.size(); j++ )
	{
	  long cnode_id = cnode_idv[j];

	  for ( long loc_id = 0; loc_id < trace.get_nlocs(); loc_id++ )
	    cb.set_sev(patm[pat_id], 
		       cnodem[cnode_id], 
		       locm[trace.get_loc(loc_id)], 
		       ptree.get_data(pat_id)->get_sev(cnode_id, loc_id));
	}
    }

  // define CUBE cartesian virtual topologies
  for ( long cart_id = 0 ; cart_id < trace.get_ncarts() ; cart_id++ ) 
    {
      const earl::Cartesian* cart = trace.get_cart(cart_id);
      std::vector<long> dimv;
      std::vector<bool> periodv;

      // get dimv
      cart->get_dimv(dimv);
      
      // get periodv
      cart->get_periodv(periodv);
      
  
      // write the cube definition
      cartm[cart] = cb.def_cart(cart->get_ndims(),
				dimv,
				periodv);
      
      // get the coords for locations
      for ( long loc_id = 0 ; loc_id < trace.get_nlocs() ; loc_id++ ) 
	{
	  earl::Location* loc = trace.get_loc(loc_id);
	  std::vector<long> coordv;
	  cart->get_coords( coordv, loc);
	  if ( coordv.size() != 0 ) 
	    {
	      // define the cart coords if the coordv is not empty
	      cb.def_coords(cartm[cart], 
			    (Sysres *) locm[loc], 
			    coordv);
	    }
	}
    }
}

void 
Report::write(std::string path)
{
  cb.write(path);
}

