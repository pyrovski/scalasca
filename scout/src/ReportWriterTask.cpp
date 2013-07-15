/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstdio>
#include <vector>

#if defined(_MPI)
  #include <mpi.h>
#endif   // _MPI

#include <Callsite.h>
#include <Cartesian.h>
#include <CNode.h>
#include <Error.h>
#include <GlobalDefs.h>
#include <Location.h>
#include <Machine.h>
#include <Node.h>
#include <Process.h>
#include <Region.h>
#include <Thread.h>
#include <iostream>
#include "stdlib.h"
#include "string.h"

#include "AnalyzeTask.h"
#include "ReportData.h"
#include "ReportWriterTask.h"
#include "TaskData.h"

using namespace std;
using namespace pearl;
using namespace scout;


//--- Symbolic names --------------------------------------------------------

#define OUTPUT_FILENAME   "scout.cube"
#define VERSION           "1.4"
#define PRIMARY_MIRROR    "http://apps.fz-juelich.de/scalasca/releases/" \
                          "scalasca/" VERSION "/help/"


/*
 *---------------------------------------------------------------------------
 *
 * class ReportWriterTask
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * @brief Constructor. Creates a new report writer task.
 *
 * This constructor creates a new report writer task instance using the
 * given parameters.
 *
 * @param  sharedData  Shared task data object
 * @param  analyzer    Analyzer task to query for the severity data
 * @param  dirname     Experiment archive directory name
 * @param  rank        Global process rank (MPI rank or 0)
 */
ReportWriterTask::ReportWriterTask(const TaskDataShared& sharedData,
                                   const AnalyzeTask*    analyzer,
                                   const string&         dirname,
                                   int                   rank)
  : Task(),
    m_sharedData(sharedData),
    m_analyzer(analyzer),
    m_dirname(dirname),
    m_rank(rank)
{
}


//--- Executing the task ----------------------------------------------------

/**
 * @brief Executes the report writer task.
 *
 * The writer task first creates the CUBE output file, writes the static
 * definition data (meta data) to it, and incrementally collates and writes
 * the per-pattern severity data.
 *
 * @return Returns @em true if the execution was successful, @em false
 *         otherwise
 * @throw  pearl::FatalError  if the CUBE file could not be created.
 */
bool ReportWriterTask::execute()
{
  // This variable is implicitly shared!
  static int error = 0;

  ReportData data;
  FILE*      fp = NULL;
  if (0 == m_rank) {
    #pragma omp master
    {
      // Create CUBE file
      std::string filename = m_dirname + "/" + OUTPUT_FILENAME;
      fp = std::fopen(filename.c_str(), "w");

      // Broadcast whether CUBE file could be opened
      error = (NULL == fp) ? 1 : 0;
      #if defined(_MPI)
        MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
      #endif   // _MPI
    }
    #pragma omp barrier
    if (error)
      throw FatalError("Could not create CUBE report!");

    #pragma omp master
    {
      // Create CUBE object
      data.cb = cube_create();

      // Write static definition data
      cube_def_mirror(data.cb, PRIMARY_MIRROR);
      cube_def_attr(data.cb, "CUBE_CT_AGGR", "SUM");

      gen_sysinfo(data);
      gen_regions(data);
      gen_cnodes(data);
      gen_cartesians(data);
      m_analyzer->gen_patterns(data);
      cube_write_def(data.cb, fp);
    }

    // Collate severity data
    m_analyzer->gen_severities(data, m_rank, fp);

    // Clean up
    #pragma omp master
    {
      cube_write_finish(data.cb, fp); 
      std::fclose(fp);
      cube_free(data.cb);
    }
  } else {
    #pragma omp master
    {
      // Check whether CUBE file could be opened
      #if defined(_MPI)
        MPI_Bcast(&error, 1, MPI_INT, 0, MPI_COMM_WORLD);
      #endif   // _MPI
    }
    #pragma omp barrier
    if (error)
      throw FatalError("Could not create CUBE report!");

    // Collate severity data
    m_analyzer->gen_severities(data, m_rank, fp);
  }

  return true;
}


//--- Report data generation ------------------------------------------------

/**
 * @brief Generates the system hierarchy information.
 *
 * This method defines the CUBE system hierarchy in the CUBE object passed
 * via @a data based on the information available in the global definitions
 * object.
 *
 * @param  data  Report data structure
 */
void ReportWriterTask::gen_sysinfo(ReportData& data) const
{
  const GlobalDefs& defs = *m_sharedData.mDefinitions; 

  // Temporary mapping tables
  vector<cube_machine*> machs;   // machine id |-> CUBE machine
  vector<cube_node*>    nodes;   // node id    |-> CUBE node

  // Create machine objects
  uint32_t count = defs.num_machines();
  for (ident_t id = 0; id < count; ++id) {
    Machine*      mach  = defs.get_machine(id);
    cube_machine* cmach = cube_def_mach(data.cb,
                                        mach->get_name().c_str(),
                                        "");
    machs.push_back(cmach);
  }

  // Create node objects
  count = defs.num_nodes();
  for (ident_t id = 0; id < count; ++id) {
    Node*      node  = defs.get_node(id);
    cube_node* cnode = cube_def_node(data.cb,
                                     node->get_name().c_str(),
                                     machs[node->get_machine()->get_id()]);
    nodes.push_back(cnode);
  }

  // Create process objects, thread objects + location mapping
  data.locs.reserve(defs.num_locations());
  count = defs.num_processes();
  for (ident_t id = 0; id < count; ++id) {
    Process*      proc  = defs.get_process(id);
    cube_process* cproc = cube_def_proc(data.cb,
                                        proc->get_name().c_str(),
                                        id,
                                        nodes[proc->get_node()->get_id()]);

    uint32_t thread_cnt = proc->num_threads();
    for (ident_t tid = 0; tid < thread_cnt; ++tid) {
      Thread*      thread  = proc->get_thread(tid);
      cube_thread* cthread = cube_def_thrd(data.cb,
                                           thread->get_name().c_str(),
                                           tid,
                                           cproc);
      data.locs[thread->get_location()->get_id()] = cthread;
    }
  }
}


