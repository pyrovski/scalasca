/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>

#include <LocalTrace.h>
#include <Synchronizer.h>

#include "SynchronizeTask.h"
#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;

/*
 *---------------------------------------------------------------------------
 *
 * class SynchronizeTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new synchronizer task.
 *
 * This constructor creates a new timestamp synchronization task working on
 * the given trace data object @p trace.
 *
 * @param  privateData  Private task data object
 */
SynchronizeTask::SynchronizeTask(const TaskDataPrivate& privateData)
  : Task(),
    m_privateData(privateData),
    m_synchronizer(NULL)
{
}


/**
 * @brief Destructor.
 *
 * Destructor. Destroys the instance and releases all occupied resources.
 */
SynchronizeTask::~SynchronizeTask()
{
  delete m_synchronizer;
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Performs the timestamp synchronization based on the controlled logical
 * clock algorithm.
 *
 * @return Always @em true
 */
bool SynchronizeTask::execute()
{
  const LocalTrace& trace = *m_privateData.mTrace;

  int rank = trace.get_location()->get_process()->get_id();
  m_synchronizer = new Synchronizer(rank, trace);
  m_synchronizer->synchronize();

  return true;
}


//--- Printing statistics ---------------------------------------------------

/**
 * @brief Prints some synchronizer-related statistics.
 *
 * This method prints some additional statistics about the timestamp
 * synchronization process. In case the synchronizer task has not been
 * called, it defaults to a no-op.
 */
void SynchronizeTask::print_statistics() const
{
  if (m_synchronizer)
    m_synchronizer->print_statistics();
}
