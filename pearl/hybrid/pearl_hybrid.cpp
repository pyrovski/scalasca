/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <new>

#include "HybridEventFactory.h"
#include "MpiDefsFactory.h"
#include "OmpSmallObjAllocator.h"
#include "pearl.h"

using namespace std;
using namespace pearl;


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
 * @note This function is intended to be used in hybrid OpenMP/MPI PEARL
 *       programs.
 *
 * @see PEARL_init(), PEARL_omp_init(), PEARL_mpi_init()
 */
void pearl::PEARL_hybrid_init()
{
  // Register factories & allocators
  DefsFactory::registerFactory(new MpiDefsFactory());
  EventFactory::registerFactory(new HybridEventFactory());
  SmallObjAllocator::registerAllocator(new OmpSmallObjAllocator());

  // Register new handler (out of memory)
  set_new_handler(PEARL_new_handler);
}
