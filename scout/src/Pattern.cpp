/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstring>

#if defined(_MPI)
  #include <mpi.h>
#endif   // _MPI
#if defined(_OPENMP)
  #include <omp.h>
#endif   // _OPENMP

#include <GlobalDefs.h>
#include <LocalTrace.h>
#include <Process.h>

#include "Pattern.h"
#include "Patterns_gen.h"
#include "ReportData.h"

using namespace std;
using namespace pearl;
using namespace scout;


//--- Symbolic names --------------------------------------------------------

#define VERSION        "1.4"
#define PATTERNS_URL   "@mirror@scalasca_patterns-" VERSION ".html#"


/*
 *---------------------------------------------------------------------------
 *
 * class Pattern
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Destroys the instance.
 *
 * Destructor. Destroys the instance and calls the cleanup() member function
 * to perform pattern-specific data management tasks.
 */
Pattern::~Pattern()
{
}


//--- Registering callbacks -------------------------------------------------

/**
 * @brief Registers the pattern-specific replay callbacks.
 *
 * This method registers the replay callbacks that need to be triggered
 * during the trace replay for this pattern and calls the init() member
 * function to perform pattern-specific data management tasks.
 *
 * @param  cbmanager  CallbackManager object with which the callbacks
 *                    should be registered
 */
void Pattern::reg_cb(pearl::CallbackManager* cbmanager)
{
  init();
}


//--- Retrieving pattern information ----------------------------------------

/**
 * @fn    virtual long Pattern::get_id() const
 * @brief Returns the pattern ID
 *
 * Returns the numeric identifier associated with this pattern.
 *
 * @return Pattern ID
 */


/**
 * @brief Returns the ID of the parent pattern
 *
 * Returns the numeric identifier associeted with the parent pattern in the
 * pattern hierarchy. The default implementation returns PAT_NONE.
 *
 * @return Parent pattern ID
 */
long Pattern::get_parent() const
{
  return PAT_NONE;
}


/**
 * @fn    virtual std::string Pattern::get_name() const
 * @brief Returns the pattern name
 *
 * Returns the display name of the pattern.
 *
 * @return Pattern name
 */


/**
 * @fn    virtual std::string Pattern::get_unique_name() const
 * @brief Returns the pattern's unique name
 *
 * Returns the unique name of the pattern which can be used to uniquely
 * identify a particular pattern.
 *
 * @return Unique name
 */


/**
 * @fn    virtual std::string Pattern::get_descr() const
 * @brief Returns a brief pattern description
 *
 * Returns a brief description of the pattern (approximately one line of
 * text).
 *
 * @return  Description
 */


/**
 * @fn    virtual std::string Pattern::get_unit() const
 * @brief Returns the pattern's unit of measurement.
 *
 * Returns the unit of measurement for this pattern. It can be either "sec"
 * (i.e., seconds), "occ" (i.e., occurrences) or "bytes".
 *
 * @return Unit of measurement
 */


/**
 * @brief Returns the pattern description URL.
 *
 * Returns the URL where a more detailed description of the pattern can be
 * found. It is composed of a base URL and the unique name.
 *
 * @return Description URL
 */
string Pattern::get_url() const
{
  return PATTERNS_URL + get_unique_name();
}


/**
 * @brief Returns the pattern's visibility.
 *
 * Returns the external visibility of the pattern. This allows for "internal"
 * patterns which can define callbacks and store data, but which are not
 * written to the generated analysis report file.
 *
 * @return Visibility
 */
bool Pattern::is_hidden() const
{
  return false;
}


//--- Writing severity values -----------------------------------------------

/**
 * @brief Collates the pattern's severities and writes the corresponding
 *        section in the CUBE report.
 *
 * Generates the severity section of the CUBE report for the pattern.
 * Therefore, it first collates the values from each process/thread on the
 * "master node" (the details depend on the parallel programming paradigm
 * used) and then writes the gathered data to file. This is done incrementally
 * per call-tree node.
 *
 * @param  data   Temporary data used for report writing
 * @param  rank   Global process rank (MPI rank or 0)
 * @param  trace  Local trace-data object
 * @param  fp     File pointer
 */
void Pattern::gen_severities(ReportData&              data,
                             int                      rank,
                             const pearl::LocalTrace& trace,
                             std::FILE*               fp)
{
  const pearl::GlobalDefs& defs = trace.get_definitions();

  //--- Determine thread information
  int num_threads;
  int thread_id;
  #if defined(_OPENMP)
    // This assumes that all processes have defined the same number of threads
    num_threads = defs.get_process(rank)->num_threads();
    thread_id   = omp_get_thread_num();
  #else   // !_OPENMP
    num_threads = 1;
    thread_id   = 0;
  #endif   // !_OPENMP

  //--- Set up local & global severity buffers
  // The local severity buffer is implicitly shared!
  static double* local_sev  = NULL;
  double*        global_sev = NULL;
  #pragma omp master
  {
    local_sev = new double[num_threads];
    memset(local_sev, 0, num_threads * sizeof(double));
    if (0 == rank) {
      int total_threads = defs.num_locations();
      global_sev = new double[total_threads];
      memset(global_sev, 0, total_threads * sizeof(double));
    }
  }

  //--- Collate results
  // Start at index 1 to ignore "UNKNOWN" node
  uint32_t count = defs.num_cnodes(); 
  for (ident_t i = 1; i < count; i++) {
    CNode* cnode = defs.get_cnode(i);
    if (skip_cnode(*cnode))
      continue;

    // Collate local severities
    #pragma omp barrier
    local_sev[thread_id] = 0;
    map<CNode*,double>::const_iterator it = m_severity.find(cnode);
    if (it != m_severity.end())
      local_sev[thread_id] = it->second;
    #pragma omp barrier

    #pragma omp master
    {
      #if defined(_MPI)
      // Gather data from all processes
        MPI_Gather(local_sev, num_threads, MPI_DOUBLE,
                   global_sev, num_threads, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
      #else   // !_MPI
        memcpy(global_sev, local_sev, num_threads * sizeof(double));
      #endif   // !_MPI

      if (0 == rank) {
        // Write data to file
        #if defined(_OPENMP)
          cube_write_sev_threads(data.cb, fp, data.metrics[get_id()],
                                 data.cnodes[i], global_sev);
        #else   // !_OPENMP
          cube_write_sev_row(data.cb, fp, data.metrics[get_id()],
                             data.cnodes[i], global_sev);
        #endif   // !_OPENMP
        fflush(fp);
      }
    }
  }

  //--- Release local & global severity buffers
  #pragma omp master
  {
    delete[] local_sev;
    delete[] global_sev;
  }
}


//--- Pattern management ----------------------------------------------------

/**
 * @brief Initializes local data.
 *
 * This method can be overwritten in derived classes to initialize local
 * data. The default implementation is empty.
 */
void Pattern::init()
{
}


//--- Data collation control ------------------------------------------------

/**
 * @brief Controls collation of data for the given call-tree node.
 *
 * This method can be overwritten in derived classes to control the collation
 * of severity data for the given call-tree node @a cnode. The method should
 * return @em true if it is guaranteed that no severity data exists for the
 * call-tree node (e.g., non-MPI call-tree nodes will always have a severity
 * of zero for MPI-specific patterns). Otherwise, the method returns @em false
 * (default).
 *
 * @param  cnode  Current call-tree node
 * @return Returns @em true if no severity data can exist, @em false otherwise
 */
bool Pattern::skip_cnode(const pearl::CNode& /* cnode */) const
{
  return false;
}
