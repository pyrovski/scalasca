/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/*===========================================================================*/
/*                                                                           */
/* EPILOG Converter OTF Backend                                              */
/*                                                                           */
/*===========================================================================*/


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "elg_readcb.h"
#include "elg_conv.h"
#include "otf.h"
#include "epk_archive.h"

typedef struct {
  elg_d8  time[3];
  elg_ui4 x_reg1;
  elg_ui4 e_reg2;
  elg_ui4 x_reg2;
} intra_coll_evs_t;

static elg_d8      lasttime = 0.0;
static OTF_Writer* writer = 0;
static int         rmatag = 101010;
static intra_coll_evs_t* intra_evs = 0;

#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  typedef struct {
    elg_d8   time[2];
    elg_ui1  metc;
    elg_ui8* metv;
  } flush_evs_t;

  static flush_evs_t* flush_ts = 0;
#endif

/*
 * Files
 */

static void conv_write_files() {
  int i;

  for (i=0; i<numfile; i++) {
    if ( filetab[i] ) OTF_Writer_writeDefSclFile(writer, 0, i+1, filetab[i]);
  }
}

/*
 * SCLs
 */

static void conv_write_scls() {
  int i;

  for (i=0; i<sclid; i++) {
    OTF_Writer_writeDefScl(writer, 0, i+1, scltab[i].fid+1, scltab[i].line);
  }
}

/*
 * Activities
 */

static void conv_write_activities() {
  int i;

  for (i=0; i<numact; i++) {
    if ( acttab[i].id == ELG_NO_ID ) return;
    OTF_Writer_writeDefFunctionGroup(writer, 0, acttab[i].id+1, acttab[i].name);
  }
}

/*
 * States
 */

static int scl_of_state(elg_ui4 rid) {
  int scl = statetab[rid].sclid;
  return ( (scl==-1) ? 0 : scl+1 );
}

static int scl_of_csite(elg_ui4 rid) {
  int scl = csitetab[rid].sclid;
  return ( (scl==-1) ? 0 : scl+1 );
}

static void conv_write_states() {
  int i;

  for (i=0; i<numstate; i++) {
    OTF_Writer_writeDefFunction(writer, 0, statetab[i].id+1, statetab[i].name,
	                        statetab[i].act+1, scl_of_state(i));
  }
}

/*
 * GlobalOps
 */

static int gtypes[] = {
  OTF_COLLECTIVE_TYPE_BARRIER,
  OTF_COLLECTIVE_TYPE_ONE2ALL,
  OTF_COLLECTIVE_TYPE_ALL2ONE,
  OTF_COLLECTIVE_TYPE_ALL2ALL,
  OTF_COLLECTIVE_TYPE_UNKNOWN
};

static void conv_write_glops() {
  int i;

  for (i=0; i<numglop; i++) {
    OTF_Writer_writeDefCollectiveOperation(writer, 0, gloptab[i].id+1,
        gloptab[i].name, gtypes[gloptab[i].rtype - ELG_FUNCTION_COLL_BARRIER]);
  }
}

/*
 * Groups
 */

static unsigned int group_print(Group *g) {
  int i;

  if ( g->member ) {
    unsigned int* array = x_malloc(g->num_member * sizeof(unsigned int));
    for (i=0; i<g->num_member; ++i) {
      array[i] = group_print(g->member + i) + 1;
    }
    ++grpid;
    OTF_Writer_writeDefProcessGroup(writer, 0, grpid+1,
	                            g->name, g->num_member, array);
    return grpid;
  } else {
    return g->id;
  }
}

static void conv_write_machines() {
  int m, n;

  /*dump machine*/
  for (m=0; m<nummachines; m++) group_print(machine+m);

  /*also: generate full machine but flat group*/
  {
    unsigned int* array = x_malloc(totallocs * sizeof(unsigned int));
    for (n=0; n<totallocs; ++n) array[n] = n + 1;
    ++grpid;
    OTF_Writer_writeDefProcessGroup(writer, 0, grpid+1,
	                            "all_flat", totallocs, array);
    samplegroup = grpid;
  }
}

static void conv_write_cpunames() {
  int i, t;

  if ( ! isthreaded ) {
    for (i=0; i<totallocs; i++) {
      OTF_Writer_writeDefProcess(writer, 0, i+1, loctab[i].proc->name, 0);
    }
  } else {
    for (i=0; i<totallocs; i++) {
      for (t=0; t<totallocs; t++) {
        if ( i == locmap[t] ) {
          OTF_Writer_writeDefProcess(writer, 0, i+1, loctab[t].thrd->name, 0);
	  /*??? parent*/
          break;
        }
      }
    }
  }
}

