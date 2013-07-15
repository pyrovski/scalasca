/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

#if defined(_MPI)
  #include <mpi.h>
#endif   // _MPI

#include <epk_archive.h>

#include <Error.h>

#include "CheckedTask.h"
#include "GlobalDefs.h"
#include "ReadDefinitionsTask.h"
#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;


//--- Helper classes --------------------------------------------------------

namespace
{

#if defined(_MPI) && !defined(SCOUT_NO_DEFS_BCAST)

  /**
   * @class ReadFileToBuffer
   * @brief Task reading the global definition data to a memory buffer.
   *
   * This task reads the contents of the global definition file to a memory
   * buffer, for example, to transfer the data to other processes.
   */

  class ReadFileToBuffer : public pearl::Task
  {
    public:
      /// @name Constructors & destructor
      /// @{

      ReadFileToBuffer(uint8_t*& buffer, long& size);

      /// @}
      /// @name Execution control
      /// @{

      virtual bool execute();

      // @}


    private:
      /// Pointer to memory buffer
      uint8_t*& m_buffer;

      /// Size of memory buffer
      long& m_size;
  };


  /**
   * @class BroadcastDefs
   * @brief Task transferring the definition file data to all processes.
   *
   * This task broadcasts the definition file data read by a previously
   * executed ReadFileToBuffer task to all processes.
   */
  class BroadcastDefs : public pearl::Task
  {
    public:
      /// @name Constructors & destructor
      /// @{

      BroadcastDefs(TaskDataShared& sharedData, uint8_t*& buffer, long& size);

      /// @}
      /// @name Execution control
      /// @{

      virtual bool execute();

      /// @}


    private:
      /// Shared task data object
      TaskDataShared& m_sharedData;

      /// Pointer to memory buffer
      uint8_t*& m_buffer;

      /// Size of the memory buffer
      long& m_size;
  };

#else   // !_MPI || SCOUT_NO_DEFS_BCAST

/**
 * @class ReadFile
 * @brief Task reading the global definitions data from file.
 *
 * This task reads the definition data from the global definition file and
 * initializes the global definitions object.
 */
class ReadFile : public pearl::Task
{
  public:
    /// @name Constructors & destructor
    /// @{

    ReadFile(TaskDataShared& sharedData);

    /// @}
    /// @name Execution control
    /// @{

    virtual bool execute();

    /// @}


  private:
    /// Shared task data object
    TaskDataShared& m_sharedData;
};

#endif   // !_MPI || SCOUT_NO_DEFS_BCAST

}   // unnamed namespace


/*
 *---------------------------------------------------------------------------
 *
 * class ReadDefinitionsTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new ReadDefinitionsTask instance.
 *
 * This constructor creates a new ReadDefinitionsTask instance by setting
 * up the sub-tasks and initializing internal data structures.
 *
 * @param  sharedData  Shared task data object
 */
ReadDefinitionsTask::ReadDefinitionsTask(TaskDataShared& sharedData)
  : CompoundTask(),
    m_sharedData(sharedData),
    m_buffer(NULL),
    m_size(0)
{
  #if defined(_MPI) && !defined(SCOUT_NO_DEFS_BCAST)
    add_task(CheckedTask::make_checked(new ReadFileToBuffer(m_buffer, m_size)));
    add_task(CheckedTask::make_checked(new BroadcastDefs(m_sharedData,
                                                         m_buffer, m_size)));
  #else
    add_task(CheckedTask::make_checked(new ReadFile(m_sharedData)));
  #endif   // _MPI
}


/**
 * @brief Destroys the instance.
 *
 * Destructor. Destroys the instance and releases all occupied resources.
 */
ReadDefinitionsTask::~ReadDefinitionsTask()
{
  delete[] m_buffer;
}


#if defined(_MPI) && !defined(SCOUT_NO_DEFS_BCAST)

/*
 *---------------------------------------------------------------------------
 *
 * class ReadFileToBuffer
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new ReadFileToBuffer task instance.
 *
 * This constructor creates a new task instance for reading the contents of
 * the global definition file into a memory buffer using the given parameters.
 *
 * @param  buffer  Pointer to the memory buffer
 * @param  size    Size of the memory buffer
 */
