/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_READTRACETASK_H
#define SCOUT_READTRACETASK_H


#include <Task.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  ReadTraceTask.h
 * @brief Declaration of the ReadTraceTask class.
 *
 * This header file provides the declaration of the ReadTraceTask class
 * which is used to read in the trace data per location.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Forward declarations --------------------------------------------------

struct TaskDataPrivate;
struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 * @class ReadTraceTask
 * @brief Task used to read the per-location trace data.
 *
 * The ReadTraceTask class is used to read the entire trace data which is
 * stored per location into memory.
 */
/*-------------------------------------------------------------------------*/

class ReadTraceTask : public pearl::Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    ReadTraceTask(const TaskDataShared& sharedData,
                  TaskDataPrivate&      privateData);

    /// @}
    /// @name Execution control
    /// @{

    virtual bool execute();

    /// @}


  private:
    /// Shared task data object
    const TaskDataShared& m_sharedData;

    /// Private task data object
    TaskDataPrivate& m_privateData;
};


}   // namespace scout


#endif   // !SCOUT_READTRACETASK_H