/**
 * @brief Generates the source-code region information.
 *
 * This method defines the CUBE source-code regions in the CUBE object passed
 * via @a data based on the information available in the global definitions
 * object.
 *
 * @param  data  Report data structure
 */
void ReportWriterTask::gen_regions(ReportData& data) const
{
  const GlobalDefs& defs = *m_sharedData.mDefinitions;

  // Create regions
  uint32_t count = defs.num_regions();
  for (ident_t id = 0; id < count; ++id) {
    Region*      region  = defs.get_region(id);
    cube_region* cregion = cube_def_region(data.cb,
                                           region->get_name().c_str(),
                                           region->get_begin(),
                                           region->get_end(),
                                           "",
                                           region->get_description().c_str(),
                                           region->get_file().c_str());
    data.regions.push_back(cregion);
  }
}


/**
 * @brief Generates the call tree information.
 *
 * This method defines the CUBE call tree in the CUBE object passed via @a
 * data based on the information available in the global definitions object.
 *
 * @param  data  Report data structure
 */
void ReportWriterTask::gen_cnodes(ReportData& data) const
{
  const GlobalDefs& defs = *m_sharedData.mDefinitions; 

  // Create dummy entry for "UNKNOWN" call-tree node
  data.cnodes.push_back(NULL);

  // Create call-tree nodes
  uint32_t count = defs.num_cnodes();
  for (ident_t id = 1; id < count; ++id) {
    CNode* cnode = defs.get_cnode(id);

    // Determine parent
    CNode*      parent  = cnode->get_parent();
    cube_cnode* cparent = NULL;
    if (parent)
      cparent = data.cnodes[parent->get_id()];

    // Create node
    cube_cnode* ccnode;
    Callsite*   csite = cnode->get_callsite();
    if (csite) {
      ccnode = cube_def_cnode_cs(data.cb,
                                 data.regions[csite->get_callee()->get_id()],
                                 csite->get_file().c_str(),
                                 csite->get_line(),
                                 cparent);
    } else {
      ccnode = cube_def_cnode(data.cb,
                              data.regions[cnode->get_region()->get_id()],
                              cparent);
    }

    data.cnodes.push_back(ccnode);
  }
}


/**
 * @brief Generates the topology information.
 *
 * This method defines the CUBE cartesian topology in the CUBE object passed
 * via @a data based on the information available in the global definitions
 * object.
 *
 * @param  data  Report data structure
 */
void ReportWriterTask::gen_cartesians(ReportData& data) const
{
  const GlobalDefs& defs = *m_sharedData.mDefinitions; 

  // Create topologies
  uint32_t ncarts = defs.num_cartesians();
  for (ident_t cid = 0; cid < ncarts; ++cid) {
    Cartesian* cart  = defs.get_cartesian(cid);
    uint32_t   ndims = cart->num_dimensions();
        // Query topology information
    vector<uint32_t> dimv;
    vector<bool> periodv;
    cart->get_dimensionv(dimv);
    cart->get_periodv(periodv);

    // Get name information
    string topo_name;
    cart->get_name(topo_name);
    vector<string> dim_names;
    cart->get_dim_names(dim_names);


    // Transform topology information to CUBE-conforming format
    long* dim    = new long[ndims];
    int*  period = new int[ndims];
    char** dim_names_c_array = new char*[ndims];
    for (uint32_t index = 0; index < ndims; ++index) {
      dim[index]    = dimv[index];
      period[index] = periodv[index];
    }

    // Create cartesian
    cube_cartesian* ccart = cube_def_cart(data.cb, ndims, dim, period);

    cube_cart_set_name(ccart,topo_name.c_str());
    if(dim_names_c_array!=NULL) {
    	for(uint32_t i=0;i<ndims;i++) {
    		dim_names_c_array[i]=strdup(dim_names[i].c_str());
    	}
    cube_cart_set_namedims(ccart,dim_names_c_array);
    }
    // Release memory
    delete[] dim;
    delete[] period;
    for(uint32_t i=0;i<ndims;i++){
    	free(dim_names_c_array[i]);
    }
    delete[] dim_names_c_array;

    // Define the coordinates
    long*    coord = new long[ndims];
    uint32_t nlocs = defs.num_locations();
    for (ident_t lid = 0; lid < nlocs; ++lid) {
      Location* loc = defs.get_location(lid);
      
      try {
        // Query coordinate information
        vector<uint32_t> coordv;
        cart->get_coords(loc, coordv);

        // Transform coordinate to CUBE-conforming format
        for (uint32_t index = 0; index < ndims; ++index)
          coord[index] = coordv[index];

        // Define coordinate
        cube_def_coords(data.cb, ccart, data.locs[lid], coord);
      }
      catch (const Error&) {
      }
    }

    // Release memory
    delete[] coord;
  }
}
