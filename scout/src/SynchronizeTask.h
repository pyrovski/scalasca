/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_SYNCHRONIZETASK_H
#define SCOUT_SYNCHRONIZETASK_H


#include <Task.h>


//--- Forward declarations --------------------------------------------------

class Synchronizer;


/*-------------------------------------------------------------------------*/
/**
 * @file  SynchronizeTask.h
 * @brief Declaration of the SynchronizeTask class.
 *
 * This header file provides the declaration of the SynchronizeTask class
 * which performs a post-mortem timestamp synchronization based on the
 * controlled logical clock algorithm.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Forward declarations --------------------------------------------------

struct TaskDataPrivate;


/*-------------------------------------------------------------------------*/
/**
 * @class SynchronizeTask
 * @brief Task performing a post-mortem timestamp synchronization.
 *
 * The SynchronizeTask class performs a post-mortem timestamp syhcnronization
 * of event streams based on the controlled logical clock algorithm.
 */
/*-------------------------------------------------------------------------*/

class SynchronizeTask : public pearl::Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    SynchronizeTask(const TaskDataPrivate& privateData);
    virtual ~SynchronizeTask();

    /// @}
    /// @name Execution control
    /// @{

    virtual bool execute();

    /// @}
    /// @name Printing statistics
    /// @{

    void print_statistics() const;

    /// @}

  private:
    /// Private task data object
    const TaskDataPrivate& m_privateData;

    /// Timestamp synchronization object
    Synchronizer* m_synchronizer;
};


}   // namespace scout


#endif   // !SCOUT_SYNCHRONIZETASK_H
