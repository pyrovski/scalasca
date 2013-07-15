/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <climits>
#include <cstdlib>
#include <sstream>
#include <unistd.h>

#if defined(_MPI)
  #include <mpi.h>
  #include <epk_defs_mpi.h>

  #include "MpiDatatypes.h"
  #include "MpiOperators.h"
  #include "SynchronizeTask.h"

#endif   // _MPI
#if defined(_OPENMP)
  #include <omp.h>
#endif   // _OPENMP

#include <elg_error.h>
#include <epk_archive.h>

#include <CompoundTask.h>
#include <GlobalDefs.h>
#include <Process.h>
#include <Thread.h>
#include <pearl.h>

#include "AnalyzeTask.h"
#include "CheckedTask.h"
#include "Logging.h"
#include "PreprocessTask.h"
#include "ReadDefinitionsTask.h"
#include "ReadTraceTask.h"
#include "ReportWriterTask.h"
#include "StatisticsTask.h"
#include "TaskData.h"
#include "TimedPhase.h"
#include "Timer.h"
#include "scout_types.h"

using namespace std;
using namespace pearl;
using namespace scout;


//--- Constants -------------------------------------------------------------

const char* const copyright =
    "Copyright (c) 1998-2013 Forschungszentrum Juelich GmbH";


//--- Global variables ------------------------------------------------------

namespace
{

/// Global process rank; always zero for non-MPI
int rank = 0;

#if defined(_MPI)
  /// Flag indicating whether CLC-based timestamp synchronization should
  /// be applied. Only makes sense for MPI and hybrid MPI/OpenMP.
  bool synchronize = false;

  /// Flag indicating whether MPI has been finalized
  bool finalized = false;
#endif   // _MPI

}   // unnamed namespace

  /// Flag indicating whether metric statistics and most-severe instance
  /// tracking should be enabled.
  bool enableStatistics = false;

//--- Function prototypes ---------------------------------------------------

//----- Generic functions -----

int main(int argc, char** argv);

void Initialize(int* argc, char*** argv);
void Finalize();

void ProcessArguments(int argc, char** argv);

void ReadDefinitions(TaskDataShared& sharedData);
void SetupPhases(CompoundTask&    scout,
                 TaskDataShared&  sharedData,
                 TaskDataPrivate& privateData);
void Analyze();


//----- MPI-specific functions -----

#if defined(_MPI)
  void InitializeMPI(int* argc, char*** argv);
  void FinalizeMPI();

  extern "C" void ExitHandlerMPI();

  void ValidateParallelizationMPI(const GlobalDefs& defs);
#endif   // _MPI


//----- OpenMP-specific functions -----

#if defined(_OPENMP)
  void InitializeOMP();

  void ValidateParallelizationOMP(int num_threads);
#endif   // _OPENMP


/*
 *---------------------------------------------------------------------------
 *
 * SCOUT  -  SCalable Optimization UTility
 *
 *---------------------------------------------------------------------------
 */

int main(int argc, char** argv)
{
  // Setup
  Initialize(&argc, &argv);

  // Display copyright notice
  LogMsg(0, "SCOUT   %s\n\n", copyright);

  // Process command-line arguments
  ProcessArguments(argc, argv);

  // Display archive name
  LogMsg(0, "Analyzing experiment archive %s\n\n", epk_archive_get_name());

  // Parallel trace analysis
  Timer total;
  Analyze();

  // Clean up & exit
  LogMsg(0, "\nTotal processing time     : %.3fs\n", total.value());
  Finalize();
  return EXIT_SUCCESS;
}


//--- Generic functions -----------------------------------------------------

/**
 * @brief Program initialization.
 *
 * Performs all necessary initialization steps at program startup. This
 * includes the initialization of global variables as well as required
 * libraries.
 *
 * @param  argc  Pointer to argument count parameter of main()
 * @param  argv  Pointer to argument vector parameter of main()
 */
