/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cstdlib>
#include <cstring>
#include <stack>
#include <string>
#include <vector>

#include <mpi.h>
#ifdef _OPENMP
#  include <omp.h>
#endif

#include <epk_archive.h>
#include <elg_error.h>

#include "CallbackData.h"
#include "CallbackManager.h"
#include "Callsite.h"
#include "CNode.h"
#include "EpkWriter.h"
#include "Error.h"
#include "Event.h"
#include "GlobalDefs.h"
#include "Location.h"
#include "Machine.h"
#include "MpiCartesian.h"
#include "MpiComm.h"
#include "MpiGroup.h"
#include "MpiWindow.h"
#include "Node.h"
#include "Process.h"
#include "Region.h"
#include "Thread.h"
#include "pearl_replay.h"


using namespace std;
using namespace pearl;


//--- Utility functions -----------------------------------------------------

#ifdef _OPENMP
namespace {

void pearl_mrg_recs(ElgOut* out, int infilec, ElgIn** inv);
void pearl_mrgthrd(char* outfile_name, int infilec, char** infile_namev);

}   /* unnamed namespace */
#endif

/*
 *---------------------------------------------------------------------------
 *
 * struct CbData
 *
 *---------------------------------------------------------------------------
 */

struct CbData : public CallbackData
{
  CbData(const GlobalDefs& defs)
    : m_defs(defs)
  {
  }

  const GlobalDefs& m_defs;
  stack<Region*>    m_stack;
};


