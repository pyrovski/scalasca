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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elg_readcb.h"
#include "elg_conv.h"
#include "epk_archive.h"

static int printdecls = 0;

/*
 * String Table Module
 */

/*
static void str_write() {
  int i;

  for (i=0; i<numstring; i++) {
    if ( stringtab[i] ) printf("string %d \"%s\"\n", i, stringtab[i]);
  }
}
*/

/*
 * File Table Module
 */

static void file_write() {
  int i;

  for (i=0; i<numfile; i++) {
    if ( filetab[i] ) printf("file %d \"%s\"\n", i, filetab[i]);
  }
}

/*
 * SCL Table Module
 */

static void scl_write() {
  int i;

  for (i=0; i<sclid; i++) {
    printf("scl %d file %d line %d\n", i, scltab[i].fid, scltab[i].line);
  }
}

/*
 * Activity Table Module
 */

static void act_write() {
  int i;

  for (i=0; i<numact; i++) {
    if ( acttab[i].id == ELG_NO_ID ) return;
    printf("group %u \"%s\"\n", acttab[i].id, acttab[i].name);
  }
}

/*
 * Callsite Table Module
 */

static void csite_write() {
  int i;

  for (i=0; i<numcsite; i++) {
    if ( csitetab[i].erid != ELG_NO_ID )
      printf("callsite %d region %u scl %d\n", i,
             csitetab[i].erid, csitetab[i].sclid);
  }
}

/*
 * State Table Module
 */

static void state_write() {
  int i;

  for (i=0; i<numstate; i++) {
    printf("region %u \"%s\" group %u",
           statetab[i].id, statetab[i].name, statetab[i].act);
    if ( statetab[i].sclid != -1 )
      printf(" scl %d", statetab[i].sclid);
    printf(" %s\n", rtypenames[statetab[i].type]);
  }
}

/*
 * GlobalOp Table Module
 */

static char* gtypenames[] = {
  "COLL_BARRIER", "COLL_ONE2ALL", "COLL_ALL2ONE", "COLL_ALL2ALL", "COLL_OTHER"
};

void glop_write() {
  int i;

  for (i=0; i<numglop; i++) {
    printf("globalop %u \"%s\" %s\n", gloptab[i].id, gloptab[i].name,
	   gtypenames[gloptab[i].rtype - ELG_FUNCTION_COLL_BARRIER]);
  }
}

/*
 * Groups Module
 */

static unsigned int group_print(Group *g) {
  int i;

  if ( g->member ) {
    unsigned int* array = x_malloc(g->num_member * sizeof(unsigned int));
    for (i=0; i<g->num_member; ++i) {
      array[i] = group_print(g->member + i);
    }
    grpid++;
    printf("locationgroup G%u \"%s\" members", (grpid-100000), g->name);
    for (i=0; i<g->num_member; ++i) {
      if ( array[i] > 100000 )
        printf(" G%u", (array[i]-100000));
      else
        printf(" %u", array[i]);
    }
    printf("\n");

    return grpid;
  } else {
    return g->id;
  }
}

static void machine_write() {
  int m;

  grpid = 100000;
  for (m=0; m<nummachines; m++) group_print(machine+m);
}

static void cpuname_write() {
  int i, t;

  if ( isthreaded ) {
    for (i=0; i<totallocs; i++) {
      for (t=0; t<totallocs; t++) {
        if ( i == locmap[t] ) {
          printf("location %d %d:%d:%d:%d \"%s\"\n", i,
                 loctab[t].mach->id, loctab[t].node->id,
                 loctab[t].proc->id, loctab[t].thrd->id,
                 loctab[t].thrd->name);
          break;
        }
      }
    }
  }
}

/*
 * Communicator Table Module
 */

