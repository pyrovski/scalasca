/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <iomanip>
#include <sstream>

#include <mpi.h>

#include "Timer.h"

using namespace std;
using namespace silas;


//--- Constructors & destructor ---------------------------------------------

Timer::Timer()
  : m_is_running(true),
    m_start(MPI_Wtime())
{
}


//--- Starting and stopping the timer ---------------------------------------

void Timer::start()
{
  m_is_running = true;
  m_start      = MPI_Wtime();
}


void Timer::stop()
{
  if (!m_is_running)
    return;

  m_value      = MPI_Wtime() - m_start;
  m_is_running = false;
}


//--- Reading the timer value -----------------------------------------------

double Timer::value()
{
  if (m_is_running)
    m_value = MPI_Wtime() - m_start;

  return m_value;
}


string Timer::value_str()
{
  if (m_is_running)
    m_value = MPI_Wtime() - m_start;

  // Assemble time string
  ostringstream result;
  result << setprecision(3) << fixed << m_value << "s";

  return result.str();
}
