/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstdlib>

#include <elg_readcb.h>
#include <epk_archive.h>

#include "Callsite.h"
#include "Calltree.h"
#include "Cartesian.h"
#include "Comm.h"
#include "Error.h"
#include "GlobalDefs.h"
#include "Group.h"
#include "Location.h"
#include "Machine.h"
#include "Metric.h"
#include "Node.h"
#include "Process.h"
#include "Region.h"
#include "RmaWindow.h"
#include "Thread.h"
#include "readcb.h"

using namespace std;
using namespace pearl;


//--- Utility functions -----------------------------------------------------

namespace
{

template<class containerT>
inline void delete_seq_container(const containerT& container)
{
  typename containerT::const_iterator it = container.begin();
  while (it != container.end()) {
    delete *it;
    ++it;
  }
}


template<class containerT>
inline typename containerT::value_type get_entry(const containerT& container,
                                                 ident_t           id,
                                                 const string&     desc)
{
  if (id >= container.size()){
    throw RuntimeError("GlobalDefs::" + desc + " -- ID out of bounds.");
  }

  return container[id];
}


template<class containerT>
inline void add_entry(containerT&                     container,
                      typename containerT::value_type entry,
                      const string&                   desc)
{
  if (entry->get_id() == container.size())
    container.push_back(entry);
  else   // This should never happen!
    throw RuntimeError("GlobalDefs::" + desc + " -- Invalid ID.");
}

}   /* unnamed namespace */