static void comm_write() {
  int i, j;
  int m, n, p, t;

  /* MPI groups (new scheme) */
  for (i=0; i<numMpiGroups; i++) {
    if ( mpiGroupTab[i].numranks || (mpiGroupTab[i].mode & ELG_GROUP_SELF) ) {
      if (printdecls) {
        printf("mpi_group %d members", i);
        if (mpiGroupTab[i].mode & ELG_GROUP_SELF) {
          printf(" 'self'");
        } else {
          for (j=0; j<mpiGroupTab[i].numranks; j++)
            printf(" %u", get_comm_loc(mpiGroupTab[i].ranks[j]));
        }
        printf("\n");
      }
    }
  }

  /* MPI communicators (new scheme) */
  for (i=0; i<numMpiComms; i++) {
    if (printdecls) {
      if ( mpiCommTab[i].group != ELG_NO_ID ) {
        printf("mpi_comm %d group %d\n", i, mpiCommTab[i].group);
      }
    }
  }

  /* MPI Communicators (old scheme) */
  for (i=0; i<numcomm; i++) {
    if ( commtab[i].numbyte || (commtab[i].mode & ELG_GROUP_SELF) ) {
      char *name = x_malloc(16 * sizeof(char));
      unsigned int *array;
      int num = 0;

      for (j=0; j<(commtab[i].numbyte*8); j++) {
        if ( (1 << (j % 8)) & commtab[i].bits[j/8] ) num++;
      }
      if (num) array = x_malloc(num * sizeof(unsigned int));
      num = 0;
      for (j=0; j<(commtab[i].numbyte*8); j++) {
        if ( (1 << (j % 8)) & commtab[i].bits[j/8] )
          array[num++] = get_comm_loc(j);
      }

      if ( commtab[i].mode & ELG_GROUP_FLAG )
        sprintf(name, "group %d", i);
      else
        sprintf(name, "comm %d", i);
      if (printdecls) {
        if ( commtab[i].mode & ELG_GROUP_FLAG )
          printf("mpi_group %d members", i);
        else
          printf("mpi_comm %d members", i);
        if (commtab[i].mode & ELG_GROUP_SELF) {
          printf(" 'self'");
        } else {
          for (j=0; j<num; j++) printf(" %u", array[j]);
        }
        printf("\n");
      }
    }
  }

  /*OpenMP Thread Groups*/
  if ( isthreaded ) {
    for (m=0; m<nummachines; m++) {
      for (n=0; n<machine[m].num_member; n++) {
        Group *node = machine[m].member + n;
        for (p=0; p<node->num_member; p++) {
          Group *proc = node->member + p;
          char *name = x_malloc(16 * sizeof(char));
          unsigned int *array = x_malloc(proc->num_member * sizeof(int));
          for (t=0; t<proc->num_member; t++) {
            array[t] = proc->member[t].id;
          }
          sprintf(name, "tgroup %d", proc->id);
          if (printdecls) {
            printf("threadgroup %d members", proc->id);
            for (j=0; j<proc->num_member; j++) printf(" %u", array[j]);
            printf("\n");
          }
        }
      }
    }
  }
}

/*
 * Metrics
 */

static char *type2str[] = { "INT", "FLOAT" };
static char *mode2str[] = { "COUNTER", "RATE", "SAMPLE" };
static char *iv2str[]   = { "START", "LAST", "NEXT" };

static void metric_write() {
  int i;

  for (i=0; i<nummet; i++) {
    if ( mettab[i].name ) {
      printf("metric %d %s", i, mettab[i].name);
      if ( mettab[i].descr )
        printf(" [%s]",  mettab[i].descr);
      else
        printf(" -");
      printf(" %s %s %s\n", type2str[mettab[i].type],
                            mode2str[mettab[i].mode], iv2str[mettab[i].iv]);
    }
  }
}

/*
 * Windows
 */

static void win_write() {
  int i;

  for (i=0; i<numwin; i++) {
    if ( wintab[i] != -1 ) printf("win %d comm %d\n", i, wintab[i]);
  }
}


/*
 * Topplogy Table Module
 */

static void topol_write() {
  int i, j, k;

  for (i=0; i<numtopol; i++) {
    if ( topoltab[i] ) {
      printf("topology %d comm %u nameid %d dim %d", i, topoltab[i]->cid, topoltab[i]->tnid,
	     topoltab[i]->sizes[0]);
      	  for(k=1;k<topoltab[i]->ndims;k++) {
      		printf("x%d", topoltab[i]->sizes[k]);
      	  }
      printf("\n");
      for (j=0; j<topoltab[i]->numlocs; j++) {
    	  if ( j >= numlocs ) break;
    	  printf("tcoord %4.4s -> (%3u", loctab[j].lstr, topoltab[i]->coords[0]);
    	  for(k=1;k<topoltab[i]->ndims;k++) {
    		  printf(",%3u", topoltab[i]->coords[k]);
    	  }
        printf(")\n");
      }
    }
  }
}


