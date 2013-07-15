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
/* EPILOG Converter VTF3 Backend                                             */
/*                                                                           */
/* Version 1.0  July 2002                                                    */
/* - basic working version                                                   */
/*                                                                           */
/* Version 1.1  March 2004                                                   */
/* - communicator def records added for VNG                                  */
/* - Performance Counter Records                                             */
/*                                                                           */
/* Version 1.2  March 2005                                                   */
/* - use new VTF3 OpenMP events                                              */
/*                                                                           */
/* Version 1.3  April 2005                                                   */
/* - support for MPI2 RMA                                                    */
/*                                                                           */
/* NOT YET HANDLED:                                                          */
/* - Processor and Thread Name Records                                       */
/*                                                                           */
/*===========================================================================*/


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

#include "elg_readcb.h"
#include "elg_conv.h"
#include "vtf3.h"
#include "epk_archive.h"

#ifndef VTF3_RECTYPE_OPENMPENTER
#  define VTF3_WriteDefopenmptype(f,ttk,na) 0
#  define VTF3_WriteDefopenmpname(f,ntk,na) 0
#  define VTF3_WriteOpenmpenter(f,ti,cpu,ttk,ntk,scl) 0
#  define VTF3_WriteOpenmpleave(f,ti,cpu,scl) 0
#else
#  define VTF3_HAS_OPENMP
#endif

static off_t   wbytes = 0;
static elg_d8  lasttime = 0.0;
static void*   fcb;    /* vtf3 file control block */
static int     rmatag = 101010;

/*
 * Files
 */

static void conv_write_files() {
  int i;

  for (i=0; i<numfile; i++) {
    if ( filetab[i] ) VTF3_WriteDefsclfile(fcb, (int) i, filetab[i]);
  }
}

/*
 * SCLs
 */

static void conv_write_scls() {
  int i;

  for (i=(VTF3_SCLNONE+1); i<sclid; i++) {
    wbytes += VTF3_WriteDefscl(fcb, i, 1, &(scltab[i].fid), &(scltab[i].line));
  }
}

/*
 * Activities
 */

static void conv_write_activities() {
  int i;

  for (i=0; i<numact; i++) {
    if ( acttab[i].id == ELG_NO_ID ) break;
    wbytes += VTF3_WriteDefact(fcb, (int) acttab[i].id, acttab[i].name);
  }

  if ( writeOMP ) {
    for (i=ELG_OMP_PARALLEL; i<=ELG_OMP_SINGLE_SBLOCK; i++) {
      wbytes += VTF3_WriteDefopenmptype(fcb, i, rtypenames[i]);
    }
  }
}

/*
 * States
 */

static int scl_of_state(elg_ui4 rid) {
  int scl = statetab[rid].sclid;
  return ( (scl==-1) ? VTF3_SCLNONE : scl );
}

static int scl_of_csite(elg_ui4 rid) {
  int scl = csitetab[rid].sclid;
  return ( (scl==-1) ? VTF3_SCLNONE : scl );
}

static void conv_write_states() {
  int i;

  for (i=0; i<numstate; i++) {
    if ( writeOMP && statetab[i].act == act_omp
	          && statetab[i].type != ELG_FUNCTION ) {
      wbytes += VTF3_WriteDefopenmpname(fcb,
                                 (int) statetab[i].id, statetab[i].name);
    } else {
      wbytes += VTF3_WriteDefstate(fcb, (int) statetab[i].act,
                                 (int) statetab[i].id, statetab[i].name,
                                 VTF3_SCLNONE);
    }
  }
}

/*
 * GlobalOps
 */

static void conv_write_glops() {
  int i;

  for (i=0; i<numglop; i++) {
    wbytes += VTF3_WriteDefglobalop(fcb, (int) gloptab[i].id, gloptab[i].name);
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
      array[i] = group_print(g->member + i);
    }
    wbytes += VTF3_WriteDefcpugrp(fcb, ++grpid, g->num_member, array, g->name);
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
    for (n=0; n<totallocs; ++n) array[n] = n;
    wbytes += VTF3_WriteDefcpugrp(fcb, ++grpid, totallocs, array, "all_flat");
    samplegroup = grpid;
  }
}

