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

#include "Error.h"
#include "Functors.h"
#include "Machine.h"
#include "Node.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Machine
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets its identifier and name to the given
 * values @a id and @a name, respectively. The third parameter @a num_nodes
 * specifies the expected number of nodes of the machine and is used as a
 * hint for memory allocation purposes.
 *
 * Right after the initialization, there are no nodes associated with
 * the machine. That is, the programmer is responsible for adding at
 * least one node to the machine using the add_node() method.
 *
 * @param id        %Machine identifier
 * @param name      %Machine name
 * @param num_nodes Expected number of nodes
 */
Machine::Machine(ident_t id, const string& name, uint32_t num_nodes)
  : NamedObject(id, name)
{
  m_nodes.reserve(num_nodes);
}


//--- Get & set machine information -----------------------------------------

/**
 * Returns the number of nodes associated with the machine.
 *
 * @return Number of nodes
 */
uint32_t Machine::num_nodes() const
{
  return m_nodes.size();
}


/**
 * Returns a pointer to an Node object representing the associated physical
 * node with the given @a index. These indices are local to this machine
 * (i.e., @a index must be an element of [0,num_nodes()-1]) and independent
 * of the global node identifiers. However, the nodes are stored in ascending
 * order with respect to their IDs.
 *
 * @param index Index of the requested node
 *
 * @return Corresponding Node object
 *
 * @exception RuntimeError if the given @a index is invalid ("Index out of
 *                         bounds")
 */
Node* Machine::get_node(uint32_t index) const
{
  if (index >= m_nodes.size())
    throw RuntimeError("Machine::get_node(uint32_t) -- Index out of bounds.");

  return m_nodes[index];
}


/**
 * Adds the given @a node to the list of nodes representing the machine.
 * If @a node is already associated to the machine, the call is silently
 * ignored. In addition, the machine pointer of @a node is updated.
 *
 * @param node New node object
 */
void Machine::add_node(Node* node)
{
  assert(node);
  assert(!node->get_machine() || node->get_machine() == this);

  // Check whether node is already in list
  node_container::iterator it = lower_bound(m_nodes.begin(),
                                            m_nodes.end(),
                                            node,
                                            less_ptr<Node*>());
  if (it != m_nodes.end() && !less_ptr<Node*>()(node, *it))
    return;

  // Update pointers and add node to list
  node->set_machine(this);
  m_nodes.insert(it, node);
}
