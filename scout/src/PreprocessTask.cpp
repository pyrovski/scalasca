/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <pearl.h>

#include "CheckedTask.h"
#include "PreprocessTask.h"
#include "TaskData.h"

using namespace scout;


//--- Helper classes --------------------------------------------------------

namespace
{

/**
 * @class VerifyCalltree
 * @brief Task verifying the local completeness of the call tree provided by
 *        the global definitions.
 *
 * This preprocessing task verifies whether the call tree provided by the
 * global definitions object is locally complete, i.e., no additional call
 * paths are created based on the local trace data. In the case of MPI, a
 * global check is performed by the UnifyCalltree task afterwards.
 */
class VerifyCalltree : public pearl::Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    VerifyCalltree(const TaskDataShared&  sharedData,
                   const TaskDataPrivate& privateData);

    /// @}
    /// @name Execution control
    /// @{

    virtual bool execute();

    /// @}


  private:
    /// Shared task data object
    const TaskDataShared& m_sharedData;

    /// Private task data object
    const TaskDataPrivate&  m_privateData;
};


#if defined(_MPI)
  /**
   * @class UnifyCalltree
   * @brief Task verifying the global completeness of the call tree provided
   *        by the global definitions and unifying it when necessary.
   *
   * This preprocessing task verifies whether the call tree provided by the
   * global definitions object is globally complete, i.e., no additional call
   * paths are created based on the entire trace data. If necessary, this task
   * also performs a unification of the local call trees.
   */
  class UnifyCalltree : public pearl::Task
  {
    public:
      /// @name Constructors & destructor
      /// @{

      UnifyCalltree(const TaskDataShared& sharedData);

      /// @}
      /// @name Execution control
      /// @{

      virtual bool execute();

      /// @}


    private:
      /// Shared task data object
      const TaskDataShared& m_sharedData;
  };
#endif   // _MPI


/**
 * @class PreprocessTrace
 * @brief Task preprocessing the trace data to provide the full trace-access
 *        functionality.
 *
 * This preprocessing task is required to provide the full trace-access
 * functionality. In particular, it links the flow events in the trace with
 * their corresponding call-tree nodes.
 */
class PreprocessTrace : public pearl::Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    PreprocessTrace(const TaskDataShared&  sharedData,
                    const TaskDataPrivate& privateData);

    /// @}
    /// @name Execution control
    /// @{

    virtual bool execute();

    /// @}


  private:
    /// Shared task data object
    const TaskDataShared& m_sharedData;

    /// Private task data object
    const TaskDataPrivate& m_privateData;
};

}   // unnamed namespace


/*
 *---------------------------------------------------------------------------
 *
 * class PreprocessTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new preprocessing task.
 *
 * This constructor creates a new preprocessing task instance using the given
 * parameters.
 *
 * @param  sharedData   Shared task data object
 * @param  privateData  Private task data object
 */
PreprocessTask::PreprocessTask(const TaskDataShared&  sharedData,
                               const TaskDataPrivate& privateData)
  : CompoundTask()
{
  add_task(CheckedTask::make_checked(new VerifyCalltree(sharedData, privateData)));
  #if defined(_MPI)
    add_task(CheckedTask::make_checked(new UnifyCalltree(sharedData)));
  #endif   // _MPI
  add_task(CheckedTask::make_checked(new PreprocessTrace(sharedData, privateData)));
}


/*
 *---------------------------------------------------------------------------
 *
 * class VerifyCalltree
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new VerifyCalltree task instance.
 *
 * This constructor creates a new task instance for locally verifying the
 * global call tree using the given parameters.
 *
 * @param  sharedData   Shared task data object
 * @param  privateData  Private task data object
 */
VerifyCalltree::VerifyCalltree(const TaskDataShared&  sharedData,
                               const TaskDataPrivate& privateData)
  : Task(),
    m_sharedData(sharedData),
    m_privateData(privateData)
{
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Verifies whether the call tree given by the global definitions object is
 * locally complete.
 *
 * @return Always @em true
 * @throw  pearl::FatalError  if the trace data has inconsistent ENTER/EXIT
 *                            events
 */
bool VerifyCalltree::execute()
{
  #if defined(_OPENMP)
    PEARL_omp_verify_calltree(*m_sharedData.mDefinitions, *m_privateData.mTrace);
  #else   // !_OPENMP
    PEARL_verify_calltree(*m_sharedData.mDefinitions, *m_privateData.mTrace);
  #endif   // !_OPENMP

  return true;
}


/*
 *---------------------------------------------------------------------------
 *
 * class UnifyCalltree
 *
 *---------------------------------------------------------------------------
 */

#if defined(_MPI)

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new UnifyCalltree task instance.
 *
 * This constructor creates a new task instance for globally verifying the
 * global call tree and unifying it if necessary using the given parameters.
 *
 * @param  sharedData  Shared task data object
 */
UnifyCalltree::UnifyCalltree(const TaskDataShared& sharedData)
  : Task(),
    m_sharedData(sharedData)
{
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Verifies whether the call tree given by the global definitions object is
 * globally complete and unifies the local call trees if necessary.
 *
 * @return Always @em true
 */
bool UnifyCalltree::execute()
{
  #pragma omp master
  {
    PEARL_mpi_unify_calltree(*m_sharedData.mDefinitions);
  }

  return true;
}

#endif   // _MPI


/*
 *---------------------------------------------------------------------------
 *
 * class PreprocessTrace
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new PreprocessTrace task instance.
 *
 * This constructor creates a new task instance for the final trace setup
 * before a replay can be started.
 *
 * @param  sharedData   Shared task data object
 * @param  privateData  Private task data object
 */
PreprocessTrace::PreprocessTrace(const TaskDataShared&  sharedData,
                                 const TaskDataPrivate& privateData)
  : Task(),
    m_sharedData(sharedData),
    m_privateData(privateData)
{
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Performs the final trace-data setup before a replay can be started. In
 * particular, it links the flow events in the trace to their corresponding
 * call-tree nodes.
 *
 * @return Always @em true
 */
bool PreprocessTrace::execute()
{
  #if defined(_OPENMP)
    PEARL_omp_preprocess_trace(*m_sharedData.mDefinitions, *m_privateData.mTrace);
  #else   // !_OPENMP
    PEARL_preprocess_trace(*m_sharedData.mDefinitions, *m_privateData.mTrace);
  #endif   // !_OPENMP

  return true;
}
