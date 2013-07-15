/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SILAS_REPLAY_H
#define SILAS_REPLAY_H

#include "CallbackManager.h"
#include "LocalTrace.h"
#include "ReenactCallbackData.h"

/*-------------------------------------------------------------------------*/
/**
 * @file    silas_replay.h
 * @brief   Declarations of replay-related library functions.
 *
 * This header file provides declarations of all replay-related library
 * functions provided by SILAS.
 */
/*-------------------------------------------------------------------------*/


namespace silas
{

//--- Function prototypes ---------------------------------------------------

/// @name Replay functionality
/// @{

extern void SILAS_forward_replay (const pearl::LocalTrace&      trace,
                                  const pearl::CallbackManager& cbmanager,
                                  CallbackData*          data,
                                  int   mincbevent);
extern void SILAS_backward_replay(const pearl::LocalTrace&      trace,
                                  const pearl::CallbackManager& cbmanager,
                                  CallbackData*                 data,
                                  int mincbevent);

/// @}


}   /* namespace silas */


#endif   /* !SILAS_REPLAY_H */
