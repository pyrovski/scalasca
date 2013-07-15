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

#include "CNode.h"
#include "Error.h"

using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class CNode
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

CNode::CNode(ident_t     id,
             Region*     region,
             Callsite*   csite,
             CNode*      parent,
             timestamp_t time)
  : IdObject(id),
    m_region(region),
    m_callsite(csite),
    m_parent(parent),
    m_timestamp(time)
{
  if (parent)
    parent->add_child(this);
}


//--- Retrieve node information ---------------------------------------------

uint32_t CNode::num_children() const
{
  return m_children.size();
}


Region* CNode::get_region() const
{
  return m_region;
}


Callsite* CNode::get_callsite() const
{
  return m_callsite;
}


CNode* CNode::get_parent() const
{
  return m_parent;
}


timestamp_t CNode::get_time() const
{
  return m_timestamp;
}


CNode* CNode::get_child(uint32_t index) const
{
  if (index >= m_children.size())
    throw RuntimeError("CNode::get_child(uint32_t) -- Index out of bounds.");

  return m_children[index];
}


//--- Private methods -------------------------------------------------------

void CNode::add_child(CNode* cnode)
{
  assert(cnode);

  m_children.push_back(cnode);
}


CNode* CNode::get_child(Region* region, Callsite* csite) const
{
  child_container::const_iterator it = m_children.begin();
  while (it != m_children.end()) {
    if ((*it)->get_region()   == region &&
        (*it)->get_callsite() == csite)
      return *it;

    ++it;
  }

  return NULL;
}


void CNode::update_time(timestamp_t time)
{
  if (time < m_timestamp)
    m_timestamp = time;
}
