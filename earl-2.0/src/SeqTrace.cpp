/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <cstring> 
#include <algorithm>
#include <vector>

#include "OMPAlock_rep.h"
#include "Alock_rep.h"
#include "SeqTrace.h"
#include "Communicator.h"
#include "CollExit_rep.h"
#include "Error.h"
#include "Enter_rep.h"
#include "Exit_rep.h"
#include "Fork_rep.h"
#include "Get1TE_rep.h"
#include "Get1TS_rep.h"
#include "Join_rep.h"
#include "Location.h"
#include "Machine.h"
#include "MPICExit_rep.h"
#include "MPIGet1TE_rep.h"
#include "MPIGet1TO_rep.h"
#include "MPIGet1TS_rep.h"
#include "MPIPut1TE_rep.h"
#include "MPIPut1TS_rep.h"
#include "MPIRma_rep.h"
#include "MPIRmaComm_rep.h"
#include "MPIRmaSync_rep.h"
#include "MPIWCExit_rep.h"
#include "MPIWExit_rep.h"
#include "MPIWLock_rep.h"
#include "MPIWUnlock_rep.h"
#include "MPIWindow.h"
#include "Node.h"
#include "OMPCExit_rep.h"
#include "OMPRlock_rep.h"
#include "Process.h"
#include "Recv_rep.h"
#include "Rlock_rep.h"
#include "Put1TE_rep.h"
#include "Put1TS_rep.h"
#include "Send_rep.h"
#include "Thread.h"
#include "Cartesian.h"

#include "Region.h"
#include "NamedObj.h"

using namespace earl;
using namespace std;



SeqTrace::SeqTrace(string path) : grid(new Grid()), lastdef(false)
{ 
  if (!(file = elg_read_open(path.c_str())))
    throw RuntimeError(string("Cannot open EPILOG trace file \"") + path + "\".");

  // Process every definition record of the trace 
  // Calls read callback functions above     
  while ( !lastdef ) 
    elg_read_next (file, this);

  // create pseudo file
  filem[filem.size()] = EARL_REGION_GENERATOR;

  // create regions for tracing and no_logging
  tracing = new Region(regm.size(), EARL_REGION_TRACING, EARL_REGION_GENERATOR, -1, -1, "Larger trace-generation task.", EARL_REGION_GENERATOR);
  regm[regm.size()] = tracing;

  no_logging = new Region(regm.size(), EARL_REGION_NO_LOGGING, EARL_REGION_GENERATOR, -1, -1, "Event logging toggled.", EARL_REGION_GENERATOR);
  regm[regm.size()] = no_logging;
}


SeqTrace::~SeqTrace() 
{ 
  elg_read_close(file);

  map<long, Region*>::const_iterator rit = regm.begin();
  while (rit != regm.end())
    {
      delete rit->second;
      rit++;
    }
  map<long, Callsite*>::const_iterator csit = csitem.begin();
  while (csit != csitem.end())
    {
      delete csit->second;
      csit++;
    }
  map<long, Metric*>::const_iterator metit = metm.begin();
  while (metit != metm.end())
    {
      delete metit->second;
      metit++;
    }
  delete grid;
}

Event 
SeqTrace::next_event(State& state, EventBuffer* buffer) 
{ 
  Event event;

  /* read buffered event if exists*/
  event = buffer->retrieve(state.get_pos() + 1);
  if ( !event.null() )
    {
      elg_read_skip_record(file);
      event.trans(state);
      return event;
    }
 
      
  /* data requiered in read call back funktions */ 
  cur_state  = &state; /* [in]  */
  cur_buffer = buffer; /* [in]  */
  cur_event  = NULL;   /* [out] */
  
  /* read next event and do callbacks */
  /* pass event descriptor to callback functions*/
  if ( !elg_read_next_event(file, this) )
    throw FatalError("Error in SeqTrace::next_event().");
  

  /* update state using event transition */
  if ( cur_event.null() ) {
    string msg = this->err_msg;
    this->err_msg = ""; // cleanup error message
    throw FatalError("Error in SeqTrace::next_event(): failed to create an event object. Reason:\n"+msg);
  }
  cur_event.trans(state);
  return cur_event;
}

void       
SeqTrace::set_offset(const fpos_t* offset) 
{ 
    elg_read_setpos(file,offset);
}
 
fpos_t
SeqTrace::get_offset() 
{ 
  fpos_t offset;

  elg_read_getpos(file, &offset);
  return offset;
}
  
string   
SeqTrace::get_str(long strid) const
{
  map<long, string>::const_iterator it = strm.find(strid);
  
  if ( strid == -1 )
    return "UNKNOWN";

  if (it == strm.end())
    throw RuntimeError("Error in SeqTrace::get_str().");
  else
    return it->second;
}

string   
SeqTrace::get_file(long file_id) const 
{ 
  map<long, string>::const_iterator it = filem.find(file_id);

  if ( file_id == -1 )
    return "UNKNOWN";

  if (it == filem.end())
    throw RuntimeError("Error in SeqTrace::get_file().");
  else
    return it->second;
}
  
