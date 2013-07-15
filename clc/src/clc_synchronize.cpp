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


#include <cerrno>
#include <cstring>
#include <iostream>

#include <mpi.h>
#ifdef _OPENMP
#  include <omp.h>
#  include <OmpData.h>
#endif

#include <pearl.h>
#include <Error.h>
#include <GlobalDefs.h>
#include <LocalTrace.h>
#include <Synchronizer.h>
#include <EpkWriter.h>

using namespace std;
using namespace pearl;

int main(int argc, char** argv) {

#if defined(_OPENMP) && (MPI_VERSION >= 2)
  int provided;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
  if (provided < MPI_THREAD_FUNNELED) {
    cerr << "Continuing even though MPI thread mode 'funneled' is not supported..." << endl;
  }
#else
  MPI_Init(&argc, &argv);
#endif

  // Determine MPI rank
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Initialize PEARL & validate arguments
#ifdef _OPENMP
  PEARL_hybrid_init();
#else
  PEARL_mpi_init();
#endif

  // Read global definitions
  // All processes will read the same file simultaneously. In case of an error,
  // bad things can happen if not all processes fail at the same time. More
  // error checking is needed here...
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
    // Each process/thread reads only "his" trace. In case of an error, bad
    // things can happen if not all processes fail at the same time. More
    // error checking is needed here...
    LocalTrace* trace = NULL;
    try {
      trace = new LocalTrace(*defs, argv[1], rank, tid);
    }
    catch (const Error& error) {
      cerr << error.what() << endl;
      exit(EXIT_FAILURE);
    }

    // Verify whether definitions provided complete call tree;
    // if not, re-generate process-local call tree from trace data
    // Both calls can throw exceptions which we silently ignore here...
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

    // Create one Synchronizer object per process
    Synchronizer *sync = new Synchronizer(rank, *trace);

    // Synchronize event stream
    sync->synchronize();

    // Print results
    sync->print_statistics();

    // Write synchronized event stream to file
    EpkWriter* writer = new EpkWriter();
    writer->write("epik_sync", *trace, *defs);

    // Clean up allocated memory
#pragma omp barrier
    delete trace;
    delete sync;

  }

  delete defs;

  // Finalize MPI and exit
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();

  return EXIT_SUCCESS;  
}

