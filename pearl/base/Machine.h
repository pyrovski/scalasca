/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MACHINE_H
#define PEARL_MACHINE_H


#include <vector>

#include "NamedObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Machine.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Machine.
 *
 * This header file provides the declaration of the class Machine.
 */
/*-------------------------------------------------------------------------*/


namespace pearl 
{

//--- Forward declarations --------------------------------------------------

class Node;


/*-------------------------------------------------------------------------*/
/**
 * @class   Machine
 * @ingroup PEARL_base
 * @brief   Stores information related to a machine used during trace file
 *          generation.
 *
 * The class Machine constitutes the top level of the system hierarchy. Its
 * instances provide information about a single machine used during trace
 * file generation. Each machine consists of at least one Node.
 *
 * The numerical identifiers of the individual machines are globally defined
 * and continuously enumerated, i.e., the ID is element of [0,@#machines-1].
 *
 * @note The Node objects added to a machine are only referenced and not
 *       owned by the corresponding Machine object. Therefore, they will
 *       not be deleted if the Machine object is released.
 */
/*-------------------------------------------------------------------------*/

class Machine : public NamedObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    Machine(ident_t id, const std::string& name, uint32_t num_nodes);

    /// @}
    /// @name Get & set machine information
    /// @{

    uint32_t num_nodes() const;

    Node* get_node(uint32_t index) const;
    void  add_node(Node* node);

    /// @}


  private:
    /// Container type for the nodes associated with this machine.
    typedef std::vector<Node*> node_container;


    /// Nodes associated with this machine
    node_container m_nodes;
};


}   /* namespace pearl */


#endif   /* !PEARL_MACHINE_H */
