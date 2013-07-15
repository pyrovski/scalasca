/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>
#include <cstddef>

#include "Thread.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Thread
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets its identifier and name to the given
 * values @a id and @a name, respectively.
 *
 * Right after the initialization, the thread is not associated with any
 * process or location.
 *
 * @param id      %Thread identifier
 * @param name    %Thread name
 */
Thread::Thread(ident_t id, const string& name)
  : NamedObject(id, name),
    m_process(NULL),
    m_location(NULL)
{
}


//--- Get & set thread information ------------------------------------------

/**
 * Returns the process this thread belongs to. If the thread has not been
 * assigned to a process yet, a NULL pointer is returned.
 *
 * @return Associated process
 */
Process* Thread::get_process() const
{
  return m_process;
}


/**
 * Returns the location associated to this thread. If the thread has not
 * been bound to a location object yet, a NULL pointer is returned.
 *
 * @return Associated location
 */
Location* Thread::get_location() const
{
  return m_location;
}


//--- Private methods -------------------------------------------------------

/**
 * Sets the process this thread belongs to. This method is automatically
 * called from within Process::add_thread().
 *
 * @param process %Process object
 */
void Thread::set_process(Process* process)
{
  assert(process && !m_process);

  m_process = process;
}


/**
 * Sets the location associated to this thread. This method is
 * automatically called from within the constructor of the Location class.
 *
 * @param location %Location object
 */
void Thread::set_location(Location* location)
{
  assert(location && !m_location);

  m_location = location;
}
