/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Error.h"
#include "GlobalDefs.h"
#include "MpiCancelled_rep.h"
#include "MpiCollExit_rep.h"
#include "MpiComm.h"
#include "MpiEventFactory.h"
#include "MpiGroup.h"
#include "MpiReceive_rep.h"
#include "MpiReceiveComplete_rep.h"
#include "MpiReceiveRequest_rep.h"
#include "MpiRequestTested_rep.h"
#include "MpiRmaCollExit_rep.h"
#include "MpiRmaExit_rep.h"
#include "MpiRmaGetEnd_rep.h"
#include "MpiRmaGetStart_rep.h"
#include "MpiRmaLock_rep.h"
#include "MpiRmaPutEnd_rep.h"
#include "MpiRmaPutStart_rep.h"
#include "MpiRmaUnlock_rep.h"
#include "MpiSend_rep.h"
#include "MpiSendRequest_rep.h"
#include "MpiSendComplete_rep.h"
#include "MpiWindow.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiEventFactory
 *
 *---------------------------------------------------------------------------
 */

//--- MPI-1 specific factory methods ----------------------------------------

Event_rep* MpiEventFactory::createMpiSend(const GlobalDefs& defs,
                                          timestamp_t       time,
                                          ident_t           comm,
                                          uint32_t          dest,
                                          uint32_t          tag,
                                          uint32_t          sent) const
{
  // Determine communicator
  MpiComm* communicator = dynamic_cast<MpiComm*>(defs.get_comm(comm));
  if (!communicator)
    throw RuntimeError("MpiEventFactory::createMpiSend() -- "
                       "Invalid MPI communicator ID.");

  return new MpiSend_rep(time,
                         communicator,
                         dest,
                         tag,
                         sent);
}


Event_rep* MpiEventFactory::createMpiRecv(const GlobalDefs& defs,
                                          timestamp_t       time,
                                          ident_t           comm,
                                          uint32_t          source,
                                          uint32_t          tag) const
{
  // Determine communicator
  MpiComm* communicator = dynamic_cast<MpiComm*>(defs.get_comm(comm));
  if (!communicator)
    throw RuntimeError("MpiEventFactory::createMpiRecv() -- "
                       "Invalid MPI communicator ID.");

  return new MpiReceive_rep(time,
                            communicator,
                            source,
                            tag);
}


Event_rep* MpiEventFactory::createMpiCollExit(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              ident_t           comm,
                                              uint32_t          root,
                                              uint32_t          sent,
                                              uint32_t          recvd,
                                              uint64_t*         metrics) const
{
  // Determine communicator
  MpiComm* communicator = dynamic_cast<MpiComm*>(defs.get_comm(comm));
  if (!communicator)
    throw RuntimeError("MpiEventFactory::createMpiCollExit() -- "
                       "Invalid MPI communicator ID.");

  return new MpiCollExit_rep(time,
                             communicator,
                             root,
                             sent,
                             recvd,
                             copyMetrics(defs, metrics));
}

//--- MPI-1 Non-blocking specific factory methods -------------------------------

Event_rep* MpiEventFactory::createMpiCancelled(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              ident_t           reqid) const
{
  return new MpiCancelled_rep(time, reqid);
}


Event_rep* MpiEventFactory::createMpiSendRequest(const GlobalDefs& defs,
                                                 timestamp_t       time,
                                                 ident_t           comm,
                                                 uint32_t          dest,
                                                 uint32_t          tag,
                                                 uint32_t          sent,
                                                 ident_t           reqid) const
{
  // Determine communicator
  MpiComm* communicator = dynamic_cast<MpiComm*>(defs.get_comm(comm));
  if (!communicator)
    throw RuntimeError("MpiEventFactory::createMpiSend() -- "
                       "Invalid MPI communicator ID.");

  return new MpiSendRequest_rep(time,
                           communicator,
                           dest,
                           tag,
                           sent,
                           reqid);
}