/*
 * Communicators
 */
#define FIRST_COMM_ID 20000000
#define FIRST_TGRP_ID 30000000

static void conv_write_comms() {
  int i, j;
  int n, p, t;

  /*MPI Communicators*/
  for (i=0; i<numcomm; i++) {
    if ( commtab[i].numbyte ) {
      char *name = x_malloc(16 * sizeof(char));
      unsigned int *array;
      int num = 0;

      for (j=0; j<(commtab[i].numbyte*8); j++) {
        if ( (1 << (j % 8)) & commtab[i].bits[j/8] ) num++;
      }
      if (num) commtab[i].array = array = x_malloc(num * sizeof(unsigned int));
      num = 0;
      for (j=0; j<(commtab[i].numbyte*8); j++) {
        if ( (1 << (j % 8)) & commtab[i].bits[j/8] )
          array[num++] = get_comm_loc(j) + 1;
      }
      commtab[i].numarray = num;

      if ( commtab[i].mode & ELG_GROUP_FLAG )
        sprintf(name, "group %d", i);
            else
        sprintf(name, "comm %d", i);
      OTF_Writer_writeDefProcessGroup(writer, 0, FIRST_COMM_ID+i,
	                              name, num, array);
    }
  }

  /*OpenMP Thread Groups*/
  if ( isthreaded ) {
    for (n=0; n<machine[0].num_member; n++) {
      Group *node = machine[0].member + n;
      for (p=0; p<node->num_member; p++) {
        Group *proc = node->member + p;
        char *name = x_malloc(16 * sizeof(char));
        unsigned int *array = x_malloc(proc->num_member * sizeof(unsigned int));
        for (t=0; t<proc->num_member; t++) {
          array[t] = proc->member[t].id + 1;
        }
        sprintf(name, "tgroup %d", /*maxcomm + 1 +*/ proc->id);
        OTF_Writer_writeDefProcessGroup(writer, 0, FIRST_TGRP_ID+proc->id,
	                                name, proc->num_member, array);
      }
    }
  }
}

/*
 * Metrics
 */

static uint32_t scopetab[] = {
  OTF_COUNTER_SCOPE_START,
  OTF_COUNTER_SCOPE_LAST,
  OTF_COUNTER_SCOPE_NEXT
};

static void conv_write_metrics() {
  int i, j = 0;

  if ( maxmet != -1 ) {
    OTF_Writer_writeDefCounterGroup(writer, 0, 1, "HW Counters");

    for (i=0; i<nummet; i++) {
      if ( mettab[i].name ) {
	if ( mettab[i].type != ELG_INTEGER ) {
          fprintf(stderr, "WARNING: OTF does not support non-integer "
	                  "counters (%s)\n", mettab[i].name);
          fflush(stderr);
	}
	elg_ui4 prop;
	if ( mettab[i].mode == ELG_COUNTER ) {
	  prop = OTF_COUNTER_TYPE_ACC + scopetab[mettab[i].iv];
	} else if ( mettab[i].mode == ELG_SAMPLE ) {
	  prop = OTF_COUNTER_TYPE_ABS + OTF_COUNTER_SCOPE_POINT;
	} else {
          fprintf(stderr, "WARNING: OTF does not support counter with "
	                  "mode ELG_RATE (%s)\n", mettab[i].name);
          fflush(stderr);
	}
	OTF_Writer_writeDefCounter(writer, 0, ++j, mettab[i].name,
	                           prop, 1, "#");
      }
    }
  }
}

/*
 * Windows
 */

/*
static void conv_write_windows() {
}
*/

/*
 * Definition Records
 */

void conv_write_definition_records() {
  int i;

  conv_write_cpunames();
  conv_write_machines();
  conv_write_comms();
  conv_write_files();
  conv_write_scls();
  conv_write_activities();
  conv_write_states();
  conv_write_metrics();
  conv_write_glops();

  intra_evs = x_malloc(totallocs * sizeof(intra_coll_evs_t));
  for (i=0; i<totallocs; i++) {
    intra_evs[i].time[0] = 0.0;
    intra_evs[i].time[1] = 0.0;
    intra_evs[i].time[2] = 0.0;
    intra_evs[i].x_reg1  = ELG_NO_ID;
    intra_evs[i].e_reg2  = ELG_NO_ID;
    intra_evs[i].x_reg2  = ELG_NO_ID;
  }

#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  flush_ts = x_malloc(totallocs * sizeof(flush_evs_t));
  for (i=0; i<totallocs; i++) {
    flush_ts[i].time[0] = 0.0;
    flush_ts[i].time[1] = 0.0;
    flush_ts[i].metc    = 0;
    flush_ts[i].metv    = 0;
  }
#endif
}