/*
 * Event Statistics
 */

static int numev   = 0;
static int numattr = 0;
static elg_d8 lasttime = 0.0;
static int** evtab;
static int* evtabvals;
static int numtypes =  ELG_LAST_EXTERNAL_EVENT - ELG_ENTER + 1;
static char *typenames[] = {
  "ENTER", "EXIT",
  "MPI_SEND", "MPI_RECV",
  "MPI_COLLEXIT",
  "OMP_FORK", "OMP_JOIN",
  "OMP_ALOCK", "OMP_RLOCK",
  "OMP_COLLEXIT", "ENTER_CS",
  "MPI_PUT_1TS", "MPI_PUT_1TE",
  "MPI_GET_1TS", "MPI_GET_1TE", "MPI_GET_1TO",
  "MPI_WINEXIT", "MPI_WINCOLLEXIT",
  "MPI_WIN_LOCK", "MPI_WIN_UNLOCK",
  "?? MPI_PUT_1TE_REMOTE", "?? MPI_GET_1TS_REMOTE",
  "PUT_1TS", "PUT_1TE",
  "GET_1TS", "GET_1TE",
  "?? PUT_1TE_REMOTE", "?? GET_1TS_REMOTE",
  "COLLEXIT", "ALOCK", "RLOCK",
  "MPI_SEND_COMPLETE", "MPI_RECV_REQUEST",
  "MPI_REQUEST_TESTED", "MPI_CANCELLED"
};
static int** entertab;
static int* entertabvals;
static int printevents = 0;
static int longstats = 0;
static int numprec = 6;

static void event_init(int numlocs) {
  int i, j;

  evtabvals = x_malloc(numlocs * numtypes * sizeof(int));
  evtab     = x_malloc(numlocs * sizeof(int *));
  entertabvals = x_malloc(numlocs * numstate * sizeof(int));
  entertab     = x_malloc(numlocs * sizeof(int *));
  for (i=0; i<numlocs; i++) {
    evtab[i] = evtabvals + (i*numtypes);
    for (j=0; j<numtypes; j++) evtab[i][j] = 0;
    entertab[i] = entertabvals + (i*numstate);
    for (j=0; j<numstate; j++) entertab[i][j] = 0;
  }
}

static
void event_mpi(elg_ui1 type, elg_ui4 lid, elg_ui4 i1, elg_ui4 i2, elg_ui4 c) {
  char *lstr = (lid == ELG_NO_ID ? "-" : loctab[locremap[lid]].lstr);

  if ( printevents ) {
    switch ( type ) {
    case ELG_MPI_SEND:
      printf(" to=%s l=%d t=%d c=%d\n", lstr, i1, i2, c);
      break;
    case ELG_MPI_RECV:
      printf(" from=%s l=%d t=%d c=%d\n", lstr, i1, i2, c);
      break;
    case ELG_COLLEXIT:
    case ELG_MPI_COLLEXIT:
      printf(" root=%s sent=%d recvd=%d c=%d\n", lstr, i1, i2, c);
      break;
    case ELG_MPI_GET_1TS:
    case ELG_MPI_PUT_1TS:
      printf(" to=%s l=%d w=%d id=%d\n", lstr, i1, c, i2);
      break;
    case ELG_MPI_PUT_1TE:
    case ELG_MPI_GET_1TE:
      printf(" from=%s w=%d id=%d\n", lstr, c, i2);
      break;
    case ELG_MPI_GET_1TO:
      printf(" id=%d\n", i2);
      break;
    case ELG_MPI_WIN_LOCK:
      printf(" at=%s w=%d %s\n", lstr, i1,
             (i2==ELG_MPI2_LOCK_SHARED ? "shared" : "excl") );
      break;
    case ELG_MPI_WIN_UNLOCK:
      printf(" at=%s w=%d\n", lstr, i1);
      break;
    case ELG_MPI_WINEXIT:
      printf(" w=%d sync=%d c=%d\n", i1, i2, c);
      break;
    case ELG_MPI_WINCOLLEXIT:
      printf(" w=%d\n", i1);
      break;
    }
  }
}

