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

#include <CallbackManager.h>
#include <LocalTrace.h>

//#include "ReenactCallbackData.h"
#include "silas_events.h"
#include "silas_replay.h"

using namespace silas;


//--- Replay functionality --------------------------------------------------

void silas::SILAS_forward_replay(const pearl::LocalTrace&      trace,
                                 const pearl::CallbackManager& cbmanager,
                                 CallbackData*          data,
                                 int                    mincbevent)
{
  pearl::LocalTrace::iterator it = trace.begin();

  if (data) {
    while (it != trace.end()) {
      int action = data->get_action(it);
      if (action >= mincbevent) {
        data->preprocess(it);
        cbmanager.notify(action, it, data);
        data->postprocess(it);
      }

      ++it;
    }
  } else {
    while (it != trace.end()) {
      cbmanager.notify(it, data);

      ++it;
    }
  }
}


void silas::SILAS_backward_replay(const pearl::LocalTrace&      trace,
                                  const pearl::CallbackManager& cbmanager,
                                  CallbackData*          data,
                                  int mincbevent)
{
  pearl::LocalTrace::reverse_iterator it = trace.rbegin();

  if (data) {
    while (it != trace.rend()) {
      pearl::LocalTrace::iterator event = ++it.base();

      data->preprocess(event);
      cbmanager.notify(event, data);
      data->postprocess(event);  
    }
  } else {
    while (it != trace.rend()) {
      pearl::LocalTrace::iterator event = ++it.base();

      cbmanager.notify(event, data);
    }
  }
}