/*
 *---------------------------------------------------------------------------
 *
 * class GlobalDefs
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

GlobalDefs::GlobalDefs(const string& dirname)
  : m_region_nolog(PEARL_NO_ID),
    m_region_trace(PEARL_NO_ID)
{
  // Create call tree object
  m_calltree = new Calltree();

  // Determine file name
  char*  tmp_str  = epk_archive_filename(EPK_TYPE_ESD, dirname.c_str());
  string filename = tmp_str;
  free(tmp_str);

  // Open definitions file
  ElgRCB* file = elg_read_open(filename.c_str());
  if (!file)
    throw RuntimeError("Cannot open EPILOG trace definition file \"" +
                       filename + "\".");

  // Read data
  DefsCbData data(*this);
  while (elg_read_next_def(file, &data))
  {
    // Check for errors
    if (!data.m_message.empty())
      throw Error(data.m_message);
  }
  elg_read_close(file);

  // Additional initialization
  setup();
}


GlobalDefs::GlobalDefs(const uint8_t* buffer, std::size_t size)
  : m_region_nolog(PEARL_NO_ID),
    m_region_trace(PEARL_NO_ID)
{
  // Create call tree object
  m_calltree = new Calltree();

  // Open memory buffer for reading
  ElgRCB* file = elg_read_open_buffer((buffer_t)buffer, size);
  if (!file)
    throw RuntimeError("Cannot open EPILOG trace definition buffer.");

  // Read data
  DefsCbData data(*this);
  while (elg_read_next_def(file, &data))
  {
    // Check for errors
    if (!data.m_message.empty())
      throw Error(data.m_message);
  }
  elg_read_close(file);

  // Additional initialization
  setup();
}


GlobalDefs::~GlobalDefs()
{
  // Delete entity containers
  delete_seq_container(m_machines);
  delete_seq_container(m_nodes);
  delete_seq_container(m_processes);
  delete_seq_container(m_locations);
  delete_seq_container(m_regions);
  delete_seq_container(m_callsites);
  delete_seq_container(m_metrics);
  delete_seq_container(m_groups);
  delete_seq_container(m_comms);
  delete_seq_container(m_cartesians);
  delete_seq_container(m_windows);

  thread_container::const_iterator thrd_it = m_threads.begin();
  while (thrd_it != m_threads.end()) {
    delete thrd_it->second;
    ++thrd_it;
  }

  // Delete call tree object
  delete m_calltree;
}


//--- Get number of stored entities -----------------------------------------

uint32_t GlobalDefs::num_cnodes() const
{
  return m_calltree->num_cnodes();
}


//--- Get stored entities by id ---------------------------------------------

Machine* GlobalDefs::get_machine(ident_t id) const
{
  return get_entry(m_machines, id, "get_machine(ident_t)");
}


Node* GlobalDefs::get_node(ident_t id) const
{
  return get_entry(m_nodes, id, "get_node(ident_t)");
}


Process* GlobalDefs::get_process(ident_t id) const
{
  return get_entry(m_processes, id, "get_process(ident_t)");
}


Thread* GlobalDefs::get_thread(ident_t process_id, ident_t thread_id) const
{
  thread_container::key_type key(process_id, thread_id);
  thread_container::const_iterator it = m_threads.find(key);
  if (it == m_threads.end())
    throw RuntimeError("GlobalDefs::get_thread(ident_t,ident_t) -- Unknown ID.");

  return it->second;
}


Location* GlobalDefs::get_location(ident_t id) const
{
  if (id == PEARL_NO_ID)
    return NULL;

  return get_entry(m_locations, id, "get_location(ident_t)");
}


Region* GlobalDefs::get_region(ident_t id) const
{
  if (id == PEARL_NO_ID)
    return NULL;

  return get_entry(m_regions, id, "get_region(ident_t)");
}


Callsite* GlobalDefs::get_callsite(ident_t id) const
{
  if (id == PEARL_NO_ID)
    return NULL;

  return get_entry(m_callsites, id, "get_callsite(ident_t)");
}


CNode* GlobalDefs::get_cnode(ident_t id) const
{
  if (id == PEARL_NO_ID)
    return NULL;

  CNode* result = m_calltree->get_cnode(id);
  if (!result)
    throw RuntimeError("GlobalDefs::get_cnode(ident_t) -- ID out of bounds.");

  return result;
}


Metric* GlobalDefs::get_metric(ident_t id) const
{
  return get_entry(m_metrics, id, "get_metric(ident_t)");
}


Group* GlobalDefs::get_group(ident_t id) const
{
  return get_entry(m_groups, id, "get_group(ident_t)");
}


Comm* GlobalDefs::get_comm(ident_t id) const
{
  return get_entry(m_comms, id, "get_comm(ident_t)");
}


Cartesian* GlobalDefs::get_cartesian(ident_t id) const
{
  return get_entry(m_cartesians, id, "get_cartesian(ident_t)");
}


RmaWindow* GlobalDefs::get_window(ident_t id) const
{
  return get_entry(m_windows, id, "get_window(ident_t)");
}


//--- Get internal regions --------------------------------------------------

ident_t GlobalDefs::get_region_nolog() const
{
  return m_region_nolog;
}


ident_t GlobalDefs::get_region_trace() const
{
  return m_region_trace;
}


//--- Call tree handling ----------------------------------------------------

Calltree* GlobalDefs::get_calltree() const
{
  return m_calltree;
}


void GlobalDefs::set_calltree(Calltree* calltree)
{
  if (m_calltree == calltree)
    return;

  delete m_calltree;
  m_calltree = calltree;
}


//--- Mapping file offset handling ------------------------------------------

uint32_t GlobalDefs::get_offset(uint32_t rank) const
{
  if (rank >= m_offsets.size())
    throw RuntimeError("GlobalDefs::get_offset(uint32_t) -- "
                       "Rank number out of bounds");

  return m_offsets[rank];
}


void GlobalDefs::set_offset(uint32_t rank, uint32_t offset)
{
  if (rank >= m_offsets.size())
    m_offsets.resize(rank + 1);

  m_offsets[rank] = offset;
}


//--- Internal methods ------------------------------------------------------

void GlobalDefs::setup()
{
  // Create "TRACING" region
  if (m_region_trace == PEARL_NO_ID) {
    m_region_trace = m_regions.size();
    Region* region = new Region(m_region_trace,
                                "TRACING",
                                "EPIK",
                                PEARL_NO_NUM,
                                PEARL_NO_NUM,
                                "EPIK",
                                ELG_UNKNOWN);

    m_regions.push_back(region);
  }

  if (m_region_nolog == PEARL_NO_ID) {
    m_region_nolog = m_regions.size();
    Region* region = new Region(m_region_nolog,
                                "PAUSING",
                                "EPIK",
                                PEARL_NO_NUM,
                                PEARL_NO_NUM,
                                "EPIK",
                                ELG_UNKNOWN);

    m_regions.push_back(region);
  }

  // Create "UNKNOWN" call path
  if (m_calltree->num_cnodes() == 0)
    m_calltree->add_cnode(NULL, NULL, NULL, 0);

  // Reset call tree modification flag
  m_calltree->set_modified(false);
}


//--- Add new entities ------------------------------------------------------

void GlobalDefs::add_machine(Machine* machine)
{
  add_entry(m_machines, machine, "add_machine(Machine*)");
}


void GlobalDefs::add_node(Node* node)
{
  add_entry(m_nodes, node, "add_node(Node*)");
}


void GlobalDefs::add_process(Process* process)
{
  ident_t  id    = process->get_id();
  uint32_t count = m_processes.size();
  if (id < count)
    throw RuntimeError("GlobalDefs::add_process(Process*) -- Invalid ID.");

  // Eventually create dummy processes
  for (uint32_t i = count; i < id; ++i)
    m_processes.push_back(new Process(i, ""));

  m_processes.push_back(process);
}


void GlobalDefs::add_thread(Thread* thread)
{
  ident_t thread_id  = thread->get_id();
  ident_t process_id = thread->get_process()->get_id();

  thread_container::key_type key(process_id, thread_id);
  thread_container::iterator it = m_threads.lower_bound(key);
  if (it != m_threads.end() && !(m_threads.key_comp()(key, it->first)))
    throw RuntimeError("GlobalDefs::add_thread(Thread*) -- Invalid ID.");

  m_threads.insert(it, thread_container::value_type(key, thread));
}


void GlobalDefs::add_location(Location* location)
{
  add_entry(m_locations, location, "add_location(Location*)");
}


void GlobalDefs::add_region(Region* region)
{
  add_entry(m_regions, region, "add_region(Region*)");

  // Check for internal "TRACING" region
  if (region->get_name()        == "TRACING" &&
      region->get_file()        == "EPIK"    &&
      region->get_description() == "EPIK")
    m_region_trace = region->get_id();
  // Check for special "PAUSING" region
  if (region->get_name()        == "PAUSING" &&
      region->get_file()        == "EPIK"    &&
      region->get_description() == "EPIK")
    m_region_nolog = region->get_id();
}


void GlobalDefs::add_callsite(Callsite* csite)
{
  add_entry(m_callsites, csite, "add_callsite(Callsite*)");
}


void GlobalDefs::add_cnode(CNode* cnode)
{
  m_calltree->add_cnode(cnode);
}


void GlobalDefs::add_metric(Metric* metric)
{
  add_entry(m_metrics, metric, "add_metric(Metric*)");
}


void GlobalDefs::add_group(Group* group)
{
  add_entry(m_groups, group, "add_group(Group*)");
}


void GlobalDefs::add_comm(Comm* comm)
{
  add_entry(m_comms, comm, "add_comm(Comm*)");
}


void GlobalDefs::add_cartesian(Cartesian* cart)
{
  add_entry(m_cartesians, cart, "add_cartesian(Cartesian*)");
}


void GlobalDefs::add_window(RmaWindow* window)
{
  add_entry(m_windows, window, "add_window(RmaWindow*)");
}
