/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <new>

#include <omp.h>

#include "Calltree.h"
#include "CNode.h"
#include "DefsFactory.h"
#include "Enter_rep.h"
#include "Error.h"
#include "Event.h"
#include "GlobalDefs.h"
#include "LocalTrace.h"
#include "OmpEventFactory.h"
#include "OmpSmallObjAllocator.h"
#include "Region.h"
#include "pearl.h"

using namespace std;
using namespace pearl;


//--- Local defines ---------------------------------------------------------

#define BALANCED          0
#define TOO_MANY_ENTERS   1
#define TOO_MANY_EXITS    2


//--- Function prototypes ---------------------------------------------------

namespace pearl
{

extern void PEARL_new_handler();

}   /* namespace pearl */


//--- Library initialization ------------------------------------------------

/**
 * Initializes the PEARL library and installs a custom out-of-memory handler.
 * It is required to call one of the PEARL initialization functions before
 * calling any other PEARL function or instantiating any PEARL class.
 *
 * @attention Make sure to initialize PEARL before installing any exit
 *            handlers using atexit(). Otherwise, you might get error
 *            messages about memory leaks.
 *
 * @note This function is intended to be used in pure OpenMP-based PEARL
 *       programs.
 *
 * @see PEARL_init(), PEARL_mpi_init(), PEARL_hybrid_init()
 */
void pearl::PEARL_omp_init()
{
  // Register factories & allocators
  DefsFactory::registerFactory(new DefsFactory());
  EventFactory::registerFactory(new OmpEventFactory());
  SmallObjAllocator::registerAllocator(new OmpSmallObjAllocator());

  // Register new handler (out of memory)
  set_new_handler(PEARL_new_handler);
}


//--- Call tree verification ------------------------------------------------

/**
 * Verifies whether the global call tree provided by the trace definition
 * data @a defs is complete with respect to the local @a trace data. If not,
 * the process-local call tree is extended accordingly. This has to be done
 * before PEARL_omp_preprocess_trace() is called.
 *
 * @note This function is intended to be used in OpenMP-based PEARL programs,
 *       i.e., pure OpenMP or hybrid OpenMP/MPI. In the case of a hybrid
 *       program, the process-local call trees need to be combined using
 *       PEARL_mpi_unify_calltree() to get the global call tree.
 *
 * @param defs  Global definitions object
 * @param trace Local trace data object
 *
 * @see PEARL_verify_calltree(), PEARL_mpi_unify_calltree(),
 *      PEARL_omp_preprocess_trace()
 */
void pearl::PEARL_omp_verify_calltree(      GlobalDefs& defs,
                                      const LocalTrace& trace)
{
  // No explicit thread synchronization is required here. The threads will be
  // synchronized when examining the Enter event of the first parallel region
  // of the trace.

  // This variable is shared!
  // Used to provide fork cnode on master thread to worker threads
  static CNode* fork_cnode = NULL;

  // Calltree verification
  Calltree* ctree   = defs.get_calltree();
  CNode*    current = NULL;
  long      depth   = 0;
  int       status  = BALANCED;
  Event     event   = trace.begin();
  while (event != trace.end()) {
    // OpenMP fork event, can only occur on master thread
    if (event->is_typeof(OMP_FORK))
    {
      // Provide current cnode to worker threads
      fork_cnode = current;
    }

    // Enter event
    else if (event->is_typeof(ENTER))
    {
      uint32_t   mask  = Region::CLASS_OMP | Region::CAT_OMP_PARALLEL;
      Enter_rep* enter = dynamic_cast<Enter_rep*>(event.operator->());

      // Begin of parallel region, immediately after OpenMP fork on master
      // ==> get parent cnode from master
      if ((enter->get_regionEntered()->get_class() & mask) == mask) {
        // Make sure all threads have reached this point
        #pragma omp barrier

        // Verify correct nesting of Enter/Exit events on worker threads
        // for *previous* parallel region
        // Something left on the call stack ==> too many Enter events
        if (omp_get_thread_num() != 0 && depth > 0 && status == BALANCED)
          status = TOO_MANY_ENTERS;

        // Get parent cnode
        current = fork_cnode;

        // Synchronize again to avoid race conditions
        #pragma omp barrier
      }

      // Update global call tree
      #pragma omp critical
      {
        current = ctree->add_cnode(enter->get_regionEntered(),
                                   enter->get_callsite(),
                                   current, enter->get_time());
      }

      // Increase call stack depth
      depth++;
    }

    // Exit event
    else if (event->is_typeof(EXIT))
    {
      // Decrease call stack depth
      depth--;

      // Verify correct nesting of Enter/Exit events
      // Call stack empty ==> too many Exit events
      if (depth < 0 && status == BALANCED)
        status = TOO_MANY_EXITS;

      // Update current cnode
      current = current->get_parent();
    }

    // Go to nect event
    ++event;
  }

  // Synchronize threads
  #pragma omp barrier

  // Verify correct nesting of Enter/Exit events
  // Something left on the call stack ==> too many Enter events
  // Call stack empty ==> too many Exit events
  if (depth < 0 || status == TOO_MANY_EXITS)
    throw FatalError("Unbalanced ENTER/EXIT events (Too many EXITs).");
  else if (depth > 0 || status == TOO_MANY_ENTERS)
    throw FatalError("Unbalanced ENTER/EXIT events (Too many ENTERs).");
}


