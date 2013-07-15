/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_REPLAY_H
#define PEARL_REPLAY_H


#include "LocalTrace.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    pearl_replay.h
 * @ingroup PEARL_replay
 * @brief   Declarations of replay-related library functions.
 *
 * This header file provides declarations of all replay-related library
 * functions provided by PEARL.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CallbackData;
class CallbackManager;


//--- Function prototypes ---------------------------------------------------

/// @name Replay functionality
/// @{

extern void PEARL_forward_replay (const LocalTrace&           trace,
                                  const CallbackManager&      cbmanager,
                                  CallbackData*               data,
                                  const LocalTrace::iterator& begin,
                                  const LocalTrace::iterator& end);
inline void PEARL_forward_replay (const LocalTrace&           trace,
                                  const CallbackManager&      cbmanager,
                                  CallbackData*               data);


extern void PEARL_backward_replay(const LocalTrace&                   trace,
                                  const CallbackManager&              cbmanager,
                                  CallbackData*                       data,
                                  const LocalTrace::reverse_iterator& rbegin,
                                  const LocalTrace::reverse_iterator& rend);
inline void PEARL_backward_replay(const LocalTrace&                   trace,
                                  const CallbackManager&              cbmanager,
                                  CallbackData*                       data);

/// @}


//--- Inline functions ------------------------------------------------------

inline void PEARL_forward_replay (const LocalTrace&      trace,
                                  const CallbackManager& cbmanager,
                                  CallbackData*          data)
{
  PEARL_forward_replay(trace, cbmanager, data, trace.begin(), trace.end());
}


inline void PEARL_backward_replay(const LocalTrace&      trace,
                                  const CallbackManager& cbmanager,
                                  CallbackData*          data)
{
  PEARL_backward_replay(trace, cbmanager, data, trace.rbegin(), trace.rend());
}


}   /* namespace pearl */


#endif   /* !PEARL_REPLAY_H */
