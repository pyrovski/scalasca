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

#ifndef EARL_COLLECTIVE_QUEUE_H
#define EARL_COLLECTIVE_QUEUE_H

/*
 *----------------------------------------------------------------------------
 *
 *  Generic implementation of all collective queues based on templates. 
 *  Supported operations are queue, dequeue events and
 *  export of completed collective operations
 *
 * At time 4 collective queues are defined:
 * 
 * 1) MPI_CollectiveQueue -  collect MPI collective operation events 
 * 
 * 2) OMP_CollectiveQueue -  collect OMP collective operation events
 *
 * 3) MPI_RmaCollQueue    -  collect MPI-2 RMA collective operation events
 *
 * 4) CollectiveQueue     -  collect SHMEM collective operation events 
 *
 * new collective queuus can be defined as typedefs at the end of this file
 * 
 *----------------------------------------------------------------------------
 */

#include <cstdio>
#include <algorithm>
#include <list>
#include <vector>

#include "CollExit_rep.h"
#include "Communicator.h"
#include "Exit_rep.h"
#include "Grid.h"
#include "MPICExit_rep.h"
#include "MPIWCExit_rep.h"
#include "OMPCExit_rep.h"


namespace earl
{

  // The default collective queue traits class 
  // uses Communicator class to define a group
  // mapping:  (commuicator_id x rank) |-> < list<Event> >
  template<typename EventT>
  struct CollQueueTraits 
  {
    typedef Communicator GroupT;
    static GroupT* GroupPtr(Event e)            {return e->get_com(); };
    static size_t  GroupID (GroupT* com)        {return com->get_id();};
    static size_t  GroupCount(const Grid* grid) {return grid->get_ncoms();};
    static long    EventID(Event e, GroupT* g)  {return g->get_rank(e->get_loc());};
    static long    GroupSize(GroupT* com)       {return com->get_nprocs();};
    static long    GroupSize(const Grid* grid, size_t com_id) 
                                                {return grid->get_com(com_id)->get_nprocs();};
  };

  //
  //  A traits template with specializations 
  //
  
  // MPI-2 RMA traits 
  // uses MPIWindow class to define a group
  // mapping:  (window_id x rank) |-> < list<Event> >
  template<>
  struct CollQueueTraits<MPIWCExit_rep> 
  {
    typedef MPIWindow GroupT;
    static GroupT* GroupPtr(Event e)            {return e->get_win(); };
    static size_t  GroupID (GroupT* win)        {return win->get_id();};
    static size_t  GroupCount(const Grid* grid) {return grid->get_nwins();};
    static long    EventID(Event e, GroupT* g)  {return g->get_com()->get_rank(e->get_loc());};
    static long    GroupSize(GroupT* win)       {return win->get_com()->get_nprocs();};
    static long    GroupSize(const Grid* grid, size_t win_id) 
                                                {return grid->get_win(win_id)->get_com()->get_nprocs();};
  };
  
  // OMP traits 
  // uses Process class to define a group
  // mapping:  (proc_id x thread_id) |-> < list<Event> >
  template<>
  struct CollQueueTraits<OMPCExit_rep> 
  {
    typedef Process GroupT;
    static GroupT* GroupPtr(Event e)            {return e->get_loc()->get_proc();};
    static size_t  GroupID(GroupT* proc)        {return proc->get_id();};
    static size_t  GroupCount(const Grid* grid) {return grid->get_nprocs();};
    static long    EventID(Event e, GroupT* g)  {return e->get_loc()->get_thrd()->get_thrd_id();};
    static long    GroupSize(GroupT* proc)      {return proc->get_nthrds();};
    static long    GroupSize(const Grid* grid, size_t proc_id) 
                                                {return grid->get_proc(proc_id)->get_nthrds();};
  };

