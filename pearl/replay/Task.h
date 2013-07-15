/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_TASK_H
#define PEARL_TASK_H


/*-------------------------------------------------------------------------*/
/**
 * @file    Task.h
 * @ingroup PEARL_replay
 * @brief   Declaration of the class Task.
 *
 * This header file provides the declaration of the class Task.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CompoundTask;


/*-------------------------------------------------------------------------*/
/**
 * @class   Task
 * @ingroup PEARL_replay
 * @brief   Abstract base class for a generic Task.
 *
 * The Task class provides an interface for a task or a subtask. 
 * Implementations need to derive from Task and implement the execute()
 * method.
 *
 * A Task can be child of (at most one) CompoundTask, which will then be the 
 * task's \em parent. Task provides means to access the parent CompoundTask.
 *
 * @see CompoundTask
 */
/*-------------------------------------------------------------------------*/

class Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~Task();

    /// @}
    /// @name Accessing task properties
    /// @{

    CompoundTask* get_parent() const;

    /// @}
    /// @name Executing the task
    /// @{

    /**
     * Pure virtual member function defining the interface for executing a
     * task. This method needs to be overwritten in derived subclasses to
     * define the task's runtime behavior.
     *
     * @return Should return @em true if execution was successful, @em false
     *         otherwise
     */
    virtual bool execute() = 0;

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    Task();

    /// @}


  private:
    /// @name Setting task properties
    /// @{

    virtual void set_parent(CompoundTask* parent);

    /// @}


    /// Pointer to the compound task this Task object belongs to
    CompoundTask* m_parent;


    /* Declare friends */
    // Both CompoundTask and TaskWrapper need access to set_parent()...
    friend class CompoundTask;
    friend class TaskWrapper;
};


}   /* namespace pearl */


#endif   /* !PEARL_TASK_H */
