/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_DEFSFACTORY_H
#define PEARL_DEFSFACTORY_H


#include <cassert>
#include <string>
#include <vector>

#include "Cartesian.h"
#include "Group.h"
#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    DefsFactory.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class DefsFactory.
 *
 * This header file provides the declaration of the class DefsFactory.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class GlobalDefs;
class Process;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   DefsFactory
 * @ingroup PEARL_base
 * @brief   Abstract factory class for global definition objects.
 */
/*-------------------------------------------------------------------------*/

class DefsFactory
{
  public:
    /// Container type for process groups
    typedef Group::container_type process_group;

    /// Container type for cartesian topology dimensions
    typedef Cartesian::cart_dims cart_dims;

    /// Container type for cartesian topology periodicity
    typedef Cartesian::cart_period cart_period;


    /// @name Constructors & destructor
    /// @{

    virtual ~DefsFactory();

    /// @}
    /// @name Generic factory methods
    /// @{

    void createMachine  (GlobalDefs&        defs,
                         ident_t            id,
                         const std::string& name,
                         uint32_t           num_nodes) const;
    void createNode     (GlobalDefs&        defs,
                         ident_t            id,
                         const std::string& name,
                         ident_t            machine_id,
                         uint32_t           num_cpus,
                         double             clockrate) const;
    void createProcess  (GlobalDefs&        defs,
                         ident_t            id,
                         const std::string& name) const;
    void createThread   (GlobalDefs&        defs,
                         ident_t            id,
                         const std::string& name,
                         ident_t            process_id) const;
    void createLocation (GlobalDefs&        defs,
                         ident_t            id,
                         ident_t            machine_id,
                         ident_t            node_id,
                         ident_t            process_id,
                         ident_t            thread_id) const;
    void createRegion   (GlobalDefs&        defs,
                         ident_t            id,
                         const std::string& name,
                         const std::string& file,
                         line_t             begin,
                         line_t             end,
                         const std::string& descr,
                         uint8_t            type) const;
    void createCallsite (GlobalDefs&        defs,
                         ident_t            id,
                         const std::string& file,
                         line_t             lineno,
                         ident_t            region_id) const;
    void createCallpath (GlobalDefs&        defs,
                         ident_t            id,
                         ident_t            region_id,
                         ident_t            csite_id,
                         ident_t            parent_id,
                         double             time) const;
    void createMetric   (GlobalDefs&        defs,
                         ident_t            id,
                         const std::string& name,
                         const std::string& descr,
                         uint8_t            type,
                         uint8_t            mode,
                         uint8_t            interval) const;
    void createCartesian(GlobalDefs&        defs,
                         ident_t            id,
                         const cart_dims&   dimensions,
                         const cart_period& periodic,
                         std::string        topo_name,
                         std::vector<std::string>) const;

    /// @}
    /// @name MPI-specific factory methods
    /// @{

    virtual void createMpiGroup    (GlobalDefs&        defs,
                                    ident_t            id,
                                    process_group&     process_ids,
                                    bool               is_self,
                                    bool               is_world) const;
    virtual void createMpiComm     (GlobalDefs&        defs,
                                    ident_t            id,
                                    ident_t            group_id) const;
    virtual void createMpiWindow   (GlobalDefs&        defs,
                                    ident_t            id,
                                    ident_t            comm_id) const;
    virtual void createMpiCartesian(GlobalDefs&        defs,
                                    ident_t            id,
                                    const cart_dims&   dimensions,
                                    const cart_period& periodic,
                                    ident_t            comm_id,
                                    std::string        topo_name,
                                    std::vector<std::string>) const;

    /// @}
    /// @name Singleton interface
    /// @{

    static const DefsFactory* instance();
    static void registerFactory(const DefsFactory* factory);

    /// @}


  private:
    /// Single factory instance
    static const DefsFactory* m_instance;
};


//--- Inline methods --------------------------------------------------------

inline const DefsFactory* DefsFactory::instance()
{
  assert(m_instance);
  return m_instance;
}


}   /* namespace pearl */


#endif   /* !PEARL_DEFSFACTORY_H */