/*
 *---------------------------------------------------------------------------
 *
 * class EpkWriter
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

EpkWriter::EpkWriter()
{
  // Determine MPI rankl of calling process
  MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);

  // Create callback manager for replay
  m_cbmanager = new CallbackManager();

  // Register callback methods
  m_cbmanager->register_callback(ENTER,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::enter));
  m_cbmanager->register_callback(EXIT,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::exit));
  m_cbmanager->register_callback(MPI_RECV,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::recv));
  m_cbmanager->register_callback(MPI_SEND,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::send));
  m_cbmanager->register_callback(OMP_FORK,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::fork));
  m_cbmanager->register_callback(OMP_JOIN,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::join));
  m_cbmanager->register_callback(OMP_ALOCK,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::alock));
  m_cbmanager->register_callback(OMP_RLOCK,
                                 PEARL_create_callback(this,
                                                       &EpkWriter::rlock));

  m_cbmanager->register_callback(MPI_RECV_REQUEST, 
                                 PEARL_create_callback(this,
                                                       &EpkWriter::recv_request));
  m_cbmanager->register_callback(MPI_SEND_COMPLETE, 
                                 PEARL_create_callback(this,
                                                       &EpkWriter::send_complete));
  m_cbmanager->register_callback(MPI_REQUEST_TESTED, 
                                 PEARL_create_callback(this,
                                                       &EpkWriter::request_tested));
  m_cbmanager->register_callback(MPI_CANCELLED, 
                                 PEARL_create_callback(this,
                                                       &EpkWriter::cancelled));
}


EpkWriter::~EpkWriter()
{
  // Release resources
  delete m_cbmanager;
}


//--- Writing experiment data -----------------------------------------------

void EpkWriter::write(const string&     archive,
                      const LocalTrace& trace,
                      const GlobalDefs& defs)
{
  // Set up experiment archive
  #pragma omp master
  {
    prepare_archive(archive);
  }

  // In principle, a simple barrier would be sufficient, but the Intel 10.1
  // compiler (and maybe others) refuses to compile this without the extra
  // block...
  {
    #pragma omp barrier
  }

  // Determine trace file name (temporary name for hybrid)
  char* outname;
#ifdef _OPENMP
  char buffer [32]; 
  sprintf (buffer, "%.5d.%.4d", m_rank, omp_get_thread_num());
  outname = epk_archive_filename(EPK_TYPE_ELG, buffer);
#else
  outname = epk_archive_rankname(EPK_TYPE_ELG, archive.c_str(), m_rank);
#endif

  // Write trace data; generates temporary per-thread files in hybrid mode
  m_elgFile = ElgOut_open(outname, ELG_BYTE_ORDER, 0);
  if (!m_elgFile)
    throw RuntimeError("Cannot open EPILOG event trace file \"" +
                       string(outname) + "\".");
  write_traces(defs, trace);
  ElgOut_close(m_elgFile);
  #pragma omp barrier

  // Merge temporary per-thread trace files into per-process trace files
#ifdef _OPENMP
  #pragma omp master 
  {
    // Determine trace file name (per-process)
    char* mergename = epk_archive_rankname(EPK_TYPE_ELG,
                                           archive.c_str(),
                                           m_rank);

    // Set up temporary file name list
    char** namev = new char*[omp_get_num_threads()];
    for (int i = 0; i < omp_get_num_threads(); ++i) {
      snprintf(buffer, 32, "%.5d.%.4d.elg", m_rank, i);
      namev[i] = strdup(buffer);
    }

    // Do the thread merge
    pearl_mrgthrd(mergename, omp_get_num_threads(), namev);

    // Release resources
    free(mergename);
    for (int i = 0; i < omp_get_num_threads(); ++i)
      free(namev[i]);
    delete[] namev;
  }
  #pragma omp barrier

  // Remove temporary per-thread files
  epk_archive_remove_file(outname);
#endif

  // Release resources
  free(outname);

  // Write definition file (only from master thread/rank 0)
  if (m_rank == 0) {
#ifdef _OPENMP
    if (0 == omp_get_thread_num())
#endif
    {
      outname = epk_archive_filename(EPK_TYPE_ESD, archive.c_str());
      m_esdFile = ElgOut_open(outname, ELG_BYTE_ORDER, ELG_UNCOMPRESSED);
      if (!m_esdFile)
        throw RuntimeError("Cannot open EPILOG trace definition file \"" +
                           string(outname) + "\".");
      write_definitions(defs);
      ElgOut_close(m_esdFile);
      free(outname);

      // Remove lock from archive
      epk_archive_unlock();
    }
  }
}


//--- Replay callback routines ----------------------------------------------

void EpkWriter::enter(const CallbackManager& cbmanager, int user_event,
                      const Event& event, CallbackData* cdata)
{
  CbData* data   = static_cast<CbData*>(cdata);
  Region* region = event->get_region();

  // Update call stack
  data->m_stack.push(region);

  // Write event record
#ifdef USE_ELG_ENTER_TRACING
  if (region->get_id() == data->m_defs.get_region_nolog()) {
    ElgOut_write_LOG_OFF(m_elgFile,
                         event.get_location()->get_id(), 
                         event->get_time(),
                         0, NULL);
  } else if (region->get_id() == data->m_defs.get_region_trace()) {
    ElgOut_write_ENTER_TRACING(m_elgFile,
                               event.get_location()->get_id(), 
                               event->get_time(),
                               0, NULL);
  } else 
#endif
  if (event->get_type() == ENTER_CS) {
    ElgOut_write_ENTER_CS(m_elgFile, 
                          event.get_location()->get_id(), 
                          event->get_time(),
                          event->get_callsite()->get_id(), 
                          0, NULL);
  } else {
    ElgOut_write_ENTER(m_elgFile, 
                       event.get_location()->get_id(), 
                       event->get_time(), 
                       region->get_id(), 
                       0, NULL);
  }
}


void EpkWriter::exit(const CallbackManager& cbmanager, int user_event,
                     const Event& event, CallbackData* cdata)
{
  CbData* data = static_cast<CbData*>(cdata);

  // Write event record
#ifdef USE_ELG_ENTER_TRACING
  // Get region to be left
  Region* region = data->m_stack.top();

  if (region->get_id() == data->m_defs.get_region_nolog()) {
    ElgOut_write_LOG_ON(m_elgFile,
                        event.get_location()->get_id(), 
                        event->get_time(),
                        0, NULL);
  } else if (region->get_id() == data->m_defs.get_region_trace()) {
    ElgOut_write_EXIT_TRACING(m_elgFile,
                              event.get_location()->get_id(), 
                              event->get_time(),
                              0, NULL);
  } else 
#endif
  if (event->get_type() == MPI_COLLEXIT) {
    ElgOut_write_MPI_COLLEXIT(m_elgFile, 
                              event.get_location()->get_id(), 
                              event->get_time(),
                              0, NULL,
                              ((event->get_root() == PEARL_NO_ID) 
                                ? ELG_NO_ID 
                                : event->get_root()),
                              event->get_comm()->get_id(), 
                              event->get_sent(), 
                              event->get_received());
  } else if (event->get_type() == OMP_COLLEXIT) {
    ElgOut_write_OMP_COLLEXIT(m_elgFile,
                              event.get_location()->get_id(), 
                              event->get_time(),
                              0, NULL);
  } else {
    ElgOut_write_EXIT(m_elgFile, 
                      event.get_location()->get_id(), 
                      event->get_time(),  
                      0, NULL);
  }

  // Update call stack
  data->m_stack.pop();
}


void EpkWriter::recv(const CallbackManager& cbmanager, int user_event,
                     const Event& event, CallbackData* cdata)
{
  if (event->is_typeof(MPI_RECV_COMPLETE))
    ElgOut_write_ATTR_UI4(m_elgFile, ELG_ATTR_REQUEST, event->get_reqid());

  ElgOut_write_MPI_RECV(m_elgFile, 
                        event.get_location()->get_id(), 
                        event->get_time(), 
                        event->get_source(), 
                        event->get_comm()->get_id(), 
                        event->get_tag());
}


void EpkWriter::send(const CallbackManager& cbmanager, int user_event,
                     const Event& event, CallbackData* cdata)
{
  if (event->is_typeof(MPI_SEND_REQUEST))
    ElgOut_write_ATTR_UI4(m_elgFile, ELG_ATTR_REQUEST, event->get_reqid());

  ElgOut_write_MPI_SEND(m_elgFile, 
                        event.get_location()->get_id(), 
                        event->get_time(),
                        event->get_dest(), 
                        event->get_comm()->get_id(), 
                        event->get_tag(), 
                        event->get_sent());
}

void EpkWriter::recv_request(const CallbackManager& cbmanager, int user_event,
                              const Event& event, CallbackData* data) 
{
  ElgOut_write_MPI_RECV_REQUEST(m_elgFile, 
                                event.get_location()->get_id(), 
                                event->get_time(),
                                event->get_reqid());
}

void EpkWriter::send_complete(const CallbackManager& cbmanager, int user_event,
		     const Event& event, CallbackData* data) 
{
  ElgOut_write_MPI_SEND_COMPLETE(m_elgFile, 
                                 event.get_location()->get_id(), 
                                 event->get_time(),
                                 event->get_reqid());
}

void EpkWriter::request_tested(const CallbackManager& cbmanager, int user_event,
                               const Event& event, CallbackData* data) 
{
  ElgOut_write_MPI_REQUEST_TESTED(m_elgFile, 
                                  event.get_location()->get_id(), 
                                  event->get_time(),
                                  event->get_reqid());
}

void EpkWriter::cancelled(const CallbackManager& cbmanager, int user_event,
                          const Event& event, CallbackData* data) 
{
  ElgOut_write_MPI_CANCELLED(m_elgFile, 
                             event.get_location()->get_id(), 
                             event->get_time(),
                             event->get_reqid());
}

void EpkWriter::fork(const CallbackManager& cbmanager, int user_event,
                     const Event& event, CallbackData* cdata)
{
  ElgOut_write_OMP_FORK(m_elgFile,
                        event.get_location()->get_id(), 
                        event->get_time());
}


void EpkWriter::join(const CallbackManager& cbmanager, int user_event,
                     const Event& event, CallbackData* cdata)
{
  ElgOut_write_OMP_JOIN(m_elgFile,
                        event.get_location()->get_id(), 
                        event->get_time());
}


void EpkWriter::alock(const CallbackManager& cbmanager, int user_event,
                      const Event& event, CallbackData* cdata)
{
  ElgOut_write_OMP_ALOCK(m_elgFile,
                         event.get_location()->get_id(), 
                         event->get_time(),
                         event->get_lockid());
}


void EpkWriter::rlock(const CallbackManager& cbmanager, int user_event,
                      const Event& event, CallbackData* cdata)
{
  ElgOut_write_OMP_RLOCK(m_elgFile,
                         event.get_location()->get_id(), 
                         event->get_time(), 
                         event->get_lockid());
}


//--- Internal I/O routines -------------------------------------------------

void EpkWriter::prepare_archive(const string& archive)
{
  // Set experiment archive name
  epk_archive_set_name(archive.c_str());

  // Create archive directories
  if (m_rank == 0) {
    if (epk_archive_verify(archive.c_str())) {
      epk_archive_create(EPK_TYPE_DIR);
      epk_archive_create(EPK_TYPE_ELG);
    } else {
      std::exit(EXIT_FAILURE);
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
}


void EpkWriter::write_traces(const GlobalDefs& defs,
                             const LocalTrace& trace)
{
  CbData data(defs);

  PEARL_forward_replay(trace, *m_cbmanager, &data);
}


void EpkWriter::write_definitions(const GlobalDefs& defs)
{
  // Write machine definitions
  for (uint32_t index = 0; index < defs.num_machines(); index++) {
    Machine* mach = defs.get_machine(index);

    ElgOut_write_MACHINE(m_esdFile,
                         mach->get_id(),
                         mach->num_nodes(), 
                         ElgOut_define_string(m_esdFile,
                                              mach->get_name().c_str()));
  }

  // Write node definitions
  for (uint32_t index = 0; index < defs.num_nodes(); index++) {
    Node* node = defs.get_node(index);

    ElgOut_write_NODE(m_esdFile, 
                      node->get_id(), 
                      node->get_machine()->get_id(),
                      node->num_cpus(), 
                      ElgOut_define_string(m_esdFile,
                                           node->get_name().c_str()),
                      node->get_clockrate());
  }

  // Write process definitions
  for (uint32_t pid = 0; pid < defs.num_processes(); pid++) {
    Process* proc = defs.get_process(pid);

    if (!proc->get_name().empty())
      ElgOut_write_PROCESS(m_esdFile,
                           proc->get_id(),
                           ElgOut_define_string(m_esdFile,
                                                proc->get_name().c_str()));

    // Write thread definitions
    for (uint32_t tid = 0; tid < proc->num_threads(); tid++) {
      Thread* thread = proc->get_thread(tid);

      if (!thread->get_name().empty())
        ElgOut_write_THREAD(m_esdFile,
                            thread->get_id(),
                            thread->get_process()->get_id(),
                            ElgOut_define_string(m_esdFile,
                                                 thread->get_name().c_str()));
    }
  }

  // Write location definitions
  for (uint32_t index= 0; index < defs.num_locations(); index++) {
    Location* loc = defs.get_location(index);

    ElgOut_write_LOCATION(m_esdFile,
                          loc->get_id(),
                          loc->get_machine()->get_id(),
                          loc->get_node()->get_id(),
                          loc->get_process()->get_id(),
                          loc->get_thread()->get_id());
  }
  
  // Write region definitions
  for (uint32_t index = 0; index < defs.num_regions(); index++) {
    Region* region = defs.get_region(index);
    line_t  begln  = region->get_begin();
    line_t  endln  = region->get_end();

    ElgOut_write_REGION(m_esdFile, 
                        region->get_id(), 
                        ElgOut_define_string(m_esdFile,
                                             region->get_name().c_str()), 
                        ElgOut_define_file(m_esdFile,
                                           region->get_file().c_str()),
                        begln == PEARL_NO_NUM ? ELG_NO_NUM : begln, 
                        endln == PEARL_NO_NUM ? ELG_NO_NUM : endln, 
                        ElgOut_define_string(m_esdFile,
                                             region->get_description().c_str()),
                        region->get_type_id());
  }

  // Write call site definitions
  for (uint32_t index = 0; index < defs.num_callsites(); index++) {
    Callsite* csite = defs.get_callsite(index);

    ElgOut_write_CALL_SITE(m_esdFile, 
                           csite->get_id(),
                           ElgOut_define_file(m_esdFile,
                                              csite->get_file().c_str()),
                           csite->get_line(),
                           csite->get_callee()->get_id(),
                           ELG_NO_ID);
  }
  
  // Write call path definitions
  for (uint32_t index = 0; index < defs.num_cnodes(); index++) {
    CNode* cpath = defs.get_cnode(index);
    union {
      double  dummy;
      elg_ui8 order;
    } convert = { cpath->get_time() };

    ElgOut_write_CALL_PATH(m_esdFile, 
                           cpath->get_id(), 
                           (cpath->get_region() == NULL) 
                             ? ELG_NO_ID 
                             : cpath->get_region()->get_id(),
                           (cpath->get_parent() == NULL) 
                             ? ELG_NO_ID 
                             : cpath->get_parent()->get_id(),
                           convert.order);
  }

  // Write MPI group definitions
  for (uint32_t index = 0; index < defs.num_groups(); index++) {
    Group *group = defs.get_group(index);

    // Set up group data
    elg_ui1  mode = 0;
    elg_ui4  grpc = 0;
    elg_ui4* grpv = NULL;

    if (group->is_world())
      mode |= ELG_GROUP_WORLD;

    if (group->is_self()) {
      mode |= ELG_GROUP_SELF;
    } else {
      grpc = group->num_ranks();
      grpv = new elg_ui4[grpc];
      for (uint32_t pid = 0; pid < grpc; ++pid)
        grpv[pid] = group->get_global_rank(pid);
    }

    ElgOut_write_MPI_GROUP(m_esdFile,
                           group->get_id(),
                           mode,
                           grpc,
                           grpv);

    delete[] grpv;
  }

  // Write MPI communicator definitions
  for (uint32_t index = 0; index < defs.num_comms(); index++) {
    Comm* comm = defs.get_comm(index);

    ElgOut_write_MPI_COMM_REF(m_esdFile,
                              comm->get_id(),
                              comm->get_group()->get_id());
  }

  // Write topology definitions
  for (uint32_t index = 0; index < defs.num_cartesians(); index++) {
    Cartesian*    cart     = defs.get_cartesian(index);    
    MpiCartesian* mpi_cart = dynamic_cast<MpiCartesian*>(cart);

    // Encode number of locations per dimension
    vector<uint32_t> nlocsv;
    elg_ui4*         nlocs = new elg_ui4[cart->num_dimensions()];
    cart->get_dimensionv(nlocsv);
    for (uint32_t i = 0; i < nlocsv.size(); i++)
      nlocs[i] = static_cast<elg_ui4>(nlocsv[i]);

    // Encode periodicity
    vector<bool> periodv;
    elg_ui1*     period = new elg_ui1[cart->num_dimensions()];
    cart->get_periodv(periodv);
    for (uint32_t i = 0; i < periodv.size(); i++)
       period[i] = static_cast<elg_ui1>(periodv[i]);
    string cart_name;
    cart->get_name(cart_name);
    vector<string> dim_names;
    cart->get_dim_names(dim_names);
    elg_ui4 * dim_ids=(elg_ui4*)calloc(nlocsv.size(), sizeof(elg_ui4));
    for(uint32_t i=0;i<nlocsv.size();i++) {
    	dim_ids[i]=ElgOut_define_string(m_esdFile,dim_names[i].c_str());
    }
    // Write record
    ElgOut_write_CART_TOPOLOGY(m_esdFile, 
                               cart->get_id(), 
                               ElgOut_define_string(m_esdFile,cart_name.c_str()),
                               mpi_cart
                                 ? mpi_cart->get_comm()->get_id()
                                 : ELG_NO_ID,
                               cart->num_dimensions(),
                               nlocs,
                               period, dim_ids);


    // Write coordinates
    vector<uint32_t> coordsv;
    elg_ui4*         coords = new elg_ui4[cart->num_dimensions()];
    for (uint32_t i = 0; i < defs.num_locations(); i++) {
      try {
        Location* loc = defs.get_location(i);

        // Encode coordinate
        cart->get_coords(loc, coordsv);
        for (uint32_t j = 0; j < coordsv.size(); j++)
          coords[j] = static_cast<elg_ui4>(coordsv[j]);

        // Write record
        ElgOut_write_CART_COORDS(m_esdFile, 
                                 cart->get_id(), 
                                 loc->get_id(), 
                                 cart->num_dimensions(),
                                 coords);
      }
      catch(RuntimeError&) {
      }
    }

    // Release resources
    delete[] coords;
    delete[] nlocs;
    delete[] period;
  }

  // Write RMA window definitions
  for (uint32_t index = 0; index < defs.num_windows(); index++) {
    MpiWindow* window = dynamic_cast<MpiWindow*>(defs.get_window(index));

    ElgOut_write_MPI_WIN(m_esdFile, 
                         window->get_id(), 
                         window->get_comm()->get_id());
  }  
}


//--- Utility functions -----------------------------------------------------

#ifdef _OPENMP
namespace {

void pearl_mrg_recs(ElgOut* out, int infilec, ElgIn** inv)
{
  int i;
  ElgRec** recv   = (ElgRec**) calloc(infilec, sizeof(ElgRec*));

  /* init */
  for (i = 0; i < infilec; i++)
    recv[i] = ElgIn_read_record(inv[i]);

  while(1)
    {
      ElgRec* min_rec = NULL;
      int min_pos;      
      
      for (i = 0; i < infilec ; i++)
        {
          if (recv[i] == NULL)
              continue;
          /* definition records are oldest by default */
          else if (!ElgRec_is_event(recv[i]))
            {
              min_rec = recv[i];      
              min_pos = i;
              break;
            }
          /* select first available record */
          else if (min_rec == NULL)
            {
              min_rec = recv[i];      
              min_pos = i;
              if (!ElgRec_is_event(min_rec))
                break;
            }
          /* compare timestamps: looking for oldest (minimum time stamp) record */     
          else
            {
              double first, second;

              ElgRec_seek(min_rec, sizeof(elg_ui4));
              ElgRec_seek(recv[i], sizeof(elg_ui4));

              first  = ElgRec_read_d8(min_rec);
              second = ElgRec_read_d8(recv[i]);

              if (first > second)
                {
                  min_rec = recv[i];
                  min_pos = i;
                }
            }
        }
      /* finish if all records are processed */
      if (min_rec == NULL) {
        free(recv);
        return;
      }

      /* replace oldest record by new record */
      recv[min_pos] = ElgIn_read_record(inv[min_pos]);

      ElgOut_write_record(out, min_rec);

      ElgRec_free(min_rec);
    }
}


void pearl_mrgthrd(char* outfile_name, 
                   int infilec, char** infile_namev)
{
  int i;
  ElgOut* out;
  ElgIn** inv; 
  int flag = 0;

  /* open infiles */

  inv = (ElgIn**) calloc(infilec, sizeof(ElgIn*));
  for (i = 0; i < infilec; i++)
    if ( !(inv[i] = ElgIn_open(infile_namev[i])) )
      elg_error_msg("Cannot open input file %s", infile_namev[i]);

  /* open outfile */
#if defined(ELG_COMPRESSED)
  if (!epk_str2bool(epk_get(ELG_COMPRESSION)))
    flag = ELG_UNCOMPRESSED;
#endif
  if ( !(out = ElgOut_open(outfile_name, ElgIn_get_byte_order(inv[0]), flag)) )
    elg_error_msg("Cannot open output file %s", outfile_name);
   
  /* merge records */
  pearl_mrg_recs(out, infilec, inv);
    
  /* close infiles */
  for (i = 0; i < infilec; i++)
    ElgIn_close(inv[i]);

  /* close outfiles */
  ElgOut_close(out);
}


}   /* unnamed namespace */
#endif   /* _OPENMP */