static void conv_write_cpunames() {
  int i, t;

  if ( ! isthreaded ) {
    for (i=0; i<totallocs; i++) {
      wbytes += VTF3_WriteDefcpuname(fcb, i, loctab[i].proc->name);
    }
  } else {
    for (i=0; i<totallocs; i++) {
      for (t=0; t<totallocs; t++) {
        if ( i == locmap[t] ) {
          wbytes += VTF3_WriteDefcpuname(fcb, i, loctab[t].thrd->name);
          break;
        }
      }
    }
  }
}

/*
 * Communicators
 */

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
          array[num++] = get_comm_loc(j);
      }
      commtab[i].numarray = num;

      if ( commtab[i].mode & ELG_GROUP_FLAG )
        sprintf(name, "group %d", i);
            else
        sprintf(name, "comm %d", i);
      if (num) wbytes += VTF3_WriteDefcpugrp(fcb, ++grpid, num, array, name);
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
          array[t] = proc->member[t].id;
        }
        sprintf(name, "tgroup %d", /*maxcomm + 1 +*/ proc->id);
        wbytes += VTF3_WriteDefcpugrp(fcb, ++grpid, proc->num_member,
                                      array, name);
      }
    }
  }
}

static void conv_write_comm_2() {
  int low = 0, high = 0, step = -1;
  int get_step = 0;
  int num = 0;
  int i, c;
  unsigned int* trips;

  /*MPI Communicators*/
  for (c=0; c<numcomm; c++) {
    if ( commtab[c].numbyte ) {
      low = 0; high = 0; step = -1;
      get_step = 0;
      num = 0;

      for (i=0; i<commtab[c].numarray; i++) {
        if (get_step ) {
          high = commtab[c].array[i];
          step = high - low;
          get_step = 0;
        } else if ( i &&
                    (commtab[c].array[i] == (commtab[c].array[i-1] + step)) ) {
          high = commtab[c].array[i];
        } else {
          num++;
          low = high = commtab[c].array[i];
          step = 1;
          get_step = 1;
        }
      }
      if (num) {
        trips = x_malloc(3 * num * sizeof(unsigned int));

        low = 0; high = 0; step = -1;
        get_step = 0;
        num = 0;
        for (i=0; i<commtab[c].numarray; i++) {
          if (get_step ) {
            high = commtab[c].array[i];
            step = high - low;
            get_step = 0;
          } else if ( i &&
                      (commtab[c].array[i] == (commtab[c].array[i-1]+step)) ) {
            high = commtab[c].array[i];
          } else {
            if ( num ) {
              trips[(num-1)*3  ] = low;
              trips[(num-1)*3+1] = high;
              trips[(num-1)*3+2] = step;
            }
            num++;
            low = high = commtab[c].array[i];
            step = 1;
            get_step = 1;
          }
        }
        trips[(num-1)*3  ] = low;
        trips[(num-1)*3+1] = high;
        trips[(num-1)*3+2] = step;
        wbytes += VTF3_WriteDefcommunicator(fcb, c, commtab[c].numarray,
                                            num, trips);
      }
    }
  }

  /*OpenMP Thread Groups*/
  if ( isthreaded ) {
    int n, p, t;
    for (n=0; n<machine[0].num_member; n++) {
      Group *node = machine[0].member + n;
      for (p=0; p<node->num_member; p++) {
        Group *proc = node->member + p;
        unsigned int trip[3];
        for (t=0; t<proc->num_member; t++) {
          int tid = proc->member[t].id;
          if (t==0) {
            trip[0] = trip[1] = tid;
            trip[2] = 1;
          } else {
            if (tid < trip[0]) trip[0] = tid;
            if (tid > trip[1]) trip[1] = tid;
          }
        }
        wbytes += VTF3_WriteDefcommunicator(fcb, maxcomm + 1 + proc->id,
                                            proc->num_member, 1, trip);
      }
    }
  }
}

/*
 * Metrics
 */

