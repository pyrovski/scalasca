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

#ifndef EXP_COMPOUNDEVENTS_H
#define EXP_COMPOUNDEVENTS_H

#include <iostream>

#include "MPIEpoch.h"
#include "MPILocEpoch.h"
#include "CountedPtr.h"

enum cetype { RECV_OP, 
	      LATE_SEND,
	      LATE_RECV,
	      MPI_COLL,
	      TEAM_CREATE,
	      LOCK_COMP,
	      OMP_COLL,
              RMA_COLL,
              RMA_LOC_EPOCH,
              RMA_GLB_EPOCH,
              EARLY_WAIT,
	      SHMEM_COLL
             };

struct cmp_event 
{
  virtual ~cmp_event() { }
  cetype type;
};

/****************************************************************************
* Receive operation
****************************************************************************/

struct recv_op : public cmp_event
{
  recv_op() { type = RECV_OP; }

  earl::Event recv;           // RECV event
  earl::Event enter_recv;     // ENTER event of receive operation
  const earl::Region* region; // receive operation 
};

/****************************************************************************
* Late sender
****************************************************************************/

struct late_send : public recv_op
{
  late_send() { type = LATE_SEND; }
  late_send(const recv_op* recv_op) 
  {
    type = LATE_SEND;
    recv = recv_op->recv;  
    enter_recv = recv_op->enter_recv;    
    region = recv_op->region;   
  }
  
  earl::Event send;           // SEND event
  earl::Event enter_send;     // ENTER event of send operation      
  double      idle_time;      // time receiver waited for message
};

/****************************************************************************
* Late receiver
****************************************************************************/

struct late_recv : public recv_op
{
  late_recv() { type = LATE_RECV; }
  late_recv(const recv_op* recv_op) 
  {
    type = LATE_RECV; 
    recv = recv_op->recv;  
    enter_recv = recv_op->enter_recv;    
    region = recv_op->region;   
  }

  earl::Event send;           // SEND event
  earl::Event enter_send;     // ENTER event of send operation      
  double      idle_time;      // time sender waited for receiver
};

/****************************************************************************
* MPI collective operation
****************************************************************************/

struct mpi_coll : public cmp_event
{
  mpi_coll() { type = MPI_COLL; }

  std::vector<earl::Event> exitv;     // list of EXIT events ( in ascending rank order )
  std::vector<earl::Event> enterv;    // list of ENTER events ( in ascending rank order )
  long                     root_idx;  // list index of root location
  earl::Region*            region;    // collective operation               
};

/****************************************************************************
* Team creation
****************************************************************************/

struct team_create : public cmp_event
{
  team_create() { type = TEAM_CREATE; }

  earl::Event enter;         // ENTER event of parallel region
  double      time;          // time difference between FORK and ENTER event 
};

/****************************************************************************
* Lock competition
****************************************************************************/

struct lock_comp : public cmp_event
{
  lock_comp() { type = LOCK_COMP; }

  earl::Event ompalock;      // OMPALOCK event
  earl::Event enter_alock;   // ENTER event of acquire-lock operation
  earl::Event omprlock;      // OMPRLOCK event
  double      idle_time;     // time thread has to wait for lock acquisition
};

/****************************************************************************
* OpenMP collective operation
****************************************************************************/

struct omp_coll : public cmp_event
{
  omp_coll() { type = OMP_COLL; }

  std::vector<earl::Event> exitv;     // list of EXIT events ( in ascending rank order )
  std::vector<earl::Event> enterv;    // list of ENTER events ( in ascending rank order )
  earl::Region*            region;    // collective operation               
};

/****************************************************************************
* MPI-2 RMA fence operation
****************************************************************************/

struct rma_coll : public cmp_event
{
  rma_coll() { type = RMA_COLL; }

  std::vector<earl::Event> exitv;     // list of EXIT events ( in ascending rank order )
  std::vector<earl::Event> enterv;    // list of ENTER events ( in ascending rank order )
  earl::Region*            region;    // collective operation              
  std::ostream& operator<< (std::ostream& os) const
    {
      os<<"reg: ";
      if (region)
        os << region->get_name()<<" id="<<region->get_id();
      os<< "\n  enterv: " << enterv.size() << "   ";
      for (size_t i=0;i<enterv.size();++i) 
        os << enterv[i].get_reg()->get_id() << ","; 
      os<< "\n  exitv: "  << exitv.size() << "   ";
      for (size_t i=0;i<exitv.size();++i) 
        os << exitv[i].get_reg()->get_id() << ",";
      os << std::endl;
      return os;
    };
};

/****************************************************************************
* MPI-2 RMA epoches 
****************************************************************************/

struct rma_loc_epoch : public cmp_event
{
  //rma_loc_epoch() { type = RMA_LOC_EPOCH; }
  rma_loc_epoch(const earl::MPILocEpoch& ep ) : loc_epoch(ep) { type = RMA_LOC_EPOCH; }
  earl::MPILocEpoch loc_epoch;     // MPI epoche at some location
};

struct rma_glb_epoch : public cmp_event
{
  rma_glb_epoch() { type = RMA_GLB_EPOCH; }
  rma_glb_epoch(const std::vector <CountedPtr<earl::MPIEpoch> >& eps ) : completed_epoches(eps) { type = RMA_GLB_EPOCH; }
  std::vector <CountedPtr<earl::MPIEpoch> > completed_epoches; 

};

struct early_wait : public cmp_event
{
  early_wait() { type = RMA_GLB_EPOCH; }
  early_wait(double idle_time, CountedPtr<earl::MPILocEpoch> exp_ep, CountedPtr<earl::MPILocEpoch> acc_ep ) : 
      idle_time(idle_time), exp_ep(exp_ep), acc_ep(acc_ep) { type = EARLY_WAIT; }
  double idle_time;
  CountedPtr<earl::MPILocEpoch> exp_ep;     // MPI expouse epoche 
  CountedPtr<earl::MPILocEpoch> acc_ep;     // MPI corresponding access epoche 
};

/****************************************************************************
* SHMEM Collective 
****************************************************************************/

struct shmem_coll : public cmp_event
{
  shmem_coll() { type = SHMEM_COLL; root_idx=-1; }

  std::vector<earl::Event> exitv;     // list of EXIT events ( in ascending rank order )
  std::vector<earl::Event> enterv;    // list of ENTER events ( in ascending rank order )
  earl::Region*            region;    // collective operation             
  long                     root_idx;  // list index of root location
};

#endif

