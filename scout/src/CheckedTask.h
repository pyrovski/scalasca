/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_CHECKEDTASK_H
#define SCOUT_CHECKEDTASK_H


#include <string>

#include <TaskWrapper.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  CheckedTask.h
 * @brief Declaration of the CheckedTask class and related functions.
 *
 * This header file provides the declaration of the CheckedTask class and
 * related functions.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

/*-------------------------------------------------------------------------*/
/**
 * @class CheckedTask
 * @brief Executes a concurrent task and globally checks whether it was
 *        successful.
 *
 * The CheckedTask class is intended to be used as a wrapper for tasks
 * executed concurrently in parallel applications whose success needs to
 * be verified on a global level. Thereby, the definition of "global"
 * depends on the parallel programming model used (OpenMP, MPI, hybrid
 * MPI/OpenMP).
 */
/*-------------------------------------------------------------------------*/

class CheckedTask : public pearl::TaskWrapper
{
  public:
    /// @name Executing the task
    /// @{

    virtual bool execute();

    /// @}
    /// @name CheckedTask wrapping method
    /// @{

    static CheckedTask* make_checked(pearl::Task* task);

    /// @}


  private:
    /// @name Constructors & destructor
    /// @{

    CheckedTask(pearl::Task* task);

    /// @}
};


//--- Related functions -----------------------------------------------------

bool CheckGlobalError(bool local_error, const std::string& message);


}   // namespace scout


#endif   // !SCOUT_CHECKEDTASK_H