void Initialize(int* argc, char*** argv)
{
  #if defined(_MPI)
    InitializeMPI(argc, argv);
  #endif   // _MPI

  #if defined(_OPENMP)
    InitializeOMP();
  #endif   // _OPENMP

  // Initialize PEARL
  #if defined(_OPENMP) && defined(_MPI)   // hybrid MPI/OpenMP
    PEARL_hybrid_init();
  #elif defined(_OPENMP)   // pure OpenMP
    PEARL_omp_init();
  #elif defined(_MPI)   // pure MPI
    PEARL_mpi_init();
  #else   // serial
    PEARL_init();
  #endif   // serial
}


/**
 * @brief Program finalization.
 *
 * Performs all necessary clean-up steps at program finalization.
 */
void Finalize()
{
  #if defined(_MPI)
    #pragma omp master
    {
      FinalizeMPI();
    }
  #endif
}


/**
 * @brief Process command line arguments.
 *
 * Processes the command line arguments given to the analyzer.
 *
 * @param  argc  Argument count parameter of main()
 * @param  argv  Argument vector parameter of main()
 */
void ProcessArguments(int argc, char** argv)
{
  // Define some strings based on programming model
    const char* const options  = "isv";
  #if defined(_MPI)
    const char* const launcher = "<launchcmd> ";
  #else   // !_MPI
    const char* const launcher = "";
  #endif   // !_MPI

  // Suppress getopt's warning about unknown characters
  opterr = 0;

  // Process argument list
  int arg;
  while (-1 != (arg = getopt(argc, argv, options))) {
    switch (arg) {
      //--- Generic options ---
      case 'v':
        LogSetVerbosity(LogGetVerbosity() + 1);
        break;
      case 'i':
        enableStatistics = true;
        break;

      //--- MPI-specific options ---
      case 's':
        #if defined(_MPI)
          synchronize = true;
        #else
          LogMsg(0, "Ignoring command-line option '-s' -- not supported.\n\n");
        #endif   // _MPI
        break;

      default:
        break;
    }

    if ('?' == arg)
      break;
  }

  // Validate argument list & print usage info if necessary
  if ('?' == arg || argc - optind > 1) {
    LogMsg(0, "Usage: %s%s [OPTION]... [ARCHIVE]\n"
              "Options:\n"
    #if defined(_MPI)
              "   -i   Enable most-severe instance tracking and statistics\n"
              "   -s   Synchronize timestamps\n"
    #endif   // _MPI
              "   -v   Increase verbosity\n\n",
              launcher, argv[0]);
    if (CheckGlobalError(true, "Aborted.")) {
      Finalize();
      exit(EXIT_FAILURE);
    }
  }

  // Treat first remaining argument as experiment archive name
  if (1 == argc - optind) {
    string archive = argv[optind];
    bool   result  = !epk_archive_set_name(archive.c_str()) ||
                     !epk_archive_exists(NULL);

    if (CheckGlobalError(result, "Archive \"" + archive +
                                 "\" does not exist or is not an EPIK archive!")) {
      Finalize();
      exit(EXIT_FAILURE);
    }
  }
}


/**
 * @brief Reads the global definitions.
 *
 * Reads the global definition data from disk and sets up the corresponding
 * data structure.
 *
 * @param  sharedData  Shared task data object
 */
void ReadDefinitions(TaskDataShared& sharedData)
{
  // Set up definition reading phase
  TimedPhase phase("Reading definitions file ");
  phase.add_task(new ReadDefinitionsTask(sharedData));

  // Execute phase & handle errors
  if (!phase.execute()) {
    Finalize();
    exit(EXIT_FAILURE);
  }
}


/**
 * @brief Sets up the individual execution phases of SCOUT.
 *
 * This function creates the task objects for the individual execution phases
 * of SCOUT and adds them to the given compound task @p scout.
 *
 * @param  scout        Compound task to be set up
 * @param  sharedData   Shared task data object
 * @param  privateData  Private task data object
 */
