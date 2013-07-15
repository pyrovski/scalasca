/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_CNODE_H
#define PEARL_CNODE_H


#include <vector>

#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    CNode.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class CNode.
 *
 * This header file provides the declaration of the class CNode.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Callsite;
class Region;


/*-------------------------------------------------------------------------*/
/**
 * @class   CNode
 * @ingroup PEARL_base
 * @brief   Stores information related to a call tree node.
 */
/*-------------------------------------------------------------------------*/

class CNode : public IdObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    CNode(ident_t     id,
          Region*     region,
          Callsite*   csite,
          CNode*      parent,
          timestamp_t time);

    /// @}
    /// @name Retrieve node information
    /// @{

    uint32_t num_children() const;

    Region*     get_region() const;
    Callsite*   get_callsite() const;
    CNode*      get_parent() const;
    timestamp_t get_time() const;
    CNode*      get_child(uint32_t index) const;

    /// @}


  private:
    /// Container type for child nodes
    typedef std::vector<CNode*> child_container;


    /// Associated source-code region
    Region* m_region;

    /// Associated call site
    Callsite* m_callsite;

    /// Parent node in call tree (NULL for root nodes)
    CNode* m_parent;

    /// Timestamp of first occurrence
    timestamp_t m_timestamp;

    /// Child nodes
    child_container m_children;


    /* Private methods */
    void   add_child(CNode* cnode);
    CNode* get_child(Region* region, Callsite* csite) const;
    void   update_time(timestamp_t time);


    /* Declare friends */
    friend class Calltree;
};


}   /* namespace pearl */


#endif   /* !PEARL_CNODE_H */
