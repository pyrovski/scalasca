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
#endif   // _OPENMP

#include <cstdarg>
#include <cstdio>

#include "Logging.h"

using namespace std;
using namespace scout;


//--- Global variables ------------------------------------------------------

namespace
{

/// Global verbosity level
int logVerbosity = 0;

}   /* unnamed namespace */


//--- Logging functions -----------------------------------------------------

/**
 * @brief Selectively prints a log message.
 *
 * Prints the given log @a message to @c stdout if @a level is less than or
 * equal to the current verbosity level. Note that in the case of OpenMP,
 * only messages from the master thread are displayed.
 *
 * @param  level    Minimal verbosity level at which the message should be
 *                  displayed
 * @param  message  Log message (format string, printf style)
 * @param  ...      Variable argument list according to log message
 */
void scout::LogMsg(int level, const char* message, ...)
{
  // Initialize variable argument list
  va_list ap;
  va_start(ap, message);

#if defined(_OPENMP)
  if (0 == omp_get_thread_num())
  {
#endif   // _OPENMP

    // Display log message on master thread
    if (logVerbosity >= level)
    {
      vprintf(message, ap);
      fflush(stdout);
    }

#if defined(_OPENMP)
  }
#endif

  va_end(ap);
}


/**
 * @brief Gets the verbosity level.
 *
 * Returns the current verbosity level.
 *
 * @return Verbosity level
 */
int scout::LogGetVerbosity()
{
  return logVerbosity;
}


/**
 * @brief Set the verbosity level.
 *
 * Sets the verbosity level to the given value @a level.
 *
 * @param  level  New verbosity level
 */
void scout::LogSetVerbosity(int level)
{
  logVerbosity = level;
}
