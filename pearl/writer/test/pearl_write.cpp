/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include <mpi.h>
#ifdef _OPENMP
#  include <omp.h>
#endif

#include <EpkWriter.h>
#include <Error.h>
#include <GlobalDefs.h>
#include <LocalTrace.h>
#include <pearl.h>
#include <pearl_replay.h>

using namespace std;
using namespace pearl;


/*
 *  Simple test program for the PEARL writer
 *
 *  Reads the trace data of the given experiment archive into memory and
 *  writes the data to an archive 'epik_test'.
 *
 *  Parameters:
 *    <archive>   Name of experiment archive
 */
int main(int argc, char** argv)
{
  // Initialize MPI
#if defined(_OPENMP) && (MPI_VERSION >= 2)
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if (provided == MPI_THREAD_SINGLE) {
    cerr << "MPI thread mode 'funneled' not supported!" << endl;
    exit(EXIT_FAILURE);
  }
#else
  MPI_Init(&argc, &argv);
#endif

  // Determine MPI rank
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Initialize PEARL
#ifdef _OPENMP
  PEARL_hybrid_init();
#else
  PEARL_mpi_init();
#endif

  // Validate arguments
  if (argc != 2) {
    cerr << "Usage: pearl_write <archive>" << endl;
    exit(EXIT_FAILURE);
  }
  
  // Read global definitions
  GlobalDefs* defs = NULL;
  try {
    defs = new GlobalDefs(argv[1]);
  }
  catch (const Error& error) {
    cerr << error.what() << endl;
    exit(EXIT_FAILURE);
  }

  // Eventually go hybrid...
  #pragma omp parallel
  {
    // Determine thread ID
    int tid = 0;
#ifdef _OPENMP
    tid = omp_get_thread_num();
#endif

    // Read trace data
    LocalTrace* trace = NULL;
    try {
      trace = new LocalTrace(*defs, argv[1], rank, tid);
    }
    catch (const Error& error) {
      cerr << error.what() << endl;
      exit(EXIT_FAILURE);
    }

    // Verify whether definitions provided complete call tree
#ifdef _OPENMP
    PEARL_omp_verify_calltree(*defs, *trace);
#else
    PEARL_verify_calltree(*defs, *trace);
#endif

    // Generate global call tree is per-process call trees were incomplete
    #pragma omp master
    {
      PEARL_mpi_unify_calltree(*defs);
    }
    #pragma omp barrier

    // Required trace preprocessing
#ifdef _OPENMP
    PEARL_omp_preprocess_trace(*defs, *trace);
#else
    PEARL_preprocess_trace(*defs, *trace);
#endif

    // Re-write trace data
    EpkWriter writer;
    writer.write("epik_test", *trace, *defs);

    #pragma omp barrier

    // Release trace data
    delete trace;
  }

  // Delete definition data
  delete defs;

  // Finalize MPI
  MPI_Finalize();

  return EXIT_SUCCESS;  
}
