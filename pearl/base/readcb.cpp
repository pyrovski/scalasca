/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <algorithm>

#include <elg_readcb.h>

#include "Cartesian.h"
#include "Comm.h"
#include "DefsFactory.h"
#include "Error.h"
#include "Event_rep.h"
#include "EventFactory.h"
#include "GlobalDefs.h"
#include "Group.h"
#include "LocalTrace.h"
#include "Location.h"
#include "MappingTable.h"
#include "RmaWindow.h"
#include "Thread.h"
#include "readcb.h"

using namespace std;
using namespace pearl;


//--- Macros ----------------------------------------------------------------

#define CONV_ID(id)     (((id)  == ELG_NO_ID)  ? PEARL_NO_ID  : (id))
#define CONV_NUM(num)   (((num) == ELG_NO_NUM) ? PEARL_NO_NUM : (num))


//--- Constants -------------------------------------------------------------

// Used for unknown entities
const string str_unknown("");


/*
 *---------------------------------------------------------------------------
 *
 * struct DefsCbData
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

DefsCbData::DefsCbData(GlobalDefs& defs)
  : m_defs(defs),
    m_strcnt(0),
    m_strid(PEARL_NO_ID),
    m_worldsize(1)
{
}


//--- Mapping functions -----------------------------------------------------

string DefsCbData::get_string(ident_t id) const
{
  if (id == PEARL_NO_ID)
    return str_unknown;

  if (id >= m_strmap.size())
	  throw RuntimeError("Oops! Invalid string ID.");

  return m_strmap[id];
}


string DefsCbData::get_file(ident_t id) const
{
  if (id == PEARL_NO_ID)
    return str_unknown;

  if (id >= m_filemap.size())
    throw RuntimeError("Oops! Invalid file name ID.");

  return m_filemap[id];
}


/*
 *---------------------------------------------------------------------------
 *
 * struct TraceCbData
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

TraceCbData::TraceCbData(const GlobalDefs& defs,
                         LocalTrace&       trace,
                         MappingTable&     table,
                         uint32_t          version,
                         uint32_t          tid,
                         int32_t           sionid)
  : m_defs(defs),
    m_trace(trace),
    m_table(table),
    m_version(version),
    m_location(PEARL_NO_ID),
    m_tid(tid),
    m_sionid(sionid),
    m_reqid(PEARL_NO_ID)
{
}


/*
 *---------------------------------------------------------------------------
 *
 * EPILOG callback functions
 *
 *---------------------------------------------------------------------------
 */


//*** Generic callbacks *****************************************************

void elg_readcb_ALL(elg_ui1 type,     /* record type */
                    elg_ui1 length,   /* record length */
                    void*   userdata)
{
  /* The type and length fields are ignored during reading. */
}


//*** Definition records ****************************************************