Region*       
SeqTrace::get_reg(long reg_id) const 
{ 
  RegionMap::const_iterator it = regm.find(reg_id);

  if (it == regm.end())
    throw RuntimeError("Error in SeqTrace::get_reg(id).");
  else
    return it->second;
}

Region*       
SeqTrace::get_reg(const string& name) const 
{ 
  RegionMap::const_iterator it;
#ifdef __SUNPRO_CC
  for ( it = regm.begin(); it != regm.end(); it++ )
    if (it->second->get_name() == name )
      return it->second;
  return NULL;
#else
  it = find_if (regm.begin(), regm.end(), NO::name_sec_eq(name)); 
  return (it == regm.end()) ? NULL : it->second;
#endif
}
  
Callsite*     
SeqTrace::get_csite(long csite_id) const 
{ 
  map<long, Callsite*>::const_iterator it = csitem.find(csite_id);

  if (it == csitem.end())
    throw RuntimeError("Error in SeqTrace::get_csite().");
  else
    return it->second;
}

Metric*       
SeqTrace::get_met(long met_id) const 
{ 
  map<long, Metric*>::const_iterator it = metm.find(met_id);

  if (it == metm.end())
    throw RuntimeError("Error in SeqTrace::get_met().");
  else
    return it->second;
}

void elg_readcb_ALL( elg_ui1 , 
                     elg_ui1 , void* )
{
  //earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
}

void elg_readcb_STRING( elg_ui4 strid,      /* string identifier */
                        elg_ui1 cntc,       /* number of continuation records */
                        const char* str,    /* string */
                        void* userdata)
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  if (cntc == 0)
    {
      trace->strm[strid]  = str; 
      return;
    }

  /* continued strings */
  char buffer[251];
  memcpy(buffer, str, 250);
  buffer[250] = '\0';     
  trace->cur_strid = strid;
  trace->strm[strid] = buffer; 
}

void elg_readcb_STRING_CNT( const char* str, /* continued string */
                            void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  char buffer[256];
  strncpy(buffer, str, 255);
  buffer[255] = '\0';         /* enforce string termination */

  std::string s = trace->strm[trace->cur_strid] + buffer;
  trace->strm[trace->cur_strid]  = s; 

}

void elg_readcb_MACHINE( elg_ui4 mid,       /* machine identifier */
                         elg_ui4 nodec,     /* number of nodes */
                         elg_ui4 mnid,      /* machine name identifier */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->grid->create_mach(mid, trace->strm[mnid]);
}

void elg_readcb_NODE( elg_ui4 nid,          /* node identifier */
                      elg_ui4 mid,          /* machine identifier */
                      elg_ui4 cpuc,         /* number of CPUs */
                      elg_ui4 nnid,         /* node name identifer */
                      elg_d8  cr,            /* number of clock cycles/sec */
                      void* userdata ) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->grid->create_node(mid, nid, trace->strm[nnid], cpuc, cr);
}

void elg_readcb_PROCESS( elg_ui4 pid,       /* process identifier */
                         elg_ui4 pnid,      /* process name identifier */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->grid->create_proc(pid, trace->strm[pnid]);
}

void elg_readcb_THREAD( elg_ui4 tid,        /* thread identifier */
                        elg_ui4 pid,        /* process identifier */
                        elg_ui4 tnid,       /* thread name identifier */
                        void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->grid->create_thrd(pid, tid, trace->strm[tnid]);
}

void elg_readcb_LOCATION( elg_ui4 lid,      /* location identifier */
                          elg_ui4 mid,      /* machine identifier */
                          elg_ui4 nid,      /* node identifier */
                          elg_ui4 pid,      /* process identifier */
                          elg_ui4 tid,      /* thread identifier */
                          void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->grid->create_loc(lid, mid, nid, pid, tid); 
}

void elg_readcb_FILE( elg_ui4 fid,          /* file identifier */
                      elg_ui4 fnid,         /* file name identifier */
                      void* userdata ) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->filem[fid] = trace->strm[fnid];
}