static
void event_rma(elg_ui1 type, elg_ui4 lid, elg_ui4 len, elg_ui4 id) {
  char *lstr = (lid == ELG_NO_ID ? "-" : loctab[locremap[lid]].lstr);

  if ( printevents ) {
    switch ( type ) {
    case ELG_GET_1TS:
    case ELG_PUT_1TS:
      printf(" to=%s l=%d id=%d\n", lstr, len, id);
      break;
    case ELG_PUT_1TE:
    case ELG_GET_1TE:
      printf(" from=%s id=%d\n", lstr, id);
      break;
    }
  }
}

static void event_add(elg_ui1 type, elg_ui4 lid, elg_ui4 rid, elg_d8 time) {
  elg_ui4 csid;

  lasttime = time;
  numev++;

  if ( type < ELG_ENTER ) {
    /* definition records */
    return;
  } else if ( type < ELG_LOG_OFF ) {
    /* visible event records */
    evtab[lid][type - ELG_ENTER]++;

    if ( type == ELG_ENTER_CS ) {
      csid = rid;
      rid = csitetab[csid].erid;
    }

    if ( type == ELG_ENTER || type == ELG_ENTER_CS ) {
      entertab[lid][rid]++;
    }
  }

  if ( printevents ) {
    printf("%03d: %13.*f %s %s", numev, numprec, time,
	  loctab[locremap[lid]].lstr,
	  (type < ELG_LOG_OFF ?  typenames[type - ELG_ENTER] : "") );
    switch ( type ) {
    case ELG_ENTER:
    case ELG_EXIT:
    case ELG_OMP_COLLEXIT:
      if ( statetab[rid].sclid != -1 ) {
        SCL* scl = &(scltab[statetab[rid].sclid]);
        printf(" %s [%s:%d]\n", statetab[rid].name, filetab[scl->fid],
               scl->line);
      } else {
        printf(" %s\n", statetab[rid].name);
      }
      break;
    case ELG_ENTER_CS:
      if ( csitetab[csid].sclid != -1 ) {
        SCL* scl = &(scltab[csitetab[csid].sclid]);
        printf(" %s [%s:%d]\n", statetab[rid].name, filetab[scl->fid],
               scl->line);
      } else {
        printf(" %s\n", statetab[rid].name);
      }
      break;
    case ELG_COLLEXIT:
    case ELG_MPI_COLLEXIT:
    case ELG_MPI_WINEXIT:
    case ELG_MPI_WINCOLLEXIT:
      printf(" %s", statetab[rid].name);
      break;
    case ELG_ALOCK:
    case ELG_RLOCK:
    case ELG_OMP_ALOCK:
    case ELG_OMP_RLOCK:
      printf(" lock=%d\n", rid);
      break;
    case ELG_MPI_SEND :
    case ELG_MPI_RECV :
    case ELG_MPI_PUT_1TS :
    case ELG_MPI_PUT_1TE :
    case ELG_MPI_GET_1TO :
    case ELG_MPI_GET_1TS :
    case ELG_MPI_GET_1TE :
    case ELG_MPI_WIN_LOCK:
    case ELG_MPI_WIN_UNLOCK:
      break;
    case ELG_PUT_1TS :
    case ELG_PUT_1TE :
    case ELG_GET_1TS :
    case ELG_GET_1TE :
      break;
    case ELG_LOG_OFF:
      printf("LOGGING OFF ****************************************\n");
      break;
    case ELG_LOG_ON:
      printf("LOGGING ON *****************************************\n");
      break;
    case ELG_ENTER_TRACING:
      printf("START TRACING **************************************\n");
      break;
    case ELG_EXIT_TRACING:
      printf("END TRACING ****************************************\n");
      break;
    default:
      printf("\n");
      break;
    }
  }
}