static int type2vtf[] = { VTF3_VALUETYPE_UINT, VTF3_VALUETYPE_FLOAT };
static struct { elg_ui8 low, high; } intlim = { 0, (elg_ui8)~(elg_ui8)0 };
static struct { elg_d8  low, high; } fltlim = { -1.0e+300, +1.0e300 };

static int* metids;
static int* metvts;

static void conv_write_metrics() {
  int i, j = 0;

  if ( maxmet != -1 ) {
    metids = x_malloc((maxmet+1) * sizeof(int));
    metvts = x_malloc((maxmet+1) * sizeof(int));

    wbytes += VTF3_WriteDefsampclass(fcb, 0, "HW Counters");

    for (i=0; i<nummet; i++) {
      if ( mettab[i].name ) {
        metids[j] = i;
        metvts[j++] = type2vtf[mettab[i].type];
        wbytes += VTF3_WriteDefsamp(fcb, i, 0, 1, samplegroup,
              type2vtf[mettab[i].type],
              (mettab[i].type == ELG_INTEGER ? (void*)&intlim : (void*)&fltlim),
              (mettab[i].iv == ELG_START), VTF3_DATAREPHINT_BEFORE,
              mettab[i].name, (mettab[i].iv == ELG_START ? "#/s" : ""));
      }
    }
  }
}

/*
 * Windows
 */

static void conv_write_windows() {
}

/*
 * Definition Records
 */

void conv_write_definition_records() {
  if ( nummachines > 1 ) {
    int i;
    int* numcpus = x_malloc(nummachines + sizeof(int));

    for (i=0; i<nummachines; i++) numcpus[i] = 0;
    for (i=0; i<totallocs; i++) numcpus[loctab[i].mach->id]++;
    wbytes += VTF3_WriteDefsyscpunums(fcb, nummachines, numcpus);
  } else {
    wbytes += VTF3_WriteDefsyscpunums(fcb, 1, &totallocs);
  }
  conv_write_cpunames();
  conv_write_machines();
  conv_write_comms();
  conv_write_files();
  conv_write_scls();
  conv_write_activities();
  conv_write_states();
  conv_write_metrics();
  conv_write_comm_2();
  conv_write_glops();
  wbytes += VTF3_WriteDefclkperiod(fcb, 1.0e-10);
}

/*
 * EPILOG read callbacks
 */

/* -- Helpers -- */

static void genWriteCOUNTER(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]) {
  if ( metc ) {
    wbytes += VTF3_WriteSamp(fcb, time*1.0e+10, lid,
                             metc, metids, metvts, metv);
  }
  lasttime = time;
}

