/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <CNode.h>
#include <CallbackManager.h>
#include <GlobalDefs.h>

#include "PatternDetectionTask.h"
#include "CbData.h"
#include "MpiPattern.h"
#ifdef _OPENMP
#  include "OmpPattern.h"
#endif
#include "Patterns_gen.h"

using namespace std;
using namespace pearl;
using namespace scout;



#line 13 "MPI.pattern"

  #include <algorithm>
  #include <cctype>
  #include <cfloat>
  #include <functional>
  #include <list>

  #include <elg_error.h>

  #include <CallbackManager.h>
  #include <Location.h>
  #include <Process.h>

  #if defined(_MPI)
    #include <EventSet.h>
    #include <Group.h>
    #include <MpiComm.h>
    #include <MpiMessage.h>
    #include <RemoteEventSet.h>

    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
  #endif   // _MPI

  #include "Predicates.h"
  #include "Roles.h"
  #include "scout_types.h"
  #include "user_events.h"


  //--- Utility functions -----------------------------------------------------

  namespace
  {

  struct fo_tolower : public std::unary_function<int,int> {
    int operator()(int x) const {
      return std::tolower(x);
    }
  };

  string lowercase(const std::string& str)
  {
    string result(str);

    std::transform(str.begin(), str.end(), result.begin(), fo_tolower());

    return result;
  }

  }   // unnamed namespace

#line 13 "OMP.pattern"

#if defined(_OPENMP)
namespace {
    int omp_nest_level = 0;
}   // unnamed namespace
#endif

#line 10 "RMA.pattern"

  #include <algorithm>
  #include <cfloat>
  #include <cstring>
  #include <iostream>
  #include <sstream>

  #include <set>

  #if defined(_MPI)
    #include <MpiWindow.h>
    #include <MpiGroup.h>
    #include "MpiOperators.h"
  #endif

namespace {
  /* offsets to individual entries in the window. As the window has an
   * internal displacement of 1 (i.e. sizeof(unsigned char)) the offset
   * for the timestamps need to be calculated with sizeof(timestamp_t).
   */
  const int LOCAL_POST      = 0;
  const int LATEST_POST     = 1 * sizeof(timestamp_t);
  const int LATEST_COMPLETE = 2 * sizeof(timestamp_t);
  const int LATEST_RMA_OP   = 3 * sizeof(timestamp_t);
  const int BITFIELD_START  = 4 * sizeof(timestamp_t);
}

#line 1 "Statistics.pattern"

  #include <cstring>
  #include <pearl_replay.h>
  #include <epk_archive.h>
  #include <sys/stat.h>
  #include "Quantile.h"
  #include "ReportData.h"

  #if defined(_MPI)
    #include "MpiDatatypes.h"
    #include "MpiOperators.h"
  #endif   // _MPI

  #if defined(_OPENMP)
    #include "omp.h"
  #endif   // _OPENMP
  
  /// Maximum number of coefficients used for quantile approximation
  #define NUMBER_COEFF 60

  extern bool enableStatistics;
  /* For debugging: #define WRITE_CONTROL_VALUES */


/*
 *---------------------------------------------------------------------------
 *
 * class PatternTime
 *
 *---------------------------------------------------------------------------
 */


class PatternTime : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(EXIT, PEARL_create_callback(this, &PatternTime::exit_cb));
      cbmanager->register_callback(FINISHED, PEARL_create_callback(this, &PatternTime::finished_cb));
      cbmanager->register_callback(OMP_MGMT_FORK, PEARL_create_callback(this, &PatternTime::omp_mgmt_fork_cb));
      cbmanager->register_callback(OMP_MGMT_JOIN, PEARL_create_callback(this, &PatternTime::omp_mgmt_join_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_TIME;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "Time";
    }

    virtual string get_unique_name() const
    {
      return "time";
    }

    virtual string get_descr() const
    {
      return "Total CPU allocation time (includes time allocated for idle threads)";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void exit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_mgmt_fork_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_mgmt_join_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternTime::exit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 37 "Generic.pattern"

      Event  enter = data->m_callstack.top();
      CNode* cnode = enter.get_cnode();

      m_severity[cnode] += event->get_time() - enter->get_time();
    
}

void PatternTime::finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 52 "Generic.pattern"

      // Exclusify profile
      uint32_t count = data->m_defs->num_cnodes();
      for (ident_t id = 0; id < count; ++id) {
        CNode* cnode = data->m_defs->get_cnode(id);

        map<CNode*,double>::iterator it = m_severity.find(cnode);
        if (it != m_severity.end()) {
          CNode* parent = cnode->get_parent();

          // If there is a parent but no severity stored, we are at the
          // outermost OpenMP parallel region (hopefully)
          if (parent && m_severity.find(parent) != m_severity.end())
            m_severity[parent] -= it->second;
        }
      }
    
}

void PatternTime::omp_mgmt_fork_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 44 "Generic.pattern"

      m_severity[event.get_cnode()] += data->m_idle;
    
}

void PatternTime::omp_mgmt_join_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 48 "Generic.pattern"

      m_severity[event.get_cnode()] += data->m_idle;
    
}


/*
 *---------------------------------------------------------------------------
 *
 * class PatternVisits
 *
 *---------------------------------------------------------------------------
 */


class PatternVisits : public Pattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(ENTER, PEARL_create_callback(this, &PatternVisits::enter_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_VISITS;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "Visits";
    }

    virtual string get_unique_name() const
    {
      return "visits";
    }

    virtual string get_descr() const
    {
      return "Number of visits";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void enter_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternVisits::enter_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 109 "Generic.pattern"

      ++m_severity[event.get_cnode()];
    
}


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(ENTER, PEARL_create_callback(this, &PatternMPI::enter_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI;
    }

    virtual long get_parent() const
    {
      return PAT_EXECUTION;
    }

    virtual string get_name() const
    {
      return "MPI";
    }

    virtual string get_unique_name() const
    {
      return "mpi";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI calls";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void enter_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI::enter_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 94 "MPI.pattern"

      Region* region = event->get_region();

      if (is_mpi_init(region))
        cbmanager.notify(INIT, event, data);
      else if (is_mpi_finalize(region))
        cbmanager.notify(FINALIZE, event, data);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SyncCollective
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SyncCollective : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_SyncCollective::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_SYNC_COLLECTIVE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "Collective";
    }

    virtual string get_unique_name() const
    {
      return "mpi_sync_collective";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SyncCollective::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 260 "MPI.pattern"

      Event enter = data->m_callstack.top();
      if (!is_mpi_barrier(enter->get_region()))
        return;

      // A single process will not wait for itself
      if (event->get_comm()->get_group()->num_ranks() < 2)
        return;

      data->m_local->add_event(enter, ROLE_BARRIER_ENTER);
      data->m_local->add_event(event, ROLE_BARRIER_EXIT);

      // Retrieve latest ENTER and earliest EXIT event
      TimeVec2 local_times;
      local_times.value[0] = event->get_time();
      local_times.value[1] = enter->get_time();
      MpiComm* comm = event->get_comm();
      MPI_Allreduce(&local_times, &data->m_timevec, 1, TIMEVEC2,
                    MINMAX_TIMEVEC2, comm->get_comm());
      cbmanager.notify(SYNC_COLL, event, data);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BarrierWait
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BarrierWait : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(SYNC_COLL, PEARL_create_callback(this, &PatternMPI_BarrierWait::sync_coll_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_BARRIER_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SYNC_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "Wait at Barrier";
    }

    virtual string get_unique_name() const
    {
      return "mpi_barrier_wait";
    }

    virtual string get_descr() const
    {
      return "Waiting time in front of MPI barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void sync_coll_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BarrierWait::sync_coll_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 307 "MPI.pattern"

      pearl::timestamp_t max_time = data->m_timevec.value[1];

      // Validate clock condition
      if (max_time > event->get_time()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %d, reg: %s, diff: %es)!\n",
                     event.get_location()->get_id(),
                     event.get_cnode()->get_region()->get_name().c_str(),
                     max_time - event->get_time());
        cbmanager.notify(CCV_COLL, event, data);

        // Restrict waiting time to time spent in operation
        max_time = event->get_time();
      }

      // Calculate waiting time
      data->m_idle = max_time - event.enterptr()->get_time();
      if (data->m_idle > 0)
        m_severity[event.get_cnode()] += data->m_idle;

      // There will always be waiting time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(WAIT_BARRIER, event, data);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BarrierCompletion
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BarrierCompletion : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(SYNC_COLL, PEARL_create_callback(this, &PatternMPI_BarrierCompletion::sync_coll_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_BARRIER_COMPLETION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SYNC_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "Barrier Completion";
    }

    virtual string get_unique_name() const
    {
      return "mpi_barrier_completion";
    }

    virtual string get_descr() const
    {
      return "Time needed to finish an MPI barrier";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void sync_coll_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BarrierCompletion::sync_coll_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 356 "MPI.pattern"

      pearl::timestamp_t min_time = data->m_timevec.value[0];
      pearl::timestamp_t max_time = data->m_timevec.value[1];

      // Validate clock condition
      if (min_time < max_time) {
        // Do not report violation again -- already done by "Wait at Barrier"
        min_time = max_time;
      }

      // Calculate waiting time
      data->m_idle = event->get_time() - min_time;
      if (data->m_idle > 0)
        m_severity[event.get_cnode()] += data->m_idle;

      // There will always be completion time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(BARRIER_COMPL, event, data);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_P2P
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_P2P : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(FINALIZE, PEARL_create_callback(this, &PatternMPI_P2P::finalize_cb));
      cbmanager->register_callback(MPI_RECV, PEARL_create_callback(this, &PatternMPI_P2P::mpi_recv_cb));
      cbmanager->register_callback(MPI_SEND, PEARL_create_callback(this, &PatternMPI_P2P::mpi_send_cb));
      cbmanager->register_callback(PENDING, PEARL_create_callback(this, &PatternMPI_P2P::pending_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_POINT2POINT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COMMUNICATION;
    }

    virtual string get_name() const
    {
      return "Point-to-point communication";
    }

    virtual string get_unique_name() const
    {
      return "mpi_point2point";
    }

    virtual string get_descr() const
    {
      return "MPI point-to-point communication";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void finalize_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pending_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 404 "MPI.pattern"

    std::vector<MpiMessage*> m_pending;
    std::vector<MPI_Request> m_requests;
    std::vector<int>         m_indices;
    std::vector<MPI_Status>  m_statuses;
  
};


/*----- Callback methods -----*/

void PatternMPI_P2P::finalize_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 490 "MPI.pattern"

      MPI_Barrier(MPI_COMM_WORLD);

      // Try to complete pending messages
      cbmanager.notify(PENDING, event, data);

      // Handle remaining messages
      if (!m_pending.empty()) {
        int count = m_pending.size();

        // Print warning
        elg_warning("Encountered %d unreceived send operations!", count);

        // Cancel pending messages
        for (int i = 0; i < count; ++i) {
          MPI_Cancel(&m_requests[i]);
          m_pending[i]->wait();
          delete m_pending[i];
        }
      }
    
}

void PatternMPI_P2P::mpi_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 434 "MPI.pattern"

      if (!is_mpi_api(event.enterptr()->get_region()))
        return;

      data->m_local->add_event(event, ROLE_RECV);
      data->m_local->add_event(event.enterptr(), ROLE_ENTER_RECV);
      data->m_local->add_event(event.exitptr(), ROLE_EXIT_RECV);

      cbmanager.notify(PRE_RECV, event, data);

      MpiComm* comm = event->get_comm();
      data->m_remote->recv(*data->m_defs,
                           *comm,
                           event->get_source(),
                           event->get_tag());

      // Validate clock condition
      RemoteEvent send = data->m_remote->get_event(ROLE_SEND);
      if (send->get_time() > event->get_time()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %d, reg: %s, diff: %es)!\n",
                     event.get_location()->get_id(),
                     event.get_cnode()->get_region()->get_name().c_str(),
                     send->get_time() - event->get_time());
        cbmanager.notify(CCV_P2P, event, data);
      }

      cbmanager.notify(POST_RECV, event, data);
    
}

void PatternMPI_P2P::mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 412 "MPI.pattern"

      if (!is_mpi_api(event.enterptr()->get_region()))
        return;

      data->m_local->add_event(event, ROLE_SEND);
      data->m_local->add_event(event.enterptr(), ROLE_ENTER_SEND);

      cbmanager.notify(PRE_SEND, event, data);

      MpiComm*    comm = event->get_comm();
      MpiMessage* msg;
      msg = data->m_local->isend(*comm,
                                 event->get_dest(),
                                 event->get_tag());

      m_pending.push_back(msg);
      m_requests.push_back(msg->get_request());

      cbmanager.notify(POST_SEND, event, data);
      cbmanager.notify(PENDING, event, data);
    
}

void PatternMPI_P2P::pending_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 463 "MPI.pattern"

      // No pending requests? ==> continue
      if (m_pending.empty())
        return;

      // Check for completed messages
      int completed;
      int count = m_pending.size();
      m_indices.resize(count);
      m_statuses.resize(count);
      MPI_Testsome(count, &m_requests[0], &completed, &m_indices[0], &m_statuses[0]);

      // Update array of pending messages
      for (int i = 0; i < completed; ++i) {
        int index = m_indices[i];

        delete m_pending[index];
        m_pending[index] = NULL;
      }
      m_pending.erase(remove(m_pending.begin(), m_pending.end(),
                             static_cast<MpiMessage*>(NULL)),
                      m_pending.end());
      m_requests.erase(remove(m_requests.begin(), m_requests.end(),
                              static_cast<MPI_Request>(MPI_REQUEST_NULL)),
                       m_requests.end());
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateSender
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateSender : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(EXIT, PEARL_create_callback(this, &PatternMPI_LateSender::exit_cb));
      cbmanager->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_LateSender::post_recv_cb));
      cbmanager->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternMPI_LateSender::pre_recv_cb));
      cbmanager->register_callback(PRE_SEND, PEARL_create_callback(this, &PatternMPI_LateSender::pre_send_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATESENDER;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_POINT2POINT;
    }

    virtual string get_name() const
    {
      return "Late Sender";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latesender";
    }

    virtual string get_descr() const
    {
      return "Time a receiving process is waiting for a message";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void exit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
  protected:
    /* Protected methods */

    virtual void init()
    {

#line 549 "MPI.pattern"

    m_max_idle = 0.0;
  
    }


  private:

#line 544 "MPI.pattern"

    double         m_max_idle;
    EventSet       m_receive;
    RemoteEventSet m_send;
  
};


/*----- Callback methods -----*/

void PatternMPI_LateSender::exit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 603 "MPI.pattern"

      if (m_max_idle > 0.0 &&
          is_mpi_wait_multi(event.enterptr()->get_region())) {
        m_severity[event.get_cnode()] += m_max_idle;

        Event recv       = m_receive.get_event(ROLE_RECV);
        RemoteEvent send = m_send.get_event(ROLE_SEND);

        data->m_idle = m_max_idle;
        data->m_local->add_event(recv, ROLE_RECV_LS);
        data->m_remote->add_event(send, ROLE_SEND_LS);

        cbmanager.notify(LATE_SENDER, recv, data);
      }

      m_max_idle = 0.0;
    
}

void PatternMPI_LateSender::post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 564 "MPI.pattern"

      RemoteEvent enter_send = data->m_remote->get_event(ROLE_ENTER_SEND_LS);
      Event       enter_recv = data->m_local->get_event(ROLE_ENTER_RECV_LS);
      Event       exit_recv  = data->m_local->get_event(ROLE_EXIT_RECV_LS);

      Region* region = enter_recv->get_region();

      if (is_mpi_testx(region))
        return;

      // Validate clock condition
      pearl::timestamp_t max_time = enter_send->get_time();
      if (max_time > exit_recv->get_time()) {
        // Do not report violation again -- already done in generic P2P code
        max_time = exit_recv->get_time();
      }

      // Calculate waiting time
      data->m_idle = max_time - enter_recv->get_time();
      if (data->m_idle > 0) {
        if (is_mpi_wait_multi(region)) {
          if (data->m_idle > m_max_idle) {
            RemoteEvent send = data->m_remote->get_event(ROLE_SEND_LS);

            m_receive.clear();
            m_send.clear();

            m_max_idle = data->m_idle;
            m_receive.add_event(event, ROLE_RECV);
            m_send.add_event(send, ROLE_SEND);
          }
        } else {
          m_severity[event.get_cnode()] += data->m_idle;

          cbmanager.notify(LATE_SENDER, event, data);
        }
      }
    
}