static void event_write() {
  int i, j;
  int sum;

  printf("\n%d events\n",   numev);
  printf("%d attributes\n", numattr);
  printf("eventrate %.1f ev/sec\n", numev/lasttime);
  printf("duration %10.*f sec\n\n", numprec, lasttime);

  for (j=0; j<numtypes; j++) {
    sum = 0;
    for (i=0; i<totallocs; i++) sum += evtab[i][j];
    printf("%21.21s :", typenames[j]);
    printf("%10d :", sum);
    if ( longstats )
      for (i=0; i<totallocs; i++) printf("%8d", evtab[i][j]);
    printf("\n");
  }
  printf("\n");

  for (j=0; j<numstate; j++) {
    sum = 0;
    for (i=0; i<totallocs; i++) sum += entertab[i][j];
    if ( sum ) {
      printf("%30.30s :", statetab[j].name);
      printf("%10d :", sum);
      if ( longstats )
	for (i=0; i<totallocs; i++) printf("%8d", entertab[i][j]);
      printf("\n");
    }
  }
}

/*
 * Definition Records
 */

void conv_write_definition_records() {
  if (printdecls) {
    cpuname_write();
    machine_write();
  }
  comm_write();
  if (printdecls) {
    win_write();
    file_write();
    scl_write();
    act_write();
    state_write();
    csite_write();
    metric_write();
    glop_write();
    topol_write();
    if (numev) printf("\n\n");
  }
}

/*
 * EPILOG read callbacks
 */

/* -- Attributes -- */

void elg_readcb_ATTR_UI1(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui1 val,       /* value                       */
                      void* userdata)
{
  ++numattr;
}

/* -- Attributes -- */

void elg_readcb_ATTR_UI4(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui4 val,       /* value                       */
                      void* userdata)
{ 
  ++numattr;
}


/* -- Region -- */

void elg_readcb_ENTER(elg_ui4 lid, elg_d8 time, elg_ui4 rid,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  stk_push(lid, rid, time);
  event_add(md->type, lid, rid, time);
}

void elg_readcb_ENTER_CS(elg_ui4 lid, elg_d8 time, elg_ui4 csid,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  stk_push(lid, csitetab[csid].erid, time);
  event_add(md->type, lid, csid, time);
}

void elg_readcb_EXIT(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;

  lid = locmap[lid];
  n = stk_pop(lid);
  event_add(md->type, lid, n->reg, time);
}

/* -- MPI-1 -- */

void elg_readcb_MPI_SEND(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 cid,
       elg_ui4 tag, elg_ui4 sent, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  dlid = locmap[dlid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, dlid, sent, tag, cid);
}

void elg_readcb_MPI_RECV(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 cid,
       elg_ui4 tag, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  slid = locmap[slid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, slid, -1, tag, cid);
}

void elg_readcb_MPI_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;

  lid = locmap[lid];
  if ( rlid != ELG_NO_ID ) rlid = locmap[rlid];
  n = stk_pop(lid);
  event_add(md->type, lid, n->reg, time);
  event_mpi(md->type, rlid, sent, recvd, cid);
}

/* -- MPI-1 Non-blocking -- */

void elg_readcb_MPI_SEND_COMPLETE(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_MPI_CANCELLED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

/* -- MPI-2 One-sided Operations -- */

void elg_readcb_MPI_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  dlid = locmap[dlid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, dlid, nbytes, rmaid, wid );
}

void elg_readcb_MPI_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  slid = locmap[slid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, slid, -1, rmaid, wid );
}

void elg_readcb_MPI_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TO(elg_ui4 lid, elg_d8 time, elg_ui4 rmaid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, -1, -1, rmaid, -1 );
}

void elg_readcb_MPI_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  dlid = locmap[dlid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, dlid, nbytes, rmaid, wid );
}

void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  slid = locmap[slid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, slid, -1, rmaid, wid );
}

void elg_readcb_MPI_WINEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, elg_ui4 cid, elg_ui1 synex,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;

  lid = locmap[lid];
  n = stk_pop(lid);
  event_add(md->type, lid, n->reg, time);
  event_mpi(md->type, 0, wid, synex, cid);
}

void elg_readcb_MPI_WINCOLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;

  lid = locmap[lid];
  n = stk_pop(lid);
  event_add(md->type, lid, n->reg, time);
  event_mpi(md->type, 0, wid, 0, 0);
}

