/****************************************************************************
**  KOJAK                                                                  **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/* TODO
 * - Machine/Node names?
 * - metrics/samples conversion??
 */

#include "vtotf_handler.h"

#include <assert.h>
#include <string.h>


#ifdef WRITE_TO_FILE

#define ELGCB_MACHINE(mid, nodec, mnid) \
        ElgOut_write_MACHINE(c->elgout, mid, nodec, mnid)
#define ELGCB_NODE(nid, mid, cpuc, nnid, cr) \
        ElgOut_write_NODE(c->elgout, nid, mid, cpuc, nnid, cr)
#define ELGCB_LOCATION(lid, mid, nid, pid, tid) \
        ElgOut_write_LOCATION(c->elgout, lid, mid, nid, pid, tid)
#define ELGCB_MPI_COMM(cid, mode, grpc, grpv) \
        ElgOut_write_MPI_COMM(c->elgout, cid, mode, grpc, grpv)
#define ELGCB_STRING(strid, str) \
        ElgOut_write_string(c->elgout, strid, str)
#define ELGCB_REGION(rid, rnid, fid, begln, endln, rdid, rtype) \
        ElgOut_write_REGION(c->elgout, rid, rnid, fid, begln, endln, \
	                    rdid, rtype)
#define ELGCB_FILE(fid, fnid) \
        ElgOut_write_FILE(c->elgout, fid, fnid)
#define ELGCB_NUM_EVENTS(eventc) \
        /* not necessary - ElgOut automatically fixes this */
#define ELGCB_LAST_DEF() \
        ElgOut_write_LAST_DEF(c->elgout)
#define ELGCB_ENTER(lid, ts, rid) \
        ElgOut_write_ENTER(c->elgout, lid, ts, rid, num_counters, counter_vals)
#define ELGCB_EXIT(lid, ts) \
        ElgOut_write_EXIT(c->elgout, lid, ts, num_counters, counter_vals)
#define ELGCB_MPI_COLLEXIT(lid, ts, rlid, cid, sent, recvd) \
        ElgOut_write_MPI_COLLEXIT(c->elgout, lid, ts, num_counters, counter_vals, \
                                  rlid, cid, sent, recvd)
#define ELGCB_ENTER_TRACING(lid, ts) \
        ElgOut_write_ENTER_TRACING(c->elgout, lid, ts, num_counters, counter_vals)
#define ELGCB_EXIT_TRACING(lid, ts) \
        ElgOut_write_EXIT_TRACING(c->elgout, lid, ts, num_counters, counter_vals)
#define ELGCB_MPI_SEND(lid, ts, dlid, cid, tag, sent) \
        ElgOut_write_MPI_SEND(c->elgout, lid, ts, dlid, cid, tag, sent)
#define ELGCB_MPI_RECV(lid, ts, slid, cid, tag) \
        ElgOut_write_MPI_RECV(c->elgout, lid, ts, slid, cid, tag)
#define ELGCB_METRIC(metid, metnid) \
		ElgOut_write_METRIC(c->elgout, metid, metnid, metnid, 0, 0, 0)

#else
#include "elg_readcb.h"

#define ELGCB_MACHINE(mid, nodec, mnid) \
        elg_readcb_ALL(ELG_MACHINE, 1, c->udata); \
        elg_readcb_MACHINE(mid, nodec, mnid, c->udata)
#define ELGCB_NODE(nid, mid, cpuc, nnid, cr) \
        elg_readcb_ALL(ELG_NODE, 1, c->udata); \
        elg_readcb_NODE(nid, mid, cpuc, nnid, cr, c->udata)
#define ELGCB_LOCATION(lid, mid, nid, pid, tid) \
        elg_readcb_ALL(ELG_LOCATION, 1, c->udata); \
        elg_readcb_LOCATION(lid, mid, nid, pid, tid, c->udata)
#define ELGCB_MPI_COMM(cid, mode, grpc, grpv) \
        elg_readcb_ALL(ELG_MPI_COMM, 1, c->udata); \
        elg_readcb_MPI_COMM(cid, mode, grpc, grpv, c->udata)
