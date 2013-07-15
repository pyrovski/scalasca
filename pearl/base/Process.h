/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_PROCESS_H
#define PEARL_PROCESS_H


#include <vector>

#include "NamedObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Process.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Process.
 *
 * This header file provides the declaration of the class Process.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Node;
class Thread;


/*-------------------------------------------------------------------------*/
/**
 * @class   Process
 * @ingroup PEARL_base
 * @brief   Stores information related to a process of the target
 *          application.
 *
 * Instances of the class Process provide information about a single
 * process of the target application. Each process is bound to exactly
 * one Node of a Machine and consists of at least one Thread.
 *
 * The numerical identifiers of the individual processes are globally
 * defined and continuously enumerated, i.e., the ID is element of
 * [0,@#processes-1]. In the context of MPI applications, the identifier
 * of a process is equal to its rank number in MPI_COMM_WORLD.
 *
 * @note The Thread objects added to a process are only referenced and not
 *       owned by the corresponding Process object. Therefore, they will
 *       not be deleted if the Process object is released.
 */
/*-------------------------------------------------------------------------*/

class Process : public NamedObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    Process(ident_t id, const std::string& name);

    /// @}
    /// @name Get & set process information
    /// @{

    uint32_t num_threads() const;

    Thread* get_thread(uint32_t index) const;
    void    add_thread(Thread* thread);

    Node* get_node() const;

    /// @}


  private:
    /// Container type for the threads running within this process
    typedef std::vector<Thread*> thread_container;


    /// %Node this process belongs to
    Node* m_node;

    /// Threads running in this process
    thread_container m_threads;


    /* Private methods */
    void set_node(Node* node);


    /* Declare friends */
    friend class Node;
};


}   /* namespace pearl */


#endif   /* !PEARL_PROCESS_H */