static void genWriteCOLLLEAVE(int w, elg_ui4 lid, elg_d8 time,
       elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd) {
  StackNode *n = stk_pop(lid);

  if ( rlid != ELG_NO_ID ) rlid = locmap[rlid];
  if ( w ) {
    elg_d8 stime = (n->tim)*1.0e+10+1.0;
    elg_d8 etime = time*1.0e+10-1.0;
    if ( etime <= stime ) {
      stime = (n->tim)*1.0e+10;
      etime = time*1.0e+10;
    }
    wbytes += VTF3_WriteGlobalop(fcb, stime, n->reg,
                                 (unsigned int) lid, (int) cid,
                                 (unsigned int) rlid, (int) sent, (int) recvd,
                                 etime-stime, VTF3_SCLNONE);
  }
  wbytes += VTF3_WriteUpfrom(fcb, time*1.0e+10, n->reg, (unsigned int) lid,
                             VTF3_SCLNONE);
}

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
    stk_push(lid, state_user, time);
    wbytes += VTF3_WriteDownto(fcb, time*1.0e+10, state_user,
                               (unsigned int) lid, VTF3_SCLNONE);
  }
  stk_push(lid, rid, time);
  if ( writeOMP && statetab[rid].act == act_omp
                && statetab[rid].type != ELG_FUNCTION ) {
    wbytes += VTF3_WriteOpenmpenter(fcb, time*1.0e+10, (unsigned int) lid,
	                            (unsigned int) statetab[rid].type,
                                    (unsigned int) rid,
                                    scl_of_state(rid));
  } else {
    wbytes += VTF3_WriteDownto(fcb, time*1.0e+10, (int) rid,
                              (unsigned int) lid, scl_of_state(rid));
  }
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_ENTER_CS(elg_ui4 lid, elg_d8 time, elg_ui4 csid,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  if ( need_usr && stk_top(lid)->reg == ELG_NO_ID ) {
    stk_push(lid, state_user, time);
    wbytes += VTF3_WriteDownto(fcb, time*1.0e+10, state_user,
                               (unsigned int) lid, VTF3_SCLNONE);
  }
  stk_push(lid, csitetab[csid].erid, time);
  wbytes += VTF3_WriteDownto(fcb, time*1.0e+10, (int) csitetab[csid].erid,
                             (unsigned int) lid, scl_of_csite(csid));
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_EXIT(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  StackNode *n;

  lid = locmap[lid];
  n = stk_pop(lid);
  if ( writeOMP && statetab[n->reg].act == act_omp
                && statetab[n->reg].type != ELG_FUNCTION ) {
    wbytes += VTF3_WriteOpenmpleave(fcb, time*1.0e+10,
                               (unsigned int) lid, scl_of_state(n->reg));
  } else {
    wbytes += VTF3_WriteUpfrom(fcb, time*1.0e+10, (int) n->reg,
                               (unsigned int) lid, VTF3_SCLNONE);
  }
  genWriteCOUNTER(lid, time, metc, metv);
}

/* -- MPI-1 -- */

void elg_readcb_MPI_SEND(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 cid,
       elg_ui4 tag, elg_ui4 sent, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  wbytes += VTF3_WriteSendmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) dlid, (int) cid,
                              (int) tag, (int) sent, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_MPI_RECV(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 cid,
       elg_ui4 tag, void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  wbytes += VTF3_WriteRecvmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) slid, (int) cid,
                              (int) tag, /*???*/ 0, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_MPI_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
  lid = locmap[lid];
  genWriteCOLLLEAVE(genmpiglop, lid, time, rlid, cid, sent, recvd);
  genWriteCOUNTER(lid, time, metc, metv);
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
  wbytes += VTF3_WriteSendmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) dlid, (int) wid,
                              (int) (rmatag+1), (int) nbytes, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_MPI_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  wbytes += VTF3_WriteRecvmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) slid, (int) wid,
                              (int) (rmatag+1), /*???*/ 0, VTF3_SCLNONE);
  lasttime = time;
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
  */
}

void elg_readcb_MPI_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  wbytes += VTF3_WriteSendmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) dlid, (int) wid,
                              (int) rmatag, (int) nbytes, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  wbytes += VTF3_WriteRecvmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) slid, (int) wid,
                              (int) rmatag, /*???*/ 0, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_MPI_WINEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, elg_ui4 cid, elg_ui1 synex,
       void* userdata) {
  StackNode *n;

  lid = locmap[lid];
  n = stk_pop(lid);
  wbytes += VTF3_WriteUpfrom(fcb, time*1.0e+10, (int) n->reg,
                             (unsigned int) lid, VTF3_SCLNONE);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_MPI_WINCOLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, void* userdata) {
  lid = locmap[lid];
  genWriteCOLLLEAVE(1, lid, time, -1, wintab[wid], 0, 0);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_MPI_WIN_LOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, elg_ui1 ltype, void* userdata) {
  /* Do nothing right now
  lid = locmap[lid];
  llid = locmap[llid];
  lasttime = time;
  */
}

void elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, void* userdata) {
  /* Do nothing right now
  lid = locmap[lid];
  llid = locmap[llid];
  lasttime = time;
  */
}

/* -- Generic One-sided Operations -- */

void elg_readcb_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  wbytes += VTF3_WriteSendmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) dlid, -1,
                              (int) (rmatag+3), (int) nbytes, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  wbytes += VTF3_WriteRecvmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) slid, -1,
                              (int) (rmatag+3), /*???*/ 0, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  lid = locmap[lid];
  dlid = locmap[dlid];
  wbytes += VTF3_WriteSendmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) dlid, -1,
                              (int) (rmatag+2), (int) nbytes, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  lid = locmap[lid];
  slid = locmap[slid];
  wbytes += VTF3_WriteRecvmsg(fcb, time*1.0e+10, (unsigned int) lid,
                              (unsigned int) slid, -1,
                              (int) (rmatag+2), /*???*/ 0, VTF3_SCLNONE);
  lasttime = time;
}

