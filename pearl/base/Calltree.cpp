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

#include "Buffer.h"
#include "Callsite.h"
#include "Calltree.h"
#include "CNode.h"
#include "Error.h"
#include "GlobalDefs.h"
#include "Region.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Calltree
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

Calltree::Calltree()
  : m_modified(false)
{
}


Calltree::Calltree(const GlobalDefs& defs, Buffer& buffer)
  : m_modified(false)
{
  uint32_t count = buffer.get_uint32();
  for (uint32_t id = 0; id < count; ++id) {
    ident_t     region_id = buffer.get_id();
    ident_t     csite_id  = buffer.get_id();
    ident_t     parent_id = buffer.get_id();
    timestamp_t time      = buffer.get_time();

    add_cnode(new CNode(id,
                        defs.get_region(region_id),
                        defs.get_callsite(csite_id),
                        get_cnode(parent_id),
                        time));
  }
}


Calltree::~Calltree()
{
  // Delete CNode objects
  cnode_container::const_iterator it = m_cnodes.begin();
  while (it != m_cnodes.end()) {
    delete *it;
    ++it;
  }
}


//--- Get & set call tree information ---------------------------------------

uint32_t Calltree::num_cnodes() const
{
  return m_cnodes.size();
}


CNode* Calltree::get_cnode(ident_t id) const
{
  if (id >= m_cnodes.size())
    return NULL;

  return m_cnodes[id];
}


CNode* Calltree::get_cnode(Region* region, Callsite* csite, CNode* parent) const
{
  CNode* result = NULL;

  // Determine if suitable CNode is already available
  if (parent) {
    result = parent->get_child(region, csite);
  } else {
    cnode_container::const_iterator it = m_roots.begin();
    while (it != m_roots.end()) {
      if ((*it)->get_region()   == region &&
          (*it)->get_callsite() == csite) {
        result = *it;
        break;
      }

      ++it;
    }
  }

  return result;
}


void Calltree::add_cnode(CNode* cnode)
{
  assert(cnode);

  if (cnode->get_id() != m_cnodes.size())
    throw RuntimeError("Calltree::add_cnode(CNode*) -- Invalid ID.");

  m_cnodes.push_back(cnode);
  if (!cnode->get_parent())
    m_roots.push_back(cnode);

  set_modified();
}


CNode* Calltree::add_cnode(Region* region, Callsite* csite, CNode* parent,
                           timestamp_t time)
{
  CNode* result = get_cnode(region, csite, parent);

  // Update timestamp if found, create new CNode otherwise
  if (result) {
    result->update_time(time);
  } else {
    result = new CNode(m_cnodes.size(), region, csite, parent, time);
    add_cnode(result);
  }

  return result;
}


//--- Merging of call trees -------------------------------------------------

void Calltree::merge(const Calltree& ctree)
{
  uint32_t count = ctree.num_cnodes();

  vector<ident_t> idmap;
  idmap.reserve(count);

  for (uint32_t i = 0; i < count; ++i) {
    CNode* current = ctree.get_cnode(i);
    CNode* parent  = current->get_parent();
    CNode* node    = add_cnode(current->get_region(),
                               current->get_callsite(),
                               parent ? m_cnodes[idmap[parent->get_id()]]
                                      : NULL,
                               current->get_time());

    idmap.push_back(node->get_id());
  }
}


//--- Serialize call tree data ----------------------------------------------

void Calltree::pack(Buffer& buffer) const
{
  uint32_t count = m_cnodes.size();

  buffer.put_uint32(count);
  for (uint32_t i = 0; i < count; ++i) {
    CNode*    node   = m_cnodes[i];
    Region*   region = node->get_region();
    Callsite* csite  = node->get_callsite();
    CNode*    parent = node->get_parent();

    buffer.put_id(region ? region->get_id() : PEARL_NO_ID);
    buffer.put_id(csite  ? csite->get_id()  : PEARL_NO_ID);
    buffer.put_id(parent ? parent->get_id() : PEARL_NO_ID);
    buffer.put_double(node->get_time());
  }
}
