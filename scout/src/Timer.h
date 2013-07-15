/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_TIMER_H
#define SCOUT_TIMER_H


/*-------------------------------------------------------------------------*/
/**
 * @file  Timer.h
 * @brief Declaration of the Timer class.
 *
 * This header file provides the declaration of the Timer class which can
 * be used to measure durations in wall-clock time.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

/*-------------------------------------------------------------------------*/
/**
 * @class Timer
 * @brief Simple timer class to measure durations in wall-clock time.
 *
 * This class provides a simple timer to measure durations in wall-clock
 * time. Depending on the programming model used, it is either based on
 * omp_get_wtime(), MPI_Wtime() or the POSIX function gettimeofday().
 */
/*-------------------------------------------------------------------------*/

class Timer
{
  public:
    /// @name Constructors & destructor
    /// @{

    Timer();

    /// @}
    /// @name Starting and stopping the timer
    /// @{

    void start();
    void stop();

    /// @}
    /// @name Reading the timer value
    /// @{

    double value();

    /// @}


  private:
    /// Flag indicating whether the timer is running or not
    bool m_is_running;

    /// Timestamp when the timer was started
    double m_start;

    /// Current timer value
    double m_value;
};


}   // namespace scout


#endif   // !SCOUT_TIMER_H