#define CALLBACK_SETUP \
  DefsCbData* data = static_cast<DefsCbData*>(userdata); \
  GlobalDefs& defs = data->m_defs; \
  try {

#define CALLBACK_SETUP_NO_DEFS \
  DefsCbData* data = static_cast<DefsCbData*>(userdata); \
  try {

#define CALLBACK_CLEANUP \
  } /* Closes the try block */ \
  catch (exception& ex) { \
    data->m_message = ex.what(); \
  }


void elg_readcb_STRING(elg_ui4     strid,   /* string identifier */
                       elg_ui1     cntc,    /* number of continuation records */
                       const char* str,     /* string */
                       void*       userdata)
{
  CALLBACK_SETUP_NO_DEFS

  // Sanity check
  if (strid != data->m_strmap.size())
    throw RuntimeError("Oops! Invalid string ID.");

  // Store (partial) string in callback data structure
  if (cntc == 0) {
    data->m_strmap.push_back(string(str));
  } else {
    data->m_strid  = strid;
    data->m_strcnt = cntc;
    data->m_strmap.push_back(string(str, 250));
  }

  CALLBACK_CLEANUP
}


void elg_readcb_STRING_CNT(const char* str,   /* continued string */
                           void*       userdata)
{
  CALLBACK_SETUP_NO_DEFS

  // Sanity check
  if (data->m_strcnt == 0)
    throw RuntimeError("Oops! Unexpected ELG_STRING_CNT record.");

  // Append (partial) string to existing entry in callback data structure
  data->m_strcnt--;
  if (data->m_strcnt == 0)
    data->m_strmap[data->m_strid].append(str);
  else
    data->m_strmap[data->m_strid].append(str, 255);

  CALLBACK_CLEANUP
}


void elg_readcb_MACHINE(elg_ui4 mid,     /* machine identifier */
                        elg_ui4 nodec,   /* number of nodes */
                        elg_ui4 mnid,    /* machine name identifier */
                        void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createMachine(defs,
                                         CONV_ID(mid),
                                         data->get_string(CONV_ID(mnid)),
                                         nodec);

  CALLBACK_CLEANUP
}


void elg_readcb_NODE(elg_ui4 nid,    /* node identifier */
                     elg_ui4 mid,    /* machine identifier */
                     elg_ui4 cpuc,   /* number of CPUs */
                     elg_ui4 nnid,   /* node name identifer */
                     elg_d8  cr,     /* number of clock cycles/sec */
                     void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createNode(defs,
                                      CONV_ID(nid),
                                      data->get_string(CONV_ID(nnid)),
                                      CONV_ID(mid),
                                      cpuc,
                                      cr);

  CALLBACK_CLEANUP
}


void elg_readcb_PROCESS(elg_ui4 pid,    /* process identifier */
                        elg_ui4 pnid,   /* process name identifier */
                        void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createProcess(defs,
                                         CONV_ID(pid),
                                         data->get_string(CONV_ID(pnid)));

  CALLBACK_CLEANUP
}


void elg_readcb_THREAD(elg_ui4 tid,    /* thread identifier */
                       elg_ui4 pid,    /* process identifier */
                       elg_ui4 tnid,   /* thread name identifier */
                       void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createThread(defs,
                                        CONV_ID(tid),
                                        data->get_string(CONV_ID(tnid)),
                                        CONV_ID(pid));

  CALLBACK_CLEANUP
}


void elg_readcb_LOCATION(elg_ui4 lid,   /* location identifier */
                         elg_ui4 mid,   /* machine identifier */
                         elg_ui4 nid,   /* node identifier */
                         elg_ui4 pid,   /* process identifier */
                         elg_ui4 tid,   /* thread identifier */
                         void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createLocation(defs,
                                          CONV_ID(lid),
                                          CONV_ID(mid),
                                          CONV_ID(nid),
                                          CONV_ID(pid),
                                          CONV_ID(tid));

  CALLBACK_CLEANUP
}


void elg_readcb_FILE(elg_ui4 fid,    /* file identifier */
                     elg_ui4 fnid,   /* file name identifier */
                     void*   userdata)
{
  CALLBACK_SETUP_NO_DEFS

  // Sanity check
  if (fid != data->m_filemap.size())
    throw RuntimeError("Oops! Invalid file name ID.");

  // Store file name in callback data structure
  data->m_filemap.push_back(data->get_string(CONV_ID(fnid)));

  CALLBACK_CLEANUP
}


void elg_readcb_REGION(elg_ui4 rid,     /* region identifier */
                       elg_ui4 rnid,    /* region name identifier */
                       elg_ui4 fid,     /* source file identifier */
                       elg_ui4 begln,   /* begin line number */
                       elg_ui4 endln,   /* end line number */
                       elg_ui4 rdid,    /* region description identifier */
                       elg_ui1 rtype,   /* region type */
                       void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createRegion(defs,
                                        CONV_ID(rid),
                                        data->get_string(CONV_ID(rnid)),
                                        data->get_file(CONV_ID(fid)),
                                        CONV_NUM(begln),
                                        CONV_NUM(endln),
                                        data->get_string(CONV_ID(rdid)),
                                        rtype);

  CALLBACK_CLEANUP
}


void elg_readcb_CALL_SITE(elg_ui4 csid,   /* call site identifier */
                          elg_ui4 fid,    /* source file identifier */
                          elg_ui4 lno,    /* line number */
                          elg_ui4 erid,   /* region identifer to be entered */
                          elg_ui4 lrid,   /* region identifer to be left */
                          void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createCallsite(defs,
                                          CONV_ID(csid),
                                          data->get_file(CONV_ID(fid)),
                                          CONV_NUM(lno),
                                          CONV_ID(erid));

  CALLBACK_CLEANUP
}


void elg_readcb_CALL_PATH(elg_ui4 cpid,   /* call-path identifier */
                          elg_ui4 rid,    /* node region identifier */
                          elg_ui4 ppid,   /* parent call-path identifier */
                          elg_ui8 order,  /* node order specifier */
                          void*   userdata)
{
  CALLBACK_SETUP

  union {
    elg_ui8 dummy;
    double  time;
  } convert = { order };

  DefsFactory::instance()->createCallpath(defs,
                                          CONV_ID(cpid),
                                          CONV_ID(rid),
                                          PEARL_NO_ID,
                                          CONV_ID(ppid),
                                          convert.time);

  CALLBACK_CLEANUP
}


void elg_readcb_METRIC(elg_ui4 metid,      /* metric identifier */
                       elg_ui4 metnid,     /* metric name identifier */
                       elg_ui4 metdid,     /* metric description identifier */
                       elg_ui1 metdtype,   /* metric data type */
                       elg_ui1 metmode,    /* metric mode */
                       elg_ui1 metiv,      /* time interval referenced */
                       void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createMetric(defs,
                                        CONV_ID(metid),
                                        data->get_string(CONV_ID(metnid)),
                                        data->get_string(CONV_ID(metdid)),
                                        metdtype,
                                        metmode,
                                        metiv);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_GROUP(elg_ui4 gid,         /* group identifier */
                          elg_ui1 mode,        /* mode flags */
                          elg_ui4 grpc,        /* number of ranks in group */
                          elg_ui4 grpv[],      /* local |-> global rank mapping */
                          void*   userdata)
{
  CALLBACK_SETUP

  // Set up process identifier list in reverse order
  // (to minimize #exceptions since the first group is in most cases
  //  COMM_WORLD, listing all ranks in ascending order and
  //  GlobalDefs::add_process() creates dummy process objects up to
  //  the given process ID)
  DefsFactory::process_group process_ids;
  process_ids.reserve(grpc);
  for (elg_ui4 i = grpc; i > 0; --i) {
    elg_ui4 pid = grpv[i - 1];

    // Create corresponding process object
    try {
      defs.get_process(pid);
    }
    catch (RuntimeError&) {
      DefsFactory::instance()->createProcess(defs, pid, "");
    }

    // Store global process identifier
    process_ids.push_back(pid);
  }
  reverse(process_ids.begin(), process_ids.end());

  // Create MPI group record
  DefsFactory::instance()->createMpiGroup(defs,
                                          CONV_ID(gid),
                                          process_ids,
                                          mode & ELG_GROUP_SELF,
                                          mode & ELG_GROUP_WORLD);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_COMM_DIST(elg_ui4 cid,    /* communicator id unique to root */
                              elg_ui4 root,   /* global rank of root process */
                              elg_ui4 lcid,   /* local communicator id on process */
                              elg_ui4 lrank,  /* local rank of process */
                              elg_ui4 size,   /* size of communicator */
                              void*   userdata)
{
  DefsCbData* data = static_cast<DefsCbData*>(userdata);

  data->m_message = "Unexpected ELG_MPI_COMM_DIST record!";
}


void elg_readcb_MPI_COMM_REF(elg_ui4 cid,      /* communicator identifier */
                             elg_ui4 gid,      /* group identifier */
                             void*   userdata)
{
  CALLBACK_SETUP

  // Create MPI communicator record
  DefsFactory::instance()->createMpiComm(defs,
                                         CONV_ID(cid),
                                         CONV_ID(gid));

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_COMM(elg_ui4  cid,    /* communicator identifier */
                         elg_ui1  mode,   /* mode flags */
                         elg_ui4  grpc,   /* size of bitstring in bytes */
                         elg_ui1* grpv,   /* bitstring defining the group */
                         void*    userdata)
{
  CALLBACK_SETUP

  // Set up process identifier list in reverse order
  // (to minimize #exceptions since GlobalDefs::add_process() creates
  //  dummy process objects up to the given process ID)
  DefsFactory::process_group process_ids;
  process_ids.reserve(grpc * 8);
  for (uint32_t i = grpc; i > 0; --i) {
    uint32_t byte  = i - 1;
    uint8_t  value = grpv[byte];

    for (int bit = 7; bit >= 0; --bit) {
      if (value & (1 << bit)) {
        uint32_t pid = byte * 8 + bit;

        // Create corresponding process object
        try {
          defs.get_process(pid);
        }
        catch (RuntimeError&) {
          DefsFactory::instance()->createProcess(defs, pid, "");
        }

        // Store global process identifier
        process_ids.push_back(pid);
      }
    }
  }
  reverse(process_ids.begin(), process_ids.end());

  // MPI_COMM_WORLD is always defined by communicator with global ID 0
  if (0 == cid)
    data->m_worldsize = process_ids.size();

  // Determine WORLD flag
  bool is_world = (data->m_worldsize == process_ids.size());

  // Create MPI group record
  DefsFactory::instance()->createMpiGroup(defs,
                                          CONV_ID(cid),
                                          process_ids,
                                          mode & ELG_GROUP_SELF,
                                          is_world);

  // Create MPI communicator record
  DefsFactory::instance()->createMpiComm(defs,
                                         CONV_ID(cid),
                                         CONV_ID(cid));

  CALLBACK_CLEANUP
}


void elg_readcb_CART_TOPOLOGY(elg_ui4  topid,     /* topology id */
                              elg_ui4  tnid,      /* topology name id */
                              elg_ui4  cid,       /* communicator id */
                              elg_ui1  ndims,     /* number of dimensions */
                              elg_ui4* dimv,      /* number of processes in each dim */  
                              elg_ui1* periodv,   /* periodicity in each dim */
                              elg_ui4* dimids,    /* string ids of dimension names */
                              void*    userdata)
{
  CALLBACK_SETUP

  // Set up dimension vector
  Cartesian::cart_dims dimensions;
  dimensions.reserve(ndims);
  for (elg_ui1 i = 0; i < ndims; ++i)
    dimensions.push_back(dimv[i]);

  // Set up periodicity vector
  Cartesian::cart_period periodic;
  periodic.reserve(ndims);
  for (elg_ui1 i = 0; i < ndims; ++i)
    periodic.push_back(periodv[i]);

  Cartesian::cart_dim_names dim_names;
  dim_names.reserve(ndims);
  for (int i = 0; i < ndims; ++i) {
      dim_names.push_back(data->get_string(CONV_ID(dimids[i])));
  }

  string topo_name=data->get_string(CONV_ID(tnid));
  //	 Create new cartesian grid
  if (cid != ELG_NO_ID) {
    DefsFactory::instance()->createMpiCartesian(defs,
                                                CONV_ID(topid),
                                                dimensions,
                                                periodic,
                                                CONV_ID(cid),
                                                topo_name,
                                                dim_names
                                                );
  } else {
    DefsFactory::instance()->createCartesian(defs,
                                             CONV_ID(topid),
                                             dimensions,
                                             periodic,
                                             topo_name,
                                             dim_names);
  }

  CALLBACK_CLEANUP
}


void elg_readcb_CART_COORDS(elg_ui4  topid,    /* topology id */
                            elg_ui4  lid,      /* location id */
                            elg_ui1  ndims,    /* number of dimensions */
                            elg_ui4* coordv,   /* coordinates in each dimension */
                            void*    userdata)
{
  CALLBACK_SETUP

  // Retrieve cartesian topology
  Cartesian* cart = defs.get_cartesian(CONV_ID(topid));

  // Set up coordinate vector
  Cartesian::cart_coords coordinate;
  coordinate.reserve(ndims);
  for (elg_ui1 i = 0; i < ndims; ++i)
    coordinate.push_back(coordv[i]);

  // Set coordinate
  cart->set_coords(defs.get_location(CONV_ID(lid)), coordinate);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_WIN(elg_ui4 wid,   /* window identifier */
                        elg_ui4 cid,   /* communicator identifier */
                        void*   userdata)
{
  CALLBACK_SETUP

  DefsFactory::instance()->createMpiWindow(defs,
                                           CONV_ID(wid),
                                           CONV_ID(cid));

  CALLBACK_CLEANUP
}


void elg_readcb_MAP_OFFSET(elg_ui4 rank,   /* rank number the offset relates to */
                           elg_ui4 offset, /* file offset in mapping file */
                           void* userdata )
{
  CALLBACK_SETUP

  defs.set_offset(rank, offset);

  CALLBACK_CLEANUP
}

#undef CALLBACK_SETUP
#undef CALLBACK_CLEANUP


//*** Mapping file records **************************************************

#define CALLBACK_SETUP \
  MappingTable& table = *static_cast<MappingTable*>(userdata); \
  try {

#define CALLBACK_CLEANUP \
  } /* Closes the try block */ \
  catch (exception& ex) { \
    table.set_message(ex.what()); \
  }


void elg_readcb_MAP_SECTION(elg_ui4 rank,   /* rank number the section relates to */
                            void* userdata)
{
  CALLBACK_SETUP

  table.set_rank(rank);
  table.set_finished();

  CALLBACK_CLEANUP
}


void elg_readcb_OFFSET(elg_d8 ltime,    /* local time */
                       elg_d8 offset,   /* offset to global time */
                       void*  userdata)
{
  CALLBACK_SETUP

  table.set_time(ltime, offset);

  CALLBACK_CLEANUP
}


void elg_readcb_IDMAP(elg_ui1 type,     /* object type to be mapped */
                      elg_ui1 mode,     /* mapping mode (dense/sparse) */
                      elg_ui4 count,    /* number of entries */
                      elg_ui4 mapv[],   /* vector of mappings */
                      void*   userdata)
{
  CALLBACK_SETUP

  table.set_table(type, mode, count, mapv);

  CALLBACK_CLEANUP
}

#undef CALLBACK_SETUP
#undef CALLBACK_CLEANUP


//--- Obsolete definition records -------------------------------------------

void elg_readcb_LAST_DEF(void* userdata)
{
}


void elg_readcb_NUM_EVENTS(elg_ui4 eventc,   /* number of events */
                           void*   userdata)
{
}


void elg_readcb_EVENT_TYPES(elg_ui4 ntypes,     /* number of event types */
                            elg_ui1 typev[],    /* vector of event types */
                            void*   userdata)
{
}


void elg_readcb_EVENT_COUNTS(elg_ui4 ntypes,     /* number of event types */
                             elg_ui4 countv[],   /* vector of event counts */
                             void*   userdata)
{
}


//*** Event records *********************************************************

// Set up shortcuts for commonly used fields in userdata and
// ignore events from other locations
#define CALLBACK_SETUP \
  TraceCbData*      data  = static_cast<TraceCbData*>(userdata); \
  const GlobalDefs& defs  = data->m_defs; \
  LocalTrace&       trace = data->m_trace; \
  MappingTable&     table = data->m_table; \
  \
  try { \
    ident_t mapped_lid; \
    if (data->m_sionid >= 0) { \
      mapped_lid = table.map_location(data->m_tid); \
    } else {				      \
      mapped_lid = table.map_location(lid); \
    } 		    		   \
    if (data->m_location == PEARL_NO_ID) { \
      if (defs.get_location(mapped_lid)->get_thread()->get_id() == data->m_tid) \
        data->m_location = mapped_lid; \
      else {                           \
        data->m_reqid = PEARL_NO_ID;   \
        return; \
      } \
    } else if (data->m_location != mapped_lid) { \
      data->m_reqid = PEARL_NO_ID; \
      return; \
    }

#define CALLBACK_CLEANUP \
    data->m_reqid = PEARL_NO_ID; \
  } /* Closes the try block */ \
  catch (exception& ex) { \
    data->m_message = ex.what(); \
  }


#define ATTR_CALLBACK_SETUP \
  TraceCbData*      data  = static_cast<TraceCbData*>(userdata);

//--- Attributes ------------------------------------------------------------

void elg_readcb_ATTR_UI1(elg_ui1 type, elg_ui1 val, void* userdata)
{
}

void elg_readcb_ATTR_UI4(elg_ui1 type, elg_ui4 val, void* userdata)
{
  ATTR_CALLBACK_SETUP

  switch (type) {
  case ELG_ATTR_REQUEST:
      data->m_reqid = val;
      break;

  default:
      break;
  }
}

//--- Programming-model-independent events ----------------------------------

void elg_readcb_ENTER(elg_ui4  lid,    /* location identifier */
                      elg_d8   time,   /* time stamp */
                      elg_ui4  rid,    /* region identifier of the region being entered */
                      elg_ui1  metc,   /* number of metrics */
                      elg_ui8* metv,   /* metric values */
                      void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createEnter(defs,
                                          table.map_time(time),
                                          table.map_region(CONV_ID(rid)),
                                          metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_EXIT(elg_ui4  lid,    /* location identifier */
                     elg_d8   time,   /* time stamp */
                     elg_ui1  metc,   /* number of metrics */
                     elg_ui8* metv,   /* metric values */
                     void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createExit(defs,
                                         table.map_time(time),
                                         metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_ENTER_CS(elg_ui4  lid,    /* location identifier */
                         elg_d8   time,   /* time stamp */
                         elg_ui4  csid,   /* call site identifier of the call site being entered */
                         elg_ui1  metc,   /* number of metrics */
                         elg_ui8* metv,   /* metric values */
                         void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createEnterCS(defs,
                                            table.map_time(time),
                                            table.map_callsite(CONV_ID(csid)),
                                            metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


//--- MPI-1 event records ---------------------------------------------------

void elg_readcb_MPI_SEND(elg_ui4 lid,    /* location identifier */
                         elg_d8  time,   /* time stamp */
                         elg_ui4 dlid,   /* destination location identifier */
                         elg_ui4 cid,    /* communicator identifier */
                         elg_ui4 tag,    /* message tag */
                         elg_ui4 sent,   /* message length in bytes */
                         void*   userdata)
{
  CALLBACK_SETUP

  // Map communicator ID
  ident_t comm_id = table.map_communicator(CONV_ID(cid));

  // Pre-1.8 traces: convert global |-> local destination rank
  if (data->m_version < 1008)
    dlid = defs.get_comm(comm_id)->get_group()->get_local_rank(dlid);


  Event_rep* event;
  if (data->m_reqid == PEARL_NO_ID) {
    event =
      EventFactory::instance()->createMpiSend(defs,
                                              table.map_time(time),
                                              comm_id,
                                              dlid,
                                              tag,
                                              sent);
  } else {
    event =
      EventFactory::instance()->createMpiSendRequest(defs,
                                                     table.map_time(time),
                                                     comm_id,
                                                     dlid,
                                                     tag,
                                                     sent,
                                                     data->m_reqid);
  }

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_RECV(elg_ui4 lid,    /* location identifier */
                         elg_d8  time,   /* time stamp */
                         elg_ui4 slid,   /* source location identifier */
                         elg_ui4 cid,    /* communicator identifier */
                         elg_ui4 tag,    /* message tag */
                         void*   userdata)
{
  CALLBACK_SETUP

  // Map communicator ID
  ident_t comm_id = table.map_communicator(CONV_ID(cid));

  // Pre-1.8 traces: convert global |-> local source rank
  if (data->m_version < 1008)
    slid = defs.get_comm(comm_id)->get_group()->get_local_rank(slid);

  Event_rep* event;
  if (data->m_reqid == PEARL_NO_ID) {
    event =
      EventFactory::instance()->createMpiRecv(defs,
                                              table.map_time(time),
                                              comm_id,
                                              slid,
                                              tag);
  } else {
    event =
      EventFactory::instance()->createMpiRecvComplete(defs,
                                                      table.map_time(time),
                                                      comm_id,
                                                      slid,
                                                      tag,
                                                      data->m_reqid);
  }

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_COLLEXIT(elg_ui4  lid,      /* location identifier */
                             elg_d8   time,     /* time stamp */
                             elg_ui1  metc,     /* number of metrics */
                             elg_ui8* metv,     /* metric values */
                             elg_ui4  rlid,     /* root location identifier */
                             elg_ui4  cid,      /* communicator identifier */
                             elg_ui4  sent,     /* bytes sent */
                             elg_ui4  recvd,    /* bytes received */
                             void*    userdata)
{
  CALLBACK_SETUP

  // Map communicator ID
  ident_t comm_id = table.map_communicator(CONV_ID(cid));

  // Pre-1.8 traces: convert global |-> local root rank
  if (rlid != ELG_NO_ID && data->m_version < 1008)
    rlid = defs.get_comm(comm_id)->get_group()->get_local_rank(rlid);

  Event_rep* event =
    EventFactory::instance()->createMpiCollExit(defs,
                                                table.map_time(time),
                                                comm_id,
                                                (rlid == ELG_NO_ID)
                                                  ? PEARL_NO_ID
                                                  : rlid,
                                                sent,
                                                recvd,
                                                metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}

//--- MPI-1 non-blocking event records --------------------------------------

void elg_readcb_MPI_SEND_COMPLETE(elg_ui4 lid,       /* location identifier              */
                                  elg_d8  time,      /* time stamp */
                                  elg_ui4 reqid,     /* request identifier of completed send */
                                  void* userdata)
{
  CALLBACK_SETUP

  assert(reqid != PEARL_NO_ID);

  Event_rep* event =
    EventFactory::instance()->createMpiSendComplete(defs, 
                                                    table.map_time(time), 
                                                    reqid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid,       /* location identifier              */
                                 elg_d8  time,      /* time stamp */
                                 elg_ui4 reqid,     /* receive request identifier       */
                                 void* userdata)
{
  CALLBACK_SETUP

  assert(reqid != PEARL_NO_ID);

  Event_rep* event = 
    EventFactory::instance()->createMpiRecvRequest(defs, 
                                                   table.map_time(time),
                                                   reqid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid,       /* location identifier              */
                                   elg_d8  time,      /* time stamp */
                                   elg_ui4 reqid,     /* receive request identifier       */
                                   void*   userdata)
{
  CALLBACK_SETUP

  assert(reqid != PEARL_NO_ID);

  Event_rep* event =
    EventFactory::instance()->createMpiRequestTested(defs, 
                                                     table.map_time(time),
                                                     reqid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_CANCELLED(elg_ui4 lid,       /* location identifier              */
                              elg_d8  time,      /* time stamp */
                              elg_ui4 reqid,     /* receive request identifier       */
                              void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createMpiCancelled(defs, 
                                                 table.map_time(time),
                                                 reqid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


//--- MPI-2 one-sided event records -----------------------------------------

void elg_readcb_MPI_PUT_1TS(elg_ui4 lid,      /* location identifier */
                            elg_d8  time,     /* time stamp */
                            elg_ui4 tlid,     /* target location identifier */
                            elg_ui4 wid,      /* window identifier */
                            elg_ui4 rmaid,    /* RMA operation identifier */
                            elg_ui4 nbytes,   /* message length in bytes */
                            void*   userdata)
{
  CALLBACK_SETUP

  // Map window ID
  ident_t win_id = table.map_window(CONV_ID(wid));

  // Pre-1.8 traces: convert global |-> local target rank
  if (data->m_version < 1008)
    tlid = defs.get_window(win_id)->get_comm()->get_group()
                                  ->get_local_rank(CONV_ID(tlid));

  Event_rep* event =
    EventFactory::instance()->createMpiRmaPutStart(defs,
                                                   table.map_time(time),
                                                   rmaid,
                                                   tlid,
                                                   nbytes,
                                                   win_id);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_PUT_1TE(elg_ui4 lid,     /* location identifier */
                            elg_d8  time,    /* time stamp */
                            elg_ui4 dlid,    /* destination location identifier */
                            elg_ui4 wid,     /* window identifier */
                            elg_ui4 rmaid,   /* RMA operation identifier */
                            void*   userdata)
{
  /* NOTE: As we are no longer rewriting the trace, these events will
   *       never exist in the remote trace. We work directly on the
   *       _REMOTE events. Eventually, these event will deprecate. */
}


void elg_readcb_MPI_PUT_1TE_REMOTE(elg_ui4 lid,     /* location identifier */
                                   elg_d8  time,    /* time stamp */
                                   elg_ui4 dlid,    /* destination location identifier  */
                                   elg_ui4 wid,     /* window identifier */
                                   elg_ui4 rmaid,   /* RMA operation identifier */
                                   void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createMpiRmaPutEnd(defs,
                                                 table.map_time(time),
                                                 rmaid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_GET_1TS(elg_ui4 lid,      /* location identifier */
                            elg_d8  time,     /* time stamp */
                            elg_ui4 dlid,     /* destination location identifier */
                            elg_ui4 wid,      /* window identifier */
                            elg_ui4 rmaid,    /* RMA operation identifier */
                            elg_ui4 nbytes,   /* message length in bytes */
                            void*   userdata)
{
  /* NOTE: As we are no longer rewriting the trace, these events will
   *       never exist in the remote trace. We work directly on the
   *       _REMOTE events. Eventually, these event will deprecate. */
}


void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid,      /* location identifier */
                                   elg_d8  time,     /* time stamp */
                                   elg_ui4 dlid,     /* destination location identifier */
                                   elg_ui4 wid,      /* window identifier */
                                   elg_ui4 rmaid,    /* RMA operation identifier */
                                   elg_ui4 nbytes,   /* message length in bytes */
                                   void*   userdata)
{
  CALLBACK_SETUP

  // Map window ID
  ident_t win_id = table.map_window(CONV_ID(wid));

  // Pre-1.8 traces: convert global |-> local origin rank
  if (data->m_version < 1008)
    dlid = defs.get_window(win_id)->get_comm()->get_group()
                                  ->get_local_rank(CONV_ID(dlid));

  Event_rep* event =
    EventFactory::instance()->createMpiRmaGetStart(defs,
                                                   table.map_time(time),
                                                   rmaid,
                                                   dlid,
                                                   nbytes,
                                                   win_id);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_GET_1TE(elg_ui4 lid,     /* location identifier */
                            elg_d8  time,    /* time stamp */
                            elg_ui4 slid,    /* source location identifier */
                            elg_ui4 wid,     /* window identifier */
                            elg_ui4 rmaid,   /* RMA operation identifier */
                            void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createMpiRmaGetEnd(defs,
                                                 table.map_time(time),
                                                 rmaid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_GET_1TO(elg_ui4 lid,     /* location identifier */
                            elg_d8  time,    /* time stamp */
                            elg_ui4 rmaid,   /* RMA operation identifier */
                            void*   userdata)
{
  /* NOTE: These events are currently completely ignored, as they are
   *       not used for analysis, but only for visualisation. */
}


void elg_readcb_MPI_WINEXIT(elg_ui4  lid,     /* location identifier */
                            elg_d8   time,    /* time stamp */
                            elg_ui1  metc,    /* number of metrics */
                            elg_ui8* metv,    /* metric values */
                            elg_ui4  wid,     /* window identifier */
                            elg_ui4  cid,     /* communicator identifier */
                            elg_ui1  synex,   /* synchronization exit flag */
                            void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createMpiRmaExit(defs,
                                               table.map_time(time),
                                               table.map_window(CONV_ID(wid)),
                                               (data->m_version < 1008)
                                                 ? table.map_communicator(CONV_ID(cid))
                                                 : table.map_group(CONV_ID(cid)),
                                               synex,
                                               metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_WINCOLLEXIT(elg_ui4  lid,    /* location identifier */
                                elg_d8   time,   /* time stamp */
                                elg_ui1  metc,   /* number of metrics */
                                elg_ui8* metv,   /* metric values */
                                elg_ui4  wid,    /* window identifier */
                                void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createMpiRmaCollExit(defs,
                                                   table.map_time(time),
                                                   table.map_window(CONV_ID(wid)),
                                                   metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_WIN_LOCK(elg_ui4 lid,     /* location identifier */
                             elg_d8  time,    /* time stamp */
                             elg_ui4 llid,    /* lock location identifier */
                             elg_ui4 wid,     /* window identifier */
                             elg_ui1 ltype,   /* lock type */
                             void*   userdata)
{
  CALLBACK_SETUP

  // Map window ID
  ident_t win_id = table.map_window(CONV_ID(wid));

  // Pre-1.8 traces: convert global |-> local remote rank
  if (data->m_version < 1008)
    llid = defs.get_window(win_id)->get_comm()->get_group()
                                  ->get_local_rank(CONV_ID(llid));

  Event_rep* event =
    EventFactory::instance()->createMpiRmaLock(defs,
                                               table.map_time(time),
                                               llid,
                                               win_id,
                                               ltype);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid,    /* location identifier */
                               elg_d8  time,   /* time stamp */
                               elg_ui4 llid,   /* lock location identifier */
                               elg_ui4 wid,    /* window identifier */
                               void*   userdata)
{
  CALLBACK_SETUP

  // Map window ID
  ident_t win_id = table.map_window(CONV_ID(wid));

  // Pre-1.8 traces: convert global |-> local remote rank
  if (data->m_version < 1008)
    llid = defs.get_window(win_id)->get_comm()->get_group()
                                  ->get_local_rank(CONV_ID(llid));

  Event_rep* event =
    EventFactory::instance()->createMpiRmaUnlock(defs,
                                                 table.map_time(time),
                                                 llid,
                                                 win_id);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


//--- Generic one-sided event records ---------------------------------------

void elg_readcb_PUT_1TS(elg_ui4 lid,      /* location identifier */
                        elg_d8  time,     /* time stamp */
                        elg_ui4 dlid,     /* destination location identifier */
                        elg_ui4 rmaid,    /* RMA operation identifier */
                        elg_ui4 nbytes,   /* message length in bytes */
                        void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createRmaPutStart(defs,
                                                table.map_time(time),
                                                rmaid,
                                                dlid,
                                                nbytes);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_PUT_1TE(elg_ui4 lid,     /* location identifier */
                        elg_d8  time,    /* time stamp */
                        elg_ui4 slid,    /* source location identifier */
                        elg_ui4 rmaid,   /* RMA operation identifier */
                        void*   userdata)
{
  /* NOTE: As we are no longer rewriting the trace, these events will
   *       never exist in the remote trace. We work directly on the
   *       _REMOTE events. Eventually, these event will deprecate. */
}


void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid,     /* location identifier */
                               elg_d8  time,    /* time stamp */
                               elg_ui4 dlid,    /* destination location identifier */
                               elg_ui4 rmaid,   /* RMA operation identifier */
                               void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createRmaPutEnd(defs,
                                              table.map_time(time),
                                              rmaid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_GET_1TS(elg_ui4 lid,      /* location identifier */
                        elg_d8  time,     /* time stamp */
                        elg_ui4 tlid,     /* target location identifier */
                        elg_ui4 rmaid,    /* RMA operation identifier */
                        elg_ui4 nbytes,   /* message length in bytes */
                        void*   userdata)
{
  /* NOTE: As we are no longer rewriting the trace, these events will
   *       never exist in the remote trace. We work directly on the
   *       _REMOTE events. Eventually, these event will deprecate.*/
}


void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid,      /* location identifier */
                               elg_d8  time,     /* time stamp */
                               elg_ui4 tlid,     /* target location identifier */
                               elg_ui4 rmaid,    /* RMA operation identifier */
                               elg_ui4 nbytes,   /* message length in bytes */
                               void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createRmaGetStart(defs,
                                                table.map_time(time),
                                                rmaid,
                                                tlid,
                                                nbytes);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_GET_1TE(elg_ui4 lid,     /* location identifier */
                        elg_d8  time,    /* time stamp */
                        elg_ui4 slid,    /* source location identifier */
                        elg_ui4 rmaid,   /* RMA operation identifier */
                        void*   userdata)
{
  /* NOTE: In pearl most of the data provided in the event can be
   *       obtained with a reference to the Start event, using the
   *       rma_id. */
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createRmaGetEnd(defs,
                                              table.map_time(time),
                                              rmaid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_COLLEXIT(elg_ui4  lid,     /* location identifier */
                         elg_d8   time,    /* time stamp */
                         elg_ui1  metc,    /* number of metrics */
                         elg_ui8* metv,    /* metric values */
                         elg_ui4  rlid,    /* root location identifier */
                         elg_ui4  cid,     /* communicator identifier */
                         elg_ui4  sent,    /* bytes sent */
                         elg_ui4  recvd,   /* bytes received */
                         void*    userdata)
{
  /*
   * One-sided communication is currently ignored. However, EXIT events
   * need to be handled correctly.
   */

  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createExit(defs,
                                         table.map_time(time),
                                         metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_ALOCK(elg_ui4 lid,    /* location identifier */
                      elg_d8  time,   /* time stamp */
                      elg_ui4 lkid,   /* identifier of the lock being aquired */
                      void*   userdata)
{
  /* One-sided communication is currently ignored. */
}


void elg_readcb_RLOCK(elg_ui4 lid,    /* location identifier */
                      elg_d8  time,   /* time stamp */
                      elg_ui4 lkid,   /* identifier of the lock being aquired */
                      void*   userdata)
{
  /* One-sided communication is currently ignored. */
}


//--- OpenMP event records --------------------------------------------------

void elg_readcb_OMP_FORK(elg_ui4 lid,    /* location identifier */
                         elg_d8  time,   /* time stamp */
                         void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createOmpFork(defs,
                                            table.map_time(time));

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_OMP_JOIN(elg_ui4 lid,    /* location identifier */
                         elg_d8  time,   /* time stamp */
                         void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createOmpJoin(defs,
                                            table.map_time(time));

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_OMP_ALOCK(elg_ui4 lid,    /* location identifier */
                          elg_d8  time,   /* time stamp */
                          elg_ui4 lkid,   /* identifier of the lock being acquired */
                          void*   userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createOmpALock(defs,
                                             table.map_time(time),
                                             lkid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_OMP_RLOCK(elg_ui4 lid,    /* location identifier */
                          elg_d8  time,   /* time stamp */
                          elg_ui4 lkid,   /* identifier of the lock being acquired */
                          void*   userdata)
{  
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createOmpRLock(defs,
                                             table.map_time(time),
                                             lkid);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_OMP_COLLEXIT(elg_ui4  lid,    /* location identifier */
                             elg_d8   time,   /* time stamp */
                             elg_ui1  metc,   /* number of metrics */
                             elg_ui8* metv,   /* metric values */
                             void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createOmpCollExit(defs,
                                                table.map_time(time),
                                                metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


//--- Other event records ---------------------------------------------------

void elg_readcb_LOG_OFF(elg_ui4  lid,    /* location identifier */
                        elg_d8   time,   /* time stamp */
                        elg_ui1  metc,   /* number of metrics */
                        elg_ui8* metv,   /* metric values */
                        void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createEnter(defs,
                                          table.map_time(time),
                                          defs.get_region_nolog(),
                                          metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_LOG_ON(elg_ui4  lid,    /* location identifier */
                       elg_d8   time,   /* time stamp */
                       elg_ui1  metc,   /* number of metrics */
                       elg_ui8* metv,   /* metric values */
                       void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createExit(defs,
                                         table.map_time(time),
                                         metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_ENTER_TRACING(elg_ui4  lid,    /* location identifier */
                              elg_d8   time,   /* time stamp */
                              elg_ui1  metc,   /* number of metrics */
                              elg_ui8* metv,   /* metric values */
                              void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createEnter(defs,
                                          table.map_time(time),
                                          defs.get_region_trace(),
                                          metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}


void elg_readcb_EXIT_TRACING(elg_ui4  lid,    /* location identifier */
                             elg_d8   time,   /* time stamp */
                             elg_ui1  metc,   /* number of metrics */
                             elg_ui8* metv,   /* metric values */
                             void*    userdata)
{
  CALLBACK_SETUP

  Event_rep* event =
    EventFactory::instance()->createExit(defs,
                                         table.map_time(time),
                                         metv);

  if (event)
    trace.add_event(event);

  CALLBACK_CLEANUP
}

#undef CALLBACK_SETUP
#undef CALLBACK_CLEANUP
