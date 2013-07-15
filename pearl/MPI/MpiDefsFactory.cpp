/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <sstream>

#include <mpi.h>

#include "Error.h"
#include "GlobalDefs.h"
#include "MpiCartesian.h"
#include "MpiComm.h"
#include "MpiDefsFactory.h"
#include "MpiGroup.h"
#include "MpiWindow.h"
#include "Process.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiDefsFactory
 *
 *---------------------------------------------------------------------------
 */

//--- MPI-specific factory methods ------------------------------------------

void MpiDefsFactory::createMpiGroup(GlobalDefs&    defs,
                                    ident_t        id,
                                    process_group& process_ids,
                                    bool           is_self,
                                    bool           is_world) const
{
  // Sanity check
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  if (static_cast<uint32_t>(world_size) < process_ids.size()) {
    ostringstream num;
    num << defs.num_processes();

    throw FatalError("MpiDefsFactory::createMpiGroup() -- "
                     "Experiment requires at least " + num.str() + " MPI ranks.");
  }

  /* handling of 'self' group */
  if (is_self) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Create corresponding process object
    try {
      defs.get_process(rank);
    }
    catch (RuntimeError&) {
      DefsFactory::instance()->createProcess(defs, rank, "");
    }

    process_ids.push_back(rank);
  }

  // Create & store new MPI group object
  defs.add_group(new MpiGroup(id, process_ids, is_self, is_world));
}


void MpiDefsFactory::createMpiComm(GlobalDefs& defs,
                                   ident_t     id,
                                   ident_t     group_id) const
{
  // Determine MPI process group
  MpiGroup* group = dynamic_cast<MpiGroup*>(defs.get_group(group_id));
  if (!group)
    throw RuntimeError("MpiDefsFactory::createMpiComm() -- "
                       "Invalid MPI group ID.");

  // Create & store new MPI communicator object
  defs.add_comm(new MpiComm(id, group));
}


void MpiDefsFactory::createMpiWindow(GlobalDefs& defs,
                                     ident_t     id,
                                     ident_t     comm_id) const
{
  // Determine communicator
  MpiComm* comm = dynamic_cast<MpiComm*>(defs.get_comm(comm_id));
  if (!comm)
    throw RuntimeError("MpiDefsFactory::createMpiWindow() -- "
                       "Invalid MPI communicator ID.");

  // Create & store new MPI RMA window
  defs.add_window(new MpiWindow(id, comm));
}


void MpiDefsFactory::createMpiCartesian(GlobalDefs&        defs,
                                        ident_t            id,
                                        const cart_dims&   dimensions,
                                        const cart_period& periodic,
                                        ident_t            comm_id,
                                        const std::string  cart_name,
                                        const std::vector<std::string> dim_names) const
{
  // Determine communicator
  MpiComm* comm = dynamic_cast<MpiComm*>(defs.get_comm(comm_id));
  if (!comm)
    throw RuntimeError("MpiDefsFactory::createMpiCartesian() -- "
                       "Invalid MPI communicator ID.");

  // Create & store new MPI cartesian topology object
  defs.add_cartesian(new MpiCartesian(id, dimensions, periodic, comm, cart_name, dim_names));
}
