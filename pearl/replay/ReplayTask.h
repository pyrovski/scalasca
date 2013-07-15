/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_REPLAYTASK_H
#define PEARL_REPLAYTASK_H


#include "Callback.h"
#include "CallbackManager.h"
#include "Error.h"
#include "Task.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    ReplayTask.h
 * @ingroup PEARL_replay
 * @brief   Declaration of the class ReplayTask.
 *
 * This header file provides the declaration of the class ReplayTask.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CallbackData;
class GlobalDefs;
class LocalTrace;


/*-------------------------------------------------------------------------*/
/**
 * @class   ReplayTask
 * @ingroup PEARL_replay
 * @brief   Abstract base class for a replay-based trace analysis task.
 *
 * ReplayTask provides an interface for a trace replay task. Its execute()
 * method will run a trace replay on a given trace using the PEARL callback 
 * system.
 *
 * Users can inherit from one of the derived classes ForwardReplayTask or 
 * BackwardReplayTask, which will perform a default PEARL forward or backward
 * replay, respectively. To specify a different replay engine, users can also
 * inherit directly from ReplayTask and overwrite the replay() method.
 */
/*-------------------------------------------------------------------------*/

class ReplayTask : public Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~ReplayTask();

    /// @}
    /// @name Executing the task
    /// @{

    virtual bool execute();

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    ReplayTask(const GlobalDefs& defs,
               LocalTrace&       trace, 
               CallbackData*     data = 0,
               CallbackManager*  cbmanager = 0);

    /// @}
    /// @name Callback registration
    /// @{

    template<class derivedT>
    void register_callback(event_t event,
                           void (derivedT::*fptr)(const CallbackManager&,
                                                  int, 
                                                  const Event&,
                                                  CallbackData*));
    template<class derivedT>
    void register_callback(int user_event,
                           void (derivedT::*fptr)(const CallbackManager&,
                                                  int, 
                                                  const Event&,
                                                  CallbackData*));
    
    /// @}
    /// @name Replay control
    /// @{

    virtual bool prepare();
    virtual bool finish();

    /**
     * Pure virtual member function defining the interface for executing a
     * replay. It needs to be overwritten in derived subclasses to define
     * the exact replay behavior.
     *
     * @return Should return @em true if the replay was sucessful, @em false
     *         otherwise
     */
    virtual bool replay() = 0;

    /// @}


    /// Global definitions
    const GlobalDefs& m_defs;

    /// Local trace data
    LocalTrace& m_trace;

    /// Callback data object
    CallbackData* m_cbdata;

    /// Callback manager object
    CallbackManager* m_cbmanager;
};


//--- Inline methods --------------------------------------------------------

/**
 * Convenience function to register a callback member-function for PEARL
 * event records. You can use this function in a derived class to register
 * one of the classes' member functions as callback function for PEARL trace
 * replay.
 *
 * Example:
 * @code
 * class MyReplay : public ReplayTask {
 *   public:
 *     MyReplay(const GlobalDefs& defs, LocalTrace& trace) 
 *       : ReplayTask(defs, trace)
 *     {
 *       register_callback(MPI_SEND, &MyReplay::cb_send);
 *     }
 *
 *   protected:
 *     void cb_send(const CallbackManager& cbmanager, int user_event,
 *                  const Event& event, CallbackData* cdata)
 *     {
 *       // ...
 *     }
 * };
 * @endcode
 *
 * @param  event  PEARL event to register callback for
 * @param  fptr   Pointer to callback member function
 */
template<class derivedT>
inline void ReplayTask::register_callback(event_t event,
                                          void (derivedT::*fptr)(const CallbackManager&,
                                                                 int, 
                                                                 const Event&,
                                                                 CallbackData*))
{
  if (derivedT* d = dynamic_cast<derivedT* const>(this))
    m_cbmanager->register_callback(event, 
                                   PEARL_create_callback(d, fptr));
  else
    throw FatalError("ReplayTask::register_callback() -- Incompatible pointer!");
}


/**
 * Convenience function to register a callback member-function for
 * user-defined events. You can use this function in a derived class to
 * register one of the classes' member functions as callback function for
 * PEARL trace replay.
 *
 * Example:
 * @code
 * class MyReplay : public ReplayTask {
 *   public:
 *     MyReplay(const GlobalDefs& defs, LocalTrace& trace) 
 *       : ReplayTask(defs, trace)
 *     {
 *       register_callback(42, &MyReplay::cb_dowork);
 *     }
 *
 *   protected:
 *     void cb_dowork(const CallbackManager& cbmanager, int user_event,
 *                    const Event& event, CallbackData* cdata)
 *     {
 *       // ...
 *     }
 * };
 * @endcode
 *
 * @param  user_event  User-defined event to register callback for
 * @param  fptr        Pointer to callback member function
 */
template<class derivedT>
inline void ReplayTask::register_callback(int user_event,
                                          void (derivedT::*fptr)(const CallbackManager&,
                                                                 int,
                                                                 const Event&,
                                                                 CallbackData*))
{
  if (derivedT* d = dynamic_cast<derivedT* const>(this))
    m_cbmanager->register_callback(user_event, 
                                   PEARL_create_callback(d, fptr));
  else
    throw FatalError("ReplayTask::register_callback() -- Incompatible pointer!");
}


/*-------------------------------------------------------------------------*/
/**
 * @class   ForwardReplayTask
 * @ingroup PEARL_replay
 * @brief   Base class for forward trace-replay tasks.
 *
 * ForwardReplayTask will perform a forward replay using
 * PEARL_forward_replay() when executed. To use this class, derive a class
 * from ForwardReplayTask and add some callback methods.
 *
 * Example:
 * @code
 * class MyReplay : public ForwardReplayTask {
 *   public:
 *     MyReplay(const GlobalDefs& defs, LocalTrace& trace) 
 *       : ForwardReplayTask(defs, trace) 
 *     {
 *       register_callback(MPI_SEND, &MyReplay::cb_send);
 *     }
 *
 *   private:
 *     void cb_send(const CallbackManager& cbmanager, int user_event,
 *                  const Event& event, CallbackData* cdata)
 *     {
 *       // ...
 *     }
 * };
 * @endcode
 *
 * @see BackwardReplayTask
 */
/*-------------------------------------------------------------------------*/

class ForwardReplayTask : public ReplayTask
{
  protected:
    /// @name Constructors & destructor
    /// @{

    ForwardReplayTask(const GlobalDefs& defs,
                      LocalTrace&       trace, 
                      CallbackData*     cbdata = 0,
                      CallbackManager*  cbmanager = 0);

    /// @}
    /// @name Replay control
    /// @{

    virtual bool replay();

    /// @}
};


/*-------------------------------------------------------------------------*/
/**
 * @class   BackwardReplayTask
 * @ingroup PEARL_replay
 * @brief   Base class for backward trace-replay tasks.
 *
 * BackwardReplayTask will perform a backward replay using
 * PEARL_backward_replay() when executed. To use this class, derive a class
 * from BackwardReplayTask and add some callback methods. For an example,
 * please refer to the description of class ForwardReplayTask.
 *
 * @see ForwardReplayTask
 */
/*-------------------------------------------------------------------------*/

class BackwardReplayTask : public ReplayTask
{
  protected:
    /// @name Constructors & destructor
    /// @{

    BackwardReplayTask(const GlobalDefs& defs,
                       LocalTrace&       trace, 
                       CallbackData*     cbdata = 0,
                       CallbackManager*  cbmanager = 0);

    /// @}
    /// @name Replay control
    /// @{

    virtual bool replay();

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_REPLAYTASK_H */