void PatternMPI_LateSender::pre_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 558 "MPI.pattern"

      data->m_local->add_event(event, ROLE_RECV_LS);
      data->m_local->add_event(event.enterptr(), ROLE_ENTER_RECV_LS);
      data->m_local->add_event(event.exitptr(), ROLE_EXIT_RECV_LS);
    
}

void PatternMPI_LateSender::pre_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 553 "MPI.pattern"

      data->m_local->add_event(event, ROLE_SEND_LS);
      data->m_local->add_event(event.enterptr(), ROLE_ENTER_SEND_LS);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateSenderWO
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateSenderWO : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternMPI_LateSenderWO::late_sender_cb));
      cbmanager->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_LateSenderWO::post_recv_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATESENDER_WO;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_LATESENDER;
    }

    virtual string get_name() const
    {
      return "Messages in Wrong Order";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latesender_wo";
    }

    virtual string get_descr() const
    {
      return "Late Sender situation due to messages received in the wrong order";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 651 "MPI.pattern"

    static const uint32_t BUFFERSIZE = 100;

    struct LateSender {
      LateSender(RemoteEvent send, Event recv, timestamp_t idle)
        : m_send(send), m_recv(recv), m_idle(idle) {}

      RemoteEvent m_send;
      Event       m_recv;
      timestamp_t m_idle;
    };
    typedef std::list<LateSender> LsBuffer;

    LsBuffer m_buffer;
  
};


/*----- Callback methods -----*/

void PatternMPI_LateSenderWO::late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 667 "MPI.pattern"

      // Construct entry
      LateSender item(data->m_remote->get_event(ROLE_SEND_LS),
                      event, data->m_idle);

      // Store entry in buffer
      if (m_buffer.size() == BUFFERSIZE)
        m_buffer.pop_front();
      m_buffer.push_back(item);
    
}

void PatternMPI_LateSenderWO::post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 678 "MPI.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND);

      // Search for "wrong order" situations
      LsBuffer::iterator it = m_buffer.begin();
      while (it != m_buffer.end()) {
        if (it->m_send->get_time() > send->get_time()) {
          double tmp = data->m_idle;

          data->m_idle = it->m_idle;
          m_severity[it->m_recv.get_cnode()] += data->m_idle;

          // Store data and notify specializations
          data->m_remote->add_event(it->m_send, ROLE_SEND_LSWO);
          data->m_local->add_event(it->m_recv, ROLE_RECV_LSWO);
          cbmanager.notify(LATE_SENDER_WO, event, data);

          it = m_buffer.erase(it);

          data->m_idle = tmp;
        } else  {
          ++it;
        }
      }
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LswoDifferent
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LswoDifferent : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_LswoDifferent::late_sender_wo_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LSWO_DIFFERENT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_LATESENDER_WO;
    }

    virtual string get_name() const
    {
      return "Messages from different sources";
    }

    virtual string get_unique_name() const
    {
      return "mpi_lswo_different";
    }

    virtual string get_descr() const
    {
      return "Wrong order situation due to messages received from different sources";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_LswoDifferent::late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 730 "MPI.pattern"

      Event recv = data->m_local->get_event(ROLE_RECV_LSWO);

      if (recv->get_source() != event->get_source())
        m_severity[recv.get_cnode()] += data->m_idle;
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LswoSame
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LswoSame : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_LswoSame::late_sender_wo_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LSWO_SAME;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_LATESENDER_WO;
    }

    virtual string get_name() const
    {
      return "Messages from same source";
    }

    virtual string get_unique_name() const
    {
      return "mpi_lswo_same";
    }

    virtual string get_descr() const
    {
      return "Wrong order situation due to messages received from the same source";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_LswoSame::late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 762 "MPI.pattern"

      Event recv = data->m_local->get_event(ROLE_RECV_LSWO);

      if (recv->get_source() == event->get_source())
        m_severity[recv.get_cnode()] += data->m_idle;
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateReceiver
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateReceiver : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(FINISHED, PEARL_create_callback(this, &PatternMPI_LateReceiver::finished_cb));
      cbmanager->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_LateReceiver::post_recv_cb));
      cbmanager->register_callback(PRE_RECV, PEARL_create_callback(this, &PatternMPI_LateReceiver::pre_recv_cb));
      cbmanager->register_callback(PRE_SEND, PEARL_create_callback(this, &PatternMPI_LateReceiver::pre_send_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATERECEIVER;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_POINT2POINT;
    }

    virtual string get_name() const
    {
      return "Late Receiver";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latereceiver";
    }

    virtual string get_descr() const
    {
      return "Time a sending process is waiting for the receiver to become ready";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void pre_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 803 "MPI.pattern"

    rem_sev_container_t m_remote_sev;
  
};


/*----- Callback methods -----*/

void PatternMPI_LateReceiver::finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 850 "MPI.pattern"

      exchange_severities(*(data->m_defs), m_remote_sev);
    
}

void PatternMPI_LateReceiver::post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 825 "MPI.pattern"

      RemoteEvent enter_sendcmp = data->m_remote->get_event(ROLE_ENTER_SEND_LR);
      RemoteEvent exit_sendcmp  = data->m_remote->get_event(ROLE_EXIT_SEND_LR);
      Event       enter_recvreq = data->m_local->get_event(ROLE_ENTER_RECV_LR);

      Region* region = enter_sendcmp->get_region();
      if (!(is_mpi_block_send(region) || is_mpi_wait_single(region)))
	return;

      // No overlap?
      if (exit_sendcmp->get_time() < enter_recvreq->get_time())
	return;

      // Calculate waiting time
      data->m_idle = enter_recvreq->get_time() - enter_sendcmp->get_time();
      if (data->m_idle > 0) {
        Location* loc     = enter_sendcmp.get_location();
        uint32_t  cnodeid = enter_sendcmp.get_cnode()->get_id();

        m_remote_sev[loc][cnodeid] += data->m_idle;

        cbmanager.notify(LATE_RECEIVER, event, data);
      }
    
}

void PatternMPI_LateReceiver::pre_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 816 "MPI.pattern"

      data->m_local->add_event(event, ROLE_RECV_LR);

      Event request = event.request();

      data->m_local->add_event(request.enterptr(), ROLE_ENTER_RECV_LR);
      data->m_local->add_event(request.exitptr(), ROLE_EXIT_RECV_LR);
    
}

