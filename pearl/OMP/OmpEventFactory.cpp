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
#include "OmpEventFactory.h"
#include "OmpFork_rep.h"
#include "OmpJoin_rep.h"
#include "OmpCollExit_rep.h"
#include "OmpALock_rep.h"
#include "OmpRLock_rep.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class OmpEventFactory
 *
 *---------------------------------------------------------------------------
 */

//--- OpenMP specific factory methods ----------------------------------------

Event_rep* OmpEventFactory::createOmpFork(const GlobalDefs& defs,
                                          timestamp_t       time) const 
{
  return new OmpFork_rep(time);
}


Event_rep* OmpEventFactory::createOmpJoin(const GlobalDefs& defs,
                                          timestamp_t       time) const 
{
  return new OmpJoin_rep(time);
}


Event_rep* OmpEventFactory::createOmpCollExit(const GlobalDefs& defs,
                                              timestamp_t       time,
                                              uint64_t*         metrics) const 
{
  return new OmpCollExit_rep(time, copyMetrics(defs, metrics));
}


Event_rep* OmpEventFactory::createOmpALock(const GlobalDefs& defs,
                                           timestamp_t       time,
                                           ident_t           lockid) const
{
  return new OmpALock_rep(time, lockid);
}


Event_rep* OmpEventFactory::createOmpRLock(const GlobalDefs& defs,
                                           timestamp_t       time,
                                           ident_t           lockid) const 
{
  return new OmpRLock_rep(time, lockid);
}


//--- Buffer-based factory methods ------------------------------------------

Event_rep* OmpEventFactory::createEvent(event_t           type,
                                        const GlobalDefs& defs,
                                        Buffer&           buffer) const
{
  Event_rep* result = NULL;

  switch (type) {
    case OMP_FORK:
      result = new OmpFork_rep(defs, buffer);
      break;

    case OMP_JOIN:
      result = new OmpJoin_rep(defs, buffer);
      break;

    case OMP_COLLEXIT:
      result = new OmpCollExit_rep(defs, buffer);
      break;

    case OMP_ALOCK:
      result = new OmpALock_rep(defs, buffer);
      break;

    case OMP_RLOCK:
      result = new OmpRLock_rep(defs, buffer);
      break;

    default:
      result = EventFactory::createEvent(type, defs, buffer);
      break;
  }

  return result;
}