Event_rep* MpiEventFactory::createMpiSendComplete(const GlobalDefs& defs,
                                                  timestamp_t       time,
                                                  ident_t           reqid) const
{
  return new MpiSendComplete_rep(time, reqid);
}


Event_rep* MpiEventFactory::createMpiRecvComplete(const GlobalDefs& defs,
                                                  timestamp_t       time,
                                                  ident_t           comm,
                                                  uint32_t          source,
                                                  uint32_t          tag,
                                                  ident_t           reqid) const
{
  // Determine communicator
  MpiComm* communicator = dynamic_cast<MpiComm*>(defs.get_comm(comm));
  if (!communicator)
    throw RuntimeError("MpiEventFactory::createMpiRecv() -- "
                       "Invalid MPI communicator ID.");

  return new MpiReceiveComplete_rep(time,
                              communicator,
                              source,
                              tag,
                              reqid);
}


Event_rep* MpiEventFactory::createMpiRecvRequest(const GlobalDefs& defs,
                                                 timestamp_t       time,
                                                 ident_t           reqid) const
{
  return new MpiReceiveRequest_rep(time, reqid);
}


Event_rep* MpiEventFactory::createMpiRequestTested(const GlobalDefs& defs,
                                                   timestamp_t       time,
                                                   ident_t           reqid) const
{
  return new MpiRequestTested_rep(time, reqid);
}


//--- MPI-2 RMA specific factory methods ----------------------------------------

Event_rep* MpiEventFactory::createMpiRmaPutStart(const GlobalDefs& defs,
                                                 timestamp_t       time,
                                                 uint32_t          rma_id,
                                                 uint32_t          target,
                                                 uint32_t          length,
                                                 ident_t           window) const
{
  MpiWindow* win = dynamic_cast<MpiWindow*>(defs.get_window(window));
  if (!win)
    throw RuntimeError("MpiEventFactory::createMpiRmaPutStart() -- "
                       "Invalid MPI window ID.");

  return new MpiRmaPutStart_rep(time,
                                rma_id,
                                target,
                                length,
                                win);
}


Event_rep* MpiEventFactory::createMpiRmaPutEnd(const GlobalDefs& defs,
                                               timestamp_t       time,
                                               uint32_t          rma_id) const
{
  return new MpiRmaPutEnd_rep(time, rma_id);
}


Event_rep* MpiEventFactory::createMpiRmaGetStart(const GlobalDefs& defs,
                                                 timestamp_t       time,
                                                 uint32_t          rma_id,
                                                 uint32_t          origin,
                                                 uint32_t          length,
                                                 ident_t           window) const
{
  MpiWindow* win = dynamic_cast<MpiWindow*>(defs.get_window(window));
  if (!win)
    throw RuntimeError("MpiEventFactory::createMpiRmaGetStart() -- "
                       "Invalid MPI window ID.");

  return new MpiRmaGetStart_rep(time,
                                rma_id,
                                origin,
                                length,
                                win);
}


Event_rep* MpiEventFactory::createMpiRmaGetEnd(const GlobalDefs& defs,
                                               timestamp_t       time,
                                               uint32_t          rma_id) const
{
  return new MpiRmaGetEnd_rep(time, rma_id);
}


Event_rep* MpiEventFactory::createMpiRmaExit(const GlobalDefs& defs,
                                             timestamp_t       time,
                                             ident_t           window,
                                             ident_t           group,
                                             bool              sync,
                                             uint64_t*         metrics) const
{
  MpiWindow* win = dynamic_cast<MpiWindow*>(defs.get_window(window));
  if (!win)
    throw RuntimeError("MpiEventFactory::createMpiRmaExit() -- "
                       "Invalid MPI window ID.");
  MpiGroup*  grp = dynamic_cast<MpiGroup*>(defs.get_group(group));
  if (!grp)
    throw RuntimeError("MpiEventFactory::createMpiRmaExit() -- "
                       "Invalid MPI group ID.");

  return new MpiRmaExit_rep(time,
                            win,
                            grp,
                            sync,
                            copyMetrics(defs, metrics));
}


