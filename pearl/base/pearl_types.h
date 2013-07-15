/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_TYPES_H
#define PEARL_TYPES_H


#include <stdint.h>


/*-------------------------------------------------------------------------*/
/**
 * @file    pearl_types.h
 * @ingroup PEARL_base
 * @brief   Definition of data types and constants.
 *
 * This header file provides definitions of PEARL-related data types and
 * constants.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Type definitions ------------------------------------------------------

/// Data type for timestamp values
typedef double timestamp_t;

/// Data type for identifiers
typedef uint32_t ident_t;

/// Data type for line number information
typedef uint32_t line_t;

/// Enumeration defining available event types
enum event_t {
  ANY,              // i.e. any of the event types defined below

  // Generic event types
  FLOW,             // i.e. ENTER, ENTER_CS, EXIT, MPI_COLLEXIT or OMP_COLLEXIT
  ENTER,
  ENTER_CS,
  EXIT,

  // MPI-1 specific event types
  MPI_COLLEXIT,
  MPI_REQUEST,
  MPI_P2P,             // i.e. MPI_SEND or MPI_RECV
  MPI_SEND,
  MPI_SEND_REQUEST,    // nonblocking send request
  MPI_SEND_COMPLETE,   // nonblocking send completion
  MPI_RECV,
  MPI_RECV_REQUEST,    // nonblocking receive request
  MPI_RECV_COMPLETE,   // nonblocking receive completion
  MPI_REQUEST_TESTED,  // unsuccessful test of request
  MPI_CANCELLED,       // cancel of request

  // OpenMP specific event types
  OMP_FORK,
  OMP_JOIN,
  OMP_COLLEXIT,
  OMP_LOCK,         // i.e. OMP_ALOCK or OMP_RLOCK
  OMP_ALOCK,
  OMP_RLOCK,

  // RMA specific event types
  RMA,
  RMA_PUT_START,
  RMA_PUT_END,
  RMA_GET_START,
  RMA_GET_END,

  // MPI-2 RMA specific event types
  MPI_RMA,
  MPI_RMA_PUT_START,
  MPI_RMA_PUT_END,

  MPI_RMA_GET_START,
  MPI_RMA_GET_END,

  MPI_RMAEXIT,
  MPI_RMACOLLEXIT,

  MPI_RMA_LOCK,
  MPI_RMA_UNLOCK,

  NUM_EVENT_TYPES   // symbolic name to retrieve the number of event types
};


//--- Constants -------------------------------------------------------------

/// Constant used to identify unused/unknown identifier fields
const ident_t PEARL_NO_ID = 0xFFFFFFFF;

/// Constant used to identify unused/unknown number fields
const line_t  PEARL_NO_NUM = 0;


}   /* namespace pearl */


#endif   /* !PEARL_TYPES_H */