  // CollectiveQueueT is template class which defines generic collective queue
  // use typedefs at the end of this file to define new collective queues
  template<typename EventT, typename TraitsT=CollQueueTraits<EventT> >
  class CollectiveQueueT
  {
  public:
    // public typedefs to use with algorithms
    typedef EventT  EventType;
    typedef TraitsT TraitsType;

    // constructor allocate memory for mapping
    CollectiveQueueT(const Grid* grid) :
      exitm(TraitsType::GroupCount(grid)),
      completed(NULL)
      {
         size_t grpcnt = exitm.size();
         for ( size_t grpid = 0; grpid < grpcnt; grpid++ ){
           long grpsize = TraitsType::GroupSize(grid, grpid);
           for ( long i = 0; i < grpsize; i++ )
             exitm[grpid].push_back(std::list<Event>());
         }
      }
    
    // CollectiveQueueT<...> class is designed to be a base class
    virtual ~CollectiveQueueT()
      {
      }

    // get events if collective operation is complete
    void   get_coll(std::vector<Event>& out)
      {
        out.clear();
        if ( !completed )
          return;
        
        long grpid  = TraitsType::GroupID(completed);
        long grpsize = TraitsType::GroupSize(completed);
        
        out.reserve(grpsize);
        for (long ev_id = 0; ev_id < grpsize; ev_id++)
          out.push_back(exitm[grpid][ev_id].front());
      
        std::sort(out.begin(), out.end());
      } 

    // queue collective event 
    void   queue_coll(EventT* pCExit)
      {
        bool is_completed = true;
       
        // create smartpointer object
        Event cexit(pCExit);
     
        // get event attributes using traits
        typename TraitsType::GroupT* grp
                    = TraitsType::GroupPtr(cexit);
        long grpid  = TraitsType::GroupID(grp);
        long ev_id  = TraitsType::EventID(cexit, grp);
        long grpsize= TraitsType::GroupSize(grp);
     
        // save
        exitm[grpid][ev_id].push_back(cexit);
        
        // check if this event completes the collective operation
        for ( ev_id = 0; ev_id < grpsize; ev_id++ )
          if ( exitm[grpid][ev_id].empty() )
            is_completed = false;
        if ( is_completed )
          completed = grp;
      }
    
    // dequeue events of complete collective operation if any
    void   dequeue_coll()
      {
        if ( !completed )
          return;
      
        long grpsize = TraitsType::GroupSize(completed);
        long grpid   = TraitsType::GroupID(completed);
        
        for (int ev_id = 0; ev_id < grpsize; ev_id++)
            exitm[grpid][ev_id].pop_front();
        completed = NULL;
      }
   
    // valid only for OMP join events
    void check_dynamic(Event join)
    {
      typename TraitsType::GroupT* grp
                   = TraitsType::GroupPtr(join);
      long grpid   = TraitsType::GroupID(grp);
      long grpsize = TraitsType::GroupSize(grp);
    
      for ( int ev_id = 0; ev_id < grpsize; ev_id++ )
        if ( ! exitm[grpid][ev_id].empty() ) {
          char pos[10];
          sprintf(pos, "%ld", join.get_pos());
          throw RuntimeError("Join event " + std::string(pos) + 
                ": Dynamically changing number of threads not supported.");
        }
    }

  private:
    // (grpid, ev_id) |-> event (CExit_rep) 
    std::vector<std::vector<std::list<Event> > > exitm; 

    typename TraitsType::GroupT* completed;
  };

// collect SHMEM collective operation events 
typedef CollectiveQueueT <CollExit_rep>  CollectiveQueue;

// collect MPI collective operation events 
typedef CollectiveQueueT <MPICExit_rep>  MPI_CollectiveQueue;

// collect MPI-2 RMA collective operation events
typedef CollectiveQueueT <MPIWCExit_rep> MPI_RmaCollQueue;

// collect OMP collective operation events
typedef CollectiveQueueT <OMPCExit_rep>  OMP_CollectiveQueue;

}
#endif

