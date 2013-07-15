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
#include "Process.h"
#include "Thread.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Process
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets its identifier and name to the given
 * values @a id and @a name, respectively.
 *
 * Right after the initialization, the process is not associated with any
 * node. Likewise, there are no threads bound to the process.
 *
 * @param id   %Process identifier
 * @param name %Process name
 */
Process::Process(ident_t id, const string& name)
  : NamedObject(id, name),
    m_node(NULL)
{
}


//--- Get & set process information -----------------------------------------

/**
 * Returns the number of threads running within this process.
 *
 * @return Number of threads
 */
uint32_t Process::num_threads() const
{
  return m_threads.size();
}


/**
 * Returns a pointer to the associated thread with the given @a index.
 * These indices are local to this process (i.e., @a index must be an
 * element of [0,num_threads()-1]) and correspond to the thread IDs,
 * provided that all threads have already been added to the process.
 *
 * @param index Index (ID) of the requested thread
 *
 * @return Corresponding Thread object
 *
 * @exception RuntimeError if an invalid @a index is given ("Index out
 *                         of bounds")
 */
Thread* Process::get_thread(uint32_t index) const
{
  if (index >= m_threads.size())
    throw RuntimeError("Process::get_thread(uint32_t) -- Index out of bounds.");

  return m_threads[index];
}


/**
 * Adds the given @a thread to the list of threads running within this
 * process. If @a thread is already associated to the process, the call
 * is silently ignored. In addition, the process pointer of the thread
 * is updated.
 *
 * @param thread New thread object
 */
void Process::add_thread(Thread* thread)
{
  assert(thread);
  assert(!thread->get_process() || thread->get_process() == this);

  // Check whether thread is already in list
  thread_container::iterator it = lower_bound(m_threads.begin(),
                                              m_threads.end(),
                                              thread,
                                              less_ptr<Thread*>());
  if (it != m_threads.end() && !less_ptr<Thread*>()(thread, *it))
    return;

  // Update pointers and add thread to list
  thread->set_process(this);
  m_threads.insert(it, thread);
}


/**
 * Returns the node this process is running on. If the process has not been
 * added to a node yet, a NULL pointer is returned.
 *
 * @return Associated node
 */
Node* Process::get_node() const
{
  return m_node;
}


//--- Private methods -------------------------------------------------------

/**
 * Sets the node this process is running on. This method is automatically
 * called from within Node::add_process().
 *
 * @param node %Node object
 */
void Process::set_node(Node* node)
{
  assert(node && !m_node);

  m_node = node;
}
