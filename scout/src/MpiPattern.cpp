/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstring>

#include <mpi.h>

#include <CNode.h>
#include <GlobalDefs.h>
#include <Location.h>
#include <MpiMessage.h>
#include <Process.h>

#include "MpiPattern.h"
#include "Predicates.h"

using namespace std;
using namespace pearl;
using namespace scout;


/*
 *---------------------------------------------------------------------------
 *
 * class MpiPattern
 *
 *---------------------------------------------------------------------------
 */

//--- Data collation control ------------------------------------------------

/**
 * @brief Controls collation of data for the given call-tree node.
 *
 * This method controls the collation of severity data for the given call-tree
 * node @a cnode. The method returns @em true for all non-MPI call paths
 * since it is guaranteed that no MPI-related severity data exists for them.
 * For all other call paths, it returns @em false.
 *
 * @param  cnode  Current call-tree node
 * @return Returns @em true if @p cnode is an MPI call path, @em false
 *         otherwise
 */
bool MpiPattern::skip_cnode(const pearl::CNode& cnode) const
{
  return !is_mpi_api(cnode.get_region());
}


//--- Exchanging remote severities ------------------------------------------

/**
 * @brief Sends collected remote severities to their source location.
 *
 * A number of MPI-specific performance properties can only be calculated
 * on the location causing the wait state. However, the severity values have
 * to be attributed to the corresponding peer location. It is therefore
 * necessary to transfer such "remote severities" back to the location where
 * they materialize. This transfer is performed by this method.
 *
 * @param  defs        Global definitions object
 * @param  remote_sev  Remote severity map to transfer
 */
void MpiPattern::exchange_severities(const pearl::GlobalDefs&   defs,
                                     const rem_sev_container_t& remote_sev)
{
  #pragma omp master
  {
    MPI_Comm comm;
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);

    // Retrieve number of processes and rank
    int rank;
    int num_procs;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_procs);

    // Create message counts array
    int* lcounts = new int[num_procs];
    memset(lcounts, 0, num_procs * sizeof(int));

    // Store message counts
    rem_sev_container_t::const_iterator lit = remote_sev.begin();
    while (lit != remote_sev.end()) {
      lcounts[lit->first->get_process()->get_id()]++;
      ++lit;
    }

    // Distribute message count
    int* recvcnts = new int[num_procs];
    for (int i = 0; i < num_procs; ++i)
      recvcnts[i] = 1;
    int num_msgs;
    MPI_Reduce_scatter(lcounts, &num_msgs, recvcnts, MPI_INT, MPI_SUM, comm);

    // Release resources
    delete[] lcounts;
    delete[] recvcnts;

    // Create messages
    std::vector<MpiMessage*> messages;
    messages.reserve(remote_sev.size());
    lit = remote_sev.begin();
    while (lit != remote_sev.end()) {
      MpiMessage* msg = new MpiMessage(comm,
                                       1 + sizeof(uint32_t) +
                                       lit->second.size() *
                                       (sizeof(ident_t) + sizeof(double)));
      msg->put_uint32(lit->second.size());
      std::map<ident_t, double>::const_iterator sit = lit->second.begin();
      while (sit != lit->second.end()) {
        msg->put_id(sit->first);
        msg->put_double(sit->second);

        ++sit;
      }
      messages.push_back(msg);
      msg->isend(lit->first->get_process()->get_id(), 42);

      ++lit;
    }

    // Receive messages
    for (int i = 0; i < num_msgs; ++i) {
      MpiMessage msg(comm);
      msg.recv(MPI_ANY_SOURCE, 42);
      uint32_t count = msg.get_uint32();
      for (uint32_t j = 0; j < count; ++j) {
        uint32_t cnode = msg.get_id();
        double   value = msg.get_double();
        m_severity[defs.get_cnode(cnode)] += value;
      }
    }

    // Wait for sending to complete and free messages
    std::vector<MpiMessage*>::iterator mit = messages.begin();
    while (mit != messages.end()) {
      (*mit)->wait();
      delete *mit;
      ++mit;
    }  

    MPI_Comm_free(&comm);
  }
}
