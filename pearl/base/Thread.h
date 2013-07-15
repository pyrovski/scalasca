/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_THREAD_H
#define PEARL_THREAD_H


#include "NamedObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Thread.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Thread.
 *
 * This header file provides the declaration of the class Thread.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Location;
class Process;


/*-------------------------------------------------------------------------*/
/**
 * @class   Thread
 * @ingroup PEARL_base
 * @brief   Stores information related to a thread of the target application.
 *
 * Instances of the class Thread provide information about to a single
 * thread of execution in the target application. Each thread is bound to
 * exactly one Process and corresponds to a unique Location.
 *
 * The numerical identifiers of the individual threads are continuously
 * enumerated, however, unlike the IDs of machines, nodes, and processes,
 * thread IDs are not global but local to the process they belong to.
 * For OpenMP applications, the thread identifier is equal to the thread
 * number returned by omp_get_thread_num().
 */
/*-------------------------------------------------------------------------*/

class Thread : public NamedObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    Thread(ident_t id, const std::string& name);

    /// @}
    /// @name Get thread information
    /// @{

    Process*  get_process() const;
    Location* get_location() const;

    /// @}


  private:
    /// %Process this thread belongs to
    Process* m_process;

    /// Associated location
    Location* m_location;


    /* Private methods */
    void set_process(Process* process);
    void set_location(Location* location);


    /* Declare friends */
    friend class Process;
    friend class Location;
};


}   /* namespace pearl */


#endif   /* !PEARL_THREAD_H */