void elg_readcb_REGION( elg_ui4 rid,        /* region identifier */
                        elg_ui4 rnid,       /* region name identifier */
                        elg_ui4 fid,        /* source file identifier */
                        elg_ui4 begln,      /* begin line number */
                        elg_ui4 endln,      /* end line number */
                        elg_ui4 rdid,       /* region description identifier */
                        elg_ui1 rtype,      /* region type */
                        void* userdata ) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  earl::Region* reg;
  std::string rtype_str;

  switch(rtype)
    {
    case ELG_UNKNOWN :    
      rtype_str = "UNKNOWN";
      break;
    case ELG_FUNCTION :              
    case ELG_FUNCTION_COLL_BARRIER :
    case ELG_FUNCTION_COLL_ONE2ALL :
    case ELG_FUNCTION_COLL_ALL2ONE :
    case ELG_FUNCTION_COLL_ALL2ALL :
    case ELG_FUNCTION_COLL_OTHER :
      rtype_str = "FUNCTION";
      break;
    case ELG_LOOP :                  
      rtype_str = "LOOP";
      break;
    case ELG_USER_REGION :                  
      rtype_str = "USER_REGION";
      break;
    case ELG_OMP_PARALLEL :          
      rtype_str = "OMP_PARALLEL";
      break;
    case ELG_OMP_LOOP :             
      rtype_str = "OMP_LOOP";
      break;
    case ELG_OMP_SECTIONS :          
      rtype_str = "OMP_SECTIONS";
      break;
    case ELG_OMP_SECTION :          
      rtype_str = "OMP_SECTION";
      break;
    case ELG_OMP_WORKSHARE :          
      rtype_str = "OMP_WORKSHARE";
      break;
    case ELG_OMP_SINGLE :           
      rtype_str = "OMP_SINGLE";
      break;
    case ELG_OMP_MASTER :           
      rtype_str = "OMP_MASTER";
      break;
    case ELG_OMP_CRITICAL :         
      rtype_str = "OMP_CRITICAL";
      break;
    case ELG_OMP_ATOMIC :         
      rtype_str = "OMP_ATOMIC";
      break;
    case ELG_OMP_BARRIER :                    
      rtype_str = "OMP_BARRIER";
      break;
    case ELG_OMP_IBARRIER :                   
      rtype_str = "OMP_IBARRIER";
      break;
    case ELG_OMP_FLUSH :                      
      rtype_str = "OMP_FLUSH";
      break;
    case ELG_OMP_CRITICAL_SBLOCK :                    
      rtype_str = "OMP_CRITICAL_SBLOCK";
      break;
    case ELG_OMP_SINGLE_SBLOCK :                      
      rtype_str = "OMP_SINGLE_SBLOCK";
      break;
    default:
      throw earl::FatalError("No such region type.");
    }

  string file;
  if ( fid == ELG_NO_ID )
    file = "UNKNOWN";
  else 
    file = trace->filem[fid];
      
  reg = new earl::Region(rid, trace->strm[rnid], file, 
                   begln, endln, trace->strm[rdid], rtype_str);
  trace->regm[rid] = reg;
}

void elg_readcb_CALL_SITE( elg_ui4 csid,    /* call site identifier */
                           elg_ui4 fid,     /* source file identifier */
                           elg_ui4 lno,     /* line number */
                           elg_ui4 erid,    /* region identifer to be entered */
                           elg_ui4 lrid,    /* region identifer to be left */
                           void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  earl::Callsite* csite;

  csite =  new earl::Callsite(csid, trace->filem[fid], lno, trace->regm[erid]);
  trace->csitem[csid]=csite;
}

void elg_readcb_CALL_PATH( elg_ui4 cpid,    /* call-path identifier */
                           elg_ui4 rid,     /* node region identifier */
                           elg_ui4 ppid,    /* parent call-path identifier */
                           elg_ui8 order,   /* node order specifier */
                           void* userdata) 
{
#if 0
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  earl::Callpath* cpath;

  cpath =  new earl::Callpath(cpid, trace->regm[rid], trace->cpitem[cpid], order);
  trace->cpitem[cpid]=cpath;
#endif
}

void elg_readcb_METRIC( elg_ui4 metid,      /* metric identifier */
                        elg_ui4 metnid,     /* metric name identifier */
                        elg_ui4 metdid,     /* metric description identifier */
                        elg_ui1 metdtype,   /* metric data type */
                        elg_ui1 metmode,    /* metric mode */
                        elg_ui1 metiv,      /* time interval referenced */
                        void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  earl::Metric* met;

  // data-type check
  if ( metdtype != ELG_INTEGER )
    throw earl::FatalError("This metric data type is not supported yet.");
  // mode check
  if ( metmode != ELG_COUNTER )
    throw earl::FatalError("This metric mode is not supported yet.");
  // interval check
  if ( metiv != ELG_START )
    throw earl::FatalError("This metric interval semantics is not supported yet.");

  met = new earl::Metric(metid, trace->strm[metnid], trace->strm[metdid],
                   "INTEGER", "COUNTER", "START");
  trace->metm[metid]=met;
}

void elg_readcb_MPI_GROUP(elg_ui4 gid,         /* group identifier */
                          elg_ui1 mode,        /* mode flags */
                          elg_ui4 grpc,        /* number of ranks in group */
                          elg_ui4 grpv[],      /* local |-> global rank mapping */
                          void*   userdata)
{
  throw earl::FatalError("Unexpected ELG_MPI_GROUP record!");
}

void elg_readcb_MPI_COMM_DIST ( elg_ui4 cid,    /* communicator id unique to root */
                                elg_ui4 root,   /* global rank of root process */
                                elg_ui4 lcid,   /* local communicator id on process */
                                elg_ui4 lrank,  /* local rank of process */
                                elg_ui4 size,   /* size of communicator */
                                void* userdata )
{
  throw earl::FatalError("Unexpected ELG_MPI_COMM_DIST record!");
}