void PatternMPI_LateReceiver::pre_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 807 "MPI.pattern"

      data->m_local->add_event(event, ROLE_SEND_LR);

      Event completion = event.completion();

      data->m_local->add_event(completion.enterptr(), ROLE_ENTER_SEND_LR);
      data->m_local->add_event(completion.exitptr(), ROLE_EXIT_SEND_LR);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_Collective
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_Collective : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_Collective::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COMMUNICATION;
    }

    virtual string get_name() const
    {
      return "Collective";
    }

    virtual string get_unique_name() const
    {
      return "mpi_collective";
    }

    virtual string get_descr() const
    {
      return "MPI collective communication";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_Collective::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 901 "MPI.pattern"

      Event   enter  = event.enterptr();
      Region* region = enter->get_region();

      // A single process will not wait for itself
      if (event->get_comm()->get_group()->num_ranks() < 2)
        return;

      data->m_local->add_event(event, ROLE_COLL_EXIT);
      data->m_local->add_event(enter, ROLE_COLL_ENTER);

      if (is_mpi_12n(region))
	cbmanager.notify(COLL_12N, event, data);
      else if (is_mpi_n21(region))
	cbmanager.notify(COLL_N21, event, data);
      else if (is_mpi_scan(region))
        cbmanager.notify(COLL_SCAN, event, data);
      else if (is_mpi_n2n(region)) {
        string name = lowercase(region->get_name());

        // Ignore MPI_Alltoallv and MPI_Alltoallw
        if (name == "mpi_alltoallv" || name == "mpi_alltoallw")
          return;

        // Set up timestamps to transfer:
        //   [0] - earliest EXIT for NxN completion calculation
        //         (w/o non-synchronizing processes)
        //   [1] - latest ENTER (w/o zero-sized transfers) for Wait at NxN
        //         calculation and CCV detection
        TimeVec2 local_times;
        local_times.value[0] = event->get_time();
        local_times.value[1] = event.enterptr()->get_time();
        if (event->get_sent() == 0)
          local_times.value[1] = -DBL_MAX;
        if (event->get_sent() == 0 || event->get_received() == 0)
          local_times.value[0] = DBL_MAX;

        // Retrieve latest ENTER (w/o zero-sized transfers) and
        // earliest EXIT (w/o non-synchronizing processes) event
        MpiComm* comm = event->get_comm();
        MPI_Allreduce(&local_times, &data->m_timevec, 1, TIMEVEC2,
                      MINMAX_TIMEVEC2, comm->get_comm());

        // Non-receiver processes do not have to wait
        if (event->get_received() == 0)
          data->m_timevec.value[1] = -DBL_MAX;

	cbmanager.notify(COLL_N2N, event, data);
      }
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_EarlyReduce
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_EarlyReduce : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(COLL_N21, PEARL_create_callback(this, &PatternMPI_EarlyReduce::coll_n21_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_EARLYREDUCE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "Early Reduce";
    }

    virtual string get_unique_name() const
    {
      return "mpi_earlyreduce";
    }

    virtual string get_descr() const
    {
      return "Waiting time due to an early receiver in MPI n-to-1 operations";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void coll_n21_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_EarlyReduce::coll_n21_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 975 "MPI.pattern"

      MpiComm* comm = event->get_comm();

      // Set up timestamps to transfer:
      //   [0] - earliest ENTER (w/o root and zero-sized transfers) for
      //         wait-state calculation
      //   [1] - latest ENTER (w/o zero-sized transfers) for CCV detection
      TimeVec2 local_times;
      local_times.value[0] = event.enterptr()->get_time();
      local_times.value[1] = event.enterptr()->get_time();
      if (event.get_location()->get_process()->get_id() ==
          comm->get_group()->get_global_rank(event->get_root()))
        local_times.value[0] = DBL_MAX;
      else if (event->get_sent() == 0) {
        local_times.value[0] = DBL_MAX;
        local_times.value[1] = -DBL_MAX;
      }

      // Retrieve earliest ENTER (w/o root and zero-sized transfers)
      // and latest ENTER (w/o zero-sized transfers)
      TimeVec2 reduced_times;
      MPI_Reduce(&local_times, &reduced_times, 1, TIMEVEC2, MINMAX_TIMEVEC2,
                 event->get_root(),
                 comm->get_comm());

      // Wait-state detection
      pearl::timestamp_t min_time = reduced_times.value[0];
      pearl::timestamp_t max_time = reduced_times.value[1];
      if ((event.get_location()->get_process()->get_id() ==
           comm->get_group()->get_global_rank(event->get_root())) &&
          (event->get_received() != 0)) {
        // Validate clock condition
        if (max_time > event->get_time()) {
          elg_cntl_msg("Unsynchronized clocks (loc: %d, reg: %s, diff: %es)!\n",
                       event.get_location()->get_id(),
                       event.get_cnode()->get_region()->get_name().c_str(),
                       max_time - event->get_time());
          cbmanager.notify(CCV_COLL, event, data);
        }

        // Restrict waiting time to time spent in operation
        if (min_time > event->get_time())
          min_time = event->get_time();

        // Calculate waiting time
	data->m_idle = min_time - event.enterptr()->get_time();
	if (data->m_idle > 0) {
	  m_severity[event.get_cnode()] += data->m_idle;

          // Early Reduce time only occurs on the root process, i.e.,
          // most-severe instance tracking can be performed locally
          cbmanager.notify(EARLY_REDUCE, event, data);
        }
      }
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_EarlyScan
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_EarlyScan : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(COLL_SCAN, PEARL_create_callback(this, &PatternMPI_EarlyScan::coll_scan_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_EARLYSCAN;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "Early Scan";
    }

    virtual string get_unique_name() const
    {
      return "mpi_earlyscan";
    }

    virtual string get_descr() const
    {
      return "Waiting time due to an early receiver in an MPI scan operation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void coll_scan_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_EarlyScan::coll_scan_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1051 "MPI.pattern"

      MpiComm* comm = event->get_comm();

      // Ignore zero-sized transfers
      if (event->get_sent() == 0 && event->get_received() == 0)
        return;

      // Retrieve latest ENTER event of ranks 0..n
      pearl::timestamp_t local_time = event.enterptr()->get_time();
      pearl::timestamp_t max_time;
      MPI_Scan(&local_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, comm->get_comm());

      // Validate clock condition
      if (max_time > event->get_time()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %d, reg: %s, diff: %es)!\n",
                     event.get_location()->get_id(),
                     event.get_cnode()->get_region()->get_name().c_str(),
                     max_time - event->get_time());
        cbmanager.notify(CCV_COLL, event, data);

        // Restrict waiting time to time spent in operation
        max_time = event->get_time();
      }

      // Calculate waiting time
      data->m_idle = max_time - local_time;
      if (data->m_idle > 0)
        m_severity[event.get_cnode()] += data->m_idle;

      // All processes need to take part in the most-severe instance tracking
      cbmanager.notify(EARLY_SCAN, event, data);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_LateBroadcast
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_LateBroadcast : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(COLL_12N, PEARL_create_callback(this, &PatternMPI_LateBroadcast::coll_12n_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_LATEBROADCAST;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "Late Broadcast";
    }

    virtual string get_unique_name() const
    {
      return "mpi_latebroadcast";
    }

    virtual string get_descr() const
    {
      return "Waiting time due to a late sender in MPI 1-to-n operations";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void coll_12n_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_LateBroadcast::coll_12n_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1107 "MPI.pattern"

      MpiComm* comm = event->get_comm();

      // Broadcast timestamp of root's ENTER event
      pearl::timestamp_t root_time = event.enterptr()->get_time();
      MPI_Bcast(&root_time, 1, MPI_DOUBLE,
                event->get_root(),
                comm->get_comm());

      // Wait-state detection (w/o root and zero-sized transfers)
      if ((event.get_location()->get_process()->get_id() !=
           comm->get_group()->get_global_rank(event->get_root())) &&
          (event->get_received() != 0)) {
        // Validate clock condition
        if (root_time > event->get_time()) {
          elg_cntl_msg("Unsynchronized clocks (loc: %d, reg: %s, diff: %es)!\n",
                       event.get_location()->get_id(),
                       event.get_cnode()->get_region()->get_name().c_str(),
                       root_time - event->get_time());
          cbmanager.notify(CCV_COLL, event, data);

          // Restrict waiting time to time spent in operation
          root_time = event->get_time();
        }

        // Calculate waiting time
        data->m_idle = root_time - event.enterptr()->get_time();
        if (data->m_idle > 0)
          m_severity[event.get_cnode()] += data->m_idle;
        cbmanager.notify(LATE_BCAST, event, data);
      } else {
        // All processes need to take part in the most-severe instance tracking
        data->m_idle = 0.0;
        if ((event.get_location()->get_process()->get_id() == comm->get_group()->get_global_rank(event->get_root())) &&
           (event->get_received() != 0))
          cbmanager.notify(LATE_BCAST, event, data);
      }
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_WaitNxN
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_WaitNxN : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(COLL_N2N, PEARL_create_callback(this, &PatternMPI_WaitNxN::coll_n2n_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_WAIT_NXN;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "Wait at N x N";
    }

    virtual string get_unique_name() const
    {
      return "mpi_wait_nxn";
    }

    virtual string get_descr() const
    {
      return "Time due to inherent synchronization in MPI n-to-n operations";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void coll_n2n_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_WaitNxN::coll_n2n_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1175 "MPI.pattern"

      pearl::timestamp_t max_time = data->m_timevec.value[1];

      // Validate clock condition
      if (max_time > event->get_time()) {
        elg_cntl_msg("Unsynchronized clocks (loc: %d, reg: %s, diff: %es)!\n",
                     event.get_location()->get_id(),
                     event.get_cnode()->get_region()->get_name().c_str(),
                     max_time - event->get_time());
        cbmanager.notify(CCV_COLL, event, data);

        // Restrict waiting time to time spent in operation
        max_time = event->get_time();
      }

      // Calculate waiting time
      data->m_idle = max_time - event.enterptr()->get_time();
      if (data->m_idle > 0)
        m_severity[event.get_cnode()] += data->m_idle;

      // There will always be waiting time at NxN collectives; all processes
      // need to take part in most-severe instance tracking
      cbmanager.notify(WAIT_NXN, event, data);
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_NxNCompletion
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_NxNCompletion : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(COLL_N2N, PEARL_create_callback(this, &PatternMPI_NxNCompletion::coll_n2n_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_NXN_COMPLETION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_COLLECTIVE;
    }

    virtual string get_name() const
    {
      return "N x N Completion";
    }

    virtual string get_unique_name() const
    {
      return "mpi_nxn_completion";
    }

    virtual string get_descr() const
    {
      return "Time needed to finish a n-to-n collective operation";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void coll_n2n_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_NxNCompletion::coll_n2n_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1223 "MPI.pattern"

      pearl::timestamp_t min_time = data->m_timevec.value[0];
      pearl::timestamp_t max_time = data->m_timevec.value[1];

      // Only an N'xN' synchronization pattern can have a completion time
      if (event->get_sent() == 0 || event->get_received() == 0)
        return;

      // Validate clock condition
      if (min_time < max_time) {
        // Do not report violation again -- already done by "Wait at NxN"
        // Restrict waiting time to time spent in operation
        min_time = max_time;
      }

      // Calculate waiting time
      data->m_idle = event->get_time() - min_time;
      if (data->m_idle > 0)
        m_severity[event.get_cnode()] += data->m_idle;

      // There will always be completion time at NxN collectives; all processes
      // need to take part in most-severe instance tracking
      cbmanager.notify(NXN_COMPL, event, data);
    
}

#endif   /* _MPI */


#if defined(_OPENMP)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Mgmt
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Mgmt : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(ENTER, PEARL_create_callback(this, &PatternOMP_Mgmt::enter_cb));
      cbmanager->register_callback(EXIT, PEARL_create_callback(this, &PatternOMP_Mgmt::exit_cb));
      cbmanager->register_callback(OMP_FORK, PEARL_create_callback(this, &PatternOMP_Mgmt::omp_fork_cb));
      cbmanager->register_callback(OMP_JOIN, PEARL_create_callback(this, &PatternOMP_Mgmt::omp_join_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_MANAGEMENT;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_TIME;
    }

    virtual string get_name() const
    {
      return "Management";
    }

    virtual string get_unique_name() const
    {
      return "omp_management";
    }

    virtual string get_descr() const
    {
      return "Time spent in OpenMP thread management";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void enter_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void exit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_fork_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_join_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Mgmt::enter_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 157 "OMP.pattern"

      // Are we entering an OpenMP parallel region?
      Region* region = event->get_region();
      if (!is_omp_parallel(region))
        return;

      // Determine timestamp of FORK event
      static timestamp_t forkTime;
      #pragma omp master
      {
        Event fork = event.prev();
        while (fork->get_type() != OMP_FORK)
          --fork;
        forkTime = fork->get_time();
      }

      // Calculate thread fork time
      #pragma omp barrier
      data->m_idle = event->get_time() - forkTime;
      if (data->m_idle > 0) {
        m_severity[event.get_cnode()] += data->m_idle;

        cbmanager.notify(OMP_MGMT_FORK, event, data);
      }
      #pragma omp barrier
    
}

void PatternOMP_Mgmt::exit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 184 "OMP.pattern"

      // Are we leaving an OpenMP parallel region?
      Event   enter  = data->m_callstack.top();
      Region* region = enter->get_region();
      if (!is_omp_parallel(region))
        return;

      // Determine timestamp of JOIN event
      static timestamp_t joinTime;
      #pragma omp master
      {
        Event join = event.next();
        while (join->get_type() != OMP_JOIN)
          ++join;

        joinTime = join->get_time();
      }

      // Calculate thread join time
      #pragma omp barrier
      data->m_idle = joinTime - event->get_time();
      if (data->m_idle > 0) {
        m_severity[enter.get_cnode()] += data->m_idle;

        cbmanager.notify(OMP_MGMT_JOIN, event, data);
      }
      #pragma omp barrier
    
}

void PatternOMP_Mgmt::omp_fork_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 147 "OMP.pattern"

      // Increment nesting level
      omp_nest_level++;
    
}

void PatternOMP_Mgmt::omp_join_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 152 "OMP.pattern"

      // Decrement nesting level
      omp_nest_level--;
    
}

#endif   /* _OPENMP */


#if defined(_OPENMP)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Mgmt_Fork
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Mgmt_Fork : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(OMP_MGMT_FORK, PEARL_create_callback(this, &PatternOMP_Mgmt_Fork::omp_mgmt_fork_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_FORK;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_MANAGEMENT;
    }

    virtual string get_name() const
    {
      return "Fork";
    }

    virtual string get_unique_name() const
    {
      return "omp_fork";
    }

    virtual string get_descr() const
    {
      return "Time spent in OpenMP thread forking";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void omp_mgmt_fork_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Mgmt_Fork::omp_mgmt_fork_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 230 "OMP.pattern"

      m_severity[event.get_cnode()] += data->m_idle;
    
}

#endif   /* _OPENMP */


#if defined(_OPENMP)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Sync_Barrier
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Sync_Barrier : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(OMP_COLLEXIT, PEARL_create_callback(this, &PatternOMP_Sync_Barrier::omp_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_BARRIER;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "Synchronization";
    }

    virtual string get_unique_name() const
    {
      return "omp_barrier";
    }

    virtual string get_descr() const
    {
      return "Time spent in OpenMP barrier synchronization";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void omp_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Sync_Barrier::omp_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 275 "OMP.pattern"

      Event   enter  = data->m_callstack.top();
      Region* region = enter->get_region();

      // Are we leaving an implicit or explicit OpenMP barrier?
      if (is_omp_ebarrier(region))
        cbmanager.notify(OMP_EBARRIER, event, data);
      else if (is_omp_ibarrier(region))
        cbmanager.notify(OMP_IBARRIER, event, data);
    
}

#endif   /* _OPENMP */


#if defined(_OPENMP)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Sync_Ebarrier_Wait
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Sync_Ebarrier_Wait : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(OMP_EBARRIER, PEARL_create_callback(this, &PatternOMP_Sync_Ebarrier_Wait::omp_ebarrier_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_EBARRIER_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_EBARRIER;
    }

    virtual string get_name() const
    {
      return "Wait at Barrier";
    }

    virtual string get_unique_name() const
    {
      return "omp_ebarrier_wait";
    }

    virtual string get_descr() const
    {
      return "Waiting time in front of explicit OpenMP barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void omp_ebarrier_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Sync_Ebarrier_Wait::omp_ebarrier_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 334 "OMP.pattern"

      // Skip OpenMP barrier waiting time calculation when running serially
      // (the barriers called in the algorithm cause ill side effects...)
      if (0 == omp_nest_level)
        return;

      Event       enter     = data->m_callstack.top();
      timestamp_t enterTime = enter->get_time();

      // Determine latest enter time
      // Implicitly shared timestamp for reduction
      static timestamp_t latestEnter;
      #pragma omp barrier
      #pragma omp master
      latestEnter = 0;
      #pragma omp barrier
      #pragma omp critical
      {
        if(latestEnter < enterTime)
          latestEnter = enterTime;
      }
      #pragma omp barrier
      
      data->m_idle = latestEnter - enterTime;
      if(data->m_idle > 0)
        m_severity[enter.get_cnode()] += data->m_idle;

      // There will always be waiting time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(OMP_EBARRIER_WAIT, event, data);
    
}

#endif   /* _OPENMP */


#if defined(_OPENMP)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternOMP_Sync_Ibarrier_Wait
 *
 *---------------------------------------------------------------------------
 */


class PatternOMP_Sync_Ibarrier_Wait : public OmpPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(OMP_IBARRIER, PEARL_create_callback(this, &PatternOMP_Sync_Ibarrier_Wait::omp_ibarrier_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_OMP_IBARRIER_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_OMP_IBARRIER;
    }

    virtual string get_name() const
    {
      return "Wait at Barrier";
    }

    virtual string get_unique_name() const
    {
      return "omp_ibarrier_wait";
    }

    virtual string get_descr() const
    {
      return "Waiting time in front of implicit OpenMP barriers";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void omp_ibarrier_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternOMP_Sync_Ibarrier_Wait::omp_ibarrier_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 414 "OMP.pattern"

      // Skip OpenMP barrier waiting time calculation when running serially
      // (the barriers called in the algorithm cause ill side effects...)
      if (0 == omp_nest_level)
        return;

      Event       enter     = data->m_callstack.top();
      timestamp_t enterTime = enter->get_time();

      // Determine latest enter time
      // Implicitly shared timestamp for reduction
      static timestamp_t latestEnter;
      #pragma omp barrier
      #pragma omp master
      latestEnter = 0;
      #pragma omp barrier
      #pragma omp critical
      {
        if(latestEnter < enterTime)
          latestEnter = enterTime;
      }
      #pragma omp barrier
      
      data->m_idle = latestEnter - enterTime;
      if(data->m_idle > 0)
        m_severity[enter.get_cnode()] += data->m_idle;

      // There will always be waiting time at barriers; all processes need
      // to take part in most-severe instance tracking
      cbmanager.notify(OMP_IBARRIER_WAIT, event, data);
    
}

#endif   /* _OPENMP */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SyncsSend
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SyncsSend : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_SEND, PEARL_create_callback(this, &PatternMPI_SyncsSend::mpi_send_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_SYNCS_SEND;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_P2P;
    }

    virtual string get_name() const
    {
      return "P2P send synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "syncs_send";
    }

    virtual string get_descr() const
    {
      return "Number of point-to-point send synchronization operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SyncsSend::mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 64 "Message.pattern"

      if (event->get_sent() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SyncsRecv
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SyncsRecv : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_SyncsRecv::post_recv_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_SYNCS_RECV;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_P2P;
    }

    virtual string get_name() const
    {
      return "P2P recv synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "syncs_recv";
    }

    virtual string get_descr() const
    {
      return "Number of point-to-point receive synchronization operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SyncsRecv::post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 85 "Message.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND);

      if (send->get_sent() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SyncsColl
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SyncsColl : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_SyncsColl::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_SYNCS_COLL;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS;
    }

    virtual string get_name() const
    {
      return "Collective synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "syncs_coll";
    }

    virtual string get_descr() const
    {
      return "Number of collective synchronization operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SyncsColl::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 119 "Message.pattern"

      if (event->get_sent() == 0 && event->get_received() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsSend
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsSend : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_SEND, PEARL_create_callback(this, &PatternMPI_CommsSend::mpi_send_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_SEND;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_P2P;
    }

    virtual string get_name() const
    {
      return "P2P send communications";
    }

    virtual string get_unique_name() const
    {
      return "comms_send";
    }

    virtual string get_descr() const
    {
      return "Number of point-to-point send communication operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsSend::mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 170 "Message.pattern"

      if (event->get_sent() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsRecv
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsRecv : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_CommsRecv::post_recv_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_RECV;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_P2P;
    }

    virtual string get_name() const
    {
      return "P2P recv communications";
    }

    virtual string get_unique_name() const
    {
      return "comms_recv";
    }

    virtual string get_descr() const
    {
      return "Number of point-to-point receive communication operations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsRecv::post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 191 "Message.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND);

      if (send->get_sent() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsExch
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsExch : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_CommsExch::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_CXCH;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_COLL;
    }

    virtual string get_name() const
    {
      return "Collective exchange communications";
    }

    virtual string get_unique_name() const
    {
      return "comms_cxch";
    }

    virtual string get_descr() const
    {
      return "Number of collective communications as source and destination";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsExch::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 238 "Message.pattern"

      if (event->get_sent() != 0 && event->get_received() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsSrc
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsSrc : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_CommsSrc::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_CSRC;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_COLL;
    }

    virtual string get_name() const
    {
      return "Collective communications as source";
    }

    virtual string get_unique_name() const
    {
      return "comms_csrc";
    }

    virtual string get_descr() const
    {
      return "Number of collective communications as source";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsSrc::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 260 "Message.pattern"

      if (event->get_sent() != 0 && event->get_received() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CommsDst
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CommsDst : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_CommsDst::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_COMMS_CDST;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_COLL;
    }

    virtual string get_name() const
    {
      return "Collective communications as destination";
    }

    virtual string get_unique_name() const
    {
      return "comms_cdst";
    }

    virtual string get_descr() const
    {
      return "Number of collective communications as destination";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CommsDst::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 282 "Message.pattern"

      if (event->get_sent() == 0 && event->get_received() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesSent
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesSent : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_SEND, PEARL_create_callback(this, &PatternMPI_BytesSent::mpi_send_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_SENT;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_P2P;
    }

    virtual string get_name() const
    {
      return "P2P bytes sent";
    }

    virtual string get_unique_name() const
    {
      return "bytes_sent";
    }

    virtual string get_descr() const
    {
      return "Number of bytes sent in point-to-point operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    /* Callback methods */
    void mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesSent::mpi_send_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 383 "Message.pattern"

      if (event->get_sent() > 0)
        m_severity[event.get_cnode()] += event->get_sent();
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesRcvd
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesRcvd : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(POST_RECV, PEARL_create_callback(this, &PatternMPI_BytesRcvd::post_recv_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_RCVD;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_P2P;
    }

    virtual string get_name() const
    {
      return "P2P bytes received";
    }

    virtual string get_unique_name() const
    {
      return "bytes_rcvd";
    }

    virtual string get_descr() const
    {
      return "Number of bytes received in point-to-point operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    /* Callback methods */
    void post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesRcvd::post_recv_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 422 "Message.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND);

      if (send->get_sent() > 0)
        m_severity[event.get_cnode()] += send->get_sent();
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesCout
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesCout : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_BytesCout::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_COUT;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_COLL;
    }

    virtual string get_name() const
    {
      return "Collective bytes outgoing";
    }

    virtual string get_unique_name() const
    {
      return "bytes_cout";
    }

    virtual string get_descr() const
    {
      return "Number of bytes outgoing in collective operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesCout::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 478 "Message.pattern"

      if (event->get_sent() > 0)
        m_severity[event.get_cnode()] += event->get_sent();
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesCin
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesCin : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternMPI_BytesCin::mpi_collexit_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_CIN;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_COLL;
    }

    virtual string get_name() const
    {
      return "Collective bytes incoming";
    }

    virtual string get_unique_name() const
    {
      return "bytes_cin";
    }

    virtual string get_descr() const
    {
      return "Number of bytes incoming in collective operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    /* Callback methods */
    void mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesCin::mpi_collexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 504 "Message.pattern"

      if (event->get_received() > 0)
        m_severity[event.get_cnode()] += event->get_received();
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesGet
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesGet : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_GET_START, PEARL_create_callback(this, &PatternMPI_BytesGet::mpi_rma_get_start_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_GET;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_RMA;
    }

    virtual string get_name() const
    {
      return "RMA bytes received";
    }

    virtual string get_unique_name() const
    {
      return "bytes_get";
    }

    virtual string get_descr() const
    {
      return "Number of bytes received in RMA operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    /* Callback methods */
    void mpi_rma_get_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesGet::mpi_rma_get_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 541 "Message.pattern"

      m_severity[event.get_cnode()] += event->get_received();
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_BytesPut
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_BytesPut : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_PUT_START, PEARL_create_callback(this, &PatternMPI_BytesPut::mpi_rma_put_start_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_BYTES_PUT;
    }

    virtual long get_parent() const
    {
      return PAT_BYTES_RMA;
    }

    virtual string get_name() const
    {
      return "RMA bytes put";
    }

    virtual string get_unique_name() const
    {
      return "bytes_put";
    }

    virtual string get_descr() const
    {
      return "Number of bytes sent in RMA operations";
    }

    virtual string get_unit() const
    {
      return "bytes";
    }

    /* Callback methods */
    void mpi_rma_put_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_BytesPut::mpi_rma_put_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 561 "Message.pattern"

      m_severity[event.get_cnode()] += event->get_sent();
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CLS_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CLS_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternMPI_CLS_Count::late_sender_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_CLS_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_RECV;
    }

    virtual string get_name() const
    {
      return "Late Senders";
    }

    virtual string get_unique_name() const
    {
      return "mpi_cls_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in communications";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CLS_Count::late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 25 "Counts.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND_LS);

      if (send->get_sent() != 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CLSWO_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CLSWO_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_CLSWO_Count::late_sender_wo_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_CLSWO_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_CLS_COUNT;
    }

    virtual string get_name() const
    {
      return "Messages in Wrong Order";
    }

    virtual string get_unique_name() const
    {
      return "mpi_clswo_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in communications were messages are received in wrong order";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_CLSWO_Count::late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 49 "Counts.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND_LSWO);
      Event       recv = data->m_local->get_event(ROLE_RECV_LSWO);

      if (send->get_sent() != 0)
        ++m_severity[recv.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_CLR_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_CLR_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(FINISHED, PEARL_create_callback(this, &PatternMPI_CLR_Count::finished_cb));
      cbmanager->register_callback(LATE_RECEIVER, PEARL_create_callback(this, &PatternMPI_CLR_Count::late_receiver_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_CLR_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_COMMS_SEND;
    }

    virtual string get_name() const
    {
      return "Late Receivers";
    }

    virtual string get_unique_name() const
    {
      return "mpi_clr_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Receiver instances in communications";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_receiver_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 73 "Counts.pattern"

    rem_sev_container_t m_remote_sev;
  
};


/*----- Callback methods -----*/

void PatternMPI_CLR_Count::finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 84 "Counts.pattern"

      exchange_severities(*(data->m_defs), m_remote_sev);
    
}

void PatternMPI_CLR_Count::late_receiver_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 77 "Counts.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND_LR);

      if (send->get_sent() != 0)
        ++m_remote_sev[send.get_location()][send.get_cnode()->get_id()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SLS_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SLS_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternMPI_SLS_Count::late_sender_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_SLS_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_RECV;
    }

    virtual string get_name() const
    {
      return "Late Senders";
    }

    virtual string get_unique_name() const
    {
      return "mpi_sls_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SLS_Count::late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 105 "Counts.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND_LS);

      if (send->get_sent() == 0)
        ++m_severity[event.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SLSWO_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SLSWO_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternMPI_SLSWO_Count::late_sender_wo_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_SLSWO_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SLS_COUNT;
    }

    virtual string get_name() const
    {
      return "Messages in Wrong Order";
    }

    virtual string get_unique_name() const
    {
      return "mpi_slswo_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Sender instances in synchronizations were messages are received in wrong order";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_SLSWO_Count::late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 130 "Counts.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND_LSWO);
      Event       recv = data->m_local->get_event(ROLE_RECV_LSWO);

      if (send->get_sent() == 0)
        ++m_severity[recv.get_cnode()];
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_SLR_Count
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_SLR_Count : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(FINISHED, PEARL_create_callback(this, &PatternMPI_SLR_Count::finished_cb));
      cbmanager->register_callback(LATE_RECEIVER, PEARL_create_callback(this, &PatternMPI_SLR_Count::late_receiver_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_SLR_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_SYNCS_SEND;
    }

    virtual string get_name() const
    {
      return "Late Receivers";
    }

    virtual string get_unique_name() const
    {
      return "mpi_slr_count";
    }

    virtual string get_descr() const
    {
      return "Number of Late Receiver instances in synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_receiver_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 154 "Counts.pattern"

    rem_sev_container_t m_remote_sev;
  
};


/*----- Callback methods -----*/

void PatternMPI_SLR_Count::finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 165 "Counts.pattern"

      exchange_severities(*(data->m_defs), m_remote_sev);
    
}

void PatternMPI_SLR_Count::late_receiver_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 158 "Counts.pattern"

      RemoteEvent send = data->m_remote->get_event(ROLE_SEND_LR);

      if (send->get_sent() == 0)
        ++m_remote_sev[send.get_location()][send.get_cnode()->get_id()];
    
}

#endif   /* _MPI */


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaSync
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaSync : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(FINISHED, PEARL_create_callback(this, &PatternMPI_RmaSync::finished_cb));
      cbmanager->register_callback(MPI_RMACOLLEXIT, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rmacollexit_cb));
      cbmanager->register_callback(MPI_RMAEXIT, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rmaexit_cb));
      cbmanager->register_callback(MPI_RMA_GET_END, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_get_end_cb));
      cbmanager->register_callback(MPI_RMA_GET_START, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_get_start_cb));
      cbmanager->register_callback(MPI_RMA_POST_COMPLETE, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_post_complete_cb));
      cbmanager->register_callback(MPI_RMA_PUT_END, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_put_end_cb));
      cbmanager->register_callback(MPI_RMA_PUT_LATEST_OP, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_put_latest_op_cb));
      cbmanager->register_callback(MPI_RMA_PUT_START, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_put_start_cb));
      cbmanager->register_callback(MPI_RMA_SET_REMOTE_BIT, PEARL_create_callback(this, &PatternMPI_RmaSync::mpi_rma_set_remote_bit_cb));
      cbmanager->register_callback(PREPARE, PEARL_create_callback(this, &PatternMPI_RmaSync::prepare_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_SYNCHRONIZATION;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "Remote Memory Access";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_synchronization";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI RMA synchonization calls";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rmacollexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rmaexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_get_end_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_get_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_post_complete_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_put_end_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_put_latest_op_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_put_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void mpi_rma_set_remote_bit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void prepare_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 51 "RMA.pattern"

    /**
     * @brief  Lookup table for bit count. 
     *
     * The lookup table includes the number of bits set in each possible
     * value of a byte. It could be algorithmically initialised with:
     *
     * @code
     * BitSetTable[0] = 0;
     * for (int i = 0; i < 256; i++)
     * {
     *   BitSetTable[i] = (i & 1) + BitSetTable[i / 2];
     * }
     * @endcode
     */
    unsigned char BitSetTable[256];
    
    typedef struct {
      timestamp_t op;
      timestamp_t post;
    } op_pair_t;

    typedef struct {
      MPI_Win                  win;               /* window for data exchange */
      uint32_t                 epoch_ctr;         /* counter for current epoch */
      bool                     pending_complete;  /* indicator whether complete needs to be done later */
      unsigned char*           buf;               /* buffer for data exchange */
      map<uint32_t, op_pair_t> op_time;           /* remote rma operation timestamp */
      timestamp_t              post_enter;        /* enter timestamp of the post call */
      timestamp_t              complete_enter;    /* enter timestamp of the complete call */
      EventSet                 events;            /* local event set for exchanging data */
      int                      bitfield_size;     /* size of the bitfield in the buffer */
      set<uint32_t>            remote_processes;  /* list of targets in an epoch */
      MPI_Aint                 rank_offset;       /* byte offset in window for my rank_mask */
      unsigned char            rank_mask;         /* mask with 1 bit set corresponding to my rank */
    } win_t;

    map<MpiWindow*, win_t> m_windows;             /* map of all windows used */
  
};


/*----- Callback methods -----*/

void PatternMPI_RmaSync::finished_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 143 "RMA.pattern"

      /* free all windows */
      uint32_t num_windows = data->m_defs->num_windows();
      for (uint32_t i = 0; i < num_windows; ++i)
      {
        MpiWindow* win = dynamic_cast<MpiWindow*>(data->m_defs->get_window(i));

        map<MpiWindow*, win_t>::iterator it = m_windows.find(win);
        if (it != m_windows.end())
        {
            MPI_Win_free(&(it->second.win));
            delete[] it->second.buf;
        }
      }
    
}

void PatternMPI_RmaSync::mpi_rmacollexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 383 "RMA.pattern"

      /* TODO: Possible assertions may lead to early exits here. As we
       *       currently do not trace the assertion itself, the first
       *       heuristic will be, to check whether any early exits
       *       occur before later enters, if not, it is assumed that 
       *       MPI_MODE_NOPRECEEDE was NOT given.
       */
      /* NOTE: future heuristic may be to check whether NO RMA calls
       *       occured in the last epoch
       */
      Event      enter  = event.enterptr();
      Region*    region = enter->get_region();

      if (is_mpi_rma_fence(region))
      {
        MpiWindow*  win           = event->get_window();
        MPI_Comm    comm          = dynamic_cast<MpiComm*>(win->get_comm())->get_comm();
        win_t&      entry         = m_windows[win];
        timestamp_t local_enter   = enter->get_time();
        timestamp_t latest_enter  = -DBL_MAX;
        timestamp_t earliest_exit =  DBL_MAX;
        timestamp_t latest_rma_op = -DBL_MAX;
        int         myrank        = -1;
        int         comm_size     = 0;
        TimeVec2    local_times;

        local_times.value[1] = enter->get_time();
        local_times.value[0] = event->get_time();

        MPI_Allreduce(&local_times, &data->m_timevec, 1, TIMEVEC2,
                      MINMAX_TIMEVEC2, comm);

        latest_enter  = data->m_timevec.value[1];
        earliest_exit = data->m_timevec.value[0];
        
        if ((earliest_exit > latest_enter) &&
            (local_enter   < latest_enter))
        {
          data->m_idle = latest_enter - local_enter;
          cbmanager.notify(MPI_RMA_WAIT_AT_FENCE, event, data);
        }
        
        /* clear bitfield */
        memset(entry.buf + BITFIELD_START, 0, entry.bitfield_size);

        /* get latest rma op to this location */
        MPI_Win_fence(MPI_MODE_NOPRECEDE, entry.win);
        cbmanager.notify(MPI_RMA_SET_REMOTE_BIT, event, data);
        cbmanager.notify(MPI_RMA_PUT_LATEST_OP, event, data);
        MPI_Win_fence(MPI_MODE_NOPUT | MPI_MODE_NOSUCCEED | MPI_MODE_NOSTORE, entry.win);

        map<uint32_t, op_pair_t>::iterator it =
            entry.op_time.begin();
        while (it != entry.op_time.end())
        {
          if (it->second.op > latest_rma_op)
          {
            latest_rma_op = it->second.op;
          }
          ++it;
        }

        if (latest_rma_op > local_enter)
        {
          data->m_idle = latest_rma_op - local_enter;
          cbmanager.notify(MPI_RMA_WAIT_AT_FENCE, event, data);
          cbmanager.notify(MPI_RMA_EARLY_FENCE, event, data);
        }

        /* analyze pairwise sync pattern */
        MPI_Comm_rank(comm, &myrank);
        MPI_Comm_size(comm, &comm_size);

        data->m_count = 2 * comm_size;
        cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

        /* calculate unneeded synchronizations as origin */
        data->m_count = 2 * comm_size - entry.remote_processes.size();
        entry.remote_processes.clear();

        /* calculate unneeded synchronizations as target */
        for (int i = BITFIELD_START; i < BITFIELD_START + entry.bitfield_size; ++i)
        {
          data->m_count -= BitSetTable[entry.buf[i] & 0xff];
        }
        
        cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);
      }
    
}

void PatternMPI_RmaSync::mpi_rmaexit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 190 "RMA.pattern"

      Event        enter     = event.enterptr();
      MpiWindow*   win       = event->get_window();
      Region*      region    = enter->get_region();
      MpiGroup*    group     = event->get_group();
      MpiComm*     comm      = dynamic_cast<MpiComm*>(event->get_window()->get_comm());
      win_t&       entry     = m_windows[win];
      int          myrank    = -1;
      timestamp_t* timestamp = reinterpret_cast<timestamp_t*>(entry.buf);


      /* analyze paiwise sync pattern */
      MPI_Comm_rank(comm->get_comm(), &myrank);

      if (is_mpi_rma_post(region))
      {
        /* initialise window buffer */
        timestamp[LATEST_COMPLETE / sizeof (timestamp_t)] = -DBL_MAX;
        timestamp[LATEST_RMA_OP   / sizeof (timestamp_t)] = -DBL_MAX;
        timestamp[LATEST_POST     / sizeof (timestamp_t)] = -DBL_MAX;
        timestamp[LOCAL_POST      / sizeof (timestamp_t)] = enter->get_time();

        /* clear bitfield */
        memset(entry.buf + BITFIELD_START, 0, entry.bitfield_size);

        /* start exposure epoch to exchange timestamps */
        MPI_Win_post(group->get_group(), 0, entry.win);
      }
      else if (is_mpi_rma_wait(region))
      {
        int group_size;
        
        MPI_Win_wait(entry.win);

        /* analyze pairwise sync pattern */
        MPI_Group_size(group->get_group(), &group_size);
        data->m_count = group_size;
        cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);
        
        data->m_count = group_size;
        for (int i = BITFIELD_START; i < BITFIELD_START + entry.bitfield_size; ++i)
        {
          data->m_count -= BitSetTable[entry.buf[i] & 0xff];
        }
        cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);

        /* analyse other patterns */
        data->m_tmptime  = timestamp[LATEST_COMPLETE / sizeof(timestamp_t)];
        data->m_tmptime2 = timestamp[LATEST_RMA_OP   / sizeof(timestamp_t)];

        cbmanager.notify(MPI_RMA_POST_WAIT, event, data);
      }
      else if (is_mpi_rma_start(region))
      {
        entry.events.add_event(enter, ROLE_RMA_START_ENTER);
        entry.events.add_event(event, ROLE_RMA_START_EXIT);

        /* start access epoch to exchange timestamps */
        MPI_Win_start(group->get_group(), 0, entry.win);

        /* get post enter-time from each exposure epoch */
        for (uint32_t i=0; i < group->num_ranks(); ++i)
        {
          /* create entry for location */
          op_pair_t& op_pair = 
              entry.op_time[group->get_global_rank(i)];
          /* 
           * initializing op time with start timestamp. This is needed
           * to have a sane value, in case no RMA op follows in this
           * access epoch.
           */
          op_pair.op = event->get_time();

          MPI_Get(&(op_pair.post), 1, MPI_DOUBLE,
                  comm->get_group()->get_local_rank(group->get_global_rank(i)), LOCAL_POST,
                  1, MPI_DOUBLE, entry.win);
        }
      }
      else if (is_mpi_rma_complete(region))
      {
        timestamp_t max_post_time = -DBL_MAX;
        int         group_size    = 0;

        cbmanager.notify(MPI_RMA_SET_REMOTE_BIT, event, data);

        cbmanager.notify(MPI_RMA_PUT_LATEST_OP, event, data);

        /* send complete timestamp to remote processes */
        entry.complete_enter = enter->get_time();
        for (uint32_t i=0; i < group->num_ranks(); ++i)
        {
          MPI_Accumulate(&entry.complete_enter, 1, MPI_DOUBLE, 
                         comm->get_group()->get_local_rank(group->get_global_rank(i)),
                         LATEST_COMPLETE, 1, MPI_DOUBLE, MPI_MAX,
                         entry.win);
        }

        /* complete access epoch on window */
        MPI_Win_complete(entry.win);

        /* search maximum post timestamp */
        map<uint32_t, op_pair_t>::iterator it = 
            entry.op_time.begin();
        while (it != entry.op_time.end())
        {
          max_post_time = max(max_post_time, it->second.post);
          ++it;
        }
            
        data->m_tmptime = max_post_time;
        *data->m_local  = entry.events;
       
        cbmanager.notify(MPI_RMA_POST_COMPLETE, event, data);

        /* clear map of latest op times */
        entry.op_time.clear();

        /* analyze paiwise sync pattern */
        MPI_Comm_rank(comm->get_comm(), &myrank);
        
        MPI_Group_size(group->get_group(), &group_size);
        data->m_count = group_size;
        cbmanager.notify(MPI_RMA_PWS_COUNT, event, data);

        data->m_count = group_size - entry.remote_processes.size();
        cbmanager.notify(MPI_RMA_PWS_UNNEEDED_COUNT, event, data);
        entry.remote_processes.clear();
      }
    
}

void PatternMPI_RmaSync::mpi_rma_get_end_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 181 "RMA.pattern"

      /* TODO: the use of prev() is not save in the presence of FLUSH events */
      MpiWindow* win          = event.prev()->get_window();
      MpiComm*   comm         = dynamic_cast<MpiComm*>(win->get_comm());
      uint32_t   m_remote_loc = event.prev()->get_remote();
      win_t&     entry        = m_windows[win];

      entry.op_time[comm->get_group()->get_global_rank(m_remote_loc)].op = event->get_time();
    
}

void PatternMPI_RmaSync::mpi_rma_get_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 165 "RMA.pattern"

      MpiWindow* win          = event->get_window();
      uint32_t   m_remote_loc = event->get_remote();
      win_t&     entry        = m_windows[win];
      
      entry.remote_processes.insert(m_remote_loc);
    
}

void PatternMPI_RmaSync::mpi_rma_post_complete_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 319 "RMA.pattern"

      MpiWindow* win         = event->get_window();
      Event      evt_it      = event.prev().prev();
      bool       start_found = false;
      win_t&     entry       = m_windows[win];
      
      while (!start_found)
      {
        if (((evt_it->get_type() == MPI_RMA_PUT_START) ||
             (evt_it->get_type() == MPI_RMA_GET_START)) &&
            (evt_it->get_window() == win))
        {
          uint32_t remote = evt_it->get_remote();
          op_pair_t& op_time = entry.op_time[remote];
          if (op_time.post > evt_it->get_time() &&
              op_time.post < evt_it.exitptr()->get_time())
          {
            data->m_idle = op_time.post - evt_it->get_time();

            cbmanager.notify(MPI_RMA_EARLY_TRANSFER, evt_it, data);
          }
        }

        if (evt_it->get_type() == ENTER &&
            is_mpi_rma_start(evt_it->get_region()) &&
            evt_it.exitptr()->get_window() == win)
        {
          start_found = true;
        }
        
        /* check previous event */
        evt_it--;
      }
    
}

void PatternMPI_RmaSync::mpi_rma_put_end_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 172 "RMA.pattern"

      /* TODO: the use of prev() is not save in the presence of FLUSH events */
      MpiWindow* win          = event.prev()->get_window();
      MpiComm*   comm         = dynamic_cast<MpiComm*>(win->get_comm());
      uint32_t   m_remote_loc = event.prev()->get_remote();
      win_t&     entry        = m_windows[win];

      entry.op_time[comm->get_group()->get_global_rank(m_remote_loc)].op = event->get_time();
    
}

void PatternMPI_RmaSync::mpi_rma_put_latest_op_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 367 "RMA.pattern"

      MpiWindow* win    = event->get_window();
      MpiComm*   comm   = dynamic_cast<MpiComm*>(event->get_window()->get_comm());
      win_t&     entry  = m_windows[win];

      /* send latest rma op times to remote processes */
      map<uint32_t, op_pair_t>::iterator it =
          entry.op_time.begin();
      while (it != entry.op_time.end())
      {
          MPI_Accumulate(&(it->second.op), 1, MPI_DOUBLE, 
                         comm->get_group()->get_local_rank(it->first), LATEST_RMA_OP, 
                         1, MPI_DOUBLE, MPI_MAX, entry.win);
          ++it;
      }
    
}

void PatternMPI_RmaSync::mpi_rma_put_start_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 158 "RMA.pattern"

      MpiWindow* win          = event->get_window();
      uint32_t   m_remote_loc = event->get_remote();
      win_t&     entry        = m_windows[win];

      entry.remote_processes.insert(m_remote_loc);
    
}

void PatternMPI_RmaSync::mpi_rma_set_remote_bit_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{

#line 353 "RMA.pattern"

      MpiWindow* win    = event->get_window();
      win_t&     entry  = m_windows[win];

      /* set bit corresponding to my rank on every remote location */
      for (set<uint32_t>::iterator it = entry.remote_processes.begin();
           it != entry.remote_processes.end(); ++it)
      {
        MPI_Accumulate(&entry.rank_mask, 1, MPI_BYTE, static_cast<int>(*it),
                       entry.rank_offset, 1, MPI_BYTE,
                       MPI_BOR, entry.win); 
      }

    
}

void PatternMPI_RmaSync::prepare_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 91 "RMA.pattern"

      /* Initialize BitSetTable for population count */
      BitSetTable[0] = 0;
      for (int i = 0; i < 256; i++)
      {
        BitSetTable[i] = (i & 1) + BitSetTable[i / 2];
      }
    
      /* get memory to store completion start times for each window */
      uint32_t num_windows = data->m_defs->num_windows();

      /* initialize internal windows for data exchange */
      for (uint32_t i=0; i < num_windows; ++i)
      {
        MpiWindow* win =
            dynamic_cast<MpiWindow*>(data->m_defs->get_window(i));
        MpiComm* comm =
            dynamic_cast<MpiComm*>(win->get_comm());

        if (comm->get_comm() != MPI_COMM_NULL)
        {
          int    myrank    = -1;
          int    comm_size =  0;
          win_t& entry     = m_windows[win];

          /* get rank in and size of current communicator */
          MPI_Comm_rank(comm->get_comm(), &myrank);
          MPI_Comm_size(comm->get_comm(), &comm_size);

          /* determine size of bitfield */
          entry.bitfield_size = ((comm_size % 8) ? 1 : 0 ) +
                                (comm_size / 8);

          /* get memory buffer for window */
          entry.buf = new unsigned char[BITFIELD_START + entry.bitfield_size];

          /* initialize epoch tracking values */
          entry.epoch_ctr                = 0;
          entry.pending_complete         = false;
          
          MPI_Win_create(entry.buf, 
                         (MPI_Aint) (BITFIELD_START + entry.bitfield_size),
                         sizeof(unsigned char), MPI_INFO_NULL, 
                         comm->get_comm(), 
                         &entry.win);

          /* setting bitfield mask and offset */
          entry.rank_mask   = 1 << (myrank % 8);
          entry.rank_offset = BITFIELD_START + (myrank / 8);
        }
      }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaLatePost
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaLatePost : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_POST_COMPLETE, PEARL_create_callback(this, &PatternMPI_RmaLatePost::mpi_rma_post_complete_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_LATE_POST;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "Late Post";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_late_post";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI RMA Late Post inefficiency pattern";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void mpi_rma_post_complete_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaLatePost::mpi_rma_post_complete_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 506 "RMA.pattern"

      Event       enter      = event.enterptr();
      timestamp_t enter_time = enter->get_time();
      Event       start_enter = 
                      data->m_local->get_event(ROLE_RMA_START_ENTER);
      Event       start_exit = 
                      data->m_local->get_event(ROLE_RMA_START_EXIT);

      if ((data->m_tmptime > enter_time) &&
          (data->m_tmptime < event->get_time()))
      {
        /* non-blocking start and post concurrent to complete */
        m_severity[enter.get_cnode()] += 
            (data->m_tmptime - enter_time);
      }
      else if ((data->m_tmptime < start_exit->get_time()) &&
               (data->m_tmptime > start_enter->get_time()))
      {
        /* blocking start */
        m_severity[start_enter.get_cnode()] += 
            (data->m_tmptime - start_enter->get_time());
      }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaEarlyWait
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaEarlyWait : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_POST_WAIT, PEARL_create_callback(this, &PatternMPI_RmaEarlyWait::mpi_rma_post_wait_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_EARLY_WAIT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "Early Wait";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_early_wait";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI_Win_wait waiting for last MPI_Win_complete.";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void mpi_rma_post_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaEarlyWait::mpi_rma_post_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 548 "RMA.pattern"

      Event       enter      = event.enterptr();
      timestamp_t enter_time = enter->get_time();
      
      if (data->m_tmptime > enter_time)
      {
        m_severity[enter.get_cnode()] += 
            (data->m_tmptime - enter_time);
      }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaLateComplete
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaLateComplete : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_POST_WAIT, PEARL_create_callback(this, &PatternMPI_RmaLateComplete::mpi_rma_post_wait_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_LATE_COMPLETE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_EARLY_WAIT;
    }

    virtual string get_name() const
    {
      return "Late Complete";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_late_complete";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI_Win_wait waiting between last rma access and last MPI_Win_complete.";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void mpi_rma_post_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaLateComplete::mpi_rma_post_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 576 "RMA.pattern"

      Event       enter      = event.enterptr();
      timestamp_t enter_time = enter->get_time();
      timestamp_t max_ts     = max(data->m_tmptime2, enter_time);
      std::ostringstream out;

      if (data->m_tmptime > max_ts)
      {
        m_severity[enter.get_cnode()] += 
            (data->m_tmptime - max_ts);
      }
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaEarlyTransfer
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaEarlyTransfer : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_EARLY_TRANSFER, PEARL_create_callback(this, &PatternMPI_RmaEarlyTransfer::mpi_rma_early_transfer_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_EARLY_TRANSFER;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_COMMUNICATION;
    }

    virtual string get_name() const
    {
      return "Early Transfer";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_early_transfer";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI RMA Early Transfer inefficiency pattern";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void mpi_rma_early_transfer_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaEarlyTransfer::mpi_rma_early_transfer_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 607 "RMA.pattern"

        m_severity[event.get_cnode()] += data->m_idle;
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaWaitAtFence
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaWaitAtFence : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_WAIT_AT_FENCE, PEARL_create_callback(this, &PatternMPI_RmaWaitAtFence::mpi_rma_wait_at_fence_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_WAIT_AT_FENCE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_SYNCHRONIZATION;
    }

    virtual string get_name() const
    {
      return "Wait at Fence";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_wait_at_fence";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI_Win_fence, waiting for other processes";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void mpi_rma_wait_at_fence_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 628 "RMA.pattern"

    pearl::timestamp_t local_enter;
    pearl::timestamp_t latest_enter;
    pearl::timestamp_t local_exit;
    pearl::timestamp_t earliest_exit;
  
};


/*----- Callback methods -----*/

void PatternMPI_RmaWaitAtFence::mpi_rma_wait_at_fence_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 635 "RMA.pattern"

      m_severity[event.get_cnode()] += data->m_idle;
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */


#if defined(_MPI) && defined(HAS_MPI2_1SIDED)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaEarlyFence
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaEarlyFence : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_EARLY_FENCE, PEARL_create_callback(this, &PatternMPI_RmaEarlyFence::mpi_rma_early_fence_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_EARLY_FENCE;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_WAIT_AT_FENCE;
    }

    virtual string get_name() const
    {
      return "Early Fence";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_early_fence";
    }

    virtual string get_descr() const
    {
      return "Time spent in MPI_Win_fence while waiting for pending RMA ops";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    /* Callback methods */
    void mpi_rma_early_fence_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
};


/*----- Callback methods -----*/

void PatternMPI_RmaEarlyFence::mpi_rma_early_fence_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 657 "RMA.pattern"

      m_severity[event.get_cnode()] += data->m_idle;
    
}

#endif   /* _MPI && HAS_MPI2_1SIDED */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaPairsyncCount
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaPairsyncCount : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_PWS_COUNT, PEARL_create_callback(this, &PatternMPI_RmaPairsyncCount::mpi_rma_pws_count_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_PAIRSYNC_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "MPI RMA Pairwise Synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_pairsync_count";
    }

    virtual string get_descr() const
    {
      return "Number of pairwise synchronizations in MPI RMA synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_rma_pws_count_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 672 "RMA.pattern"

  
};


/*----- Callback methods -----*/

void PatternMPI_RmaPairsyncCount::mpi_rma_pws_count_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 675 "RMA.pattern"

        m_severity[event.get_cnode()] += data->m_count;
    
}

#endif   /* _MPI */


#if defined(_MPI)

/*
 *---------------------------------------------------------------------------
 *
 * class PatternMPI_RmaPairsyncUnneededCount
 *
 *---------------------------------------------------------------------------
 */


class PatternMPI_RmaPairsyncUnneededCount : public MpiPattern
{
  public:
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(MPI_RMA_PWS_UNNEEDED_COUNT, PEARL_create_callback(this, &PatternMPI_RmaPairsyncUnneededCount::mpi_rma_pws_unneeded_count_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_MPI_RMA_PAIRSYNC_UNNEEDED_COUNT;
    }

    virtual long get_parent() const
    {
      return PAT_MPI_RMA_PAIRSYNC_COUNT;
    }

    virtual string get_name() const
    {
      return "MPI RMA Unneeded Pairwise Synchronizations";
    }

    virtual string get_unique_name() const
    {
      return "mpi_rma_pairsync_unneeded_count";
    }

    virtual string get_descr() const
    {
      return "Number of unneeded pairwise synchronizations in MPI RMA synchronizations";
    }

    virtual string get_unit() const
    {
      return "occ";
    }

    /* Callback methods */
    void mpi_rma_pws_unneeded_count_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);


  private:

#line 694 "RMA.pattern"

  
};


/*----- Callback methods -----*/

void PatternMPI_RmaPairsyncUnneededCount::mpi_rma_pws_unneeded_count_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 697 "RMA.pattern"

        m_severity[event.get_cnode()] += data->m_count;
    
}

#endif   /* _MPI */


/*
 *---------------------------------------------------------------------------
 *
 * class PatternStatistics
 *
 *---------------------------------------------------------------------------
 */


class PatternStatistics : public Pattern
{
  public:

    /* Constructors & destructor */
    virtual ~PatternStatistics()
    {

#line 1106 "Statistics.pattern"

    // Release statistics objects
#if defined(_MPI)
    delete ls_quant;
    delete lsw_quant;
    delete lr_quant;
    delete wnxn_quant;
    delete wb_quant;
    delete er_quant;
    delete es_quant;
    delete lb_quant;
    delete bc_quant;
    delete nxnc_quant;
#endif

#if defined(_OPENMP)
    delete omp_eb_quant;
    delete omp_ib_quant;
#endif // _OPENMP
  
    }
    /* Registering callbacks */
    virtual void reg_cb(CallbackManager* cbmanager)
    {
      init();

      cbmanager->register_callback(BARRIER_COMPL, PEARL_create_callback(this, &PatternStatistics::barrier_compl_cb));
      cbmanager->register_callback(EARLY_REDUCE, PEARL_create_callback(this, &PatternStatistics::early_reduce_cb));
      cbmanager->register_callback(EARLY_SCAN, PEARL_create_callback(this, &PatternStatistics::early_scan_cb));
      cbmanager->register_callback(LATE_BCAST, PEARL_create_callback(this, &PatternStatistics::late_bcast_cb));
      cbmanager->register_callback(LATE_RECEIVER, PEARL_create_callback(this, &PatternStatistics::late_receiver_cb));
      cbmanager->register_callback(LATE_SENDER, PEARL_create_callback(this, &PatternStatistics::late_sender_cb));
      cbmanager->register_callback(LATE_SENDER_WO, PEARL_create_callback(this, &PatternStatistics::late_sender_wo_cb));
      cbmanager->register_callback(NXN_COMPL, PEARL_create_callback(this, &PatternStatistics::nxn_compl_cb));
      cbmanager->register_callback(OMP_EBARRIER_WAIT, PEARL_create_callback(this, &PatternStatistics::omp_ebarrier_wait_cb));
      cbmanager->register_callback(OMP_IBARRIER_WAIT, PEARL_create_callback(this, &PatternStatistics::omp_ibarrier_wait_cb));
      cbmanager->register_callback(PREPARE, PEARL_create_callback(this, &PatternStatistics::prepare_cb));
      cbmanager->register_callback(WAIT_BARRIER, PEARL_create_callback(this, &PatternStatistics::wait_barrier_cb));
      cbmanager->register_callback(WAIT_NXN, PEARL_create_callback(this, &PatternStatistics::wait_nxn_cb));
    }

    /* Get pattern information */
    virtual long get_id() const
    {
      return PAT_STATISTICS;
    }

    virtual long get_parent() const
    {
      return PAT_NONE;
    }

    virtual string get_name() const
    {
      return "STATISTICS";
    }

    virtual string get_unique_name() const
    {
      return "statistics";
    }

    virtual string get_descr() const
    {
      return "Statistics for waiting time distribution";
    }

    virtual string get_unit() const
    {
      return "sec";
    }

    virtual bool is_hidden() const
    {
      return true;
    }

    /* Callback methods */
    void barrier_compl_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void early_reduce_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void early_scan_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_bcast_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_receiver_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void nxn_compl_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_ebarrier_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void omp_ibarrier_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void prepare_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void wait_barrier_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
    void wait_nxn_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata);
  protected:
    /* Protected methods */

    virtual void init()
    {

#line 1085 "Statistics.pattern"

    // Create statistics objects for MPI metrics
#if defined(_MPI)
    ls_quant   = new Quantile(NUMBER_COEFF);
    lsw_quant  = new Quantile(NUMBER_COEFF);
    lr_quant   = new Quantile(NUMBER_COEFF);
    wnxn_quant = new Quantile(NUMBER_COEFF);
    wb_quant   = new Quantile(NUMBER_COEFF);
    er_quant   = new Quantile(NUMBER_COEFF);
    es_quant   = new Quantile(NUMBER_COEFF);
    lb_quant   = new Quantile(NUMBER_COEFF);
    bc_quant   = new Quantile(NUMBER_COEFF);
    nxnc_quant = new Quantile(NUMBER_COEFF);
#endif

#if defined(_OPENMP)
    omp_eb_quant   = new Quantile(NUMBER_COEFF);
    omp_ib_quant   = new Quantile(NUMBER_COEFF);
#endif // _OPENMP
  
    }


  private:

#line 35 "Statistics.pattern"

    /// Symbolic names for entries in arrays storing upper bounds of metric
    /// durations
    enum duration_t {
      LS_MAX_DURATION = 0,
      LR_MAX_DURATION,
      WNXN_MAX_DURATION,
      WB_MAX_DURATION,
      ER_MAX_DURATION,
      ES_MAX_DURATION,
      LB_MAX_DURATION,
      BC_MAX_DURATION,
      NXNC_MAX_DURATION,
      OMP_EB_MAX_DURATION,
      OMP_IB_MAX_DURATION,
      MAX_DURATION_ENTRIES
    };

    /// CallbackData struct used for preparation replay
    struct MaxDurations : public pearl::CallbackData
    {
      double m_max_duration[MAX_DURATION_ENTRIES];

      MaxDurations()
      {
        memset(m_max_duration, 0, MAX_DURATION_ENTRIES * sizeof(double));
      }

      void update_duration(duration_t idx, double value)
      {
        if (value > m_max_duration[idx]) {
          m_max_duration[idx] = value;
        }
      }
    };

    // returns the position with first identical cnode
    class find_cnode_position
    {
      public:
        find_cnode_position (pearl::ident_t n)
        : cnode(n)
        {}

        bool operator() (TopMostSevere element) const
        {
          return element.cnode == cnode;
        }

      private:
        pearl::ident_t cnode;
    };

    // returns the position of first element with cnode not in this list and 
    // bigger idletime
    class find_idletime_position
    {
      public:
        find_idletime_position (pearl::ident_t n, pearl::timestamp_t m)
        : cnode(n), idle (m)
        {}

        bool operator() (TopMostSevere element) const
        {
          return ((element.cnode != cnode) && (element.idletime < idle));
        }

      private:
        pearl::ident_t cnode;
        pearl::timestamp_t idle;
    };

    void mpicexit_cb(const pearl::CallbackManager& cbmanager,
                     int                           user_event,
                     const pearl::Event&           event,
                     pearl::CallbackData*          cdata)
    {
      Event         enter    = event.enterptr();
      double        duration = event->get_time() - enter->get_time();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      Region* region = event.get_cnode()->get_region();
      if (is_mpi_barrier(region)) {
        // Wait at Barrier
        data->update_duration(WB_MAX_DURATION, duration);
        // Barrier Completion
        data->update_duration(BC_MAX_DURATION, duration);
      } else if (is_mpi_12n(region)) {
        // Late Broadcast
        data->update_duration(LB_MAX_DURATION, duration);
      } else if (is_mpi_n21(region)) {
        // Early Reduce
        data->update_duration(ER_MAX_DURATION, duration);
      } else if (is_mpi_scan(region)) {
        // Early Scan
        data->update_duration(ES_MAX_DURATION, duration);
      } else if (is_mpi_n2n(region)) {
        // Wait at NxN
        data->update_duration(WNXN_MAX_DURATION, duration);
        // NxN Completion
        data->update_duration(NXNC_MAX_DURATION, duration);
      }
    }

    void send_cb(const pearl::CallbackManager& cbmanager,
                 int                           user_event,
                 const pearl::Event&           event,
                 pearl::CallbackData*          cdata)
    {
      Event         enter    = event.enterptr();
      Event         exitev   = event.exitptr();
      double        duration = exitev->get_time() - enter->get_time();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      // Late Receiver
      data->update_duration(LR_MAX_DURATION, duration);
    }

    void recv_cb(const pearl::CallbackManager& cbmanager,
                 int                           user_event,
                 const pearl::Event&           event,
                 pearl::CallbackData*          cdata)
    {
      Event         enter    = event.enterptr();
      Event         exitev   = event.exitptr();
      double        duration = exitev->get_time() - enter->get_time();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      // Late Sender
      data->update_duration(LS_MAX_DURATION, duration);
    }

    void ompbarrier_cb(const pearl::CallbackManager& cbmanager,
                 int                           user_event,
                 const pearl::Event&           event,
                 pearl::CallbackData*          cdata)
    {
      Event         enter    = event.enterptr();
      double        duration = event->get_time() - enter->get_time();
      MaxDurations* data     = static_cast<MaxDurations*>(cdata);

      Region* region = event.get_cnode()->get_region();
      if (is_omp_ebarrier(region)) {
        // OpenMP explicit barrier
        data->update_duration(OMP_EB_MAX_DURATION, duration);
      } else if (is_omp_ibarrier(region)) {
        // OpenMP implicit barrier
        data->update_duration(OMP_IB_MAX_DURATION, duration);
      }
    }

    // Statistics collector objects for individual patterns
#if defined(_MPI)
    Quantile* ls_quant;
    Quantile* lsw_quant;
    Quantile* lr_quant;
    Quantile* lrw_quant;
    Quantile* wnxn_quant;
    Quantile* wb_quant;
    Quantile* er_quant;
    Quantile* es_quant;
    Quantile* lb_quant;
    Quantile* bc_quant;
    Quantile* nxnc_quant;
#endif // _MPI

#if defined(_OPENMP)
    Quantile* omp_eb_quant;
    Quantile* omp_ib_quant;
#endif // (_OPENMP)

    double global_timebase;

    // define deques for most severe instances
#if defined(_MPI)
    vector<TopMostSevere> LateSender, LateSenderWO, LateReceiver, EarlyReduce;
    vector<TopMostSevere> BarrierNxNsum, BarrierSum, NxnComplSum, BarrierComplSum, LateBcastSum, EarlyScanSum;
#endif // _MPI

#if defined(_OPENMP)
    vector<TopMostSevere> OmpEBarrierSum, OmpIBarrierSum;
#endif // _OPENMP

#if defined(MOST_SEVERE_MAX) && defined(_MPI)
    vector<TopMostSevere> BarrierNxNmax, BarrierMax, NxnComplMax, BarrierComplMax, LateBcastMax, EarlyScanMax;
#endif

#if defined(MOST_SEVERE_MAX) && defined(_OPENMP)
    vector<TopMostSevere> OmpEBarrierMax, OmpIBarrierMax;
#endif

    // merge results of found patterns
    // from all processes and threads 
    // static variables used here as an implicit shared variables
    // to perform reduction for OpenMP threads
    void result_merge(Quantile* quant)
    {
      // Determine global number of instances
      // (The 'static' variable 'shared_n' is implicitly shared!)
      double n, global_n;
      static double shared_n;
      #pragma omp master
      {
        shared_n = 0.0;
      }
      
      // Determine global coefficients for collecting statistics
      // (The 'static' array 'shared_coeff' is implicitly shared!)
      double coeff[NUMBER_COEFF], global_coeff[NUMBER_COEFF];
      static double shared_coeff[NUMBER_COEFF];

      #pragma omp master
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_coeff[i] = 0.0;
        }
      }

#ifdef ADAPTIVE_APPROACH
      // Determine global coefficients for control values
      // (The 'static' array 'shared_control_val' is implicitly shared!)
      double control_val[NUMBER_COEFF], global_control_val[NUMBER_COEFF];
      static double shared_control_val[NUMBER_COEFF];
    
      #pragma omp master
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_control_val[i] = 0.0;
        }
      }
#endif

      // Determine global max
      // (The 'static' variable 'shared_max' is implicitly shared!)
      double max_val, global_max;
      static double shared_max;
      #pragma omp master
      {
        shared_max = -DBL_MAX;
      }
      
      // Determine global min
      // (The 'static' variable 'shared_min' is implicitly shared!)
      double min_val, global_min;
      static double shared_min;
      #pragma omp master
      {
        shared_min = DBL_MAX;
      }

      // Determine global sum
      // (The 'static' variable 'shared_sum' is implicitly shared!)
      double sum, global_sum;
      static double shared_sum;
      #pragma omp master
      {
        shared_sum = 0.0;
      }

      // Determine global squared sum
      // (The 'static' variable 'shared_squared_sum' is implicitly shared!)
      double squared_sum, global_squared_sum;
      static double shared_squared_sum;
      #pragma omp master
      {
        shared_squared_sum = 0.0;
      }
      
      n = quant->get_n();
      
      #pragma omp barrier
      #pragma omp critical
      {
        shared_n += n;
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(&shared_n, &global_n, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        shared_n = global_n;
      }
#endif // _MPI
      
      #pragma omp barrier
      global_n = shared_n;
      #pragma omp barrier

      for(int i=0; i<NUMBER_COEFF; i++) {
        coeff[i] = (n/global_n) * quant->get_coeff(i);
      }
      
      #pragma omp barrier
      #pragma omp critical
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_coeff[i] += coeff[i];
        }
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(shared_coeff, global_coeff, NUMBER_COEFF, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_coeff[i] = global_coeff[i];
        }
      }
#endif // _MPI

      #pragma omp barrier
      for (int i = 0; i < NUMBER_COEFF; i++) {
        global_coeff[i] = shared_coeff[i];
      }

#ifdef ADAPTIVE_APPROACH    
      for(int i=0; i<NUMBER_COEFF; i++) {
        control_val[i] = (n/global_n) * quant->get_control_val(i);
      }
      
      #pragma omp barrier
      #pragma omp critical
      {
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_control_val[i] += control_val[i];
        }
      }
      #pragma omp barrier
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(shared_control_val, global_control_val, NUMBER_COEFF, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        for (int i = 0; i < NUMBER_COEFF; i++) {
          shared_control_val[i] = global_control_val[i];
        }
      }
#endif // _MPI

      #pragma omp barrier
      for (int i = 0; i < NUMBER_COEFF; i++) {
        global_control_val[i] = shared_control_val[i];
      }

#endif    

      max_val     = quant->get_max_val();
      min_val     = quant->get_min_val();
      sum         = quant->get_sum();
      squared_sum = quant->get_squared_sum();

      #pragma omp barrier
      
      #pragma omp critical
      {
        if(shared_max < max_val)
        {
          shared_max = max_val;
        }
      
        if(shared_min > min_val)
        {
          shared_min = min_val;
        }
      
        shared_sum += sum;
        shared_squared_sum += squared_sum;
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        MPI_Allreduce(&shared_max, &global_max, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
        MPI_Allreduce(&shared_min, &global_min, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        MPI_Allreduce(&shared_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&shared_squared_sum, &global_squared_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        shared_max = global_max;
        shared_min = global_min;
        shared_sum = global_sum;
        shared_squared_sum = global_squared_sum;
      }
#endif // _MPI

      #pragma omp barrier
      global_max = shared_max;
      global_min = shared_min;
      global_sum = shared_sum;
      global_squared_sum = shared_squared_sum;
      #pragma omp barrier
      
#ifdef ADAPTIVE_APPROACH
      quant->set_global_values(global_n, global_sum, global_squared_sum, global_min, global_max, global_coeff, global_control_val);
#else      
      quant->set_global_values(global_n, global_sum, global_squared_sum, global_min, global_max, global_coeff);
#endif
    }

    void write_cube_file(Quantile* quant, vector<TopMostSevere>& instance, ReportData& data, FILE* cube_fp)
    {
      string patternName     = quant->get_metric();
      double number_obs      = quant->get_n();
      double sum_obs         = quant->get_sum();
      double squared_sum_obs = quant->get_squared_sum();
      double min_val         = quant->get_min_val();
      double max_val         = quant->get_max_val();
      double variance        = (squared_sum_obs/number_obs)-(sum_obs * sum_obs)/(number_obs*number_obs);
      double mean            = sum_obs/number_obs;
      double lower_quant     = quant->get_lower_quant();
      double median          = quant->get_median();
      double upper_quant     = quant->get_upper_quant();

      if (number_obs>0) {
        fprintf(cube_fp, "%-26s %9.0f %1.7f %1.7f %1.10f %1.10f %1.10f", patternName.c_str(), number_obs, mean, median, min_val, max_val, sum_obs);
        if (number_obs >= 2)
          fprintf(cube_fp, " %1.10f", variance);
        if (number_obs >= 5)
          fprintf(cube_fp, " %1.10f %1.10f", lower_quant, upper_quant);

        unsigned int i = 0;
        if (instance[i].idletime > 0.0)
          fprintf(cube_fp, "\n");
        while (instance[i].idletime > 0.0 && instance.size()> i) {               
          fprintf(cube_fp, "- cnode %d enter: %1.10f exit: %1.10f duration: %1.10f rank: %d\n",data.cnodes[instance[i].cnode]->id, instance[i].entertime, instance[i].exittime, instance[i].idletime, instance[i].rank);
          i++;
        }

        fprintf(cube_fp, "\n");
      }
    }  

#ifdef WRITE_CONTROL_VALUES

#if defined(_MPI)
    vector<double> md_ls, md_lsw, md_lr, md_lrw, md_wnxn, md_wb, md_er, md_es, md_lb, md_bc, md_nxnc;
#endif // _MPI

#if defined(_OPENMP)
    vector<double> md_omp_eb, md_omp_ib;
#endif // _OPENMP

    void write_values(Quantile* quant)
    {
      string filename = "stats/evaluation/" + quant->get_metric() + "_values.txt";
      FILE* quant_fp = fopen(filename.c_str(), "w");

#ifdef ADAPTIVE_APPROACH
      int opt_number_coeff = quant->get_number_coeff();
#endif
      double number_obs = quant->get_n();
      double sum_obs = quant->get_sum();
      double squared_sum_obs = quant->get_squared_sum();
      double min_val = quant->get_min_val();
      double max_val = quant->get_max_val();
      double factor = quant->get_factor();
      double coeff[NUMBER_COEFF];

      for(int i=0; i<NUMBER_COEFF; i++) {
        coeff[i] = quant->get_coeff(i);
      }

      double mean = sum_obs/number_obs;
      double variance = (squared_sum_obs/number_obs)-(sum_obs * sum_obs)/(number_obs*number_obs);

      fprintf(quant_fp, "Number of all coefficients and all observations n : %d and %10.0f\n", NUMBER_COEFF, number_obs);
#ifdef ADAPTIVE_APPROACH
      fprintf(quant_fp, "Optimal number of coefficients: %d\n", opt_number_coeff);
#endif
      fprintf(quant_fp, "sum : %2.10f, squared sum: %2.10f\n", sum_obs, squared_sum_obs);
      fprintf(quant_fp, "mean : %2.10f, variance: %2.10f\n", mean, variance);

      double lower_quant = quant->get_lower_quant();
      double median = quant->get_median();
      double upper_quant = quant->get_upper_quant();

      fprintf(quant_fp, "upper bound   : %2.15f\n\n", (1/factor));
      fprintf(quant_fp, "minimum       : %2.15f\n", min_val);
      fprintf(quant_fp, "lower quartile: %2.15f\n", (lower_quant));
      fprintf(quant_fp, "median        : %2.15f\n", (median));
      fprintf(quant_fp, "upper quartile: %2.15f\n", (upper_quant));
      fprintf(quant_fp, "maximum       : %2.15f\n\n", max_val);
    
      for(int i=0; i<NUMBER_COEFF; i++) {
        fprintf(quant_fp, "coeff nr %2d : %2.15f\n", (i+1), coeff[i]);
      }

      fclose(quant_fp);
    }

#endif

    // This function performs insertion
    // sort of array of structs, where 
    // arr - input array to be sorted
    // len - length of the array to be sorted
    void insertionSort(TopMostSevere* arr, int len)
    {
      int j;
      TopMostSevere tmp;
      for (int i = 1; i < len; i++) {
        j = i;
        while (j > 0 && arr[j - 1].idletime < arr[j].idletime) {
          tmp        = arr[j];
          arr[j]     = arr[j - 1];
          arr[j - 1] = tmp;
          j--;
        }
      }
    }

    // This function inserts an element 
    // into the sorted vector of structures
    // and shifts the rest elements, where
    // in    - First input array
    // inout - Second input array, also storing the result
    // i     - the id of the element which should be inserted
    // j     - pointer to the position where the structure should be inserted
    // len   - length of the vestor os structures
    void insertAndShift(TopMostSevere* invec, TopMostSevere* inoutvec, int i, int& j, int& len)
    {
      //shift
      std::memmove(&inoutvec[j + 1], &inoutvec[j], (len - j - 1) * sizeof(TopMostSevere));
      //insert
      inoutvec[j] = invec[i];
    }

#if defined(_MPI)
    // performs reduction to compute
    // the top most severe instances
    void result_top_most(vector<TopMostSevere>& instance, TopMostSevere  * tmpRecv)
    {
      int len_sum;
      int len = instance.size();
      // check if one needs to make reduction
      MPI_Allreduce(&len, &len_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
      if (len_sum > 0) {
        if (instance.size() < TOP_SEVERE_INSTANCES) {
          if (len > 1) {
            insertionSort(&instance[0], len);
          }
          for (int i = TOP_SEVERE_INSTANCES - instance.size(); i > 0; i--) {
            instance.push_back(TopMostSevere());
          }
        }

        // define global rank
        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        MPI_Reduce(&instance[0], &tmpRecv[0], 1, TOPSEVEREARRAY,
                   MAX_TOPSEVEREARRAY, 0, MPI_COMM_WORLD);
        
        if (rank == 0) {
          for (int i = 0; i < TOP_SEVERE_INSTANCES; ++i) {
            instance[i] = tmpRecv[i];
          }
        }
      }
    }
#endif // _MPI

    // collects and keep sorted
    // most severe instances
    // with different cnodes
    void stat_collector(vector<TopMostSevere> &instance, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t rank)
    {
      TopMostSevere tmp;
      vector<TopMostSevere>::iterator iter_cnode, iter_idle;
      int pos_cnode, pos_idle, len;
      tmp = TopMostSevere(idle, enter, exit, cnode, rank);
      
      // if our vector buffer is not full
      if (instance.size() < TOP_SEVERE_INSTANCES) {
        // first severity data is inserted without any conditions
        if (instance.empty()) {
          instance.push_back(TopMostSevere(idle, enter, exit, cnode, rank));
        }
        // if it is not the first one, we have to check further conditions
        else {
          // find the position of the severity item with an identical "cnode" value
          iter_cnode = find_if(instance.begin(), instance.end(), find_cnode_position(cnode));
          pos_cnode = (int) (iter_cnode - instance.begin());
          
          // if we have severity with such "cnode" value and greater "idletime"
          // then we update data with new values of time
          if (((unsigned int)pos_cnode != instance.size()) && (idle > instance[pos_cnode].idletime)) {
            instance[pos_cnode] = tmp;
          }
          // if we've not found any sevirity item with identical "cnode" value
          // it will be inserted into the vector
          else if ((unsigned int)pos_cnode == instance.size()) {
            instance.push_back(TopMostSevere(idle, enter, exit, cnode, rank));
            // if the maximum value of severities was reached, sort the vector due to "idletime"
            if (instance.size() == TOP_SEVERE_INSTANCES) {
              insertionSort(&instance[0], instance.size());
            }
          }
        }
      }
      // now our vector buffer is full
      else {
        len = instance.size();
        
        // find the position of the severity item with an identical "cnode" value
        iter_cnode = find_if(instance.begin(), instance.end(), find_cnode_position(cnode));
        pos_cnode = (int) (iter_cnode - instance.begin());
        
        // find the position of the severity item with not identical "cnode" and greater "idletime"
        iter_idle = find_if(instance.begin(), instance.end(), find_idletime_position(cnode, idle));
        pos_idle = (int) (iter_idle - instance.begin());
        
        // update the values if our next item with the same "cnode" has greater "idletime"
        // and sort it afterwards due to "idletime"
        if (((unsigned int)pos_cnode != instance.size()) && (idle > instance[pos_cnode].idletime)) {
          instance[pos_cnode] = tmp;
          insertionSort(&instance[0], len);
        }
        // if we found the severity with a new "cnode" vlue and greater "idletime"
        // then the vector will be shifted
        else if (((unsigned int)pos_idle != instance.size()) && ((unsigned int)pos_cnode == instance.size())) {  
          insertAndShift(&tmp, &instance[0], 0, pos_idle, len);
        }
      }
    }
    
#if defined(_MPI)
    // performs MPI reduction of idletime, enter and exit times
    // of most severe instances for collective operations
#if !defined(MOST_SEVERE_MAX)
    void stat_collective(vector<TopMostSevere> &instSum, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t global_rank, MpiComm* comm)
#else
    void stat_collective(vector<TopMostSevere> &instMax, vector<TopMostSevere> &instSum, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t global_rank, MpiComm* comm)
#endif
    {
      // define local rank
      int local_rank;
      MPI_Comm_rank(comm->get_comm(), &local_rank);
      
#if !defined(MOST_SEVERE_MAX)
      TopMostSevere tmpSend, tmpRecv;
      pearl::timestamp_t tmp_idletime_sum = 0.0;
      tmpSend = TopMostSevere(idle, enter, exit, cnode, global_rank);

      MPI_Reduce(&tmpSend.idletime, &tmp_idletime_sum, 1, MPI_DOUBLE, MPI_SUM, 0, comm->get_comm());
      MPI_Reduce(&tmpSend, &tmpRecv, 1, TOPSEVERE, SUM_TOPSEVERE, 0, comm->get_comm());
#else
      TopMostSevereMaxSum tmpSend, tmpRecv;
      tmpSend = TopMostSevereMaxSum(idle, idle, enter, exit, cnode, global_rank);
      
      MPI_Reduce(&tmpSend, &tmpRecv, 1, TOPSEVERECOLL, MAXSUM_TOPSEVERECOLL, 0, comm->get_comm());
#endif

      if (local_rank == 0) {
        stat_collector(instSum, tmp_idletime_sum, tmpRecv.entertime, tmpRecv.exittime, tmpRecv.cnode, tmpRecv.rank);
#if defined(MOST_SEVERE_MAX)
        stat_collector(instMax, tmpRecv.idletime_max, tmpRecv.entertime, tmpRecv.exittime, tmpRecv.cnode, tmpRecv.rank);
#endif
      }
    }
#endif // _MPI

#if defined(_OPENMP)
    // performs OpenMP reduction of idletime, enter and exit times
    // of most severe instances for collective operations
#if !defined(MOST_SEVERE_MAX)
    void stat_collective(vector<TopMostSevere> &instSum, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t rank)
    {
      // define shared severe instance
      static TopMostSevere sharedInst;
      static pearl::timestamp_t sharedTime;
      
      #pragma omp barrier
      #pragma omp master
      {
        sharedInst = TopMostSevere(0.0, DBL_MAX, -DBL_MAX, cnode, rank);
        sharedTime = 0.0;
      }
      
      #pragma omp barrier
      #pragma omp critical
      {
        if(sharedInst.idletime < idle){
          sharedInst.idletime = idle;
          sharedInst.rank = rank;
        }
        
        sharedTime += idle;
        
        if(sharedInst.entertime > enter)
          sharedInst.entertime = enter;
        
        if(sharedInst.exittime < exit)
          sharedInst.exittime = exit;
      }
      #pragma omp barrier
      
      #pragma omp master
      {
        stat_collector(instSum, sharedTime, sharedInst.entertime, sharedInst.exittime, sharedInst.cnode, sharedInst.rank);
      }
    }
#else
    void stat_collective(vector<TopMostSevere> &instMax, vector<TopMostSevere> &instSum, pearl::timestamp_t idle, pearl::timestamp_t enter, pearl::timestamp_t exit, pearl::ident_t cnode, pearl::ident_t rank)
    {
      // define shared severe instance
      static TopMostSevereMaxSum sharedInst;

      #pragma omp barrier
      #pragma omp master
      {
        sharedInst = TopMostSevereMaxSum(-DBL_MAX, 0.0, DBL_MAX, -DBL_MAX, cnode, rank);
      }
      
      #pragma omp barrier
      #pragma omp critical
      {
        sharedInst.idletime += idle;
        
        if(sharedInst.entertime > enter)
          sharedInst.entertime = enter;
        
        if(sharedInst.exittime < exit)
          sharedInst.exittime = exit;
        if(sharedInst.idletime < idle)
          sharedInst.rank = rank;
        if(sharedInst.idletime_max < idle)
        {
          sharedInst.idletime_max = idle;
          sharedInst.rank = rank;
        }
      }
      #pragma omp barrier
      
      #pragma omp master
      {
        stat_collector(instMax, sharedInst.idletime_max, sharedInst.entertime, sharedInst.exittime, sharedInst.cnode, sharedInst.rank);
      }
    }
#endif
#endif // _OPENMP

    
    public:
      void gen_severities(ReportData&              data,
                          int                      rank,
                          const pearl::LocalTrace& trace,
                          std::FILE*               fp)
      {
#if defined(_MPI)
        result_merge(ls_quant);
        result_merge(lsw_quant);
        result_merge(lr_quant);
        result_merge(wnxn_quant);
        result_merge(wb_quant);
        result_merge(er_quant);
        result_merge(es_quant);
        result_merge(lb_quant);
        result_merge(bc_quant);
        result_merge(nxnc_quant);
#endif // _MPI
#if defined(_OPENMP)
        result_merge(omp_eb_quant);
        result_merge(omp_ib_quant);
#endif
        #pragma omp master
        {
#ifdef WRITE_CONTROL_VALUES
          if (rank == 0) {
            mkdir("stats", 0700);
            mkdir("stats/evaluation", 0700);
#if defined(_MPI)
            mkdir("stats/ls_stats",   0700);
            mkdir("stats/lsw_stats",  0700);
            mkdir("stats/lr_stats",   0700);
            mkdir("stats/wnxn_stats", 0700);
            mkdir("stats/wb_stats",   0700);
            mkdir("stats/er_stats",   0700);
            mkdir("stats/es_stats",   0700);
            mkdir("stats/lb_stats",   0700);
            mkdir("stats/bc_stats",   0700);
            mkdir("stats/nxnc_stats", 0700);
#endif // _MPI

#if defined(_OPENMP)
            mkdir("stats/omp_eb_stats", 0700);
            mkdir("stats/omp_ib_stats", 0700);
#endif // _OPENMP
          }
#if defined(_MPI)
          MPI_Barrier(MPI_COMM_WORLD);
#endif // _MPI
        }
          
          // write values for OpenMp patterns
#if defined(_OPENMP)
          char name_omp[50];
          FILE* stats_omp_fp;
          unsigned int thread_id;
          thread_id = omp_get_thread_num();
          #pragma omp barrier
          
          // OpenMP explicit barrier
          snprintf(name_omp, 50, "stats/omp_eb_stats/omp_eb_stats.%06dx%06d", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_omp_eb.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_omp_eb[i]);
          fclose(stats_omp_fp);

          // OpenMP implicit barrier
          snprintf(name_omp, 50, "stats/omp_ib_stats/omp_ib_stats.%06dx%06d", rank, thread_id);
          stats_omp_fp = fopen(name_omp, "w");
          for (size_t i = 0; i < md_omp_ib.size(); i++)
            fprintf(stats_omp_fp, "%2.14f\n", md_omp_ib[i]);
          fclose(stats_omp_fp);
#endif

        #pragma omp master
        {
#if defined(_MPI)
          char name_mpi[40];
          FILE* stats_mpi_fp;

          // Late Sender values
          snprintf(name_mpi, 40, "stats/ls_stats/ls_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_ls.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_ls[i]);
          fclose(stats_mpi_fp);

          // Late Sender, Wrong Order values
          snprintf(name_mpi, 40, "stats/lsw_stats/lsw_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_lsw.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_lsw[i]);
          fclose(stats_mpi_fp);

          // Late Receiver values
          snprintf(name_mpi, 40, "stats/lr_stats/lr_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_lr.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_lr[i]);
          fclose(stats_mpi_fp);

          // Wait at NxN values
          snprintf(name_mpi, 40, "stats/wnxn_stats/wnxn_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_wnxn.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_wnxn[i]);
          fclose(stats_mpi_fp);

          // Wait at Barrier values
          snprintf(name_mpi, 40, "stats/wb_stats/wb_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_wb.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_wb[i]);
          fclose(stats_mpi_fp);

          // Early Reduce values
          snprintf(name_mpi, 40, "stats/er_stats/er_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_er.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_er[i]);
          fclose(stats_mpi_fp);

          // Early Scan values
          snprintf(name_mpi, 40, "stats/es_stats/es_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_es.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_es[i]);
          fclose(stats_mpi_fp);

          // Late Broadcast values
          snprintf(name_mpi, 40, "stats/lb_stats/lb_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_lb.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_lb[i]);
          fclose(stats_mpi_fp);

          // Barrier Completion values
          snprintf(name_mpi, 40, "stats/bc_stats/bc_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_bc.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_bc[i]);
          fclose(stats_mpi_fp);

          // NxN Completion values
          snprintf(name_mpi, 40, "stats/nxnc_stats/nxnc_stats.%06d", rank);
          stats_mpi_fp = fopen(name_mpi, "w");
          for (size_t i = 0; i < md_nxnc.size(); i++)
            fprintf(stats_mpi_fp, "%2.14f\n", md_nxnc[i]);
          fclose(stats_mpi_fp);
#endif // _MPI
#endif

          // Merge results for each pattern
#if defined(_MPI)
          // define temporary receive buffer 
          TopMostSevere * tmpRecv;
          tmpRecv = new TopMostSevere[TOP_SEVERE_INSTANCES];

          result_top_most(LateSender, tmpRecv);
          result_top_most(LateSenderWO, tmpRecv);
          result_top_most(LateReceiver, tmpRecv);
          result_top_most(EarlyReduce, tmpRecv);
          result_top_most(BarrierNxNsum, tmpRecv);
          result_top_most(BarrierSum, tmpRecv);
          result_top_most(NxnComplSum, tmpRecv);
          result_top_most(BarrierComplSum, tmpRecv);
          result_top_most(LateBcastSum, tmpRecv);
          result_top_most(EarlyScanSum, tmpRecv);
#if defined(_OPENMP)
          result_top_most(OmpEBarrierSum, tmpRecv);
          result_top_most(OmpIBarrierSum, tmpRecv);
#endif // _OPENMP
#if defined(MOST_SEVERE_MAX)
          result_top_most(BarrierNxNmax, tmpRecv);
          result_top_most(BarrierMax, tmpRecv);
          result_top_most(NxnComplMax, tmpRecv);
          result_top_most(BarrierComplMax, tmpRecv);
          result_top_most(LateBcastMax, tmpRecv);
          result_top_most(EarlyScanMax, tmpRecv);
#if defined(_OPENMP)
          result_top_most(OmpEBarrierMax, tmpRecv);
          result_top_most(OmpIBarrierMax, tmpRecv);
#endif // _OPENMP
#endif
      // free temporary receive buffer 
      delete [] tmpRecv;
#endif // _MPI

          if (rank == 0) {
            // Open statistics file
            string filename = epk_archive_get_name();
            filename += "/trace.stat";
            FILE*  cube_fp  = fopen(filename.c_str(), "w");
            if (NULL == cube_fp) {
              // Only warn (instead of abort) since we still might get a
              // valid CUBE file
              elg_warning("Could not create statistics file!");
            } else {
              // Write statistics
              fprintf(cube_fp, "PatternName               Count      Mean    Median      Minimum      Maximum      Sum     Variance    Quartil25    Quartil75\n");

#if defined(_MPI)
              ls_quant->set_metric("mpi_latesender");
              ls_quant->calc_quantiles();
              write_cube_file(ls_quant, LateSender, data, cube_fp);

              lsw_quant->set_metric("mpi_latesender_wo");
              lsw_quant->calc_quantiles();
              write_cube_file(lsw_quant, LateSenderWO, data, cube_fp);

              lr_quant->set_metric("mpi_latereceiver");
              lr_quant->calc_quantiles();
              write_cube_file(lr_quant, LateReceiver, data, cube_fp);

              wnxn_quant->set_metric("mpi_wait_nxn");
              wnxn_quant->calc_quantiles();
              write_cube_file(wnxn_quant, BarrierNxNsum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(wnxn_quant, BarrierNxNmax, data, cube_fp);
#endif

              wb_quant->set_metric("mpi_barrier_wait");
              wb_quant->calc_quantiles();
              write_cube_file(wb_quant, BarrierSum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(wb_quant, BarrierMax, data, cube_fp);
#endif

              er_quant->set_metric("mpi_earlyreduce");
              er_quant->calc_quantiles();
              write_cube_file(er_quant, EarlyReduce, data, cube_fp);

              es_quant->set_metric("mpi_earlyscan");
              es_quant->calc_quantiles();
              write_cube_file(es_quant, EarlyScanSum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(es_quant, EarlyScanMax, data, cube_fp);
#endif

              lb_quant->set_metric("mpi_latebroadcast");
              lb_quant->calc_quantiles();
              write_cube_file(lb_quant, LateBcastSum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(lb_quant, LateBcastMax, data, cube_fp);
#endif

              bc_quant->set_metric("mpi_barrier_completion");
              bc_quant->calc_quantiles();
              write_cube_file(bc_quant, BarrierComplSum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(bc_quant, BarrierComplMax, data, cube_fp);
#endif

              nxnc_quant->set_metric("mpi_nxn_completion");
              nxnc_quant->calc_quantiles();
              write_cube_file(nxnc_quant, NxnComplSum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(nxnc_quant, NxnComplMax, data, cube_fp);
#endif
#endif // _MPI

#if defined(_OPENMP)
              omp_eb_quant->set_metric("omp_ebarrier_wait");
              omp_eb_quant->calc_quantiles();
              write_cube_file(omp_eb_quant, OmpEBarrierSum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(omp_eb_quant, OmpEBarrierMax, data, cube_fp);
#endif

              omp_ib_quant->set_metric("omp_ibarrier_wait");
              omp_ib_quant->calc_quantiles();
              write_cube_file(omp_ib_quant, OmpIBarrierSum, data, cube_fp);
#if defined(MOST_SEVERE_MAX)
              write_cube_file(omp_ib_quant, OmpIBarrierMax, data, cube_fp);
#endif
#endif // _OPENMP

              // Close statistics file
              fclose(cube_fp);
              
#ifdef WRITE_CONTROL_VALUES
              // Write coefficients per pattern for debugging
#if defined(_MPI)
              write_values(ls_quant);
              write_values(lsw_quant);
              write_values(lr_quant);
              write_values(wnxn_quant);
              write_values(wb_quant);
              write_values(er_quant);
              write_values(es_quant);
              write_values(lb_quant);
              write_values(bc_quant);
              write_values(nxnc_quant);
#endif // _MPI

#if defined(_OPENMP)
              write_values(omp_eb_quant);
              write_values(omp_ib_quant);
#endif // _OPENMP
#endif

            }
          }
        }
      }
  
};


/*----- Callback methods -----*/

void PatternStatistics::barrier_compl_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1476 "Statistics.pattern"

#if defined(_MPI)
      if (data->m_idle > 0)
        bc_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;
        
      MpiComm* comm = event->get_comm();

#if !defined(MOST_SEVERE_MAX)
      stat_collective(BarrierComplSum, data->m_idle,
#else
      stat_collective(BarrierComplMax, BarrierComplSum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_bc.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::early_reduce_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1361 "Statistics.pattern"

#if defined(_MPI)
      er_quant->add_value(data->m_idle);

      stat_collector(EarlyReduce,
                     data->m_idle,
                     data->m_callstack.top()->get_time() - global_timebase,
                     event->get_time() - global_timebase,
                     event.get_cnode()->get_id(),
                     event.get_location()->get_process()->get_id());

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_er.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::early_scan_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1383 "Statistics.pattern"

#if defined(_MPI)
      if (data->m_idle > 0)
        es_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;
        
      MpiComm* comm = event->get_comm();
      
#if !defined(MOST_SEVERE_MAX)      
      stat_collective(EarlyScanSum, data->m_idle,
#else
      stat_collective(EarlyScanMax, EarlyScanSum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id(),
                      comm);
      
#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_es.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_bcast_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1414 "Statistics.pattern"

#if defined(_MPI)
      if (data->m_idle > 0)
        lb_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;
        
      MpiComm* comm = event->get_comm();

#if !defined(MOST_SEVERE_MAX)      
      stat_collective(LateBcastSum, data->m_idle,
#else
      stat_collective(LateBcastMax, LateBcastSum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_lb.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_receiver_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1276 "Statistics.pattern"

#if defined(_MPI)
      lr_quant->add_value(data->m_idle);
      
      RemoteEvent enter = data->m_remote->get_event(ROLE_ENTER_SEND_LR);
      
      stat_collector(LateReceiver,
                     data->m_idle,
                     enter->get_time() - global_timebase,
                     event->get_time() - global_timebase,
                     enter.get_cnode()->get_id(),
                     event.get_location()->get_process()->get_id());
                     
#ifdef WRITE_CONTROL_VALUES
      md_lr.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_sender_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1229 "Statistics.pattern"

#if defined(_MPI)
      ls_quant->add_value(data->m_idle);

      Event enter = data->m_local->get_event(ROLE_RECV_LS);

      stat_collector(LateSender,
                     data->m_idle,
                     enter.enterptr()->get_time() - global_timebase,
                     event->get_time() - global_timebase,
                     enter.get_cnode()->get_id(),
                     event.get_location()->get_process()->get_id());
      
#ifdef WRITE_CONTROL_VALUES
      md_ls.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::late_sender_wo_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1252 "Statistics.pattern"

#if defined(_MPI)
      lsw_quant->add_value(data->m_idle);
      
      RemoteEvent enter = data->m_remote->get_event(ROLE_SEND);
      Event exit = data->m_local->get_event(ROLE_RECV_LSWO);
      
      stat_collector(LateSenderWO,
                     data->m_idle,
                     enter->get_time() - global_timebase,
                     event->get_time() - global_timebase,
                     exit.get_cnode()->get_id(),
                     event.get_location()->get_process()->get_id());
                     
#ifdef WRITE_CONTROL_VALUES
      md_lsw.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::nxn_compl_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1445 "Statistics.pattern"

#if defined(_MPI)
      if (data->m_idle > 0)
        nxnc_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;
        
      MpiComm* comm = event->get_comm();

#if !defined(MOST_SEVERE_MAX)
      stat_collective(NxnComplSum, data->m_idle,
#else
      stat_collective(NxnComplMax, NxnComplSum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_nxnc.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::omp_ebarrier_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1507 "Statistics.pattern"

#if defined(_OPENMP)
      if (data->m_idle > 0)
        omp_eb_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;

#if !defined(MOST_SEVERE_MAX)
      stat_collective(OmpEBarrierSum, data->m_idle,
#else
      stat_collective(OmpEBarrierMax, OmpEBarrierSum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id());

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_omp_eb.push_back(data->m_idle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::omp_ibarrier_wait_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1535 "Statistics.pattern"

#if defined(_OPENMP)
      if (data->m_idle > 0)
        omp_ib_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;

#if !defined(MOST_SEVERE_MAX)
      stat_collective(OmpIBarrierSum, data->m_idle,
#else
      stat_collective(OmpIBarrierMax, OmpIBarrierSum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id());

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_omp_ib.push_back(data->m_idle);
#endif
#else   // !_OPENMP
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _OPENMP
    
}

void PatternStatistics::prepare_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1128 "Statistics.pattern"

      // Determine minimal absolute timestamp of the application
      // (The 'static' variable 'shared_time' is implicitly shared!)
      static double shared_time = DBL_MAX;
      double        local_time  = data->m_trace->begin()->get_time();
      #pragma omp barrier
      #pragma omp critical
      {
        if (local_time < shared_time)
          shared_time = local_time;
      }
      #pragma omp barrier
      ;   /* FIXME: This extra "statement" is required for some versions
                    of the Intel 10.1 compilers */

#if defined(_MPI)
      double global;
      #pragma omp master
      {
        MPI_Allreduce(&shared_time, &global, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
        shared_time = global;
      }
#endif // _MPI

      #pragma omp barrier
      global_timebase = shared_time;
      #pragma omp barrier
      ;   /* FIXME: This extra "statement" is required for some versions
                    of the Intel 10.1 compilers */

      // Determine local upper bounds for pattern durations
      CallbackManager cb;
      MaxDurations    durations;

#if defined(_MPI)
      cb.register_callback(MPI_COLLEXIT, PEARL_create_callback(this, &PatternStatistics::mpicexit_cb));
      cb.register_callback(MPI_SEND, PEARL_create_callback(this, &PatternStatistics::send_cb));
      cb.register_callback(MPI_RECV, PEARL_create_callback(this, &PatternStatistics::recv_cb));
#endif // _MPI

#if defined(_OPENMP)
      cb.register_callback(OMP_COLLEXIT, PEARL_create_callback(this, &PatternStatistics::ompbarrier_cb));
#endif // _OPENMP
      
      PEARL_forward_replay(*(data->m_trace), cb, &durations);
      
      // Determine global upper bounds for pattern durations
      // (The 'static' variable 'global_durations' is implicitly shared!)
      static MaxDurations global_durations;
      
      #pragma omp master
      {
        global_durations.m_max_duration[OMP_EB_MAX_DURATION] = - DBL_MAX;
        global_durations.m_max_duration[OMP_IB_MAX_DURATION] = - DBL_MAX;
      }
      
      #pragma omp barrier 
      #pragma omp critical
      {
        if (global_durations.m_max_duration[OMP_EB_MAX_DURATION] < durations.m_max_duration[OMP_EB_MAX_DURATION]) {
          global_durations.m_max_duration[OMP_EB_MAX_DURATION] = durations.m_max_duration[OMP_EB_MAX_DURATION];
        }
        if (global_durations.m_max_duration[OMP_IB_MAX_DURATION] < durations.m_max_duration[OMP_IB_MAX_DURATION]) {
          global_durations.m_max_duration[OMP_IB_MAX_DURATION] = durations.m_max_duration[OMP_IB_MAX_DURATION];
        }
      }
      #pragma omp barrier
      
#if defined(_MPI)
      #pragma omp master
      {
        durations.m_max_duration[OMP_EB_MAX_DURATION] = global_durations.m_max_duration[OMP_EB_MAX_DURATION];
        durations.m_max_duration[OMP_IB_MAX_DURATION] = global_durations.m_max_duration[OMP_IB_MAX_DURATION];
        MPI_Allreduce(&(durations.m_max_duration),
                      &(global_durations.m_max_duration),
                      MAX_DURATION_ENTRIES, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
      }
#endif // _MPI

      #pragma omp barrier
      
#if defined(_MPI)
      ls_quant->set_factor_by_upperbound(global_durations.m_max_duration[LS_MAX_DURATION]);
      lsw_quant->set_factor_by_upperbound(global_durations.m_max_duration[LS_MAX_DURATION]);
      lr_quant->set_factor_by_upperbound(global_durations.m_max_duration[LR_MAX_DURATION]);
      wnxn_quant->set_factor_by_upperbound(global_durations.m_max_duration[WNXN_MAX_DURATION]);
      wb_quant->set_factor_by_upperbound(global_durations.m_max_duration[WB_MAX_DURATION]);
      er_quant->set_factor_by_upperbound(global_durations.m_max_duration[ER_MAX_DURATION]);
      es_quant->set_factor_by_upperbound(global_durations.m_max_duration[ES_MAX_DURATION]);
      lb_quant->set_factor_by_upperbound(global_durations.m_max_duration[LB_MAX_DURATION]);
      bc_quant->set_factor_by_upperbound(global_durations.m_max_duration[BC_MAX_DURATION]);
      nxnc_quant->set_factor_by_upperbound(global_durations.m_max_duration[NXNC_MAX_DURATION]);
#endif // _MPI

#if defined(_OPENMP)
      omp_eb_quant->set_factor_by_upperbound(global_durations.m_max_duration[OMP_EB_MAX_DURATION]);
      omp_ib_quant->set_factor_by_upperbound(global_durations.m_max_duration[OMP_IB_MAX_DURATION]);
#endif // _OPENMP  
    
}

void PatternStatistics::wait_barrier_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1330 "Statistics.pattern"

#if defined(_MPI)
      if (data->m_idle > 0)
        wb_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;
      
      MpiComm* comm = event->get_comm();

#if !defined(MOST_SEVERE_MAX)      
      stat_collective(BarrierSum, data->m_idle,
#else
      stat_collective(BarrierMax, BarrierSum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_wb.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}

void PatternStatistics::wait_nxn_cb(const pearl::CallbackManager& cbmanager, int user_event, const pearl::Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

#line 1299 "Statistics.pattern"

#if defined(_MPI)
      if (data->m_idle > 0)
        wnxn_quant->add_value(data->m_idle);
      else
        data->m_idle = 0.0;
      
      MpiComm* comm = event->get_comm();

#if !defined(MOST_SEVERE_MAX)
      stat_collective(BarrierNxNsum, data->m_idle,
#else
      stat_collective(BarrierNxNmax, BarrierNxNsum, data->m_idle,
#endif
                      data->m_callstack.top()->get_time() - global_timebase,
                      event->get_time() - global_timebase,
                      event.get_cnode()->get_id(),
                      event.get_location()->get_process()->get_id(),
                      comm);

#ifdef WRITE_CONTROL_VALUES
      if (data->m_idle > 0)
        md_wnxn.push_back(data->m_idle);
#endif
#else   // !_MPI
      // Ugly hack to avoid 'unused variable' warning
      (void)data;
#endif   // _MPI
    
}


//--- Implementation --------------------------------------------------------

void scout::create_patterns(PatternDetectionTask* analyzer)
{
  analyzer->add_pattern(new PatternTime());
  analyzer->add_pattern(new PatternVisits());
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_SyncCollective());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BarrierWait());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BarrierCompletion());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_P2P());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_LateSender());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_LateSenderWO());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_LswoDifferent());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_LswoSame());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_LateReceiver());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_Collective());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_EarlyReduce());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_EarlyScan());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_LateBroadcast());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_WaitNxN());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_NxNCompletion());
#endif   /* _MPI */
#if defined(_OPENMP)
  analyzer->add_pattern(new PatternOMP_Mgmt());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->add_pattern(new PatternOMP_Mgmt_Fork());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->add_pattern(new PatternOMP_Sync_Barrier());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->add_pattern(new PatternOMP_Sync_Ebarrier_Wait());
#endif   /* _OPENMP */
#if defined(_OPENMP)
  analyzer->add_pattern(new PatternOMP_Sync_Ibarrier_Wait());
#endif   /* _OPENMP */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_SyncsSend());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_SyncsRecv());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_SyncsColl());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CommsSend());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CommsRecv());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CommsExch());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CommsSrc());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CommsDst());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BytesSent());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BytesRcvd());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BytesCout());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BytesCin());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BytesGet());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_BytesPut());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CLS_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CLSWO_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_CLR_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_SLS_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_SLSWO_Count());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_SLR_Count());
#endif   /* _MPI */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->add_pattern(new PatternMPI_RmaSync());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->add_pattern(new PatternMPI_RmaLatePost());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->add_pattern(new PatternMPI_RmaEarlyWait());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->add_pattern(new PatternMPI_RmaLateComplete());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->add_pattern(new PatternMPI_RmaEarlyTransfer());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->add_pattern(new PatternMPI_RmaWaitAtFence());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI) && defined(HAS_MPI2_1SIDED)
  analyzer->add_pattern(new PatternMPI_RmaEarlyFence());
#endif   /* _MPI && HAS_MPI2_1SIDED */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_RmaPairsyncCount());
#endif   /* _MPI */
#if defined(_MPI)
  analyzer->add_pattern(new PatternMPI_RmaPairsyncUnneededCount());
#endif   /* _MPI */
  if (enableStatistics)
    analyzer->add_pattern(new PatternStatistics());
}