ReadFileToBuffer::ReadFileToBuffer(uint8_t*& buffer, long& size)
  : Task(),
    m_buffer(buffer),
    m_size(size)
{
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Reads in the definition file data into a newly allocated memory buffer.
 *
 * @return Returns @em true if successful, @em false otherwise
 * @throw  pearl::FatalError  if the definition file could not be read.
 */
bool ReadFileToBuffer::execute()
{
  // Determine process rank
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Read definitions on master rank
  if (0 == rank) {
    char* name = epk_archive_filename(EPK_TYPE_ESD, epk_archive_get_name());

    // Determine file size
    struct stat info;
    if (stat(name, &info) != 0)
      throw FatalError("Unable to determine trace definition file size.");
    m_size = info.st_size;

    // Allocate buffer
    m_buffer = new uint8_t[m_size];

    // Read definition file data
    FILE* fp = fopen(name, "rb");
    if (NULL == fp)
      throw FatalError("Could not open trace definition file.");
    if (1 != fread(m_buffer, m_size, 1, fp)) {
      fclose(fp);
      throw FatalError("Error reading trace definition file.");
    }
    fclose(fp);

    free(name);
  }

  return true;
}


/*
 *---------------------------------------------------------------------------
 *
 * class BroadcastDefs
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new BroadcastDefs task instance.
 *
 * This constructor creates a new task instance for broadcasting the contents
 * of the global definition file to all processes and initializing the global
 * definitions object.
 *
 * @param  sharedData  Shared task data object
 * @param  buffer      Pointer to the memory buffer
 * @param  size        Size of the memory buffer
 */
BroadcastDefs::BroadcastDefs(TaskDataShared& sharedData,
                             uint8_t*&       buffer,
                             long&           size)
  : Task(),
    m_sharedData(sharedData),
    m_buffer(buffer),
    m_size(size)
{
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Broadcasts the contents of the global definition file from the master
 * process to all other processes and initializes the global definitions
 * object accordingly. To avoid failures in the MPI transfer due to
 * insufficient buffer space, the broadcast is done in chunks of 1 MB.
 *
 * @return Returns @em true if successful, @em false otherwise
 */
bool BroadcastDefs::execute()
{
  // Determine process rank
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Broadcast buffer size & allocate buffer
  MPI_Bcast(&m_size, 1, MPI_LONG, 0, MPI_COMM_WORLD);
  if (0 != rank)
    m_buffer = new uint8_t[m_size];

  // Transfer definitions in chunks of 1 MB
  // Larger transfers might lead to lock-ups in the MPI due
  // to insufficient buffer space.
  uint8_t* bufptr = m_buffer;
  long     bytes  = m_size;
  while (bytes > 0) {
    long chunk = min(bytes, static_cast<long>(1024 * 1024));

    MPI_Bcast(bufptr, chunk, MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    bufptr += chunk;
    bytes  -= chunk;
  }

  m_sharedData.mDefinitions = new GlobalDefs(m_buffer, m_size);

  delete[] m_buffer;
  m_buffer = 0;
  m_size   = 0;

  return (NULL != m_sharedData.mDefinitions);
}

#else   // !_MPI || SCOUT_NO_DEFS_BCAST

/*
 *---------------------------------------------------------------------------
 *
 * class ReadFile
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new BroadcastDefs task instance.
 *
 * This constructor creates a new task instance for initializing the global
 * definitions object directly from the global definition file.
 *
 * @param  sharedData  Shared task data object
 */
ReadFile::ReadFile(TaskDataShared& sharedData)
  : Task(),
    m_sharedData(sharedData)
{
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Initializes the global definitions object by reading the global definition
 * file from disk.
 *
 * @return Returns @em true if successful, @em false otherwise
 */
bool ReadFile::execute()
{
  // Read definitions
  m_sharedData.mDefinitions = new GlobalDefs(epk_archive_get_name());

  return (NULL != m_sharedData.mDefinitions);
}

#endif   // !_MPI || SCOUT_NO_DEFS_BCAST
