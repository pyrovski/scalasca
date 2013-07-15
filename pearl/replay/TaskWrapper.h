/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_TASKWRAPPER_H
#define PEARL_TASKWRAPPER_H


#include "Task.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    TaskWrapper.h
 * @ingroup PEARL_replay
 * @brief   Declaration of the class TaskWrapper.
 *
 * This header file provides the declaration of the class TaskWrapper.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   TaskWrapper
 * @ingroup PEARL_replay
 * @brief   Abstract base class for a wrapper task.
 *
 * A wrapper task can be used to add additional functionality to the
 * execution of arbitrary Tasks, which is orthogonal to the wrapped Task's 
 * function. Derived classes need to overwrite the execute() method.
 *
 * The TaskWrapper class ensures correct parent pointer handling, i.e.
 * it sets the wrapped task's parent pointer to the innermost CompoundTask
 * the TaskWrapper is added to.
 *
 * TaskWrapper takes ownership of the wrapped task, and deletes it on 
 * destruction.
 */
/*-------------------------------------------------------------------------*/

class TaskWrapper : public Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~TaskWrapper();

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    explicit TaskWrapper(Task* task);

    /// @}


    /// Pointer to the wrapped task
    Task* m_task;


  private:
    /// @name Setting task properties
    /// @{

    virtual void set_parent(CompoundTask* parent);

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_TASKWRAPPER_H */
