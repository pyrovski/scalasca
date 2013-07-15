/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "CallbackData.h"
#include "CallbackManager.h"
#include "pearl_replay.h"

using namespace pearl;


//--- Replay functionality --------------------------------------------------

void pearl::PEARL_forward_replay(const LocalTrace&           trace,
                                 const CallbackManager&      cbmanager,
                                 CallbackData*               data,
                                 const LocalTrace::iterator& begin,
                                 const LocalTrace::iterator& end)
{
  LocalTrace::iterator it = begin;

  if (data) {
    while (it != end) {
      data->preprocess(it);
      cbmanager.notify(it, data);
      data->postprocess(it);

      ++it;
    }
  } else {
    while (it != end) {
      cbmanager.notify(it, data);

      ++it;
    }
  }
}


void pearl::PEARL_backward_replay(const LocalTrace&                   trace,
                                  const CallbackManager&              cbmanager,
                                  CallbackData*                       data,
                                  const LocalTrace::reverse_iterator& rbegin,
                                  const LocalTrace::reverse_iterator& rend)
{
  LocalTrace::reverse_iterator it = rbegin;

  if (data) {
    while (it != rend) {
      LocalTrace::iterator event = (++it).base();

      data->preprocess(event);
      cbmanager.notify(event, data);
      data->postprocess(event);
    }
  } else {
    while (it != rend) {
      LocalTrace::iterator event = (++it).base();

      cbmanager.notify(event, data);
    }
  }
}
