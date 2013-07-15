/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#if defined(_OPENMP)
  #include <omp.h>
#elif defined(_MPI)
  #include <mpi.h>
#else   // !_OPENMP && !_MPI
  #include <sys/time.h>
  #include <time.h>
#endif   // !_OPENMP && !_MPI

#include "Timer.h"

using namespace scout;


//--- Macros ----------------------------------------------------------------

#if defined(_OPENMP)
  #define READ_TIMER   omp_get_wtime
#elif defined(_MPI)
  #define READ_TIMER   MPI_Wtime
#else   // !_OPENMP && !_MPI
  #define READ_TIMER   get_wtime
#endif   // !_OPENMP && !_MPI


//--- Function prototypes ---------------------------------------------------

namespace
{

//----- Helper functions -----

#if !defined(_OPENMP) && !defined(_MPI)

/**
 * @internal
 * @brief Provides a timestamp in seconds.
 *
 * This helper function provides a timestamp in seconds as double-precision
 * floating-point value based on the POSIX function gettimeofday(), i.e., the
 * time in seconds since the Epoch.
 *
 * @return Timestamp in seconds
 */
inline double get_wtime()
{
  struct timeval tv;

  if (0 == gettimeofday(&tv, NULL))
  {
    return tv.tv_sec + tv.tv_usec * 1.0e-6;
  }

  return 0.0;
}

#endif   // !_OPENMP && !_MPI

}   // unnamed namespace


/*
 *---------------------------------------------------------------------------
 *
 * class Timer
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Creates a new timer.
 *
 * Creates a new timer instance which is immediately started.
 */
Timer::Timer()
  : m_is_running(true),
    m_start(READ_TIMER())
{
}


//--- Starting and stopping the timer ---------------------------------------

/**
 * @brief Starts the timer.
 *
 * This method activates the timer and resets the starting time.
 */
void Timer::start()
{
  m_is_running = true;
  m_start      = READ_TIMER();
}


/**
 * @brief Stops the timer.
 *
 * This method stops the timer if it is active and stores its current value.
 * Otherwise, i.e., if the timer is not active, no operation is performed.
 */
void Timer::stop()
{
  if (!m_is_running)
    return;

  m_value      = READ_TIMER() - m_start;
  m_is_running = false;
}


//--- Reading the timer value -----------------------------------------------

/**
 * @brief Reads the timer value.
 *
 * Returns the current timer value in seconds as double-precision
 * floating-point value if the timer is active. Otherwise, the timer
 * value at the last call to the stop() method is returned.
 *
 * @return Current timer value
 */
double Timer::value()
{
  if (m_is_running)
    m_value = READ_TIMER() - m_start;

  return m_value;
}
