/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstddef>

#include <elg_defs.h>

#include "Callsite.h"
#include "Comm.h"
#include "CNode.h"
#include "DefsFactory.h"
#include "Error.h"
#include "GlobalDefs.h"
#include "Location.h"
#include "Machine.h"
#include "Metric.h"
#include "Node.h"
#include "Process.h"
#include "Region.h"
#include "RmaWindow.h"
#include "Thread.h"

using namespace std;
using namespace pearl;


//--- Constants -------------------------------------------------------------

// For unknown entities
const string str_unknown("");

// For Metric objects
const string str_integer("INTEGER");
const string str_counter("COUNTER");
const string str_start  ("START");


//--- Static member variables -----------------------------------------------

const DefsFactory* DefsFactory::m_instance = NULL;


/*
 *---------------------------------------------------------------------------
 *
 * class DefsFactory
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

DefsFactory::~DefsFactory()
{
}


//--- Generic factory methods -----------------------------------------------

void DefsFactory::createMachine(GlobalDefs&   defs,
                                ident_t       id,
                                const string& name,
                                uint32_t      num_nodes) const
{
  // Create & store new machine
  defs.add_machine(new Machine(id, name, num_nodes));
}


void DefsFactory::createNode(GlobalDefs&   defs,
                             ident_t       id,
                             const string& name,
                             ident_t       machine_id,
                             uint32_t      num_cpus,
                             double        clockrate) const
{
  // Create new node object
  Node* node = new Node(id, name, num_cpus, clockrate);

  // Create cross-references (node <-> machine)
  Machine* machine = defs.get_machine(machine_id);
  machine->add_node(node);

  // Store node object
  defs.add_node(node);
}


void DefsFactory::createProcess(GlobalDefs&   defs,
                                ident_t       id,
                                const string& name) const
{
  try {
    // Process already defined => Set name
    Process* process = defs.get_process(id);
    process->set_name(name);
  }
  catch (RuntimeError&) {
    // Create & store new process object
    defs.add_process(new Process(id, name));
  }
}


void DefsFactory::createThread(GlobalDefs&   defs,
                               ident_t       id,
                               const string& name,
                               ident_t       process_id) const
{
  try {
    // Thread already defined => Set name
    Thread* thread = defs.get_thread(process_id, id);
    thread->set_name(name);
  }
  catch (RuntimeError&) {
    // Create new thread object
    Thread* thread = new Thread(id, name);

    // Create cross-references (thread <-> process)
    Process* process;
    try {
      process = defs.get_process(process_id);
    }
    catch (RuntimeError&) {
      // Create & store new process object
      process = new Process(process_id, str_unknown);
      defs.add_process(process);
    }
    process->add_thread(thread);

    // Store thread object
    defs.add_thread(thread);
  }
}


void DefsFactory::createLocation(GlobalDefs& defs,
                                 ident_t     id,
                                 ident_t     machine_id,
                                 ident_t     node_id,
                                 ident_t     process_id,
                                 ident_t     thread_id) const
{
  // Retrieve machine & node object
  Machine* machine = defs.get_machine(machine_id);
  Node*    node    = defs.get_node(node_id);

  // Process already defined?
  Process* process;
  try {
    process = defs.get_process(process_id);
  }
  catch (RuntimeError&) {
    // Create & store new process object
    process = new Process(process_id, str_unknown);
    defs.add_process(process);
  }

  // Thread already defined?
  Thread* thread;
  try {
    thread = defs.get_thread(process_id, thread_id);
  }
  catch (RuntimeError&) {
    // Create & store new thread object
    thread = new Thread(thread_id, str_unknown);

    // Create cross-references (thread <-> process)
    process->add_thread(thread);

    // Store thread object
    defs.add_thread(thread);
  }

  // Create & store new location object
  defs.add_location(new Location(id, machine, node, process, thread));
}


void DefsFactory::createRegion(GlobalDefs&   defs,
                               ident_t       id,
                               const string& name,
                               const string& file,
                               line_t        begin,
                               line_t        end,
                               const string& descr,
                               uint8_t       type) const
{
  // Create & store new region object
  defs.add_region(new Region(id, name, file, begin, end, descr, type));
}


void DefsFactory::createCallsite(GlobalDefs&   defs,
                                 ident_t       id,
                                 const string& file,
                                 line_t        lineno,
                                 ident_t       region_id) const
{
  // Create & store new call site object
  defs.add_callsite(new Callsite(id,
                                 file,
                                 lineno,
                                 defs.get_region(region_id)));
}


void DefsFactory::createCallpath(GlobalDefs& defs,
                                 ident_t     id,
                                 ident_t     region_id,
                                 ident_t     csite_id,
                                 ident_t     parent_id,
                                 double      time) const
{
  // Create & store new call tree node
  defs.add_cnode(new CNode(id,
                           defs.get_region(region_id),
                           defs.get_callsite(csite_id),
                           defs.get_cnode(parent_id),
                           time));
}


void DefsFactory::createMetric(GlobalDefs&   defs,
                               ident_t       id,
                               const string& name,
                               const string& descr,
                               uint8_t       type,
                               uint8_t       mode,
                               uint8_t       interval) const
{
  // Check parameter values
  if (type != ELG_INTEGER)
    throw FatalError("Unsupported metric data type.");
  if (mode != ELG_COUNTER)
    throw FatalError("Unsupported metric mode.");
  if (interval != ELG_START)
    throw FatalError("Unsupported metric interval semantics.");

  // Create & store new metric object
  defs.add_metric(new Metric(id,
                             name,
                             descr,
                             str_integer,
                             str_counter,
                             str_start));
}


void DefsFactory::createCartesian(GlobalDefs&        defs,
                                  ident_t            id,
                                  const cart_dims&   dimensions,
                                  const cart_period& periodic,
                                  std::string topo_name,
                                  std::vector<std::string> dim_names) const
{
  // Create & store new cartesian topology object
  defs.add_cartesian(new Cartesian(id, dimensions, periodic, topo_name, dim_names));
}


//--- MPI-specific factory methods ------------------------------------------

void DefsFactory::createMpiGroup(GlobalDefs&    defs,
                                 ident_t        id,
                                 process_group& process_ids,
                                 bool           is_self,
                                 bool           is_world) const
{
  // Create & store new group object
  defs.add_group(new Group(id, process_ids, is_self, is_world));
}


void DefsFactory::createMpiComm(GlobalDefs& defs,
                                ident_t     id,
                                ident_t     group_id) const
{
  // Create & store new communicator object
  defs.add_comm(new Comm(id, defs.get_group(group_id)));
}


void DefsFactory::createMpiWindow(GlobalDefs& defs,
                                  ident_t     id,
                                  ident_t     comm_id) const
{
  // Create & store new RMA window object
  defs.add_window(new RmaWindow(id, defs.get_comm(comm_id)));
}


void DefsFactory::createMpiCartesian(GlobalDefs&        defs,
                                     ident_t            id,
                                     const cart_dims&   dimensions,
                                     const cart_period& periodic,
                                     ident_t            comm_id,
                                     std::string        topo_name,
                                     std::vector<std::string> dim_names) const
{
  // Create "standard" cartesian topology
  createCartesian(defs, id, dimensions, periodic, topo_name, dim_names);
}


///--- Singleton interface ---------------------------------------------------

void DefsFactory::registerFactory(const DefsFactory* factory)
{
  assert(factory);

  if (m_instance)
    throw FatalError("DefsFactory::registerFactory(const DefsFactory*) -- "
                     "Factory already registered.");

  m_instance = factory;
}