void elg_readcb_MPI_COMM_REF(elg_ui4 cid,      /* communicator identifier */
                             elg_ui4 gid,      /* group identifier */
                             void* userdata)
{
  throw earl::FatalError("Unexpected ELG_MPI_COMM_REF record!");
}

void elg_readcb_MPI_COMM( elg_ui4 cid,      /* communicator identifier */
                          elg_ui1 mode,     /* mode flags */
                          elg_ui4 grpc,     /* size of bitstring in bytes */
                          elg_ui1 grpv[],   /* bitstring defining the group */
                          void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<long> groupv;

  for (unsigned int i = 0; i < grpc; i++)
   {
     elg_ui1 byte = grpv[i];
     for (int j = 0; j < 8; j++)
       if (byte & (1 << j))
         groupv.push_back(i*8 + j);
   }
  trace->grid->create_com(cid, groupv);
}

void elg_readcb_CART_TOPOLOGY(elg_ui4 topid,     /* topology id */
                              elg_ui4 tnid,      /* topology name id */
                              elg_ui4 cid,       /* communicator id */
                              elg_ui1 ndims,     /* number of dimensions */
                              elg_ui4 dimv[],    /* number of processes in each dim */  
                              elg_ui1 periodv[], /* periodicity in each dim */
                              elg_ui4 dimids[],  /* string ids of dimension names */
                              void* userdata)      
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<long>  dimvec;
  std::vector<bool>  periodvec;

  /* copy the periods into STL object */
  dimvec.reserve(ndims);
  for (int i=0;i<ndims;i++)
    dimvec.push_back(dimv[i]);

  /* copy the periods into STL object */
  periodvec.reserve(ndims);
  for (int i=0;i<ndims;i++)
    periodvec.push_back(periodv[i]);

  /* create the topology object */
  if(ndims <= 3)
    trace->grid->create_cart(topid, cid, ndims, dimvec, periodvec);
}

void elg_readcb_CART_COORDS ( elg_ui4 topid,   /* topology id */
                          elg_ui4 lid,         /* location id */
                          elg_ui1 ndims,       /* number of dimensions */
                          elg_ui4 coordv[],    /* coordinates in each dimension */
                          void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<long>  coordvec;
  earl::Location*          loc;
  earl::Cartesian*         cart;

  /* push the coordinates in coordv */
  coordvec.reserve(ndims);
  for (int i=0;i<ndims;i++)
    coordvec.push_back(coordv[i]);
  //copy(coordv, coordv+ndims, coordvec.begin());

  /* get the Location corresponding to the lid */
  loc = trace->grid->get_loc(lid);

  /* get the Cartesian corresponding to topid */
  cart = trace->grid->get_cart(topid);     

  /* now fill in the loc2coords map of cartptr */ 
  trace->set_cart_coords(cart,loc,coordvec); 
  //cart->set_coords(loc, coordvec);
}

void elg_readcb_OFFSET( elg_d8  ltime,     /* local time */
                        elg_d8  offset,    /* offset to global time */
                        void* userdata) 
{
  //earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
}

void elg_readcb_NUM_EVENTS( elg_ui4 eventc, void* userdata ) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->nevents=eventc;
}

void elg_readcb_EVENT_TYPES( elg_ui4 ntypes, elg_ui1 typev[], void* userdata ) 
{
  /* earl::SeqTrace* trace = (earl::SeqTrace*) userdata; */
  /* XXXX ignored XXXX */
}

void elg_readcb_EVENT_COUNTS( elg_ui4 ntypes, elg_ui4 countv[], void* userdata ) 
{
  /* earl::SeqTrace* trace = (earl::SeqTrace*) userdata; */
  /* XXXX ignored XXXX */
}

void elg_readcb_MAP_SECTION( elg_ui4 rank, void* userdata )
{
  /* earl::SeqTrace* trace = (earl::SeqTrace*) userdata; */
  /* XXXX ignored XXXX */
}

void elg_readcb_MAP_OFFSET( elg_ui4 rank, elg_ui4 offset, void* userdata )
{
  /* earl::SeqTrace* trace = (earl::SeqTrace*) userdata; */
  /* XXXX ignored XXXX */
}

void elg_readcb_IDMAP( elg_ui1 type, elg_ui1 mode, elg_ui4 count,
                       elg_ui4 mapv[], void* userdata )
{
  /* earl::SeqTrace* trace = (earl::SeqTrace*) userdata; */
  /* XXXX ignored XXXX */
}

void elg_readcb_LAST_DEF( void* userdata ) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->lastdef=true;
}

void elg_readcb_MPI_WIN( elg_ui4 wid,      /* window identifier */
                         elg_ui4 cid,      /* communicator identifier */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  trace->grid->create_win(wid, cid);
}

/* -- Attributes -- */

void elg_readcb_ATTR_UI1(elg_ui1 type, elg_ui1 val, void* userdata)
{
}

void elg_readcb_ATTR_UI4(elg_ui1 type, elg_ui4 val, void* userdata)
{
}

