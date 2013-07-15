/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_CALLTREE_H
#define PEARL_CALLTREE_H


#include <vector>

#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    Calltree.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Calltree.
 *
 * This header file provides the declaration of the class Calltree.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Buffer;
class Callsite;
class CNode;
class GlobalDefs;
class Region;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   Calltree
 * @ingroup PEARL_base
 * @brief   Container class for storing the data of local or global call
 *          trees.
 */
/*-------------------------------------------------------------------------*/

class Calltree
{
  public:
    /// @name Constructors & destructor
    /// @{

    Calltree();
    Calltree(const GlobalDefs& defs, Buffer& buffer);
    ~Calltree();

    /// @}
    /// @name Get & set call tree information
    /// @{

    uint32_t num_cnodes() const;

    CNode* get_cnode(ident_t id) const;
    CNode* get_cnode(Region* region, Callsite* csite, CNode* parent) const;

    void   add_cnode(CNode* cnode);
    CNode* add_cnode(Region* region, Callsite* csite, CNode* parent, 
                     timestamp_t time);

    /// @}
    /// @name Merging of call trees
    /// @{

    void merge(const Calltree& ctree);

    /// @}
    /// @name Tracking modifications
    /// @{

    bool get_modified() const { return m_modified; }
    void set_modified(bool flag=true) { m_modified = flag; }

    /// @}
    /// @name Serialize call tree data
    /// @{

    void pack(Buffer& buffer) const;

    /// @}


  private:
    /// Container type for call tree nodes
    typedef std::vector<CNode*> cnode_container;


    /// Call tree node definitions: call tree node id |-@> call tree node
    cnode_container m_cnodes;

    /// Roots nodes of call trees
    cnode_container m_roots;

    /// Denotes whether the call tree has been modified after the last reset
    /// of this flag
    bool m_modified;
};


}   /* namespace pearl */


#endif   /* PEARL_CALLTREE_H */
