/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>

#include <GlobalDefs.h>
#include <LocalTrace.h>

#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;


/*-------------------------------------------------------------------------*/
/**
 *  @file  TaskData.cpp
 *  @brief Implementation of the TaskDataShared and TaskDataPrivate structs.
 *
 *  This file provides the implementation of the TaskDataShared and
 *  TastDataPrivate structs used to transfer data between the individual
 *  tasks of the SCOUT trace analyzer.
 **/
/*-------------------------------------------------------------------------*/


//--- Constructors & destructor (TaskDataShared) ----------------------------

TaskDataShared::TaskDataShared()
    : mDefinitions(NULL)
{
}


TaskDataShared::~TaskDataShared()
{
    delete mDefinitions;
}


//--- Constructors & destructor (TaskDataPrivate) ---------------------------

TaskDataPrivate::TaskDataPrivate()
    : mTrace(NULL)
{
}


TaskDataPrivate::~TaskDataPrivate()
{
    delete mTrace;
}
