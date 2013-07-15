/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <CallbackManager.h>
#include <LocalTrace.h>

#include <cube.h>

#include "CbData.h"
#include "Pattern.h"
#include "PatternDetectionTask.h"
#include "Patterns_gen.h"
#include "ReportData.h"
#include "StatisticsTask.h"
#include "TaskData.h"
#include "user_events.h"

using namespace std;
using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class PatternDetectionTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new pattern detection task.
 *
 * This constructor creates a new pattern detection task instance using the
 * given parameters. This constructor is also responsible for creating and
 * registering all pattern objects.
 *
 * @param  sharedData    Shared task data object
 * @param  privateData   Private task data object
 * @param  callbackData  Callback data object
 */
PatternDetectionTask::PatternDetectionTask(const TaskDataShared&  sharedData,
                                           const TaskDataPrivate& privateData,
                                           CbData*                callbackData)
  : ForwardReplayTask(*sharedData.mDefinitions,
                      *privateData.mTrace,
                      callbackData),
    m_trace(*privateData.mTrace)
{
  // Create patterns
  create_patterns(this);
}


/**
 * @brief Destructor. Destroys the instance and releases all resources.
 *
 * The destructor is also responsible for destroying all pattern objects
 * and their associated data structures.
 */
PatternDetectionTask::~PatternDetectionTask()
{
  // Free resources
  PatternList::const_iterator it = m_patterns.begin();
  while (it != m_patterns.end()) {
    delete *it;
    ++it;
  }
}


//--- Adding performance patterns -------------------------------------------

/**
 * @brief Adds a performance pattern object to the task.
 *
 * This method adds the given performance @a pattern object to the task and
 * implicitely registers it's callbacks. Adding a pattern to the task also
 * leads to it's inclusion in the final analysis report.
 */
void PatternDetectionTask::add_pattern(Pattern* pattern)
{
  // Add pattern to list and register callbacks
  m_patterns.push_back(pattern);
  pattern->reg_cb(m_cbmanager);
}


//--- Retrieve statistics ---------------------------------------------------

/**
 * @brief Registers a statistics task with this pattern detection task.
 *
 * This method registers the given statistics task @p sttask with this
 * pattern detection task. Before starting the analysis phase, the callback
 * of this statistics task will be registered, so that the detection task
 * can trigger those to centrally collect various statistics.
 *
 * @param  sttask  Pointer to statistics task to register
 */
void PatternDetectionTask::register_statistics_cbs(StatisticsTask* sttask)
{
  sttask->register_patterndetect_cbs(*m_cbmanager);
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
void PatternDetectionTask::gen_patterns(ReportData& data) const
{
  // Create performance metrics
  uint32_t count = m_patterns.size();
  for (uint32_t index = 0; index < count; ++index) {
    Pattern*     pattern = m_patterns[index];
    cube_metric* cmetric = NULL;

    if (!pattern->is_hidden()) {
      // Determine parent
      long         parent_id = pattern->get_parent();
      cube_metric* cparent   = NULL;
      if (parent_id != PAT_NONE)
        cparent = data.metrics[parent_id];

      // Determine data type
      string dtype;
      if (pattern->get_unit() == "sec")
        dtype = "FLOAT";
      else
        dtype = "INTEGER";

      // Create metric
      cmetric = cube_def_met(data.cb,
                             pattern->get_name().c_str(),
                             pattern->get_unique_name().c_str(),
                             dtype.c_str(),
                             pattern->get_unit().c_str(),
                             "",
                             pattern->get_url().c_str(),
                             pattern->get_descr().c_str(),
                             cparent);
    }

    data.metrics[pattern->get_id()] = cmetric;
  }
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
void PatternDetectionTask::gen_severities(ReportData& data,
                                          int         rank,
                                          std::FILE*  fp) const
{
  PatternList::const_iterator it = m_patterns.begin();
  while (it != m_patterns.end()) {
    if (!(*it)->is_hidden() || (*it)->get_name() == "STATISTICS")
      (*it)->gen_severities(data, rank, m_trace, fp);
    ++it;
  }
}


//--- Execution control (protected) -----------------------------------------

/**
 * @brief Task preparation.
 *
 * The prepare() callback method is called before the trace replay starts.
 * In case of a pattern detection task, only the user-defined PREPARE event
 * is triggered.
 *
 * @return Always @em true
 */
bool PatternDetectionTask::prepare()
{
  m_cbmanager->notify(PREPARE, m_trace.end(), m_cbdata);

  return true;
}


/**
 * @brief Task clean up.
 *
 * The finish() callback method is called after the trace replay finishes.
 * In case of a pattern detection task, only the user-defined FINISHED event
 * is triggered.
 *
 * @return Always @em true
 */
bool PatternDetectionTask::finish()
{
  m_cbmanager->notify(FINISHED, m_trace.end(), m_cbdata);

  return true;
}