/*
 * EPILOG read callbacks
 */

/* -- Helpers -- */

static void genWriteENTER(elg_ui4 lid, elg_d8 time, elg_ui4 rid) {
  StackNode *p = stk_top(lid);

  stk_push(lid, rid, time);

  if ( is_glop(p->reg) && p->tim != time ) {
    /* 2nd (Severoty) ENTER inside collective: save + generate after CollOp */
    intra_evs[lid].time[1] = time;
    intra_evs[lid].e_reg2  = rid;
  } else {
    OTF_Writer_writeEnter(writer, (uint64_t) (time*1.0e+10), rid+1,
	                  lid+1, scl_of_state(rid));
  }
}

static void genWriteCOUNTER(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]) {
  int i;

  for (i=0; i<metc; i++) {
    OTF_Writer_writeCounter(writer, (uint64_t) (time*1.0e+10),
	                    lid+1, i+1, metv[i]);
  }
  lasttime = time;
}

static void genWriteLEAVE(elg_ui4 lid, elg_d8 time) {
  StackNode *n = stk_pop(lid);
  StackNode *p = stk_top(lid);
  if ( is_glop(p->reg) ) {
    /* (Severity) EXIT inside collective: save + generate after CollOp record */
    if ( intra_evs[lid].e_reg2 != ELG_NO_ID ) {
      /* second exit (probably exit_severity(completion) */
      intra_evs[lid].time[2] = time;
      intra_evs[lid].x_reg2  = n->reg;
    } else {
      /* first exit */
      intra_evs[lid].time[0] = time;
      intra_evs[lid].x_reg1  = n->reg;
    }
  } else {
    OTF_Writer_writeLeave(writer, (uint64_t) (time*1.0e+10), n->reg+1,
                          lid+1, 0);
  }
}

static void genWriteCOLLLEAVE(int w, elg_ui4 lid, elg_d8 time,
       elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd) {
  StackNode *n = stk_pop(lid);

  rlid = ( rlid != ELG_NO_ID ) ? locmap[rlid] + 1 : 0;
  if ( w ) {
    elg_d8 stime = (n->tim)*1.0e+10+1.0;
    elg_d8 etime = time*1.0e+10-1.0;
    if ( etime <= stime ) {
      stime = (n->tim)*1.0e+10;
      etime = time*1.0e+10;
    }
    OTF_Writer_writeCollectiveOperation(writer, (uint64_t)(stime),
                                      lid+1, n->reg+1, cid,
				      rlid, sent, recvd,
				      (uint64_t)(etime-stime), 0);
  }
  /* (Severity) Events inside collective: generate saved events */
  if ( intra_evs[lid].x_reg1 != ELG_NO_ID ) {
    OTF_Writer_writeLeave(writer, (uint64_t) (intra_evs[lid].time[0]*1.0e+10),
                          intra_evs[lid].x_reg1+1, lid+1, 0);
    intra_evs[lid].x_reg1 = ELG_NO_ID;
  }
  if ( intra_evs[lid].e_reg2 != ELG_NO_ID ) {
    OTF_Writer_writeEnter(writer, (uint64_t) (intra_evs[lid].time[1]*1.0e+10),
                          intra_evs[lid].e_reg2+1, lid+1, 0);
    OTF_Writer_writeLeave(writer, (uint64_t) (intra_evs[lid].time[2]*1.0e+10),
                          intra_evs[lid].x_reg2+1, lid+1, 0);
    intra_evs[lid].e_reg2 = ELG_NO_ID;
    intra_evs[lid].x_reg2 = ELG_NO_ID;
  }
  OTF_Writer_writeLeave(writer, (uint64_t) (time*1.0e+10), n->reg+1, lid+1, 0);
}

