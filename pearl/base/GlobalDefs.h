/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_GLOBALDEFS_H
#define PEARL_GLOBALDEFS_H


#include <map>
#include <string>
#include <vector>

#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    GlobalDefs.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class GlobalDefs.
 *
 * This header file provides the declaration of the class GlobalDefs.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CNode;
class Callsite;
class Calltree;
class Cartesian;
class Comm;
class Group;
class LocalTrace;
class Location;
class Machine;
class Metric;
class Node;
class Process;
class Region;
class RmaWindow;
class Thread;


/*-------------------------------------------------------------------------*/
/**
 * @class   GlobalDefs
 * @ingroup PEARL_base
 * @brief   Stores the global definitions of a tracing experiment.
 */
/*-------------------------------------------------------------------------*/

class GlobalDefs
{
  public:
    /// @name Constructors & destructor
    /// @{

    GlobalDefs(const std::string& dirname);
    GlobalDefs(const uint8_t* buffer, std::size_t size);
    ~GlobalDefs();

    /// @}
    /// @name Get number of stored entities
    /// @{

    uint32_t num_machines() const   { return m_machines.size();   }
    uint32_t num_nodes() const      { return m_nodes.size();      }
    uint32_t num_processes() const  { return m_processes.size();  }
    uint32_t num_threads() const    { return m_threads.size();    }
    uint32_t num_locations() const  { return m_locations.size();  }
    uint32_t num_regions() const    { return m_regions.size();    }
    uint32_t num_callsites() const  { return m_callsites.size();  }
    uint32_t num_cnodes() const;
    uint32_t num_metrics() const    { return m_metrics.size();    }
    uint32_t num_groups() const     { return m_groups.size();     }
    uint32_t num_comms() const      { return m_comms.size();      }
    uint32_t num_cartesians() const { return m_cartesians.size(); }
    uint32_t num_windows() const    { return m_windows.size();    }

    /// @}
    /// @name Get stored entities by id
    /// @{

    Machine*   get_machine(ident_t id) const;
    Node*      get_node(ident_t id) const;
    Process*   get_process(ident_t id) const;
    Thread*    get_thread(ident_t process_id, ident_t thread_id) const;
    Location*  get_location(ident_t id) const;
    Region*    get_region(ident_t id) const;
    Callsite*  get_callsite(ident_t id) const;
    CNode*     get_cnode(ident_t id) const;
    Metric*    get_metric(ident_t id) const;
    Group*     get_group(ident_t id) const;
    Comm*      get_comm(ident_t id) const;
    Cartesian* get_cartesian(ident_t id) const;
    RmaWindow* get_window(ident_t id) const;

    /// @}
    /// @name Get internal regions
    /// @{

    ident_t get_region_nolog() const;
    ident_t get_region_trace() const;

    /// @}
    /// @name Call tree handling
    /// @{

    Calltree* get_calltree() const;
    void      set_calltree(Calltree* calltree);

    /// @}
    /// @name Mapping file offset handling
    /// @{

    uint32_t get_offset(uint32_t rank) const;
    void     set_offset(uint32_t rank, uint32_t offset);

    /// @}


  private:
    /// Container type for machine definitions
    typedef std::vector<Machine*> machine_container;

    /// Container type for node definitions
    typedef std::vector<Node*> node_container;

    /// Container type for process definitions
    typedef std::vector<Process*> process_container;

    /// Container type for thread definitions
    typedef std::map<std::pair<ident_t,ident_t>, Thread*> thread_container;

    /// Container type for location definitions
    typedef std::vector<Location*> location_container;

    /// Container type for region definitions
    typedef std::vector<Region*> region_container;

    /// Container type for call site definitions
    typedef std::vector<Callsite*> callsite_container;

    /// Container type for metric definitions
    typedef std::vector<Metric*> metric_container;

    /// Container type for group definitions
    typedef std::vector<Group*> group_container;

    /// Container type for communicator definitions
    typedef std::vector<Comm*> comm_container;

    /// Container type for cartesian topology definitions
    typedef std::vector<Cartesian*> cartesian_container;

    /// Container type for RMA window definitions
    typedef std::vector<RmaWindow*> window_container;

    /// Container type for mapping file offsets
    typedef std::vector<uint32_t> offset_container;


    /// %Machine definitions: machine id |-@> machine
    machine_container m_machines;

    /// %Node definitions: node id |-@> node
    node_container m_nodes;

    /// %Process definitions: process id |-@> process
    process_container m_processes;

    /// %Thread definitions: (process id, thread id) |-@> thread
    thread_container m_threads;

    /// %Location definitions: location id |-@> location
    location_container m_locations;

    /// %Region definitions: region id |-@> region
    region_container m_regions;

    /// Call site definitions: call site id |-@> call site
    callsite_container m_callsites;

    /// %Metric definitions: metric id |-@> metric
    metric_container m_metrics;

    /// %Group definitions: group id |-@> group
    group_container m_groups;

    /// Communicator definitions: communicator id |-@> communicator
    comm_container m_comms;

    /// %Cartesian topology definitions: topology id |-@> cartesian
    cartesian_container m_cartesians;

    /// RMA window definitions: window id |-@> window
    window_container m_windows;

    // Mapping file offset values: rank id |-@> file offset
    offset_container m_offsets;

    /// Global call tree
    Calltree* m_calltree;

    /// ID of internal region where tracing is temporarily turned off
    ident_t m_region_nolog;

    /// ID of internal region where trace buffers are flushed to disk
    ident_t m_region_trace;


    /// @name Internal methods
    /// @{

    void setup();

    /// @}
    /// @name Add new entities
    /// @{

    void add_machine(Machine* machine);
    void add_node(Node* node);
    void add_process( Process* process);
    void add_thread(Thread* thread);
    void add_location(Location* location);
    void add_region(Region* region);
    void add_callsite(Callsite* csite);
    void add_cnode(CNode* cnode);
    void add_metric(Metric* metric);
    void add_group(Group* group);
    void add_comm(Comm* comm);
    void add_cartesian(Cartesian* cart);
    void add_window(RmaWindow*);

    /// @}


    /* Declare friends */
    friend class LocalTrace;
    friend class DefsFactory;
    friend class MpiDefsFactory;
};


}   /* namespace pearl */


#endif   /* !PEARL_GLOBALDEFS_H */