#define ELGCB_STRING(strid, str) \
        elg_readcb_ALL(ELG_STRING, 1, c->udata); \
        elg_readcb_STRING(strid, 0, str, c->udata)
        /*??? cntc*/
#define ELGCB_REGION(rid, rnid, fid, begln, endln, rdid, rtype) \
        elg_readcb_ALL(ELG_REGION, 1, c->udata); \
        elg_readcb_REGION(rid, rnid, fid, begln, endln, rdid, rtype, c->udata)
#define ELGCB_FILE(fid, fnid) \
        elg_readcb_ALL(ELG_FILE, 1, c->udata); \
        elg_readcb_FILE(fid, fnid, c->udata)
#define ELGCB_NUM_EVENTS(eventc) \
        elg_readcb_ALL(ELG_NUM_EVENTS, 1, c->udata); \
        elg_readcb_NUM_EVENTS(eventc, c->udata)
#define ELGCB_LAST_DEF() \
        elg_readcb_ALL(ELG_LAST_DEF, 1, c->udata); \
        elg_readcb_LAST_DEF(c->udata)
#define ELGCB_ENTER(lid, ts, rid) \
        elg_readcb_ALL(ELG_ENTER, 1, c->udata); \
        elg_readcb_ENTER(lid, ts, rid, num_counters, counter_vals, c->udata)
#define ELGCB_EXIT(lid, ts) \
        elg_readcb_ALL(ELG_EXIT, 1, c->udata); \
        elg_readcb_EXIT(lid, ts, num_counters, counter_vals, c->udata)
#define ELGCB_MPI_COLLEXIT(lid, ts, rlid, cid, sent, recvd) \
        elg_readcb_ALL(ELG_MPI_COLLEXIT, 1, c->udata); \
        elg_readcb_MPI_COLLEXIT(lid, ts, num_counters, counter_vals, rlid, \
                                cid, sent, recvd, c->udata)
#define ELGCB_ENTER_TRACING(lid, ts) \
        elg_readcb_ALL(ELG_ENTER_TRACING, 1, c->udata); \
        elg_readcb_ENTER_TRACING(lid, ts, num_counters, counter_vals, c->udata)
#define ELGCB_EXIT_TRACING(lid, ts) \
        elg_readcb_ALL(ELG_EXIT_TRACING, 1, c->udata); \
        elg_readcb_EXIT_TRACING(lid, ts, num_counters, counter_vals, c->udata)
#define ELGCB_MPI_SEND(lid, ts, dlid, cid, tag, sent) \
        elg_readcb_ALL(ELG_MPI_SEND, 1, c->udata); \
        elg_readcb_MPI_SEND(lid, ts, dlid, cid, tag, sent, c->udata)
#define ELGCB_MPI_RECV(lid, ts, slid, cid, tag) \
        elg_readcb_ALL(ELG_MPI_RECV, 1, c->udata); \
        elg_readcb_MPI_RECV(lid, ts, slid, cid, tag, c->udata)

#endif /*WRITE_TO_FILE*/

#define ELG_TRACING 42

/*
 * Global variables -- include in VTOTF_Control?
*/
elg_ui1 num_counters = 0;
int cur_counter = 0;
uint64_t *counter_vals = NULL;
int enter = 1;

/* To Buffer Enter Events */
uint32_t b_process;
uint32_t b_function;
elg_d8 b_ts;
int b_tracing;

/*
 * --- static helper functions ----------------------------------------------
 */

/* collective operation */
typedef struct
{
   char*   name;
   elg_ui1 type;
} ElgMPIColl;

