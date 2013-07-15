/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <algorithm>
#include <cassert>
#include <cstddef>

#include "Error.h"
#include "Functors.h"
#include "Node.h"
#include "Process.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Node
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes the data with the given values.
 * The number of CPUs is used as a hint for memory allocation purposes,
 * since it is considered to be the expected number of processes running
 * on this node.
 *
 * Right after the initialization, the node is not associated with any
 * machine. Likewise, there are no processes bound to the node.
 *
 * @param id        %Node identifier
 * @param name      %Node name
 * @param num_cpus  Number of CPUs
 * @param clockrate Clock frequency of each CPU
 */
Node::Node(ident_t       id,
           const string& name,
           uint32_t      num_cpus,
           double        clockrate)
  : NamedObject(id, name),
    m_machine(NULL),
    m_num_cpus(num_cpus),
    m_clockrate(clockrate)
{
  m_processes.reserve(num_cpus);
}


//--- Get & set node information --------------------------------------------

/**
 * Returns the number of processes running on this node.
 *
 * @return Number of processes
 */
uint32_t Node::num_processes() const
{
  return m_processes.size();
}


/**
 * Returns the number of CPUs of this node.
 *
 * @return Number of CPUs
 */
uint32_t Node::num_cpus() const
{
  return m_num_cpus;
}


/**
 * Returns a pointer to the associated process with the given @a index.
 * These indices are local to this node (i.e., @a index must be an element
 * of [0,num_processes()-1]) and are independent of the global process
 * identifiers. However, the processes are stored in ascending order with
 * respect to their IDs.
 *
 * @param index Index of the requested process
 *
 * @return Corresponding Process object
 *
 * @exception RuntimeError if an invalid @a index is given ("Index out of
 *                         bounds")
 */
Process* Node::get_process(uint32_t index) const
{
  if (index >= m_processes.size())
    throw RuntimeError("Node::get_process(uint32_t) -- Index out of bounds.");

  return m_processes[index];
}


/**
 * Adds the given @a process to the list of processes running to this node.
 * If @a process is already associated to the node, the call is silently
 * ignored. In addition, the node pointer of the process is updated.
 *
 * @param process New process object
 */
void Node::add_process(Process* process)
{
  assert(process);
  assert(!process->get_node() || process->get_node() == this);

  // Check whether process is already in list
  process_container::iterator it = lower_bound(m_processes.begin(),
                                               m_processes.end(),
                                               process,
                                               less_ptr<Process*>());
  if (it != m_processes.end() && !less_ptr<Process*>()(process, *it))
    return;

  // Update pointers and add process to list
  process->set_node(this);
  m_processes.insert(it, process);
}


/**
 * Returns the machine this node belongs to. If the node has not been added
 * to a machine yet, a NULL pointer is returned.
 *
 * @return Associated machine
 */
Machine* Node::get_machine() const
{
  return m_machine;
}


/**
 * Returns the clock frequency of each CPU of this node.
 *
 * @return Clock frequency
 */
double Node::get_clockrate() const
{
  return m_clockrate;
}


//--- Private methods -------------------------------------------------------

/**
 * Sets the machine this node is associated with. This method is
 * automatically called from within Machine::add_node().
 *
 * @param machine %Machine object
 */
void Node::set_machine(Machine* machine)
{
  assert(machine && !m_machine);

  m_machine = machine;
}