Event_rep* MpiEventFactory::createMpiRmaCollExit(const GlobalDefs& defs,
                                                 timestamp_t       time,
                                                 ident_t           window,
                                                 uint64_t*         metrics) const
{
  MpiWindow* win = dynamic_cast<MpiWindow*>(defs.get_window(window));
  if (!win)
    throw RuntimeError("MpiEventFactory::createMpiRmaCollExit() -- "
                       "Invalid MPI window ID.");

  return new MpiRmaCollExit_rep(time, win, copyMetrics(defs, metrics));
}


Event_rep* MpiEventFactory::createMpiRmaLock(const GlobalDefs& defs,
                                             timestamp_t       time,
                                             uint32_t          location,
                                             ident_t           window,
                                             bool              exclusive) const
{
  MpiWindow* win = dynamic_cast<MpiWindow*>(defs.get_window(window));
  if (!win)
    throw RuntimeError("MpiEventFactory::createMpiRmaLock() -- "
                       "Invalid MPI window ID.");

  return new MpiRmaLock_rep(time, location, win, exclusive);
}


Event_rep* MpiEventFactory::createMpiRmaUnlock(const GlobalDefs& defs,
                                               timestamp_t       time,
                                               uint32_t          location,
                                               ident_t           window) const
{
  MpiWindow* win = dynamic_cast<MpiWindow*>(defs.get_window(window));
  if (!win)
    throw RuntimeError("MpiEventFactory::createMpiRmaUnlock() -- "
                       "Invalid MPI window ID.");

  return new MpiRmaUnlock_rep(time, location, win);
}


//--- Buffer-based factory methods ------------------------------------------

Event_rep* MpiEventFactory::createEvent(event_t           type,
                                        const GlobalDefs& defs,
                                        Buffer&           buffer) const
{
  Event_rep* result = NULL;

  switch (type) {
    case MPI_SEND:
      result = new MpiSend_rep(defs, buffer);
      break;

    case MPI_SEND_REQUEST:
      result = new MpiSendRequest_rep(defs, buffer);
      break;

    case MPI_SEND_COMPLETE:
      result = new MpiSendComplete_rep(defs, buffer);
      break;

    case MPI_RECV:
      result = new MpiReceive_rep(defs, buffer);
      break;

    case MPI_RECV_REQUEST:
      result = new MpiReceiveRequest_rep(defs, buffer);
      break;

    case MPI_RECV_COMPLETE:
      result = new MpiReceiveComplete_rep(defs, buffer);
      break;

    case MPI_REQUEST_TESTED:
      result = new MpiRequestTested_rep(defs, buffer);
      break;

    case MPI_CANCELLED:
      result = new MpiCancelled_rep(defs, buffer);
      break;

    case MPI_COLLEXIT:
      result = new MpiCollExit_rep(defs, buffer);
      break;

    case MPI_RMA_PUT_START:
      result = new MpiRmaPutStart_rep(defs, buffer);
      break;

    case MPI_RMA_PUT_END:
      result = new MpiRmaPutEnd_rep(defs, buffer);
      break;

    case MPI_RMA_GET_START:
      result = new MpiRmaGetStart_rep(defs, buffer);
      break;

    case MPI_RMA_GET_END:
      result = new MpiRmaGetEnd_rep(defs, buffer);
      break;

    case MPI_RMAEXIT:
      result = new MpiRmaExit_rep(defs, buffer);
      break;

    case MPI_RMACOLLEXIT:
      result = new MpiRmaCollExit_rep(defs, buffer);
      break;

    case MPI_RMA_LOCK:
      result = new MpiRmaLock_rep(defs, buffer);
      break;

    case MPI_RMA_UNLOCK:
      result = new MpiRmaUnlock_rep(defs, buffer);
      break;

    default:
      result = EventFactory::createEvent(type, defs, buffer);
      break;
  }

  return result;
}
