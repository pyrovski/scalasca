/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "ReplayTask.h"
#include "pearl_replay.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class ReplayTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Destructor. Releases the callback manager and destroys the instance.
 */
ReplayTask::~ReplayTask()
{
    delete m_cbmanager;
}


//--- Executing the task ----------------------------------------------------

/**
 * Executes the prepare(), replay() and finish() methods (in this order). If
 * any of those indicate a failure, the task will be aborted immediately.
 *
 * @return @em True all steps were successful, @em false otherwise.
 */
bool ReplayTask::execute()
{
  if (!prepare())
    return false;
  if (!replay())
    return false;
  if (!finish())
    return false;

  return true;
}


//--- Constructors & destructor (protected) ---------------------------------

/**
 * Constructor. Initializes internal data.
 *
 * The constructor creates a default callback manager of type
 * pearl::CallbackManager. You can specify a different callback manager
 * by passing a non-null pointer to a callback manager object via the
 * @a cbmanager argument. Note that ReplayTask takes ownership of the
 * callback manager object, i.e., it will be freed automatically in the
 * ReplayTask destructor.
 *
 * @param  defs       Reference to global definition object
 * @param  trace      Reference to the trace object the replay will be
 *                    performed on
 * @param  cbdata     Pointer to callback data object to be passed to the
 *                    callback functions (default 0)
 * @param  cbmanager  Pointer to callback manager object (default 0)
 */
ReplayTask::ReplayTask(const GlobalDefs& defs,
                       LocalTrace&       trace,
                       CallbackData*     cbdata,
                       CallbackManager*  cbmanager)
  : Task(),
    m_defs(defs),
    m_trace(trace),
    m_cbdata(cbdata),
    m_cbmanager(cbmanager)
{
  if (!m_cbmanager)
    m_cbmanager = new CallbackManager;
}


//--- Replay control --------------------------------------------------------

/**
 * The prepare() callback method is called before trace replay starts.
 * You can overwrite it if you need to perform any initialization steps 
 * immediately before the replay. Trace replay will only be executed if
 * prepare() returns successfully.
 *
 * The default implementation does nothing and returns successfully.
 *
 * @return Always @em true
 */

bool ReplayTask::prepare()
{
  return true;
}


/**
 * The finish() callback method is called after trace replay finishes.
 * You can overwrite it if you need to perform any postprocessing steps 
 * immediately after the replay. It will only be called if the replay
 * was successful.
 *
 * The default implementation does nothing and returns successfully.
 *
 * @return Always @em true
 */
bool ReplayTask::finish()
{
    return true;
}


/*
 *---------------------------------------------------------------------------
 *
 * class ForwardReplayTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor (protected) ---------------------------------

/**
 * Constructor. Initializes internal data.
 *
 * The constructor creates a default callback manager of type
 * pearl::CallbackManager. You can specify a different callback manager
 * by passing a non-null pointer to a callback manager object via the
 * @a cbmanager argument. Note that ForwardReplayTask takes ownership of
 * the callback manager object, i.e., it will be freed automatically in the
 * ForwardReplayTask destructor.
 *
 * @param  defs       Reference to global definition object
 * @param  trace      Reference to the trace object the replay will be
 *                    performed on
 * @param  cbdata     Pointer to callback data object to be passed to the
 *                    callback functions (default 0)
 * @param  cbmanager  Pointer to callback manager object (default 0)
 */
ForwardReplayTask::ForwardReplayTask(const GlobalDefs& defs,
                                     LocalTrace&       trace,
                                     CallbackData*     cbdata,
                                     CallbackManager*  cbmanager)
  : ReplayTask(defs, trace, cbdata, cbmanager)
{
}


//--- Replay control (protected) --------------------------------------------

/**
 * Executes a forward replay, i.e., iterates over the events of the
 * associated trace from the beginning to the end and executes the
 * corresponding callbacks registered with the callback manager.
 *
 * @return  Always @em true
 */
bool ForwardReplayTask::replay()
{
  PEARL_forward_replay(m_trace, *m_cbmanager, m_cbdata);

  return true;
}


/*
 *---------------------------------------------------------------------------
 *
 * class BackwardReplayTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor (protected) ---------------------------------

/**
 * Constructor. Initializes internal data.
 *
 * The constructor creates a default callback manager of type
 * pearl::CallbackManager. You can specify a different callback manager
 * by passing a non-null pointer to a callback manager object via the
 * @a cbmanager argument. Note that BackwardReplayTask takes ownership of
 * the callback manager object, i.e., it will be freed automatically in the
 * BackwardReplayTask destructor.
 *
 * @param  defs       Reference to global definition object
 * @param  trace      Reference to the trace object the replay will be
 *                    performed on
 * @param  cbdata     Pointer to callback data object to be passed to the
 *                    callback functions (default 0)
 * @param  cbmanager  Pointer to callback manager object (default 0)
 */
BackwardReplayTask::BackwardReplayTask(const GlobalDefs& defs,
                                       LocalTrace&       trace,
                                       CallbackData*     cbdata,
                                       CallbackManager*  cbmanager)
  : ReplayTask(defs, trace, cbdata, cbmanager)
{
}


//--- Replay control (protected) --------------------------------------------

/**
 * Executes a backward replay, i.e., iterates over the events of the
 * associated trace in reverse order (i.e., from the end to the beginning)
 * and executes the corresponding callbacks registered with the callback
 * manager.
 *
 * @return  Always @em true
 */
bool BackwardReplayTask::replay()
{
  PEARL_backward_replay(m_trace, *m_cbmanager, m_cbdata);

  return true;
}
