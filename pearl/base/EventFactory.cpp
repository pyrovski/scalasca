/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>
#ifdef PEARL_ENABLE_METRICS
  #include <cstring>
#endif

#include "Enter_rep.h"
#include "EnterCS_rep.h"
#include "Error.h"
#include "EventFactory.h"
#include "Exit_rep.h"
#include "GlobalDefs.h"
#include "RmaGetEnd_rep.h"
#include "RmaGetStart_rep.h"
#include "RmaPutEnd_rep.h"
#include "RmaPutStart_rep.h"

using namespace std;
using namespace pearl;


//--- Static member variables -----------------------------------------------

const EventFactory* EventFactory::m_instance = NULL;


/*
 *---------------------------------------------------------------------------
 *
 * class EventFactory
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

EventFactory::~EventFactory()
{
}


//--- Generic factory methods -----------------------------------------------

Event_rep* EventFactory::createEnter(const GlobalDefs& defs,
                                     timestamp_t       time,
                                     ident_t           region,
                                     uint64_t*         metrics) const
{
  return new Enter_rep(time,
                       defs.get_region(region),
                       copyMetrics(defs, metrics));
}


Event_rep* EventFactory::createEnterCS(const GlobalDefs& defs,
                                       timestamp_t       time,
                                       ident_t           csite,
                                       uint64_t*         metrics) const
{
  return new EnterCS_rep(time,
                         defs.get_callsite(csite),
                         copyMetrics(defs, metrics));
}


Event_rep* EventFactory::createExit(const GlobalDefs& defs,
                                    timestamp_t       time,
                                    uint64_t*         metrics) const
{
  return new Exit_rep(time,
                      copyMetrics(defs, metrics));
}


//--- MPI-1 specific factory methods ----------------------------------------

Event_rep* EventFactory::createMpiSend(const GlobalDefs& defs,
                                       timestamp_t       time,
                                       ident_t           comm,
                                       uint32_t          dest,
                                       uint32_t          tag,
                                       uint32_t          sent) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRecv(const GlobalDefs& defs,
                                       timestamp_t       time,
                                       ident_t           comm,
                                       uint32_t          source,
                                       uint32_t          tag) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiCollExit(const GlobalDefs& defs,
                                           timestamp_t       time,
                                           ident_t           comm,
                                           uint32_t          root,
                                           uint32_t          sent,
                                           uint32_t          recvd,
                                           uint64_t*         metrics) const
{
  return new Exit_rep(time,
                      copyMetrics(defs, metrics));
}


//--- MPI-1 non-blocking specific methods ------------------------------------

Event_rep* EventFactory::createMpiCancelled(const GlobalDefs& defs,
                                            timestamp_t       time,
                                            ident_t           reqid) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiSendRequest(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              ident_t           comm,
                                              uint32_t          dest,
                                              uint32_t          tag,
                                              uint32_t          sent,
                                              ident_t           reqid) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiSendComplete(const GlobalDefs& defs,
                                               timestamp_t       time,
                                               ident_t           reqid) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRecvComplete(const GlobalDefs& defs,
                                               timestamp_t       time,
                                               ident_t           comm,
                                               uint32_t          source,
                                               uint32_t          tag,
                                               ident_t           reqid) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRecvRequest(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              ident_t           reqid) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRequestTested(const GlobalDefs& defs,
                                                timestamp_t       time,
                                                ident_t           reqid) const
{
  return NULL;
}


//--- OpenMP specific factory methods ----------------------------------------

Event_rep* EventFactory::createOmpFork(const GlobalDefs& defs,
                                       timestamp_t       time) const
{
  return NULL;
}


Event_rep* EventFactory::createOmpJoin(const GlobalDefs& defs,
                                       timestamp_t       time) const
{
  return NULL;
}


Event_rep* EventFactory::createOmpCollExit(const GlobalDefs& defs,
					   timestamp_t       time,
					   uint64_t*         metrics) const
{
  return new Exit_rep(time,
                      copyMetrics(defs, metrics));
}


Event_rep* EventFactory::createOmpALock(const GlobalDefs& defs,
                                        timestamp_t       time,
                                        ident_t           lkid) const
{
  return NULL;
}


Event_rep* EventFactory::createOmpRLock(const GlobalDefs& defs,
                                        timestamp_t       time,
                                        ident_t           lkid) const
{
  return NULL;
}


//--- RMA factory methods ------------------------------------------

Event_rep* EventFactory::createRmaPutStart(const GlobalDefs& defs,
                                           timestamp_t       time,
                                           uint32_t          rma_id,
                                           uint32_t          target,
                                           uint32_t          length) const
{
  return new RmaPutStart_rep(time, rma_id, target, length);
}


Event_rep* EventFactory::createRmaPutEnd  (const GlobalDefs& defs,
                                           timestamp_t       time,
                                           uint32_t          rma_id) const
{
  return new RmaPutEnd_rep(time, rma_id);
}


Event_rep* EventFactory::createRmaGetStart(const GlobalDefs& defs,
                                           timestamp_t       time,
                                           uint32_t          rma_id,
                                           uint32_t          origin,
                                           uint32_t          length) const
{
  return new RmaGetStart_rep(time, rma_id, origin, length);
}


Event_rep* EventFactory::createRmaGetEnd  (const GlobalDefs& defs,
                                           timestamp_t       time,
                                           uint32_t          rma_id) const
{
  return new RmaGetEnd_rep(time, rma_id);
}


//--- MPI-2 RMA specific factory methods ----------------------------------------

Event_rep* EventFactory::createMpiRmaPutStart(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              uint32_t          rma_id,
                                              uint32_t          target,
                                              uint32_t          length,
                                              ident_t           window) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRmaPutEnd(const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          rma_id) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRmaGetStart(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              uint32_t          rma_id,
                                              uint32_t          origin,
                                              uint32_t          length,
                                              ident_t           window) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRmaGetEnd(const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          rma_id) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRmaLock(const GlobalDefs& defs,
                                          timestamp_t       time,
                                          uint32_t          location,
                                          ident_t           window,
                                          bool              sync) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRmaUnlock(const GlobalDefs& defs,
                                            timestamp_t       time,
                                            uint32_t          location,
                                            ident_t           window) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRmaExit(const GlobalDefs& defs,
                                          timestamp_t       time,
                                          ident_t           window,
                                          ident_t           group,
                                          bool              sync,
                                          uint64_t*         metrics) const
{
  return NULL;
}


Event_rep* EventFactory::createMpiRmaCollExit(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              ident_t           window,
                                              uint64_t*         metrics) const
{
  return NULL;
}


//--- Buffer-based factory methods ------------------------------------------

Event_rep* EventFactory::createEvent(event_t           type,
                                     const GlobalDefs& defs,
                                     Buffer&           buffer) const
{
  Event_rep* result = NULL;

  switch (type) {
    case ENTER:
      result = new Enter_rep(defs, buffer);
      break;

    case ENTER_CS:
      result = new EnterCS_rep(defs, buffer);
      break;

    case EXIT:
      result = new Exit_rep(defs, buffer);
      break;

    case RMA_PUT_START:
      result = new RmaPutStart_rep(defs, buffer);
      break;

    case RMA_PUT_END:
      result = new RmaPutEnd_rep(defs, buffer);
      break;

    case RMA_GET_START:
      result = new RmaGetStart_rep(defs, buffer);
      break;

    case RMA_GET_END:
      result = new RmaGetEnd_rep(defs, buffer);
      break;

    default:
      throw RuntimeError("EventFactory::createEvent() -- Unknown event type.");
  }

  return result;
}


//--- Singleton interface ---------------------------------------------------

void EventFactory::registerFactory(const EventFactory* factory)
{
  assert(factory);

  if (m_instance)
    throw FatalError("EventFactory::registerFactory(const EventFactory*) -- "
                     "Factory already registered.");

  m_instance = factory;
}


//--- Data copy methods -----------------------------------------------------

uint64_t* EventFactory::copyMetrics(const GlobalDefs& defs,
                                    uint64_t*         metrics) const
{
  uint64_t* result = NULL;

#ifdef PEARL_ENABLE_METRICS
  // Copy hardware counter metric values
  uint32_t count = defs.num_metrics();
  if (count > 0) {
    result = new uint64_t[count];
    memcpy(result, metrics, count * sizeof(uint64_t));
  }
#endif

  return result;
}
