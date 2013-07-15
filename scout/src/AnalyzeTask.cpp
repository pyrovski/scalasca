/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>
#include <cstddef>

#include "AnalyzeTask.h"
#include "CheckedTask.h"
#include "PatternDetectionTask.h"
#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class AnalyzeTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new analysis task.
 *
 * This constructor creates a new analysis task instance using the given
 * parameters.
 *
 * @param  sharedData   Shared task data object
 * @param  privateData  Private task data object
 */
AnalyzeTask::AnalyzeTask(const TaskDataShared&  sharedData,
                         const TaskDataPrivate& privateData)
  : CompoundTask(),
    m_sharedData(sharedData),
    m_privateData(privateData),
    m_patterndetect(NULL),
    m_statistics(NULL)
{
}


//--- Register statistics task ----------------------------------------------

/**
 * @brief Registers a statistics task with this analysis task.
 *
 * This method registers the given statistics task @p sttask with this
 * analysis task. Before starting the analysis phase, the callbacks of
 * this statistics task will be registered, so that the individual analysis
 * tasks can trigger those to centrally collect various statistics.
 *
 * @param  sttask  Pointer to statistics task to register
 */
void AnalyzeTask::register_statistics_cbs(StatisticsTask* sttask)
{
  assert(sttask);

  m_statistics = sttask;
}


//--- Retrieve report data --------------------------------------------------

/**
 * @brief Generates the performance metric information.
 *
 * This method defines the CUBE performance metric hierarchy in the CUBE
 * object passed via @a data based on the information provided by the
 * individual pattern classes.
 *
 * @param  data  Report data structure
 */
void AnalyzeTask::gen_patterns(ReportData& data) const
{
  assert(m_patterndetect);

  m_patterndetect->gen_patterns(data);
}


/**
 * @brief Collates all pattern severities and writes the corresponding
 *        section in the CUBE report.
 *
 * Generates the severity section of the CUBE report for all patterns. The
 * actual collation and writing id done by the individual pattern classes.
 *
 * @param  data   Temporary data used for report writing
 * @param  rank   Global process rank (MPI rank or 0)
 * @param  fp     File pointer
 */
void AnalyzeTask::gen_severities(ReportData& data,
                                 int         rank,
                                 std::FILE*  fp) const
{
  assert(m_patterndetect);

  m_patterndetect->gen_severities(data, rank, fp);
}


//--- Execution control (protected) -----------------------------------------

/**
 * @brief Task preparation.
 *
 * The prepare() callback method is called before execution of the subtasks
 * starts. In case of an analyze task, the sub-tasks are created, initialized
 * and added to the analysis task.
 *
 * @return @em true if all sub-tasks could be initialized, @em false otherwise
 */
bool AnalyzeTask::prepare()
{
  m_cbdata.m_defs  = m_sharedData.mDefinitions;
  m_cbdata.m_trace = m_privateData.mTrace;

  m_patterndetect = new PatternDetectionTask(m_sharedData, m_privateData,
                                             &m_cbdata);

  if (!m_patterndetect)
    return false;

  add_task(CheckedTask::make_checked(m_patterndetect));

  if (m_statistics)
    m_patterndetect->register_statistics_cbs(m_statistics);

  return m_patterndetect != 0;
}
