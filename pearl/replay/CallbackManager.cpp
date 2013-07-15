/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "CallbackManager.h"
#include "Event.h"

using namespace std;
using namespace pearl;


//--- Utility functions -----------------------------------------------------

namespace
{

template<class containerT>
void execute_callbacks(const containerT&             cbmap,
                       typename containerT::key_type key,
                       const CallbackManager&        cbmanager,
                       int                           user_event,
                       const Event&                  event,
                       CallbackData*                 data)
{
  // Any callbacks defined?
  typename containerT::const_iterator it = cbmap.find(key);
  if (it == cbmap.end())
    return;

  // Execute callback routines
  typename containerT::mapped_type::const_iterator cb = it->second.begin();
  while (cb != it->second.end()) {
    (*cb)->execute(cbmanager, user_event, event, data);
    ++cb;
  }
}

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class CallbackManager
 *
 *---------------------------------------------------------------------------
 */

//--- Registering callbacks -------------------------------------------------

void CallbackManager::register_callback(event_t     event_type,
                                        CallbackPtr callback)
{
  switch (event_type) {
    case ANY:
      m_event_cbs[ENTER             ].push_back(callback);
      m_event_cbs[ENTER_CS          ].push_back(callback);
      m_event_cbs[EXIT              ].push_back(callback);
      m_event_cbs[MPI_COLLEXIT      ].push_back(callback);
      m_event_cbs[MPI_SEND          ].push_back(callback);
      m_event_cbs[MPI_SEND_REQUEST  ].push_back(callback);
      m_event_cbs[MPI_SEND_COMPLETE ].push_back(callback);
      m_event_cbs[MPI_RECV          ].push_back(callback);
      m_event_cbs[MPI_RECV_REQUEST  ].push_back(callback);
      m_event_cbs[MPI_RECV_COMPLETE ].push_back(callback);
      m_event_cbs[MPI_REQUEST_TESTED].push_back(callback);
      m_event_cbs[MPI_CANCELLED     ].push_back(callback);
      m_event_cbs[OMP_FORK          ].push_back(callback);
      m_event_cbs[OMP_JOIN          ].push_back(callback);
      m_event_cbs[OMP_COLLEXIT      ].push_back(callback);
      m_event_cbs[OMP_ALOCK         ].push_back(callback);
      m_event_cbs[OMP_RLOCK         ].push_back(callback);
      m_event_cbs[RMA_PUT_START     ].push_back(callback);
      m_event_cbs[RMA_PUT_END       ].push_back(callback);
      m_event_cbs[RMA_GET_START     ].push_back(callback);
      m_event_cbs[RMA_GET_END       ].push_back(callback);
      m_event_cbs[MPI_RMA_PUT_START ].push_back(callback);
      m_event_cbs[MPI_RMA_PUT_END   ].push_back(callback);
      m_event_cbs[MPI_RMA_GET_START ].push_back(callback);
      m_event_cbs[MPI_RMA_GET_END   ].push_back(callback);
      m_event_cbs[MPI_RMAEXIT       ].push_back(callback);
      m_event_cbs[MPI_RMACOLLEXIT   ].push_back(callback);
      m_event_cbs[MPI_RMA_LOCK      ].push_back(callback);
      m_event_cbs[MPI_RMA_UNLOCK    ].push_back(callback);
      break;

    case FLOW:
      m_event_cbs[ENTER             ].push_back(callback);
      m_event_cbs[ENTER_CS          ].push_back(callback);
      m_event_cbs[EXIT              ].push_back(callback);
      m_event_cbs[MPI_COLLEXIT      ].push_back(callback);
      m_event_cbs[OMP_COLLEXIT      ].push_back(callback);
      m_event_cbs[MPI_RMAEXIT       ].push_back(callback);
      m_event_cbs[MPI_RMACOLLEXIT   ].push_back(callback);
      break;

    case ENTER:
      m_event_cbs[ENTER             ].push_back(callback);
      m_event_cbs[ENTER_CS          ].push_back(callback);
      break;

    case EXIT:
      m_event_cbs[EXIT              ].push_back(callback);
      m_event_cbs[MPI_COLLEXIT      ].push_back(callback);
      m_event_cbs[OMP_COLLEXIT      ].push_back(callback);
      m_event_cbs[MPI_RMAEXIT       ].push_back(callback);
      m_event_cbs[MPI_RMACOLLEXIT   ].push_back(callback);
      break;

    case MPI_REQUEST:
      m_event_cbs[MPI_SEND_COMPLETE ].push_back(callback);
      m_event_cbs[MPI_RECV_REQUEST  ].push_back(callback);
      m_event_cbs[MPI_REQUEST_TESTED].push_back(callback);
      m_event_cbs[MPI_CANCELLED     ].push_back(callback);
      break;

    case MPI_P2P:
      m_event_cbs[MPI_SEND          ].push_back(callback);
      m_event_cbs[MPI_SEND_REQUEST  ].push_back(callback);
      m_event_cbs[MPI_RECV          ].push_back(callback);
      m_event_cbs[MPI_RECV_COMPLETE ].push_back(callback);
      break;

    case MPI_RECV:
      m_event_cbs[MPI_RECV          ].push_back(callback);
      m_event_cbs[MPI_RECV_COMPLETE ].push_back(callback);
      break;

    case MPI_SEND:
      m_event_cbs[MPI_SEND          ].push_back(callback);
      m_event_cbs[MPI_SEND_REQUEST  ].push_back(callback);
      break;

    case OMP_LOCK:
      m_event_cbs[OMP_ALOCK         ].push_back(callback);
      m_event_cbs[OMP_RLOCK         ].push_back(callback);
      break;

    default:
      m_event_cbs[event_type        ].push_back(callback);
      break;
  }
}


void CallbackManager::register_callback(int user_event, CallbackPtr callback)
{
  m_user_cbs[user_event].push_back(callback);
}


//--- Notification routines -------------------------------------------------

void CallbackManager::notify(const Event& event, CallbackData* data) const
{
  execute_callbacks(m_event_cbs, event->get_type(), *this, 0, event, data);
}


void CallbackManager::notify(int           user_event,
                             const Event&  event,
                             CallbackData* data) const
{
  execute_callbacks(m_user_cbs, user_event, *this, user_event, event, data);
}