void elg_readcb_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
  lid = locmap[lid];
  genWriteCOLLLEAVE(genmpiglop, lid, time, rlid, cid, sent, recvd);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR VTF3
  lid = locmap[lid];
  lasttime = time;
  */
}

void elg_readcb_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR VTF3
  lid = locmap[lid];
  lasttime = time;
  */
}

/* -- OpenMP -- */

void elg_readcb_OMP_FORK(elg_ui4 lid, elg_d8 time, void* userdata) {
  lid = locmap[lid];
  if ( !writeOMP ) {
    wbytes += VTF3_WriteDownto(fcb, time*1.0e+10, state_pregion,
                               (unsigned int) lid, VTF3_SCLNONE);
  }
  lasttime = time;
}

void elg_readcb_OMP_JOIN(elg_ui4 lid, elg_d8 time, void* userdata) {
  lid = locmap[lid];
  if ( !writeOMP ) {
    wbytes += VTF3_WriteUpfrom(fcb, time*1.0e+10, state_pregion,
                               (unsigned int) lid, VTF3_SCLNONE);
  }
  lasttime = time;
}

void elg_readcb_OMP_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR VTF3
  lid = locmap[lid];
  lasttime = time;
  */
}

void elg_readcb_OMP_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR VTF3
  lid = locmap[lid];
  lasttime = time;
  */
}

void elg_readcb_OMP_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  StackNode *n;

  lid = locmap[lid];
  n = stk_pop(lid);

  if ( genompglop && is_glop(n->reg) ) {
    elg_d8 stime = (n->tim)*1.0e+10+1.0;
    elg_d8 etime = time*1.0e+10-1.0;
    if ( etime <= stime ) {
      stime = (n->tim)*1.0e+10;
      etime = time*1.0e+10;
    }
    wbytes += VTF3_WriteGlobalop(fcb, stime, n->reg,
                       (unsigned int) lid,
                       /*com*/ maxcomm + 1 + loctab[lid].proc->id,
                       /*root*/ -1, /*sent*/ 0, /*recvd*/ 0,
                       etime-stime, VTF3_SCLNONE);
  }
  if ( writeOMP ) {
    wbytes += VTF3_WriteOpenmpleave(fcb, time*1.0e+10, (unsigned int) lid,
                     scl_of_state(n->reg));
  } else {
    wbytes += VTF3_WriteUpfrom(fcb, time*1.0e+10, n->reg, (unsigned int) lid,
                     VTF3_SCLNONE);
  }
  genWriteCOUNTER(lid, time, metc, metv);
}

/* -- EPILOG Internal -- */