static const ElgMPIColl mpi_colls[] = {
  /* must be alphabetically sorted ! */
  { "MPI_Allgather",      ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Allgatherv",     ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Allreduce",      ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Alltoall",       ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Alltoallv",      ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Alltoallw",      ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Barrier",        ELG_FUNCTION_COLL_BARRIER },
  { "MPI_Bcast",          ELG_FUNCTION_COLL_ONE2ALL },
  { "MPI_Exscan",         ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Gather",         ELG_FUNCTION_COLL_ALL2ONE },
  { "MPI_Gatherv",        ELG_FUNCTION_COLL_ALL2ONE },
  { "MPI_Reduce",         ELG_FUNCTION_COLL_ALL2ONE },
  { "MPI_Reduce_scatter", ELG_FUNCTION_COLL_ALL2ALL },
  { "MPI_Scan",           ELG_FUNCTION_COLL_OTHER   },
  { "MPI_Scatter",        ELG_FUNCTION_COLL_ONE2ALL },
  { "MPI_Scatterv",       ELG_FUNCTION_COLL_ONE2ALL },
  { "MPI_Win_create",     ELG_FUNCTION_COLL_BARRIER },
  { "MPI_Win_fence",      ELG_FUNCTION_COLL_BARRIER },
  { "MPI_Win_free",       ELG_FUNCTION_COLL_BARRIER }
};

static int mycmp(const void *v1, const void *v2)
{
   return strcasecmp((char*)v1, ((ElgMPIColl*)v2)->name);
}

static elg_ui1 get_region_type(const char* str)
{
  ElgMPIColl * match =
    bsearch(str, (ElgMPIColl*)mpi_colls, sizeof(mpi_colls)/sizeof(ElgMPIColl),
            sizeof(ElgMPIColl), mycmp);

  return match ? match->type : ELG_FUNCTION;
}

/*
 * --- OTF reader callbacks setup -------------------------------------------
 */

OTF_HandlerArray* setupOTFHandlers(VTOTF_Control* fha) {
  OTF_HandlerArray* handlers = OTF_HandlerArray_open();

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefTimerResolution,
    OTF_DEFTIMERRESOLUTION_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFTIMERRESOLUTION_RECORD);

  OTF_HandlerArray_setHandler( handlers, 
    (OTF_FunctionPointer*) handleDefProcess,
    OTF_DEFPROCESS_RECORD );
  OTF_HandlerArray_setFirstHandlerArg( handlers, 
        fha, OTF_DEFPROCESS_RECORD );

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefProcessGroup,
    OTF_DEFPROCESSGROUP_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFPROCESSGROUP_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefFunction,
    OTF_DEFFUNCTION_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFFUNCTION_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefFunctionGroup,
    OTF_DEFFUNCTIONGROUP_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFFUNCTIONGROUP_RECORD);

  /*
  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefCollectiveOperation,
    OTF_DEFCOLLOP_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFCOLLOP_RECORD);
  */

  OTF_HandlerArray_setHandler(handlers,
    (OTF_FunctionPointer*) handleDefCounter,
    OTF_DEFCOUNTER_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFCOUNTER_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefCounterGroup,
    OTF_DEFCOUNTERGROUP_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFCOUNTERGROUP_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefScl,
    OTF_DEFSCL_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFSCL_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleDefSclFile,
    OTF_DEFSCLFILE_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_DEFSCLFILE_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleCounter,
    OTF_COUNTER_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_COUNTER_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleEnter,
    OTF_ENTER_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_ENTER_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleCollectiveOperation,
    OTF_COLLOP_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_COLLOP_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleRecvMsg,
    OTF_RECEIVE_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_RECEIVE_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleSendMsg,
    OTF_SEND_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_SEND_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleLeave,
    OTF_LEAVE_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, 
        fha, OTF_LEAVE_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleBeginProcess,
    OTF_BEGINPROCESS_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, fha,
    OTF_BEGINPROCESS_RECORD);

  OTF_HandlerArray_setHandler(handlers, 
    (OTF_FunctionPointer*) handleEndProcess,
    OTF_ENDPROCESS_RECORD);
  OTF_HandlerArray_setFirstHandlerArg(handlers, fha,
    OTF_ENDPROCESS_RECORD);

  return handlers;
}

/*
 * --- complete definitions -------------------------------------------------
 */

