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

#ifndef EARL_EVENTTRACE_H
#define EARL_EVENTTRACE_H

/*
 *----------------------------------------------------------------------------
 *
 * class EventTrace
 *
 * EARL interface to an EPILOG event trace
 *
 *----------------------------------------------------------------------------
 */

#include <vector>

#include "BufTrace.h"
#include "Event.h"
#include "State.h"

namespace earl
{
  class EventTrace
  {
  public:

    EventTrace(std::string path);

    // static trace information

    std::string get_name() const;
    std::string get_path() const;
    
    bool has_shmem() const { return has_shmemreg;   };
    bool has_omp()   const { return has_ompreg; };
    bool has_mpi2()  const { return has_mpirma;   };

    long get_nevents() const;
    long get_nfiles() const;
    long get_nregs() const;
    long get_ncsites() const;
    long get_nmets() const;
    long get_nmachs() const; 
    long get_nnodes() const; 
    long get_nprocs() const; 
    long get_nthrds() const; 
    long get_nlocs() const; 
    long get_ncoms() const; 
    long get_ncarts() const;
    long get_nwins() const;
  
    std::string   get_file(long file_id) const;
    Region*       get_reg(long reg_id) const;
    Callsite*     get_csite(long csite_id) const;
    Metric*       get_met(long met_id) const;
    Machine*      get_mach(long mach_id) const;
    Node*         get_node(long mach_id, long node_id) const;
    Process*      get_proc(long proc_id) const;
    Thread*       get_thrd(long proc_id, long thrd_id) const;
    Location*     get_loc(long loc_id) const;
    Communicator* get_com(long com_id) const;
    Cartesian*    get_cart(long cart_id) const;
    MPIWindow*    get_win(long win_id) const;

    // topology information

    // get coordinates of a location with respect to a topology
    void get_coords(std::vector<long>& out, long cart_id, long loc_id) const;    
    // get location at given coordinates with respect to a topology
    Location* get_loc(std::vector<long>& in, long cart_id) const;    

    // dynamic trace information

    // event access
    Event event(long pos);
    bool  is_type(long pos, etype type);
    // region stack
    void stack(std::vector<Event>& out, long pos, long loc_id);
    // inherited region stack
    void istack(std::vector<Event>& out, long pos, long loc_id);
    // message queue
    void queue(std::vector<Event>& out, long pos, long src_id = -1, long dest_id = -1);
    // MPI collective operation
    void mpicoll(std::vector<Event>& out, long pos);
    // OpenMP collective operation
    void ompcoll(std::vector<Event>& out, long pos);

    //
    // MPI-2 RMA calls
    //
    // collective RMA synchronisation: fence, win_create, win_free
    void get_mpisyncccoll(std::vector<Event>& out, long pos);
    // MPI-2 RMA GAT Synchronisation: post, start, complete, wait
    // epoches which are completed with event at position 'pos'
    void get_mpiglobalepoches(std::vector<CountedPtr<MPIEpoch> >& out, long pos);
    // MPI-2 RMA GAT transfers: MPI_Put() and MPI_Get() from transfer queue
    void get_mpitransfers(std::vector<Event>& out, long pos, long loc_id);
    // MPI-2 RMA GAT transfers: MPI_Put() and MPI_Get() from transfer queue
    void get_mpigets(std::vector<Event>& out, long pos, long loc_id);
    // MPI-2 RMA GAT transfers: MPI_Put() and MPI_Get() from transfer queue
    void get_mpiputs(std::vector<Event>& out, long pos, long loc_id);
    // MPI-2 RMA GAT Synchronisation: origins of Get transfer operation 

    //
    //  NON MPI rma calls. 
    //
    void get_mpiorigins(std::vector<Event>& out, long pos, long loc_id);
    // non MPI RMA transfers: shmem_get_xxx() shmem_put_xxx(), etc.
    void get_rmatransfers(std::vector<Event>& out, long pos, long loc_id);
    // non MPI RMA transfers: shmem_get_xxx()
    void get_rmagets(std::vector<Event>& out, long pos, long loc_id);
    // non MPI RMA transfers: shmem_put_xxx()
    void get_rmaputs(std::vector<Event>& out, long pos, long loc_id);
    // non MPI RMA collective operation
    void get_rmacolls(std::vector<Event>& out, long pos);
    
    // call-tree nodes
    void calltree(std::vector<Event>& out, long pos = -1);
    // call-tree root nodes
    void ctroots(std::vector<Event>& out, long pos = -1);
    // call path
    void callpath(std::vector<Event>& out, long cnode);
    // children of a call-tree node
    void ctchildren(std::vector<Event>& out, long cnode);
    // number of times a call-tree node has been visited
    long ctvisits(long cnode);
    // number of call-tree nodes
    long ctsize(long pos = -1);

  private:
  
    BufTrace btrace;
    State state;

    std::string name;
    std::string path;

    const bool has_mpirma; 
    const bool has_ompreg;
    const bool has_shmemreg;
    
  };

#ifndef NO_INLINE
#  include "EventTrace.inl"
#endif
}
#endif
