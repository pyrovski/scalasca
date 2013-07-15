/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_NODE_H
#define PEARL_NODE_H


#include <vector>

#include "NamedObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Node.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Node.
 *
 * This header file provides the declaration of the class Node.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Machine;
class Process;


/*-------------------------------------------------------------------------*/
/**
 * @class   Node
 * @ingroup PEARL_base
 * @brief   Stores information related to a node that is part of a machine
 *          used for trace generation.
 *
 * Instances of the Node class provide information about a single node.
 * A node is a physical part of a Machine with a single address space and
 * one or more CPUs. It can host a subset of the application's processes
 * (see class Process).
 *
 * The numerical identifiers of the individual nodes are globally defined
 * and continuously enumerated, i.e., the ID is element of [0,@#nodes-1].
 *
 * @note The Process objects added to a node are only referenced and not
 *       owned by the corresponding Node object. Therefore, they will not
 *       be deleted if the Node object is released.
 */
/*-------------------------------------------------------------------------*/

class Node : public NamedObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    Node(ident_t            id,
         const std::string& name,
         uint32_t           num_cpus,
         double             clockrate);

    /// @}
    /// @name Get & set node information
    /// @{

    uint32_t num_processes() const;
    uint32_t num_cpus() const;

    Process* get_process(uint32_t index) const;
    void     add_process(Process* process);

    Machine* get_machine() const;
    double   get_clockrate() const;

    /// @}


  private:
    /// Container type for the processes running on this node
    typedef std::vector<Process*> process_container;


    /// %Machine this node belongs to
    Machine* m_machine;

    /// Number of CPUs of the node
    uint32_t m_num_cpus;

    /// Clock frequency of each CPU
    double m_clockrate;

    /// Processes running on this node
    process_container m_processes;


    /* Private methods */
    void set_machine(Machine* machine);


    /* Declare friends */
    friend class Machine;
};


}   /* namespace pearl */


#endif   /* !PEARL_NODE_H */