void completeDefinitionRecords(VTOTF_Control* c) {
  int i;

  ELGCB_STRING(c->stringid, "UnknownMachine");
  ELGCB_MACHINE(0, 1, c->stringid++);
  ELGCB_STRING(c->stringid, "UnknownNode");
  ELGCB_NODE(0, 0, c->numloc, c->stringid, 0);
  for (i=0; i<c->numloc; i++) {
    ELGCB_LOCATION(i, 0, 0, i, 0);
  }
  ELGCB_NUM_EVENTS(1007022); /*???*/
  ELGCB_LAST_DEF();

  c->c_com  = malloc(c->numloc * sizeof(elg_ui4));
  c->c_root = malloc(c->numloc * sizeof(elg_ui4));
  c->c_sent = malloc(c->numloc * sizeof(elg_ui4));
  c->c_revd = malloc(c->numloc * sizeof(elg_ui4));
  assert(c->c_revd);
  for (i=0; i<c->numloc; i++) {
    c->c_com[i] = ELG_NO_ID;
  }

  /* initialize counter array */
  if(num_counters > 0) counter_vals = malloc(num_counters * sizeof(uint64_t));
}

/*
 * --- initialize OTF Control Block -----------------------------------------
 */
void initOTFHandle(VTOTF_Control* fha) {
  fha->num = 0;
  fha->ticksPerSecond = 0;
  fha->elgout = 0;
  fha->stringid = 0;
  fha->numloc = 0;
  fha->udata = 0;
  dvec_create(fha->funcgrouptab, elg_ui4, 10);
  dvec_create(fha->scltab, SCL, 10);
  dvec_zero(fha->comtab, 10);
  dvec_create(fha->vtapitab, elg_ui4, 10);
}

/*
 * --- OTF reader callbacks -------------------------------------------------
 */

int handleDefTimerResolution( void* userData, uint32_t stream,
  uint64_t ticksPerSecond ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  c->ticksPerSecond = ticksPerSecond;
  return OTF_RETURN_OK;
}

int handleDefProcess( void* userData, uint32_t stream, uint32_t process,
  const char* name, uint32_t parent ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  if (process >= c->numloc) c->numloc=process;
  return OTF_RETURN_OK;
}

int handleDefProcessGroup( void* userData, uint32_t stream,
  uint32_t group, const char* name, uint32_t numprocs,
  const uint32_t* procs ) {
  int i;
  VTOTF_Control* c = (VTOTF_Control*) userData;

  if ( (strncmp(name, "MPI Communicator ", 17) == 0) ||
       (strncmp(name, "MPI_COMM_", 9) == 0) ) {
    assert(numprocs <= c->numloc);
    int gsize = c->numloc / 8 + (c->numloc % 8 ? 1 : 0);
    elg_ui1* grpv = malloc(gsize * sizeof(elg_ui1));
    for (i = 0; i < gsize; i++) grpv[i] = 0;
    for (i = 0; i <numprocs; i++) {
      int p = procs[i]-1;
      grpv[p / 8] |= (1 << (p % 8));
    }
    dvec_pushback(c->comtab, elg_ui4, group);
    ELGCB_MPI_COMM(c->comtab.size-1, 0, gsize, grpv);
    free(grpv);
  } else {
#ifdef WRITE_TO_FILE
    fprintf( stdout, "(#%llu) \tDefProcessGroup: stream %u, group %u, name \"%s\", procs ",
        (long long unsigned) c->num, stream, group, name );

    for(i = 0; i < numprocs; ++i )
      fprintf( stdout, "%u, ", procs[i] );
    fprintf( stdout, "\n" );
#endif
  }
  return OTF_RETURN_OK;
}

int handleDefFunction( void* userData, uint32_t stream, uint32_t func,
  const char* name, uint32_t funcGroup, uint32_t source ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;

  ELGCB_STRING(c->stringid, name);
  ELGCB_REGION((func-1), c->stringid++, 
               (source ? dvec_get(c->scltab, SCL, source-1).fnid : ELG_NO_ID),
               (source ? dvec_get(c->scltab, SCL, source-1).line : ELG_NO_LNO),
               ELG_NO_LNO,
	       dvec_get(c->funcgrouptab, elg_ui4, funcGroup-1),
	       get_region_type(name));
  if (c->tracinggid == funcGroup) {
    /* VT_API function */
    dvec_pushback(c->vtapitab, elg_ui4, func);
  }
  return OTF_RETURN_OK;
}