void elg_readcb_MPI_WIN_LOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, elg_ui1 ltype, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  llid = locmap[llid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, llid, wid, ltype, 0);
}

void elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid  = locmap[lid];
  event_add(md->type, lid, -1, time);
  event_mpi(md->type, llid, wid, 0, 0);
}

/* -- Generic One-sided Operations -- */

void elg_readcb_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  dlid = locmap[dlid];
  event_add(md->type, lid, -1, time);
  event_rma(md->type, dlid, nbytes, rmaid);
}

void elg_readcb_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  slid = locmap[slid];
  event_add(md->type, lid, -1, time);
  event_rma(md->type, slid, -1, rmaid);
}

void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  dlid = locmap[dlid];
  event_add(md->type, lid, -1, time);
  event_rma(md->type, dlid, nbytes, rmaid);
}

void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  slid = locmap[slid];
  event_add(md->type, lid, -1, time);
  event_rma(md->type, slid, -1, rmaid);
}

void elg_readcb_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;

  lid = locmap[lid];
  if ( rlid != ELG_NO_ID ) rlid = locmap[rlid];
  n = stk_pop(lid);
  event_add(md->type, lid, n->reg, time);
  event_mpi(md->type, rlid, sent, recvd, cid);
}

void elg_readcb_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, lkid, time);
}

void elg_readcb_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, lkid, time);
}

/* -- OpenMP -- */

void elg_readcb_OMP_FORK(elg_ui4 lid, elg_d8 time, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_OMP_JOIN(elg_ui4 lid, elg_d8 time, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_OMP_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, lkid, time);
}

void elg_readcb_OMP_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, lkid, time);
}

void elg_readcb_OMP_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  StackNode *n;

  lid = locmap[lid];
  n = stk_pop(lid);
  event_add(md->type, lid, n->reg, time);
}

/* -- EPILOG Internal -- */

void elg_readcb_LOG_OFF(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_LOG_ON(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_ENTER_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

void elg_readcb_EXIT_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  MYDATA* md = (MYDATA*) userdata;

  lid = locmap[lid];
  event_add(md->type, lid, -1, time);
}

/*
 * M A I N
 */

void conv_setup() {
  event_init(totallocs);
}

int main(int argc, char **argv) {
  char*   infile;
  int     ch, nlen;
  int     eflag = 0;
  struct stat statbuf;
  elg_ui4 numrec = 0;
  ElgRCB* handle = NULL;
  MYDATA  md = { 0, 0, 0, 0 };

  while ((ch = getopt(argc, argv, "dehln:?")) != EOF) {
    switch (ch) {
    case 'd': printdecls = 1;
              break;
    case 'e': printevents = 1;
              break;
    case 'l': longstats = 1;
              break;
    case 'n': numprec = atoi(optarg);
              break;
    case 'h':
    case '?': eflag = 1;
              break;
    }
  }

  if ((argc-optind) == 1) {
    infile  = epk_get_elgfilename(argv[optind]);
  } else {
    eflag = 1;
  }

  if ( eflag ) {
    fprintf(stderr, "Usage:   %s [-d] [-e] [-l] (<infile>.elg | <experiment_archive>)\n", argv[0]);
    fprintf(stderr, "Options: -d print declaration event info\n");
    fprintf(stderr, "         -e print event info\n");
    fprintf(stderr, "         -l print long event info\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  nlen = strlen(infile);
  if (nlen>4 && strcmp(infile+nlen-4, ".esd")==0) printdecls = 1;

  handle = elg_read_open(infile);
  if ( ! handle ) exit(EXIT_FAILURE);
  stat(infile, &statbuf);

  while ( elg_read_next(handle, (void*) &md) ) {
    numrec++;
    if ( !printevents && numrec % 1000 == 0 ) {
      printf("done: %3.0f%%\r", ((double) md.done / statbuf.st_size) * 100.0);
      fflush(stdout);
    }
  }

  elg_read_close(handle);

  if (numev) {
    event_write();
  } else if (printdecls) {
    elg_readcb_LAST_DEF((void*) &md);
  }
  return 0;
}
