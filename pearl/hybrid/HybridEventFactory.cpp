/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>

#include "HybridEventFactory.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class HybridEventFactory
 *
 *---------------------------------------------------------------------------
 */


//--- Buffer-based factory methods ------------------------------------------

Event_rep* HybridEventFactory::createEvent(event_t           type,
                                           const GlobalDefs& defs,
                                           Buffer&           buffer) const
{
  Event_rep* result = NULL;
  
  switch (type) {
    case MPI_COLLEXIT:
    case MPI_SEND:
    case MPI_SEND_REQUEST:
    case MPI_SEND_COMPLETE:
    case MPI_RECV:
    case MPI_RECV_REQUEST:
    case MPI_RECV_COMPLETE:
    case MPI_REQUEST_TESTED:
    case MPI_CANCELLED:
    case MPI_RMA_PUT_START:
    case MPI_RMA_PUT_END:
    case MPI_RMA_GET_START:
    case MPI_RMA_GET_END:
    case MPI_RMAEXIT:
    case MPI_RMACOLLEXIT:
    case MPI_RMA_LOCK:
    case MPI_RMA_UNLOCK:
      result = MpiEventFactory::createEvent(type, defs, buffer);
      break;

    case OMP_FORK:
    case OMP_JOIN:
    case OMP_RLOCK:
    case OMP_ALOCK:
    case OMP_COLLEXIT:
      result = OmpEventFactory::createEvent(type, defs, buffer);
      break;

  default:
    result = EventFactory::createEvent(type, defs, buffer);
    break;
  }
  
  return result;
}