#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  static void CHECK_FOR_INCORRECT_FLUSH_EVENTS(elg_ui4 lid) {
    if ( flush_ts[lid].time[0] != 0.0 ) {
      genWriteENTER(lid, flush_ts[lid].time[0], state_tracing);
      genWriteCOUNTER(lid, flush_ts[lid].time[0],
                      flush_ts[lid].metc, flush_ts[lid].metv);
      genWriteLEAVE(lid, flush_ts[lid].time[1]);
      genWriteCOUNTER(lid, flush_ts[lid].time[1],
                      flush_ts[lid].metc, flush_ts[lid].metv);
      flush_ts[lid].time[0] = 0.0;
      flush_ts[lid].time[1] = 0.0;
      flush_ts[lid].metc    = 0;
      flush_ts[lid].metv    = 0;
    }
  }
#else
#  define CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid)
#endif

/* -- Attributes -- */

void elg_readcb_ATTR_UI1(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui1 val,       /* value                       */
                      void* userdata)
{ 
}

void elg_readcb_ATTR_UI4(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui4 val,       /* value                       */
                      void* userdata)
{ 
}

/* -- Region -- */

void elg_readcb_ENTER(elg_ui4 lid, elg_d8 time, elg_ui4 rid,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  if ( need_usr && stk_top(lid)->reg == ELG_NO_ID ) {
    genWriteENTER(lid, time, state_user);
  }
  genWriteENTER(lid, time, rid);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_ENTER_CS(elg_ui4 lid, elg_d8 time, elg_ui4 csid,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  if ( need_usr && stk_top(lid)->reg == ELG_NO_ID ) {
    genWriteENTER(lid, time, state_user);
  }
  stk_push(lid, csitetab[csid].erid, time);
  OTF_Writer_writeEnter(writer, (uint64_t) (time*1.0e+10),
                        csitetab[csid].erid+1, lid+1, scl_of_csite(csid));
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_EXIT(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  lid = locmap[lid];
  genWriteLEAVE(lid, time);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

/* -- MPI-1 -- */

void elg_readcb_MPI_SEND(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 cid,
       elg_ui4 tag, elg_ui4 sent, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  OTF_Writer_writeSendMsg(writer, (uint64_t) (time*1.0e+10), lid+1, dlid+1,
                          FIRST_COMM_ID+cid, tag, sent, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_RECV(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 cid,
       elg_ui4 tag, void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  OTF_Writer_writeRecvMsg(writer, (uint64_t) (time*1.0e+10), lid+1, slid+1,
                          FIRST_COMM_ID+cid, tag, 0, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {

  lid = locmap[lid];
  genWriteCOLLLEAVE(genmpiglop,
                    lid, time, rlid, FIRST_COMM_ID+cid, sent, recvd);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

/* -- MPI-1 Non-blocking -- */

void elg_readcb_MPI_SEND_COMPLETE(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

void elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

void elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

void elg_readcb_MPI_CANCELLED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
}

/* -- MPI-2 One-sided Operations -- */

void elg_readcb_MPI_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  OTF_Writer_writeSendMsg(writer, (uint64_t) (time*1.0e+10), lid+1, dlid+1,
                          FIRST_COMM_ID+wid, (rmatag+1), nbytes, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  OTF_Writer_writeRecvMsg(writer, (uint64_t) (time*1.0e+10), lid+1, slid+1,
                          FIRST_COMM_ID+wid, (rmatag+1), 0, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TO(elg_ui4 lid, elg_d8 time, elg_ui4 rmaid,
       void* userdata) {
  /* Do nothing right now
  lid = locmap[lid];
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
  */
}

void elg_readcb_MPI_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  OTF_Writer_writeSendMsg(writer, (uint64_t) (time*1.0e+10), lid+1, dlid+1,
                          FIRST_COMM_ID+wid, rmatag, nbytes, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  OTF_Writer_writeRecvMsg(writer, (uint64_t) (time*1.0e+10), lid+1, slid+1,
                          FIRST_COMM_ID+wid, rmatag, 0, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_WINEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, elg_ui4 cid, elg_ui1 synex,
       void* userdata) {
  lid = locmap[lid];
  genWriteLEAVE(lid, time);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_WINCOLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, void* userdata) {
  lid = locmap[lid];
  genWriteCOLLLEAVE(1, lid, time, ELG_NO_ID, FIRST_COMM_ID+wintab[wid], 0, 0);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_MPI_WIN_LOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, elg_ui1 ltype, void* userdata) {
  /* Do nothing right now
  lid = locmap[lid];
  llid = locmap[llid];
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
  */
}

void elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, void* userdata) {
  /* Do nothing right now
  lid = locmap[lid];
  llid = locmap[llid];
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
  */
}

/* -- Generic One-sided Operations -- */

void elg_readcb_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  OTF_Writer_writeSendMsg(writer, (uint64_t) (time*1.0e+10), lid+1, dlid+1,
                          0, (rmatag+3), nbytes, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  OTF_Writer_writeRecvMsg(writer, (uint64_t) (time*1.0e+10), lid+1, slid+1,
                          0, (rmatag+3), 0, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  OTF_Writer_writeSendMsg(writer, (uint64_t) (time*1.0e+10), lid+1, dlid+1,
                          0, (rmatag+2), nbytes, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  OTF_Writer_writeRecvMsg(writer, (uint64_t) (time*1.0e+10), lid+1, slid+1,
                          0, (rmatag+2), 0, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
  lid = locmap[lid];
  genWriteCOLLLEAVE(genmpiglop,
                    lid, time, rlid, FIRST_COMM_ID+cid, sent, recvd);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR OTF
  lid = locmap[lid];
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
  */
}

void elg_readcb_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR OTF
  lid = locmap[lid];
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
  */
}

/* -- OpenMP -- */

void elg_readcb_OMP_FORK(elg_ui4 lid, elg_d8 time, void* userdata) {
  lid = locmap[lid];
  OTF_Writer_writeEnter(writer, (uint64_t) (time*1.0e+10), state_pregion+1,
	                  lid+1, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_OMP_JOIN(elg_ui4 lid, elg_d8 time, void* userdata) {
  lid = locmap[lid];
  OTF_Writer_writeLeave(writer, (uint64_t) (time*1.0e+10), state_pregion+1,
                        lid+1, 0);
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_OMP_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR OTF
  lid = locmap[lid];
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
  */
}

void elg_readcb_OMP_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR OTF
  lid = locmap[lid];
  lasttime = time;
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
  */
}

void elg_readcb_OMP_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  StackNode *n;

  lid = locmap[lid];
  n = stk_top(lid);
  genWriteCOLLLEAVE(genompglop && is_glop(n->reg), lid, time,
                    ELG_NO_ID, FIRST_TGRP_ID+loctab[lid].proc->id, 0, 0);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

/* -- EPILOG Internal -- */

void elg_readcb_LOG_OFF(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  lid = locmap[lid];
  genWriteENTER(lid, time, state_off);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_LOG_ON(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  lid = locmap[lid];
  genWriteLEAVE(lid, time);
  genWriteCOUNTER(lid, time, metc, metv);
  CHECK_FOR_INCORRECT_FLUSH_EVENTS(lid);
}

void elg_readcb_ENTER_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  if ( is_mpi_init_fin(stk_top(lid)->reg) ) {
#endif
    genWriteENTER(lid, time, state_tracing);
    genWriteCOUNTER(lid, time, metc, metv);
#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  } else {
    flush_ts[lid].time[0] = time;
  }
#endif
}

void elg_readcb_EXIT_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  if ( flush_ts[lid].time[0] == 0.0 ) {
#endif
    genWriteLEAVE(lid, time);
    genWriteCOUNTER(lid, time, metc, metv);
#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  } else {
    flush_ts[lid].time[1] = time;
    flush_ts[lid].metc    = metc;
    flush_ts[lid].metv    = metv;
  }
#endif
}

void gen_otf_makerfile(FILE *sf) {
#ifdef OTF_MARKER_RECORD
  static char line[1024], pattern[128], msg[1024];
  int cnode, mloc;
  double mtime;
  int notDef = 1;

  while ( fgets(line, 1024, sf) ) {
    if ( line[0] == '-' ) {
      sscanf(line, "- %*s %d %*s %*g %*s %*g %*s %*g %*s %lg %*s %d",
             &cnode, &mtime, &mloc);
      if ( notDef ) {
        OTF_Writer_writeDefMarker(writer, 0, 1,
                                  "Severe Performance Pattern Instance", 0);
        notDef = 0;
      }
      sprintf(msg, "'%s' at cnode %d", pattern, cnode);
      OTF_Writer_writeMarker(writer, (mtime*1.0e+10), mloc+1, 1, msg);
    } else {
      sscanf(line, "%s", pattern);
    }
  }
#endif
}

/*
 * M A I N
 */

void conv_setup() {
  int lid;

  if ( isthreaded && state_idle != -1 && addidle ) {
    for(lid=0; lid<totallocs; lid++) {
      if ( loctab[lid].proc->member[0].id != lid ) {
        /*slave thread*/
        OTF_Writer_writeEnter(writer, (uint64_t) 0, state_idle+1,
                              locmap[lid]+1, 0);
      }
    }
  }
}

int main(int argc, char **argv) {
  char*   infile;
  char*   outfile;
  char*   dot;
  int     ch;
  int     eflag = 0;
  struct stat statbuf;
  elg_ui4 numrec = 0;
  elg_ui4 limrec = 0xFFFFFFFF;
  ElgRCB* handle = NULL;
  MYDATA  md = { 0, 0, 0, 0 };
  OTF_FileCompression compression= OTF_FILECOMPRESSION_COMPRESSED;
  unsigned int nstreams = 0;
  unsigned int maxfilehandles = 512;
  uint64_t timres = 0;
  OTF_FileManager* manager;

  while ((ch = getopt(argc, argv, "ghin:r:Gz")) != EOF) {
    switch (ch) {
    case 'g': genompglop = 0;
              break;
    case 'G': genmpiglop = 0;
              genompglop = 0;
              break;
    case 'i': addidle = 0;
              break;
    case 'n': limrec = atol(optarg);
              break;
    case 'r': rmatag = atol(optarg);
              break;
    case 'z': compression = OTF_FILECOMPRESSION_UNCOMPRESSED;
              break;
    case 'h':
    case '?': eflag = 1;
              break;
    }
  }

  if ((argc-optind) == 2) {
    infile  = epk_get_elgfilename(argv[optind]);
    outfile = argv[optind+1];
  } else  if ((argc-optind) == 1) {
    infile  = epk_get_elgfilename(argv[optind]);
    outfile = strdup(infile);
    dot = strrchr(outfile, '.');
    *(dot + 1) = 'o';
    *(dot + 2) = 't';
    *(dot + 3) = 'f';
  } else {
    eflag = 1;
  }

  if ( eflag ) {
    fprintf(stderr, "Usage:   %s [-i] [-g|-G] [-n #] [-r #] [-z]"
                    " (<infile>.elg | <experiment_archive>) [<outfile>]\n",
		    argv[0]);
    fprintf(stderr, "Options: -g do not generate globalops for OMP barriers\n");
    fprintf(stderr, "         -G do not generate globalops (for MPI + OMP)\n");
    fprintf(stderr, "         -i do not add idle state\n");
    fprintf(stderr, "         -n stop converting after # records\n");
    fprintf(stderr, "         -r use # as tag for RMA operations\n");
    fprintf(stderr, "         -z do not compress OTF traces\n");
    fprintf(stderr, "If <outfile> is omitted, <infile>.otf is used for output\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  handle = elg_read_open(infile);
  if ( ! handle ) exit(EXIT_FAILURE);
  stat(infile, &statbuf);

  manager = OTF_FileManager_open(maxfilehandles);
  if ( (writer = OTF_Writer_open(outfile, nstreams, manager)) == 0 ) {
    fprintf(stderr, "ERROR: Can't open output file %s\n", outfile);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
  OTF_Writer_setCompression(writer, compression);
  OTF_Writer_writeDefCreator(writer, 0, "epilog -> otf converter 1.0");
  /* protable stupid way of generating 10000000000 */
  timres = 100000;
  timres *= 100000;
  OTF_Writer_writeDefTimerResolution(writer, 0, timres);

  while ( elg_read_next(handle, (void*) &md) ) {
    numrec++;

    if ( numrec % 1000 == 0 ) {
      printf("done: %3.0f%%\r", ((double) md.done / statbuf.st_size) * 100.0);
      fflush(stdout);
    }

    if ( numrec > limrec ) {
      printf("Limit of %u records reached.\n", limrec);
      break;
    }
  }

  {
  int lid;
  if ( isthreaded && state_idle != -1 && addidle ) {
    for(lid=0; lid<totallocs; lid++) {
      if ( loctab[lid].proc->member[0].id != lid ) {
        OTF_Writer_writeLeave(writer, (uint64_t) (lasttime*1.0e+10),
	                      state_idle+1, locmap[lid]+1, 0);
      }
    }
  }
  }

  /* Check for *.stat file and generate extra OTF markers */
  /* Requires OTF 1.5 or later                            */
  {
    char *statfile = x_malloc(strlen(infile) + 2);
    strcpy(statfile, infile);
    dot = strrchr(statfile, '.');
    strcpy(dot, ".stat");
    if ( access(statfile, R_OK) == 0 ) {
      FILE* sf = fopen(statfile, "r");
      gen_otf_makerfile(sf);
      fclose(sf);
    }
  }

  elg_read_close(handle);
  OTF_Writer_close(writer);
  OTF_FileManager_close(manager);
  return 0;
}
