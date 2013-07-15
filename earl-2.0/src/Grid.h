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

#ifndef EARL_GRID_H
#define EARL_GRID_H

/*
 *----------------------------------------------------------------------------
 *
 * class Grid
 *
 *----------------------------------------------------------------------------
 */


#include <map>
#include <vector>
#include <string>

#include "Communicator.h"
#include "Location.h"
#include "Machine.h"
#include "Node.h"
#include "Process.h"
#include "Thread.h"
#include "Cartesian.h"
#include "MPIWindow.h"

namespace earl
{
  class Grid
  {
  public:
  
    Grid() {}
    ~Grid();
    
    long       get_nmachs()  const { return machm.size(); }
    long       get_nnodes()  const { return nodem.size(); }
    long       get_nprocs()  const { return procm.size(); }
    long       get_nthrds()  const { return thrdm.size(); }
    long       get_nlocs()   const { return locm.size(); }
    long       get_ncoms()   const { return comm.size(); }
    long       get_nwins()   const { return winm.size(); }
    long       get_ncarts()  const { return cartm.size(); } 
  
    Machine*      get_mach(long mach_id) const;
    Node*         get_node(long mach_id, long node_id) const;
    Process*      get_proc(long proc_id) const;
    Thread*       get_thrd(long proc_id, long thrd_id) const;
    Location*     get_loc(long loc_id) const;
    Communicator* get_com(long com_id) const;
    Cartesian*    get_cart(long cart_id) const;
    MPIWindow*    get_win (long win_id) const;
  
    Machine*      create_mach(long id, std::string name);    
    Node*         create_node(long mach_id, long node_id, std::string name, long cpuc, double cr);       
    Process*      create_proc(long id, std::string name);    
    Thread*       create_thrd(long thrd_id, long proc_id, std::string name); 
    Location*     create_loc(long id, long mach_id, long node_id, long proc_id, long thrd_id);     
    Communicator* create_com(long id, std::vector<long> proc_idv);     
    MPIWindow*    create_win(long wid, long cid);     
    Cartesian*    create_cart(long id, 
			      long cid,
			      long ndims, 
			      std::vector<long> dimv, 
			      std::vector<bool> periodv);

  private:
  
    std::map<long, Machine*>       machm;
    std::map<long, Location*>      locm;
    std::map<long, Process*>       procm;
    std::map<long, Communicator*>  comm;
    std::map<long, Cartesian*>     cartm;
    std::map<long, MPIWindow*>     winm;
  
    // node id only unique with respect to a machine
    std::map<std::pair<long, long>, Node*>    nodem;     
    // thread id only unique with respect to a process
    std::map<std::pair<long, long>, Thread*>  thrdm;  
  };
}  
#endif


