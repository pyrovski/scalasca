/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Logging.h"
#include "TimedPhase.h"

using namespace std;
using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class TimedPhase
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/*
 * @brief Creates a new timed phase.
 *
 * Creates a new timed phase which print use the given log @a message when
 * it is started.
 *
 * @param  message  Log message to be printed when phase starts
 */
TimedPhase::TimedPhase(const string& message)
  : CompoundTask(),
    m_message(message)
{
}


//--- Execution control (protected) -----------------------------------------

/**
 * @brief Task preparation.
 *
 * The prepare() callback method is called before execution of the subtasks
 * starts. In case of a timed phase, it will print the initial log message
 * provided on phase creation and start the timer.
 *
 * @return Always @em true
 */
bool TimedPhase::prepare()
{
  LogMsg(0, "%s ... ", m_message.c_str());
  m_timer.start();

  return true;
}


/**
 * @brief Task clean up.
 *
 * The finish() callback method is called after the execution of the subtasks
 * finishes. In case of a timed phase, it will stop the timer and print the
 * "done" message, followed by the wall-clock duration of the pase.
 *
 * @return Always @em true
 */
bool TimedPhase::finish()
{
  m_timer.stop();
  LogMsg(0, "done (%.3fs).\n", m_timer.value());

  return true;
}