int handleDefFunctionGroup( void* userData, uint32_t stream,
  uint32_t funcGroup, const char* name ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;

  ELGCB_STRING(c->stringid, name);
  dvec_insert(c->funcgrouptab, elg_ui4, funcGroup-1, c->stringid++);
  if ( strcmp(name, "VT_API") == 0 ) c->tracinggid = funcGroup;
  return OTF_RETURN_OK;
}

/*
int handleDefCollectiveOperation( void* userData, uint32_t stream,
  uint32_t collOp, const char* name, uint32_t type ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;

  fprintf( stdout, "(#%llu) \tDefCollective: stream %u, collective %u, name \"%s\", type %u\n",
        (long long unsigned) c->num, stream,  collOp, name, type );
  return OTF_RETURN_OK;
}
*/

int handleDefCounter( void* userData, uint32_t stream, uint32_t counter,
  const char* name, uint32_t properties, uint32_t counterGroup,
  const char* unit ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  int id;

  if(counterGroup == 2) {
	num_counters++;
	ELGCB_STRING(c->stringid, name);
	id = c->stringid++;
    ELGCB_METRIC(counter,id);

/*    fprintf( stdout, "(#%llu) \tDefCounter: stream %u, counter %u, name \"%s\", properties %u, group %u, unit \"%s\"\n",
        (long long unsigned) c->num, stream, counter, name, properties, counterGroup, unit );*/
  }
  return OTF_RETURN_OK;
}

int handleDefCounterGroup( void* userData, uint32_t stream,
  uint32_t counterGroup, const char* name ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;

  if(counterGroup == 2) {
    if (strncmp(name, "PAPI", 4) != 0) {
	  fprintf(stderr,"Error! OTF file contains non-PAPI counters!\n");
	  exit(EXIT_FAILURE);
    } else {
      ELGCB_STRING(c->stringid, "OTF_PAPI_SAMPLE");
      ELGCB_REGION(4242, c->stringid++, ELG_NO_ID, ELG_NO_LNO, ELG_NO_LNO, ELG_NO_ID, ELG_USER_REGION);
      // register region
    }
  }

/*  fprintf( stdout, "(#%llu) \tDefCounterGroup: stream %u, group %u, name \"%s\"\n",
        (long long unsigned) c->num, stream, counterGroup, name );*/
  return OTF_RETURN_OK;
}

int handleDefScl( void* userData, uint32_t stream, uint32_t source,
  uint32_t sourceFile, uint32_t line ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  SCL scl1;

  scl1.fnid = sourceFile - 1;
  scl1.line = line;
  dvec_insert(c->scltab, SCL, source-1, scl1);
  return OTF_RETURN_OK;
}

int handleDefSclFile( void* userData, uint32_t stream,
  uint32_t sourceFile, const char* name ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;

  ELGCB_STRING(c->stringid, name);
  ELGCB_FILE((sourceFile-1), c->stringid++);
  return OTF_RETURN_OK;
}

int handleEnter( void* userData, uint64_t time, uint32_t function,
  uint32_t process, uint32_t source ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  elg_d8 ts = (elg_d8) time / c->ticksPerSecond;
  int idx;

  dvec_getidx(c->vtapitab, elg_ui4, function, idx);
  if ( idx == -1 ) {
	if(num_counters > 0) {
	  b_function = function;
	  b_process = process;
	  b_ts = ts;
	  b_tracing = 1;
	  enter = 0;
	} else {
	  ELGCB_ENTER(process-1, ts, function-1);
	}
    c->c_root[process-1] = ELG_NO_ID;
  } else {
	if(num_counters > 0) {
	  b_function = function;
	  b_process = process;
	  b_ts = ts;
	  b_tracing = 0;
	  enter = 0;
	} else {
      ELGCB_ENTER_TRACING(process-1, ts);
	}
    c->c_root[process-1] = ELG_TRACING;
  }
  c->c_com[process-1] = ELG_NO_ID;
  return OTF_RETURN_OK;
}

