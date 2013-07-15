/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_ANALYZETASK_H
#define SCOUT_ANALYZETASK_H


#include <cstdio>
#include <CompoundTask.h>

#include "CbData.h"


/*-------------------------------------------------------------------------*/
/**
 * @file  AnalyzeTask.h
 * @brief Declaration of the AnalyzeTask class.
 *
 * This header file provides the declaration of the AnalyzeTask class which
 * combines individual trace analysis tasks.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Forward declarations --------------------------------------------------

class PatternDetectionTask;
class StatisticsTask;

struct ReportData;
struct TaskDataPrivate;
struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 * @class AnalyzeTask
 * @brief Compound task combining individual trace-analysis tasks.
 *
 * This class abstracts away the details of the trace-analysis phase by
 * providing a single, convenient interface to a potentially larger number
 * of individual trace-analysis tasks.
 */
/*-------------------------------------------------------------------------*/

class AnalyzeTask : public pearl::CompoundTask
{
  public:
    /// @name Constructors & destructor
    /// @{

    AnalyzeTask(const TaskDataShared&  sharedData,
                const TaskDataPrivate& privateData);

    /// @}
    /// @name Register statistics task
    /// @{

    void register_statistics_cbs(StatisticsTask* sttask);

    /// @}
    /// @name Retrieve report data
    /// @{

    void gen_patterns(ReportData& data) const;
    void gen_severities(ReportData& data, int rank, std::FILE* fp) const;

    /// @}

    
  protected:
    /// @name Execution control
    /// @{

    virtual bool prepare();

    /// @}


  private:
    /// Shared task data object
    const TaskDataShared& m_sharedData;

    /// Private task data object
    const TaskDataPrivate& m_privateData;

    /// Callback data object passed to the individual analysis tasks
    CbData m_cbdata;

    /// Pointer to the pattern detection task object
    PatternDetectionTask* m_patterndetect;

    /// Pointer to the statistics task object
    StatisticsTask* m_statistics;
};


}   // namespace scout


#endif   // !SCOUT_ANALYZETASK_H
