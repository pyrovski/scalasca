/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_READDEFINITIONS_TASK
#define SCOUT_READDEFINITIONS_TASK


#include <stdint.h>

#include <CompoundTask.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  ReadDefinitionsTask.h
 * @brief Declaration of the ReadDefinitionsTask class.
 *
 * This header file provides the declaration of the ReadDefinitionsTask
 * class which is used to read in the global definition data.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

//--- Forward declarations --------------------------------------------------

struct TaskDataShared;


/*-------------------------------------------------------------------------*/
/**
 * @class ReadDefinitionsTask
 * @brief CompoundTask used to read the global definition data.
 *
 * Depending on the parallel programming paradigm used, the global definition
 * data is read in different ways. For serial and OpenMP, the data is read
 * directly from the global definition file. For MPI and hybrid MPI/OpenMP,
 * the data of the definition file is first read into a memory buffer on
 * the master process (rank 0), broadcast to all other processes, which then
 * initialize the GlobalDefs data structure from the memory buffer.
 */
/*-------------------------------------------------------------------------*/

class ReadDefinitionsTask : public pearl::CompoundTask
{
  public:
    /// @name Constructors & destructor
    /// @{

    ReadDefinitionsTask(TaskDataShared& sharedData);
    ~ReadDefinitionsTask();

    /// @}


  private:
    /// Shared task data object
    TaskDataShared& m_sharedData;

    /// Memory buffer used to broadcast definition data
    uint8_t* m_buffer;

    /// Size of the memory buffer
    long m_size;
};


}   // namespace scout


#endif   // !SCOUT_READDEFINITIONS_TASK
