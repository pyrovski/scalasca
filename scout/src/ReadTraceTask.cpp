/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>

#if defined(_MPI)
  #include <mpi.h>
#endif   // _MPI
#if defined(_OPENMP)
  #include <omp.h>
#endif

#ifdef USE_SIONLIB
  #include <cstdlib>
  #include <sion.h>
#endif

#include <epk_archive.h>

#include <Error.h>
#include <LocalTrace.h>

#include "ReadTraceTask.h"
#include "TaskData.h"

using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class ReadTraceTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new trace reader task instance.
 *
 * This constructor creates a new trace data reader task with the given
 * parameters.
 *
 * @param  sharedData   Shared task data object
 * @param  privateData  Private task data object
 */
ReadTraceTask::ReadTraceTask(const TaskDataShared& sharedData,
                             TaskDataPrivate&      privateData)
  : Task(),
    m_sharedData(sharedData),
    m_privateData(privateData)
{
}


//--- Execution control -----------------------------------------------------

/**
 * @brief Executes the task.
 *
 * Initializes the local trace dataobject by reading the corresponding trace
 * file from disk.
 *
 * @return Returns @em true if successful, @em false otherwise
 */
bool ReadTraceTask::execute()
{
  // FIXME: This extra block is required for some versions of the
  //        Intel 11 compilers
  {
    // Determine thread ID
    int tid = 0;
    #if defined(_OPENMP)
      tid = omp_get_thread_num();
    #endif   // _OPENMP

    // Determine process rank
    int rank = 0;
    #if defined(_MPI)
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    #endif   // _MPI

    // Open SION file (if applicable)
    int sion_id = -1;
    #if defined(USE_SIONLIB)
      // Determine SION file name
      char* archive_dir = epk_archive_directory(EPK_TYPE_ELG);
      char* sion_fname  = epk_archive_filename(EPK_TYPE_SION, archive_dir);
      free(archive_dir);

      // Collective open if SION file exists
      int is_sion_file = _sion_stat_file(sion_fname);
      if (is_sion_file) {
        sion_int64 localsize;
        sion_int32 fsblocksize;
        int        sion_rank = rank;
        FILE*      fp;
        #if defined(_MPI)
          int      numfiles;
          MPI_Comm lcomm = MPI_COMM_WORLD;
        #endif

        #if (!defined(_MPI) && defined(_OPENMP))
          sion_id = sion_paropen_omp(sion_fname, "rb",
                                     &localsize, &fsblocksize,
                                     &sion_rank, &fp, NULL);
        #elif (defined(_MPI) && !defined(_OPENMP))
          sion_id = sion_paropen_mpi(sion_fname, "rb",
                                     &numfiles, MPI_COMM_WORLD, &lcomm,
                                     &localsize, &fsblocksize,
                                     &sion_rank, &fp, NULL);
        #elif (defined(_MPI) && defined(_OPENMP))
          sion_id = sion_paropen_ompi(sion_fname, "rb",
                                      &numfiles, MPI_COMM_WORLD, &lcomm,
                                      &localsize, &fsblocksize,
                                      &sion_rank, &fp, NULL);
        #else
          #error "No SIONlib support for this programming model!"
        #endif
      }

      // Free resources
      free(sion_fname);
    #endif   // USE_SIONLIB

    // Read trace data
    m_privateData.mTrace = new LocalTrace(*m_sharedData.mDefinitions,
                                          epk_archive_get_name(), rank, tid,
                                          sion_id);

    // Close SION file (if applicable)
    #if defined(USE_SIONLIB)
      if (is_sion_file) {
        #if (!defined(_MPI) && defined(_OPENMP))
          sion_parclose_omp(sion_id);
        #elif (defined(_MPI) && !defined(_OPENMP))
          sion_parclose_mpi(sion_id);
        #elif (defined(_MPI) && defined(_OPENMP))
          sion_parclose_ompi(sion_id);
        #else
          #error "No SIONlib support for this programming model!"
        #endif
      }
    #endif   // USE_SIONLIB

    // Skip further checks in case of error
    if (NULL == m_privateData.mTrace)
      return false;

    #if defined(_OPENMP)
      const LocalTrace& trace = *m_privateData.mTrace; 

      // These variables are implicitly shared
      static uint32_t num_parallel = 0;
      static bool     is_ok        = true;

      //--- Ensure equal number of OpenMP parallel regions per thread ---
      // Initialize number of OpenMP parallel regions by master thread
      #pragma omp master
      {
        num_parallel = trace.num_omp_parallel();
      }

      // Compare parallel region count on each thread and update status
      #pragma omp barrier
      #pragma omp critical
      {
        is_ok = is_ok && (num_parallel == trace.num_omp_parallel());
      }
      #pragma omp barrier

      // Check status and abort if numbers don't match
      if (!is_ok)
        throw FatalError("Varying numbers of threads in OpenMP parallel regions "
                         "not supported!");

      //--- Ensure that MPI calls only occur on master thread ---
      // Compare MPI region count on each thread and update status
      #pragma omp barrier
      #pragma omp critical
      {
        if (0 != omp_get_thread_num())
          is_ok = is_ok && (0 == trace.num_mpi_regions());
      }
      #pragma omp barrier

      // Check status and abort if MPI calls found on non-master thread
      if (!is_ok)
        throw FatalError("MPI calls on threads other than the master thread "
                         "not supported!");
    #endif   // _OPENMP
  }

  return true;
}
