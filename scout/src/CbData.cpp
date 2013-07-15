/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>

#if defined(_MPI)
  #include <EventSet.h>
  #include <RemoteEventSet.h>
#endif   // _MPI

#include "CbData.h"


using namespace std;
using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * struct CbData
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor.
 *
 * Creates a new callback data object.
 */
CbData::CbData()
  : m_local(NULL),
    m_remote(NULL),
    m_defs(NULL),
    m_trace(NULL)
{
  #if defined(_MPI)
    m_local  = new EventSet;
    m_remote = new RemoteEventSet;
  #endif   // _MPI
}


/**
 * @brief Destructor.
 *
 * Frees up all occupied resources and destroys the instance.
 */
CbData::~CbData()
{
  #if defined(_MPI)
    delete m_local;
    delete m_remote;
  #endif   // _MPI
}


//--- Pre- and postprocessing -----------------------------------------------

/**
 * @brief Postprocessing after an event is handled.
 *
 * Updates a number of attributes after the given @a event has been handled.
 * In particular, the current callstack is updated and the local and remote
 * event sets are cleared.
 *
 * @param  event  Processed event
 */
void CbData::postprocess(const Event& event)
{
  // Update callstack
  if (event->is_typeof(ENTER))
    m_callstack.push(event);
  else if (event->is_typeof(EXIT))
    m_callstack.pop();

  // Clear event sets
  #if defined(_MPI)
    m_local->clear();
    m_remote->clear();
  #endif   // _MPI
}