//--- Trace preprocessing ---------------------------------------------------

/**
 * Performs some local preprocessing of the given @a trace which is required
 * to provide the full trace-access functionality. This has to be done as the
 * last step in setting up the data structures, i.e., after calling
 * PEARL_omp_verify_calltree() and PEARL_mpi_unify_calltree().
 *
 * @note This function is intended to be used in OpenMP-based PEARL programs,
 *       i.e., pure OpenMP or hybrid OpenMP/MPI.
 *
 * @param defs  Global definitions object
 * @param trace Local trace data object
 *
 * @see PEARL_preprocess_trace(), PEARL_omp_verify_calltree(),
 *      PEARL_mpi_unify_calltree()
 */
void pearl::PEARL_omp_preprocess_trace(const GlobalDefs& defs,
                                       const LocalTrace& trace)
{
  // No explicit thread synchronization is required here. The threads will be
  // synchronized when examining the Enter event of the first parallel region
  // of the trace.

  // This variable is shared!
  // Used to provide fork cnode on master thread to worker threads
  static CNode* fork_cnode = NULL;

  // Precompute CNODE pointers
  Calltree* ctree   = defs.get_calltree();
  CNode*    current = NULL;
  Event     event   = trace.begin();
  while (event != trace.end()) {
    // OpenMP fork event, can only occur on master thread
    if (event->is_typeof(OMP_FORK))
    {
      // Provide current cnode to worker threads
      fork_cnode = current;
    }

    // Enter event
    else if (event->is_typeof(ENTER))
    {
      uint32_t   mask  = Region::CLASS_OMP | Region::CAT_OMP_PARALLEL;
      Enter_rep* enter = dynamic_cast<Enter_rep*>(event.operator->());

      // Begin of parallel region, immediately after OpenMP fork on master
      // ==> get parent cnode from master
      if ((enter->get_regionEntered()->get_class() & mask) == mask) {
        // Make sure all threads have reached this point
        #pragma omp barrier

        // Get parent cnode
        current = fork_cnode;

        // Synchronize again to avoid race conditions
        #pragma omp barrier
      }

      // Update current cnode
      // Only read access, so no synchronization necessary
      current = ctree->add_cnode(enter->get_regionEntered(),
                                 enter->get_callsite(),
                                 current, enter->get_time());

      // Set CNODE pointer
      enter->set_cnode(current);
    }

    // Exit event
    else if (event->is_typeof(EXIT))
    {
      // Update current cnode
      current = current->get_parent();
    }

    // Go to next event
    ++event;
  }

  // Synchronize threads
  #pragma omp barrier
}
