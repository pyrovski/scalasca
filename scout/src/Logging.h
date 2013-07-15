/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_LOGGING_H
#define SCOUT_LOGGING_H


/*-------------------------------------------------------------------------*/
/**
 * @file  Logging.h
 * @brief Declaration of functions for printing log messages.
 *
 * This header file provides the declarations of a number of functions for
 * printing log messages. Using a global verbosity level, it is possible
 * to control the amount of information that is actually printed by the
 * application.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Function prototypes ---------------------------------------------------

//----- Logging functions -----

void LogMsg(int level, const char* message, ...);

int  LogGetVerbosity();
void LogSetVerbosity(int level);

}   // namespace scout


#endif   // !SCOUT_LOGGING_H
