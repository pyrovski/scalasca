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

#ifndef SILAS_TIMER_H
#define SILAS_TIMER_H

#include <string>

namespace silas
{

class Timer
{
  public:
    /* Constructors & destructor */
    Timer();

    /* Starting and stopping the timer */
    void start();
    void stop();

    /* Reading the timer value */
    double      value();
    std::string value_str();


  private:
    /* Data */
    bool   m_is_running;   // indicates whether the timer is running or not
    double m_start;        // starting time
    double m_value;        // current timer value
};


}   /* namespace silas */

#endif   /* !SILAS_TIMER_H */