int handleLeave( void* userData, uint64_t time, uint32_t function,
  uint32_t process, uint32_t source ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  elg_d8 ts = (elg_d8) time / c->ticksPerSecond;

  if (c->c_com[process-1] != ELG_NO_ID) {
    ELGCB_MPI_COLLEXIT(process-1, ts, c->c_root[process-1], c->c_com[process-1],
                       c->c_sent[process-1], c->c_revd[process-1]);  
  } else if (c->c_root[process-1] == ELG_TRACING) {
    ELGCB_EXIT_TRACING(process-1, ts);
  } else {
    ELGCB_EXIT(process-1, ts);
  }
  c->c_com[process-1] = ELG_NO_ID;
  c->c_root[process-1] = ELG_NO_ID;
  cur_counter = 0;
  return OTF_RETURN_OK;
}

int handleSendMsg( void* userData, uint64_t time, uint32_t sender,
  uint32_t receiver, uint32_t group, uint32_t type, uint32_t length,
  uint32_t source ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  elg_d8 ts = (elg_d8) time / c->ticksPerSecond;
  int communicator_idx;

  dvec_getidx(c->comtab, elg_ui4, group, communicator_idx);
  assert(communicator_idx != -1);
  ELGCB_MPI_SEND(sender-1, ts, receiver-1, communicator_idx, type, length);
  return OTF_RETURN_OK;
}

int handleRecvMsg( void* userData, uint64_t time, uint32_t receiver,
  uint32_t sender, uint32_t group, uint32_t type, uint32_t length, 
  uint32_t source ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  elg_d8 ts = (elg_d8) time / c->ticksPerSecond;
  int idx;

  dvec_getidx(c->comtab, elg_ui4, group, idx);
  ELGCB_MPI_RECV(receiver-1, ts, sender-1, idx, type);
  return OTF_RETURN_OK;
}

int handleCounter( void* userData, uint64_t time, uint32_t process,
  uint32_t counter, uint64_t value ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  elg_d8 ts = (elg_d8) time / c->ticksPerSecond;

  /*fprintf( stdout, "(#%llu) \t%llu Counter: process %u, counter %u, value %llu\n",
  		  (long long unsigned) c->num, (long long unsigned) time,
  		  process, counter, (long long unsigned) value );*/

  if(cur_counter == num_counters) {
	// own Region - to be added
	ELGCB_ENTER(process-1, ts, 4242);
	ELGCB_EXIT(process-1, ts);
	counter_vals[0] = value;
	cur_counter = 1;
	return OTF_RETURN_OK;
  }
  counter_vals[cur_counter] = value;
  cur_counter++;
  if(num_counters == cur_counter && enter == 0) {
	if(b_tracing == 0) {
	  ELGCB_ENTER_TRACING(b_process-1, b_ts);
	} else {
      ELGCB_ENTER(b_process-1, b_ts, b_function-1);
	}
	cur_counter = 0;
	enter = 1;
  }
  return OTF_RETURN_OK;
}

int handleCollectiveOperation( void* userData, uint64_t time,
  uint32_t process, uint32_t collective, uint32_t procGroup,
  uint32_t rootProc, uint32_t sent, uint32_t received, uint64_t duration, 
  uint32_t source ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;
  int idx;

  dvec_getidx(c->comtab, elg_ui4, procGroup, idx);
  c->c_com[process-1]  = idx;
  c->c_root[process-1] = rootProc-1;
  c->c_sent[process-1] = sent;
  c->c_revd[process-1] = received;
  return OTF_RETURN_OK;
}

int handleBeginProcess( void* userData, uint64_t time, uint32_t process ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;

#ifdef WRITE_TO_FILE
  fprintf( stdout, "(#%llu) \t%llu BeginProcess: process %u\n",
        (long long unsigned) c->num, (long long unsigned) time, process );
#endif
  return OTF_RETURN_OK;
}

int handleEndProcess( void* userData, uint64_t time, uint32_t process ) {
  VTOTF_Control* c = (VTOTF_Control*) userData;

#ifdef WRITE_TO_FILE
  fprintf( stdout, "(#%llu) \t%llu EndProcess: process %u\n",
        (long long unsigned) c->num, (long long unsigned) time, process );
#endif
  return OTF_RETURN_OK;
}