void elg_readcb_LOG_OFF(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  lid = locmap[lid];
  wbytes += VTF3_WriteDownto(fcb, time*1.0e+10, state_off,
                             (unsigned int) lid, VTF3_SCLNONE);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_LOG_ON(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  lid = locmap[lid];
  wbytes += VTF3_WriteUpfrom(fcb, time*1.0e+10, state_off,
                             (unsigned int) lid, VTF3_SCLNONE);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_ENTER_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  wbytes += VTF3_WriteDownto(fcb, time*1.0e+10, state_tracing,
                             (unsigned int) lid, VTF3_SCLNONE);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_EXIT_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  wbytes += VTF3_WriteUpfrom(fcb, time*1.0e+10, state_tracing,
                             (unsigned int) lid, VTF3_SCLNONE);
  genWriteCOUNTER(lid, time, metc, metv);
}

/*
 * M A I N
 */

static void conv_init() {
  sclid = VTF3_SCLNONE + 1;
  grpid = 0x1u << 31;
}

void conv_setup() {
  int lid;

  if ( state_idle != -1 && addidle ) {
    for(lid=0; lid<totallocs; lid++) {
      if ( loctab[lid].proc->member[0].id != lid ) {
        /*slave thread*/
        wbytes += VTF3_WriteDownto(fcb, 0.0, state_idle,
                                   locmap[lid], VTF3_SCLNONE);
      }
    }
  }
}

int main(int argc, char **argv) {
  char*   infile;
  char*   outfile;
  char*   dot;
  int     ch;
  int     fmt = VTF3_FILEFORMAT_STD_ASCII;
  int     eflag = 0;
  struct stat statbuf;
  elg_ui4 numrec = 0;
  elg_ui4 limrec = 0xFFFFFFFF;
  elg_ui4 limsiz = 0xFFFFFFFF;
  ElgRCB* handle = NULL;
  MYDATA  md = { 0, 0, 0, 0 };

  while ((ch = getopt(argc, argv, "abfghin:r:s:GO")) != EOF) {
    switch (ch) {
    case 'a': fmt = VTF3_FILEFORMAT_STD_ASCII;
              break;
    case 'b': fmt = VTF3_FILEFORMAT_STD_BINARY;
              break;
    case 'f': fmt = VTF3_FILEFORMAT_FST_ASCII;
              break;
    case 'g': genompglop = 0;
              break;
    case 'G': genmpiglop = 0;
              genompglop = 0;
              break;
    case 'i': addidle = 0;
              break;
    case 'n': limrec = atol(optarg);
              break;
    case 'O':
#ifdef VTF3_HAS_OPENMP
	      writeOMP = 1;
              genompglop = 0;
#endif
	      break;
    case 's': limsiz = atol(optarg);
              break;
    case 'r': rmatag = atol(optarg);
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
    *(dot + 1) = 'v';
    *(dot + 2) = 'p';
    *(dot + 3) = 't';
  } else {
    eflag = 1;
  }

  if ( eflag ) {
    fprintf(stderr, "Usage:   %s [-a|-b|-f] [-i] [-g|-G] [-O] [-n #] [-s #]"
                    " [-r #]"
                    " (<infile>.elg | <experiment_archive>) [<outfile>]\n",
                    argv[0]);
    fprintf(stderr, "Options: -a force standard ascii output\n");
    fprintf(stderr, "         -b force binary output\n");
    fprintf(stderr, "         -f force fast ascii output\n");
    fprintf(stderr, "         -g do not generate globalops for OMP barriers\n");
    fprintf(stderr, "         -G do not generate globalops (for MPI + OMP)\n");
    fprintf(stderr, "         -i do not add idle state\n");
    fprintf(stderr, "         -O generate VTF3 OpenMP events\n");
    fprintf(stderr, "         -n stop converting after # records\n");
    fprintf(stderr, "         -s stop converting after # bytes written\n");
    fprintf(stderr, "         -r use # as tag for RMA operations\n");
    fprintf(stderr, "If <outfile> is omitted, <infile>.vpt is used for output\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  handle = elg_read_open(infile);
  if ( ! handle ) exit(EXIT_FAILURE);
  stat(infile, &statbuf);

  conv_init();
  VTF3_InitTables();
  if ( (fcb = VTF3_OpenFileOutput(outfile, fmt, genmpiglop)) == 0 ) {
    fprintf(stderr, "Can't open output file %s\n", outfile);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  wbytes += VTF3_WriteDefversion(fcb, VTF3_GetVersionNumber());
  wbytes += VTF3_WriteDefcreator(fcb, "epilog -> vtf3 converter 1.3");

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
    if ( wbytes > limsiz ) {
      printf("Limit of %ld bytes reached.\n", (long)wbytes);
      break;
    }
  }

  {
  int lid;
  if ( state_idle != -1 && addidle ) {
    for(lid=0; lid<totallocs; lid++) {
      if ( loctab[lid].proc->member[0].id != lid ) {
        wbytes += VTF3_WriteUpfrom(fcb, lasttime*1.0e+10, (int) state_idle,
                                   locmap[lid], VTF3_SCLNONE);
      }
    }
  }
  }

  elg_read_close(handle);
  VTF3_Close(fcb);
  return 0;
}
