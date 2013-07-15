/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_TIMEDPHASE_H
#define SCOUT_TIMEDPHASE_H


#include <string>

#include <CompoundTask.h>

#include "Timer.h"


/*-------------------------------------------------------------------------*/
/**
 * @file  TimedPhase.h
 * @brief Declaration of the TimedPhase class.
 *
 * This header file provides the declaration of the TimedPhase class.
 */
/*-------------------------------------------------------------------------*/


namespace scout 
{

/*-------------------------------------------------------------------------*/
/**
 * @class TimedPhase
 * @brief A compound task that measures and logs its execution time.
 *
 * A TimedPhase is a compound task (derived from pearl::CompoundTask) which
 * measures and logs its overall execution time. The log message initially
 * consists of a user-defined string and three dots. After all component
 * tasks have finished successfully, the message "done (?.???s)." is printed,
 * where the question marks represent the wall-clock duration of the phase.
 */
/*-------------------------------------------------------------------------*/

class TimedPhase : public pearl::CompoundTask 
{
  public:
    /// @name Constructors & destructor
    /// @{

    explicit TimedPhase(const std::string& message);

    /// @}


  protected:
    /// @name Execution control
    /// @{

    virtual bool prepare();
    virtual bool finish();

    /// @}


  private:
    /// %Timer object used to determine execution time
    Timer m_timer;

    /// Log message to be written the phase is started
    std::string m_message;
};


}   // namespace scout


#endif   // !SCOUT_TIMEDPHASE_H
