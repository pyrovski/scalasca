/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>
#include <iostream>

#include "Location.h"
#include "Machine.h"
#include "Node.h"
#include "Process.h"
#include "Thread.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Location
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and initializes it with the given values. In
 * addition, the cross-references between the different entities in the
 * system hierarchy are created.
 *
 * @param id      %Location identifier
 * @param machine %Machine the node belongs to
 * @param node    %Node the process is running on
 * @param process %Process the thread belongs to
 * @param thread  %Thread corresponding to the location
 */
Location::Location(ident_t  id,
                   Machine* machine,
                   Node*    node,
                   Process* process,
                   Thread*  thread)
  : IdObject(id),
    m_machine(machine),
    m_node(node),
    m_process(process),
    m_thread(thread)
{
  assert(machine && node && process && thread);

  // Create cross-references (machine <-> node <-> process <-> thread)
  machine->add_node(node);
  node->add_process(process);
  process->add_thread(thread);

  // Set location pointer for the given thread
  thread->set_location(this);
}


//--- Get location information ----------------------------------------------

/**
 * Returns the machine this location belongs to.
 *
 * @return Associated machine
 */
Machine* Location::get_machine() const
{
  return m_machine;
}


/**
 * Returns the node this location belongs to.
 *
 * @returns Associated node
 */
Node* Location::get_node() const
{
  return m_node;
}


/**
 * Returns the process this location belongs to.
 *
 * @return Associated process
 */
Process* Location::get_process() const
{
  return m_process;
}


/**
 * Returns the thread this location corresponds to.
 *
 * @return Corresponding thread
 */
Thread* Location::get_thread() const
{
  return m_thread;
}


/*
 *---------------------------------------------------------------------------
 *
 * Related functions
 *
 *---------------------------------------------------------------------------
 */

/**
 * Writes the location information stored for the given @a item consisting
 * of the machine, node, process, and thread (including IDs and names) to
 * the output stream @a stream.
 *
 * @param stream Output stream
 * @param item   Location instance
 *
 * @return Output stream
 *
 * @relatesalso Location
 */
ostream& pearl::operator<<(ostream& stream, const Location& item)
{
  return stream << item.get_id() << " {" << *item.get_machine() << ","
                                         << *item.get_node()    << ","
                                         << *item.get_process() << ","
                                         << *item.get_thread()  << "}";
}
