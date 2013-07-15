/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_STATISTICSTASK_H
#define SCOUT_STATISTICSTASK_H


#include <stdint.h>

#include <Task.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  StatisticsTask.h
 * @brief Declaration of the StatisticsTask class.
 *
 * This header file provides the declaration of the StatisticsTask class
 * which keeps track of some statistics and displays them when executed.
 */
/*-------------------------------------------------------------------------*/


//--- Forward declarations --------------------------------------------------

namespace pearl
{

class CallbackData;
class CallbackManager;
class Event;

}   // namespace pearl


namespace scout
{

//--- Forward declarations --------------------------------------------------

class SynchronizeTask;

struct TaskDataPrivate;


/*-------------------------------------------------------------------------*/
/**
 * @class StatisticsTask
 * @brief Task for tracking and printing trace-analysis statistics at the
 *        end of execution.
 *
 * The StatisticsTask class, on the one hand, provides a number of callback
 * methods which can be triggered during the trace analysis phase to keep
 * track of various events (such as clock-condition violations). On the other
 * hand, it provides the functionality to collate and display some statistics
 * when executed.
 */
/*-------------------------------------------------------------------------*/

class StatisticsTask : public pearl::Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    StatisticsTask(const TaskDataPrivate& privateData,
                   SynchronizeTask*       synctask);

    /// @}
    /// @name Callback registration
    /// @{

    void register_patterndetect_cbs(pearl::CallbackManager& cbmgr);

    /// @}
    /// @name  Executing the task
    /// @{

    virtual bool execute();

    /// @}


  private:
    /// @name Callback methods
    /// @{

    void ccv_p2p_cb (const pearl::CallbackManager& cbmanager,
                     int                           user_event,
                     const pearl::Event&           event,
                     pearl::CallbackData*          cdata);

    void ccv_coll_cb(const pearl::CallbackManager& cbmanager,
                     int                           user_event,
                     const pearl::Event&           event,
                     pearl::CallbackData*          cdata);

    /// @}


    /// Private task data object
    const TaskDataPrivate& m_privateData;

    /// Pointer to CLC-based timestamp synchronization task
    SynchronizeTask* m_synctask;

    /// Counter for point-to-point clock condition violations
    uint64_t m_ccv_p2p;

    /// Counter for collective clock condition violations
    uint64_t m_ccv_coll;
};


}   // namespace scout


#endif   // !SCOUT_STATISTICSTASK_H