void SetupPhases(pearl::CompoundTask& scout,
                 TaskDataShared&      sharedData,
                 TaskDataPrivate&     privateData)
{
  TimedPhase*    phase;
  ReadTraceTask* reader;
  phase  = new TimedPhase("Reading event trace files");
  reader = new ReadTraceTask(sharedData, privateData);
  phase->add_task(CheckedTask::make_checked(reader));
  scout.add_task(phase);

  PreprocessTask* prep;
  phase = new TimedPhase("Preprocessing            ");
  prep  = new PreprocessTask(sharedData, privateData);
  phase->add_task(prep);
  scout.add_task(phase);

  SynchronizeTask* synchronizer = NULL;
  #if defined(_MPI)
    if (synchronize) {
      phase = new TimedPhase("Timestamp synchronization");
      synchronizer = new SynchronizeTask(privateData);
      phase->add_task(CheckedTask::make_checked(synchronizer));
      scout.add_task(phase);
    }
  #endif

  AnalyzeTask* analyzer;
  phase    = new TimedPhase("Analyzing trace data     ");
  analyzer = new AnalyzeTask(sharedData, privateData);
  phase->add_task(analyzer);
  scout.add_task(phase);

  ReportWriterTask* writer;
  phase  = new TimedPhase("Writing report files     ");
  writer = new ReportWriterTask(sharedData, analyzer, epk_archive_get_name(), rank);
  phase->add_task(CheckedTask::make_checked(writer));
  scout.add_task(phase);

  StatisticsTask* stats;
  stats = new StatisticsTask(privateData, synchronizer);
  analyzer->register_statistics_cbs(stats);
  scout.add_task(stats);
}


/**
 * @brief Parallel trace analysis core.
 *
 * This is the core of the parallel trace analyzer. It first reads in the
 * required data files (definitions, trace data), performs some preprocessing,
 * executes the parallel pattern search, and then collates the analysis report.
 */
void Analyze()
{
  // Read global definitions
  TaskDataShared sharedData;
  ReadDefinitions(sharedData);

  #if defined(_MPI) || defined(_OPENMP)
    const GlobalDefs& defs = *sharedData.mDefinitions;
  #endif   // _MPI || _OPENMP
  #if defined(_MPI)
    ValidateParallelizationMPI(defs);
  #endif   // _MPI

  // Eventually go hybrid...
  #if defined(_OPENMP)
    // Determine number of non-VOID threads
    Process* process = defs.get_process(rank);
    uint32_t count   = process->num_threads();
    int num_threads  = 0;
    for (uint32_t i = 0; i < count; ++i)
      if (process->get_thread(i)->get_name() != "VOID")
        ++num_threads;

    // Configure OpenMP runtime appropriately
    omp_set_num_threads(num_threads);
  #endif   // _OPENMP
  #pragma omp parallel
  {
    #if defined(_OPENMP)
      ValidateParallelizationOMP(num_threads);
    #endif   // _OPENMP

    // Set up & run SCOUT phases
    CompoundTask    scout;
    TaskDataPrivate privateData;
    SetupPhases(scout, sharedData, privateData);
    if (CheckGlobalError(!scout.execute(), "Internal error!")) {
      Finalize();
      exit(EXIT_FAILURE);
    }
  }   // omp parallel
}


//--- MPI-specific functions ------------------------------------------------

#if defined(_MPI)

/**
 * @brief MPI-specific initialization.
 *
 * Performs all MPI-specific initialization steps, i.e., initialization of
 * the library, registration of user-defined reduction operators, datatypes,
 * etc.
 *
 * @param  argc  Pointer to argument count parameter of main()
 * @param  argv  Pointer to argument vector parameter of main()
 */
