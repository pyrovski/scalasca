/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <exception>

#if defined(_OPENMP)
  #include <omp.h>
#endif   // _OPENMP
#if defined(_MPI)
  #include <mpi.h>
#endif   // _MPI

#include <elg_error.h>

#include "CheckedTask.h"

using namespace std;
using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class CheckedTask
 *
 *---------------------------------------------------------------------------
 */


//--- Executing the task ----------------------------------------------------

/**
 * @brief Executes the concurrent task and verifies its success.
 *
 * Executes the concurrent task and globally verifies its success. A task
 * is considered unsuccessful if its execute() method either returns @em
 * false or throws an exception.
 *
 * @return Returns @em true if all concurrent executions were successful,
 *         @em false otherwise
 */
bool CheckedTask::execute()
{
  bool        local_error = false;
  std::string message     = "Task failed!";

  try
  {
    local_error = (m_task->execute() == false);
  } 
  catch (const std::exception& ex)
  {
    local_error = true;
    message     = ex.what();
  }

  return !CheckGlobalError(local_error, message);
}


//--- CheckedTask wrapping method -------------------------------------------

/**
 * @brief Creates a checked task.
 *
 * This method creates a new checked task by wrapping the given @a task.
 *
 * @param  task  Task to be wrapped
 * @return %CheckedTask instance
 */
CheckedTask* CheckedTask::make_checked(pearl::Task* task)
{
  return new CheckedTask(task);
}


//--- Constructors & destructor (private) -----------------------------------

/**
 * @brief Creates a new checked task instance.
 *
 * Creates a new checked task instance. Since this constructor is private,
 * the only way to really create a checked task is to call the make_checked()
 * method.
 *
 * @param  task  Task to be wrapped
 */
CheckedTask::CheckedTask(pearl::Task* task)
  : TaskWrapper(task)
{
}


//--- Related functions -----------------------------------------------------

/**
 * @brief Performs a global error check based on local error states.
 *
 * This function performs a synchronizing, global error check in parallel
 * applications. It basically performs an Allreduce operation across all
 * processes and threads of the parallel application, returning the logical
 * OR of all @a local_error values provided by the participating partners.
 * In case of an error, the given error @p message is printed (only once
 * if all partners fail, otherwise a message for each failing partner is
 * printed).
 *
 * Basically, the reduction is performed by first determining the error
 * state per process (i.e., across threads), then exchanging these states
 * among processes, and finally distributing the result back to all threads.
 * Note that the actual algorithm used depends on the parallel programming
 * model.
 *
 * @param  local_error  Flag indicating the local error state
 * @param  message      Error message
 * @return Returns %em true if any of the participating partners indicate
 *         an error, @em false otherwise.
 */
bool scout::CheckGlobalError(bool local_error, const string& message)
{
  // Initialize local error & thread count
  // (1st field: error, 2nd field: thread)
  int local[2];
  local[0] = local_error ? 1 : 0;
  local[1] = 1;

  // Process-local reduction of error & thread count
  #if defined(_OPENMP)
    // This variable is implicitly shared!
    static int shared[2];

    if (omp_in_parallel())
    {
      #pragma omp barrier
      #pragma omp master
      shared[0] = 0;
      #pragma omp barrier
      #pragma omp critical
      shared[0] += local[0];
      #pragma omp barrier
      local[0] = shared[0];

      // Adjust thread count only when inside parallel regions
      local[1] = omp_get_num_threads();
    }
  #endif   // !_OPENMP

  // Determine global error & thread count
  // (1st field: error, 2nd field: thread)
  int global[2];
  #if defined(_MPI)
    #pragma omp master
    MPI_Allreduce(local, global, 2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  #else   // !_MPI
    // Only a single process ==> global count == local count
    global[0] = local[0];
    global[1] = local[1];
  #endif   // !_MPI

  // Broadcast global error & thread counts across threads
  #if _OPENMP
    if (omp_in_parallel())
    {
      #pragma omp barrier
      #pragma omp master
      {
        shared[0] = global[0];
        shared[1] = global[1];
      }
      #pragma omp barrier
      global[0] = shared[0];
      global[1] = shared[1];
    }
  #endif

  // No error at all ==> return
  if (0 == global[0])
    return false;

  // All threads failed ==> display message as control message from each
  //                        thread and as warning from the master thread
  if (global[0] == global[1])
  {
    elg_cntl_msg("SCOUT: %s", message.c_str());
    #pragma omp master
    {
      // Determine process rank
      int rank = 0;
      #if defined(_MPI)
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      #endif   // _MPI

      if (0 == rank)
        elg_warning("SCOUT: %s", message.c_str());
    }
  }
  // Only some threads failed: display message as warning from failing threads
  else if (local_error)
    elg_warning("SCOUT: %s", message.c_str());

  return true;
}