/* -- Region -- */
void elg_readcb_ENTER(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 rid,       /* region identifier of the region being entered */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  for (size_t i = 0 ; i < metc; i++)
    metvec.push_back(metv[i]);
  
  try {
    trace->cur_event = new earl::Enter_rep(*trace->cur_state, 
                                           trace->cur_buffer, 
                                           trace->grid->get_loc(lid), 
                                           time, 
                                           trace->regm[rid], 
                                           trace->metm, 
                                           metvec,
                                           NULL);
  } catch (const Error& err) {
      trace->err_msg =  err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_ENTER_CS(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 csid,      /* call-site identifier of the call-site being entered */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],     /* metric values                    */
                      void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  for (size_t i = 0 ; i < metc; i++)
    metvec.push_back(metv[i]);

  try {
    earl::Callsite* csite = trace->get_csite(csid);
    trace->cur_event = new earl::Enter_rep(*trace->cur_state, 
                            trace->cur_buffer, 
                            trace->grid->get_loc(lid), 
                            time, 
                            csite->get_callee(), 
                            trace->metm, 
                            metvec,
                            csite);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_EXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],     /* metric values                    */
                      void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  for (size_t i = 0 ; i < metc; i++)
    metvec.push_back(metv[i]);

  try {
    trace->cur_event = new earl::Exit_rep(*trace->cur_state, 
                           trace->cur_buffer, 
                           trace->grid->get_loc(lid), 
                           time, 
                           trace->metm, 
                           metvec);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


/* -- MPI-1 -- */

void elg_readcb_MPI_SEND(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 dlid,      /* destination location identifier  */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui4 tag,       /* message tag                      */
                      elg_ui4 sent,      /* message length in bytes          */
                      void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::Send_rep( *trace->cur_state, 
                                           trace->cur_buffer, 
                                           trace->grid->get_loc(lid), 
                                           time, 
                                           trace->grid->get_com(cid),
                                           tag,
                                           sent,
                                           trace->grid->get_loc(dlid));
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_RECV(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui4 slid,      /* source location identifier       */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui4 tag,       /* message tag                      */
                      void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;

  try {
    trace->cur_event = new earl::Recv_rep(*trace->cur_state, 
                           trace->cur_buffer, 
                           trace->grid->get_loc(lid), 
                           time, 
                           trace->grid->get_com(cid),
                           tag,
                           trace->grid->get_loc(slid));
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_COLLEXIT(
                      elg_ui4 lid,       /* location identifier              */
                      elg_d8  time,      /* time stamp                       */
                      elg_ui1 metc,      /* number of metrics                */
                      elg_ui8 metv[],    /* metric values                    */
                      elg_ui4 rlid,      /* root location identifier         */
                      elg_ui4 cid,       /* communicator identifier          */
                      elg_ui4 sent,      /* bytes sent                       */
                      elg_ui4 recvd,     /* bytes received                   */
                      void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;

  std::vector<elg_ui8> metvec;
  metvec.reserve(metc);
  for (int i=0; i<metc; i++ )
    metvec.push_back(metv[i]);

  // some collective operations don't have a root location 
  earl::Location* root;
  if (rlid == ELG_NO_ID)
    root = NULL;
  else
    root = trace->grid->get_loc(rlid);
  
  try {
    trace->cur_event = new earl::MPICExit_rep(trace->get_state(), 
                                              trace->get_buffer(), 
                                              trace->grid->get_loc(lid), 
                                              time, 
                                              trace->get_metm(), 
                                              metvec,
                                              root,
                                              trace->grid->get_com(cid),
                                              sent,
                                              recvd);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


/* -- MPI-1 non-blocking */

void elg_readcb_MPI_SEND_COMPLETE(elg_ui4 lid,      /* location identifier              */
                                  elg_d8  time,     /* time stamp */
                                  elg_ui4 reqid,    /* request identifier of completed send */
                                  void* userdata)
{
}

void elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid,       /* location identifier              */
                                 elg_d8  time,      /* time stamp */
                                 elg_ui4 reqid,     /* receive request identifier       */
                                 void* userdata)
{
}

void elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid,     /* location identifier              */
                                   elg_d8  time,    /* time stamp */
                                   elg_ui4 reqid,   /* receive request identifier       */
                                   void*   userdata)
{
}

void elg_readcb_MPI_CANCELLED(elg_ui4 lid,          /* location identifier              */
                              elg_d8  time,         /* time stamp */
                              elg_ui4 reqid,        /* receive request identifier       */
                              void*   userdata)
{
}


/* -- MPI-2 One-sided Operations -- */

void elg_readcb_MPI_PUT_1TS( elg_ui4 lid,       /* location identifier              */
                             elg_d8  time,      /* time stamp                       */
                             elg_ui4 dlid,      /* destination location identifier  */
                             elg_ui4 wid,       /* window identifier                */
                             elg_ui4 rmaid,     /* RMA operation identifier         */
                             elg_ui4 nbytes,    /* message length in bytes          */
                             void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {  
    trace->cur_event = new earl::MPIPut1TS_rep( trace->get_state(),
                                                trace->get_buffer(),
                                                trace->grid->get_loc(lid), 
                                                time,
                                                trace->grid->get_loc(dlid), 
                                                trace->grid->get_win(wid),
                                                rmaid,
                                                nbytes);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_PUT_1TE( elg_ui4 lid,       /* location identifier              */
                             elg_d8  time,      /* time stamp                       */
                             elg_ui4 slid,      /* source location identifier       */
                             elg_ui4 wid,       /* window identifier                */
                             elg_ui4 rmaid,     /* RMA operation identifier         */
                             void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {  
    trace->cur_event = new earl::MPIPut1TE_rep( trace->get_state(),
                                           trace->get_buffer(),
                                           trace->grid->get_loc(lid), 
                                           time,
                                           trace->grid->get_loc(slid), 
                                           trace->grid->get_win(wid),
                                           rmaid);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}


void elg_readcb_MPI_GET_1TO( elg_ui4 lid,       /* location identifier              */
                             elg_d8  time,      /* time stamp                       */
                             elg_ui4 rmaid,     /* RMA operation identifier         */
                             void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
 
  try {
    trace->cur_event = new earl::MPIGet1TO_rep( trace->get_state(),
                                                trace->get_buffer(),
                                                trace->grid->get_loc(lid), 
                                                time,
                                                rmaid);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_GET_1TS( elg_ui4 lid,       /* location identifier              */
                             elg_d8  time,      /* time stamp                       */
                             elg_ui4 dlid,      /* destination location identifier  */
                             elg_ui4 wid,       /* window identifier                */
                             elg_ui4 rmaid,     /* RMA operation identifier         */
                             elg_ui4 nbytes,    /* message length in bytes          */
                             void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try { 
    trace->cur_event = new earl::MPIGet1TS_rep( trace->get_state(),
                                                trace->get_buffer(),
                                                trace->grid->get_loc(lid), 
                                                time,
                                                trace->grid->get_loc(dlid), 
                                                trace->grid->get_win(wid),
                                                rmaid,
                                                nbytes);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}


void elg_readcb_MPI_GET_1TE( elg_ui4 lid,       /* location identifier              */
                             elg_d8  time,      /* time stamp                       */
                             elg_ui4 slid,      /* source location identifier       */
                             elg_ui4 wid,       /* window identifier                */
                             elg_ui4 rmaid,     /* RMA operation identifier         */
                             void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try { 
    trace->cur_event = new earl::MPIGet1TE_rep( trace->get_state(),
                                                trace->get_buffer(),
                                                trace->grid->get_loc(lid), 
                                                time,
                                                trace->grid->get_loc(slid), 
                                                trace->grid->get_win(wid),
                                                rmaid );
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_WINEXIT( elg_ui4 lid,       /* location identifier              */
                             elg_d8  time,      /* time stamp                       */
                             elg_ui1 metc,      /* number of metrics                */
                             elg_ui8 metv[],    /* metric values                    */
                             elg_ui4 wid,       /* window identifier                */
                             elg_ui4 cid,       /* communicator identifier          */
                             elg_ui1 synex,     /* synchronization exit flag        */
                             void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  for (size_t i = 0 ; i < metc; i++)
    metvec.push_back(metv[i]);

  try {
    trace->cur_event = new earl::MPIWExit_rep( trace->get_state(),
                                               trace->get_buffer(),
                                               trace->grid->get_loc(lid), 
                                               time,
                                               trace->metm,
                                               metvec,
                                               trace->grid->get_win(wid),
                                               trace->grid->get_com(cid), 
                                               synex);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_WINCOLLEXIT( elg_ui4 lid,       /* location identifier              */
                                 elg_d8  time,      /* time stamp                       */
                                 elg_ui1 metc,      /* number of metrics                */
                                 elg_ui8 metv[],    /* metric values                    */
                                 elg_ui4 wid,       /* window identifier                */
                                 void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  for (size_t i = 0 ; i < metc; i++)
    metvec.push_back(metv[i]);

  try {
    trace->cur_event = new earl:: MPIWCExit_rep( trace->get_state(),
                                                 trace->get_buffer(),
                                                 trace->grid->get_loc(lid), 
                                                 time,
                                                 trace->metm,
                                                 metvec,
                                                 trace->grid->get_win(wid) );
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_WIN_LOCK( elg_ui4 lid,       /* location identifier              */
                              elg_d8  time,      /* time stamp                       */
                              elg_ui4 llid,      /* lock location identifier         */
                              elg_ui4 wid,       /* window identifier                */
                              elg_ui1 ltype,     /* lock type                        */
                              void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  
  try {
    trace->cur_event = new earl::MPIWLock_rep( trace->get_state(),
                                               trace->get_buffer(),
                                               trace->grid->get_loc(lid), 
                                               time,
                                               trace->grid->get_loc(llid), 
                                               trace->grid->get_win(wid),
                                               ltype);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_MPI_WIN_UNLOCK( elg_ui4 lid,       /* location identifier              */
                                elg_d8  time,      /* time stamp                       */
                                elg_ui4 llid,      /* lock location identifier         */
                                elg_ui4 wid,       /* window identifier                */
                                void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  
  try {
    trace->cur_event = new earl::MPIWUnlock_rep( trace->get_state(),
                                                 trace->get_buffer(),
                                                 trace->grid->get_loc(lid), 
                                                 time,
                                                 trace->grid->get_loc(llid), 
                                                 trace->grid->get_win(wid) );
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


/* -- Generic One-sided Operations -- */

void elg_readcb_PUT_1TS( elg_ui4 lid,       /* location identifier              */
                         elg_d8  time,      /* time stamp                       */
                         elg_ui4 dlid,      /* destination location identifier  */
                         elg_ui4 rmaid,     /* RMA operation identifier         */
                         elg_ui4 nbytes,    /* message length in bytes          */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::Put1TS_rep( trace->get_state(),
                                             trace->get_buffer(),
                                             trace->grid->get_loc(lid), 
                                             time,
                                             trace->grid->get_loc(dlid), 
                                             rmaid,
                                             nbytes );
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_PUT_1TE( elg_ui4 lid,       /* location identifier              */
                         elg_d8  time,      /* time stamp                       */
                         elg_ui4 slid,      /* source location identifier       */
                         elg_ui4 rmaid,     /* RMA operation identifier         */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::Put1TE_rep( trace->get_state(),
                                             trace->get_buffer(),
                                             trace->grid->get_loc(lid), 
                                             time,
                                             trace->grid->get_loc(slid), 
                                             rmaid );
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}


void elg_readcb_GET_1TS( elg_ui4 lid,       /* location identifier              */
                         elg_d8  time,      /* time stamp                       */
                         elg_ui4 dlid,      /* destination location identifier  */
                         elg_ui4 rmaid,     /* RMA operation identifier         */
                         elg_ui4 nbytes,    /* message length in bytes          */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::Get1TS_rep( trace->get_state(),
                                             trace->get_buffer(),
                                             trace->grid->get_loc(lid), 
                                             time,
                                             trace->grid->get_loc(dlid), 
                                             rmaid,
                                             nbytes );
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}


void elg_readcb_GET_1TE( elg_ui4 lid,       /* location identifier              */
                         elg_d8  time,      /* time stamp                       */
                         elg_ui4 slid,      /* source location identifier       */
                         elg_ui4 rmaid,     /* RMA operation identifier         */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::Get1TE_rep( trace->get_state(),
                                             trace->get_buffer(),
                                             trace->grid->get_loc(lid), 
                                             time,
                                             trace->grid->get_loc(slid), 
                                             rmaid );
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_COLLEXIT( elg_ui4 lid,       /* location identifier              */
                          elg_d8  time,      /* time stamp                       */
                          elg_ui1 metc,      /* number of metrics                */
                          elg_ui8 metv[],    /* metric values                    */
                          elg_ui4 rlid,      /* root location identifier         */
                          elg_ui4 cid,       /* communicator identifier          */
                          elg_ui4 sent,      /* bytes sent                       */
                          elg_ui4 recvd,     /* bytes received                   */
                          void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  metvec.reserve(metc);
  for (int i=0; i<metc; i++ )
    metvec.push_back(metv[i]);

  // some collective operations don't have a root location 
  earl::Location* root;
  if (rlid == ELG_NO_ID)
    root = NULL;
  else
    root = trace->grid->get_loc(rlid);

  try {
    trace->cur_event = new earl::CollExit_rep(trace->get_state(), 
                                              trace->get_buffer(), 
                                              trace->grid->get_loc(lid), 
                                              time, 
                                              trace->get_metm(), 
                                              metvec,
                                              root,
                                              trace->grid->get_com(cid),
                                              sent,
                                              recvd);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_ALOCK( elg_ui4 lid,       /* location identifier              */
                       elg_d8  time,      /* time stamp                       */
                       elg_ui4 lkid,      /* identifier of the lock being acquired */
                       void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;

  try {
    trace->cur_event = new earl::Alock_rep(trace->get_state(), 
                                           trace->get_buffer(), 
                                           trace->grid->get_loc(lid), 
                                           time,
                                           lkid);    
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_RLOCK( elg_ui4 lid,       /* location identifier              */
                       elg_d8  time,      /* time stamp                       */
                       elg_ui4 lkid,      /* identifier of the lock being releasedd */
                       void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::Rlock_rep( trace->get_state(), 
                                            trace->get_buffer(), 
                                            trace->grid->get_loc(lid), 
                                            time,
                                            lkid );    
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


/* -- OpenMP -- */

void elg_readcb_OMP_FORK( elg_ui4 lid,       /* location identifier              */
                          elg_d8  time,      /* time stamp                       */
                          void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;

  try {
    trace->cur_event = new earl::Fork_rep( trace->get_state(), 
                                           trace->get_buffer(), 
                                           trace->grid->get_loc(lid), 
                                           time);     
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_OMP_JOIN( elg_ui4 lid,       /* location identifier              */
                          elg_d8  time,      /* time stamp                       */
                          void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::Join_rep(trace->get_state(), 
                                          trace->get_buffer(), 
                                          trace->grid->get_loc(lid), 
                                          time);     
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_OMP_ALOCK( elg_ui4 lid,       /* location identifier              */
                           elg_d8  time,      /* time stamp                       */
                           elg_ui4 lkid,      /* identifier of the lock being acquired */
                           void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  try {
    trace->cur_event = new earl::OMPAlock_rep(trace->get_state(), 
                                              trace->get_buffer(), 
                                              trace->grid->get_loc(lid), 
                                              time,
                                              lkid);    
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_OMP_RLOCK( elg_ui4 lid,       /* location identifier              */
                           elg_d8  time,      /* time stamp                       */
                           elg_ui4 lkid,      /* identifier of the lock being releasedd */
                           void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;

  try {
    trace->cur_event = new earl::OMPRlock_rep( trace->get_state(), 
                                               trace->get_buffer(), 
                                               trace->grid->get_loc(lid), 
                                               time,
                                               lkid );    
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


void elg_readcb_OMP_COLLEXIT( elg_ui4 lid,       /* location identifier              */
                              elg_d8  time,      /* time stamp                       */
                              elg_ui1 metc,      /* number of metrics                */
                              elg_ui8 metv[],    /* metric values                    */
                              void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  metvec.reserve(metc);
  for (int i=0; i<metc; i++ )
    metvec.push_back(metv[i]);

  try {
    trace->cur_event = new earl::OMPCExit_rep(trace->get_state(), 
                                              trace->get_buffer(), 
                                              trace->grid->get_loc(lid), 
                                              time, 
                                              trace->get_metm(), 
                                              metvec);
  } catch (const Error& err){
    trace->err_msg=err.get_msg();
  } catch (...) {
      // caller function sould check the value of trace->cur_event
  }
}


/* -- EPILOG Internal -- */

void elg_readcb_LOG_OFF( elg_ui4 lid,       /* location identifier              */
                         elg_d8  time,      /* time stamp                       */
                         elg_ui1 metc,      /* number of metrics                */
                         elg_ui8 metv[],    /* metric values                    */
                         void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  earl::Grid* grid = trace->get_grid();
  std::vector<elg_ui8> metvec;
  metvec.reserve(metc);
  for (int i=0; i<metc; i++ )
    metvec.push_back(metv[i]);

  trace->cur_event = new earl::Enter_rep(trace->get_state(), 
                                         trace->get_buffer(), 
                                         grid->get_loc(lid), 
                                         time, 
                                         trace->get_no_logging(), 
                                         trace->get_metm(), 
                                         metvec,
                                         NULL);

}


void elg_readcb_LOG_ON( elg_ui4 lid,       /* location identifier              */
                        elg_d8  time,      /* time stamp                       */
                        elg_ui1 metc,      /* number of metrics                */
                        elg_ui8 metv[],    /* metric values                    */
                        void* userdata) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  earl::Grid* grid = trace->get_grid();
  std::vector<elg_ui8> metvec;
  metvec.reserve(metc);
  for (int i=0; i<metc; i++ )
    metvec.push_back(metv[i]);

  trace->cur_event = new earl::Exit_rep(trace->get_state(), 
                                        trace->get_buffer(), 
                                        grid->get_loc(lid), 
                                        time, 
                                        trace->get_metm(), 
                                        metvec);

}


void elg_readcb_ENTER_TRACING( elg_ui4 lid,       /* location identifier              */
                               elg_d8  time,      /* time stamp                       */
                               elg_ui1 metc,      /* number of metrics                */
                               elg_ui8 metv[],    /* metric values                    */
                               void* userdata ) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  earl::Grid* grid = trace->get_grid();
  std::vector<elg_ui8> metvec;
  metvec.reserve(metc);
  for (int i=0; i<metc; i++ )
    metvec.push_back(metv[i]);

  trace->cur_event = new earl::Enter_rep(trace->get_state(), 
                                         trace->get_buffer(), 
                                         grid->get_loc(lid), 
                                         time, 
                                         trace->get_tracing(), 
                                         trace->get_metm(), 
                                         metvec,
                                         NULL);
      
}


void elg_readcb_EXIT_TRACING( elg_ui4 lid,       /* location identifier              */
                              elg_d8  time,      /* time stamp                       */
                              elg_ui1 metc,      /* number of metrics                */
                              elg_ui8 metv[],    /* metric values                    */
                              void* userdata ) 
{
  earl::SeqTrace* trace = (earl::SeqTrace*) userdata;
  std::vector<elg_ui8> metvec;
  metvec.reserve(metc);
  for (int i=0; i<metc; i++ )
    metvec.push_back(metv[i]);

  trace->cur_event = new earl::Exit_rep(trace->get_state(), 
                                        trace->get_buffer(), 
                                        trace->grid->get_loc(lid), 
                                        time, 
                                        trace->get_metm(), 
                                        metvec);
      
}

