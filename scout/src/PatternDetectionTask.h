/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_PATTERNDETECTIONTASK_H
#define SCOUT_PATTERNDETECTIONTASK_H


#include <cstdio>
#include <vector>

#include <ReplayTask.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  PatternDetectionTask.h
 * @brief Declaration of the PatternDetectionTask class.
 *
 * This header file provides the declaration of the PatternDetectionTask
 * class which performs a trace-based wait-state analysis.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Forward declarations --------------------------------------------------

class Pattern;
class StatisticsTask;

struct CbData;
struct ReportData;
struct TaskDataPrivate;
struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 * @class PatternDetectionTask
 * @brief Task performing a trace-based wait-state detection.
 *
 * The wait-state detection is done using a forward replay of the trace
 * data, exchanging necessary data between processes at the synchronization
 * points recorded in the trace.
 */
/*-------------------------------------------------------------------------*/

class PatternDetectionTask : public pearl::ForwardReplayTask
{
  public:
    /// @name Constructors & destructor
    /// @{

    PatternDetectionTask(const TaskDataShared&  sharedData,
                         const TaskDataPrivate& privateData,
                         CbData*                callbackData);
    ~PatternDetectionTask();

    /// @}
    /// @name Adding performance patterns
    /// @{

    void add_pattern(Pattern* pattern);

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
    virtual bool finish();

    /// @}


  private:
    /// Container type for pattern objects
    typedef std::vector<Pattern*> PatternList;


    /// Local event trace data object
    pearl::LocalTrace& m_trace;

    /// Stores pointers to the individual pattern objects
    PatternList m_patterns;
};


}   // namespace scout


#endif   // !SCOUT_PATTERNDETECTIONTASK_H