void InitializeMPI(int* argc, char*** argv)
{
  #if defined(HAS_MPI_INIT_THREAD)

    // Initialize MPI with required level of thread support
    int required;
    int provided;
    #if defined(_OPENMP)
      required = MPI_THREAD_FUNNELED;
    #else
      required = MPI_THREAD_SINGLE;
    #endif   // _OPENMP
    MPI_Init_thread(argc, argv, required, &provided);

  #else   // !HAS_MPI_INIT_THREAD

    // Initialize MPI
    MPI_Init(argc, argv);

  #endif   // !HAS_MPI_INIT_THREAD

  // Determine global process rank
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Initialize logging
  if (0 != rank)
    LogSetVerbosity(INT_MIN);
  elg_error_pid(rank);

  #if defined(HAS_MPI_INIT_THREAD)

    // Verify thread level
    if (provided < required) {
      LogMsg(0, "MPI thread mode 'funneled' not supported!\n");
      LogMsg(0, "Continuing with analysis, but you might experience problems.\n\n");
    }

  #elif defined(_OPENMP)   // !HAS_MPI_INIT_THREAD && _OPENMP

    // Display warning since level of thread support is implementation defined
    LogMsg(0, "Using MPI_Init; level of thread support unknown!\n");
    LogMsg(0, "Continuing with analysis, but you might experience problems.\n\n");

  #endif   // !HAS_MPI_INIT_THREAD && _OPENMP

  // Register user-defined reduction operators
  InitDatatypes();
  InitOperators();

  // Register MPI-specific exit handler
  if (0 != atexit(ExitHandlerMPI))
    LogMsg(0, "Unable to register exit handler!\n");
}


/**
 * @brief MPI-specific finalization.
 *
 * Performs all MPI-specific finalization steps, i.e., de-registration of
 * user-defined datatypes, reduction operators, etc.
 */
void FinalizeMPI()
{
  // De-register user-defined operators & datatypes
  FreeOperators();
  FreeDatatypes();

  MPI_Finalize();
  finalized = true;
}


/**
 * @brief MPI-specific exit handler.
 *
 * This MPI-specific exit handler triggers MPI_Abort() when the application
 * exits unexpectedly (e.g., when only a single process calls exit()).
 */
void ExitHandlerMPI()
{
  // In case of a "controlled" exit (i.e., at the end of execution or as a
  // result of handling an error) MPI is already finalized. We need to call
  // MPI_Abort() only in unexpected situations.
  if (!finalized)
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
}


/**
 * @brief Validates whether the number of processes matches the experiment.
 *
 * This function validates whether the number of MPI processes used matches
 * the number of processes used to record the experiment. If this condition
 * is not fulfilled, SCOUT is gracefully aborted.
 *
 * @param  defs  Global definitions object
 */
void ValidateParallelizationMPI(const GlobalDefs& defs)
{
  // Determine number of required processes
  int required = defs.num_processes();

  // Determine number of provided processes
  int provided;
  MPI_Comm_size(MPI_COMM_WORLD, &provided);

  // Make sure that the number of processes matches the experiment
  ostringstream message;
  message << "Number of processes does not match experiment (required "
          << required << ", got " << provided << ")!";
  if (CheckGlobalError(required != provided, message.str())) {
    Finalize();
    exit(EXIT_FAILURE);
  }
}

#endif   // _MPI


//--- OpenMP-specific functions ---------------------------------------------

#if defined(_OPENMP)

/**
 * @brief OpenMP-specific initialization.
 *
 * Performs all OpenMP-specific initialization steps.
 */
void InitializeOMP()
{
  // Initialize logging
  elg_error_thread_init(omp_get_thread_num);
}

/**
 * @brief Validates whether the number of threads matches the experiment.
 *
 * This function validates whether the number of threads used matches the
 * number of threads used to record the experiment (on a per-process basis).
 * If this condition is not fulfilled, SCOUT is gracefully aborted.
 *
 * @param  num_threads  Number of threads used in the experiment
 */
void ValidateParallelizationOMP(int num_threads)
{
  // Determine number of provided threads
  int provided = omp_get_num_threads();

  // Make sure that the number of threads matches the experiment
  ostringstream message;
  message << "Number of threads does not match experiment (required "
          << num_threads << ", got " << provided << ")!";
  if (CheckGlobalError(num_threads != provided, message.str())) {
    Finalize();
    exit(EXIT_FAILURE);
  }
}

#endif   // _OPENMP
