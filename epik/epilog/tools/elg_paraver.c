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
/* EPILOG Converter Paraver Backend                                          */
/*                                                                           */
/*===========================================================================*/


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>

#include "elg_error.h"
#include "elg_readcb.h"
#include "elg_conv.h"
#include "elg_conv_msgqueue.h"
#include "epk_archive.h"

/*
 * Paraver Eventtypes
 */
#define ET_APPLIC    40000001 
#define ET_FLUSH     40000003 
#define ET_TRACING   40000012
#define ET_PREGION   60000018
#define ET_PARALLEL  60000001
#define ET_USER      60000019
#define ET_MPI_P2P   50000001
#define ET_MPI_COLL  50000002
#define ET_MPI_MISC  50000003
#define ET_MPI_RMA   50000004
#define ET_MPI_COMM  ET_MPI_MISC
#define ET_MPI_GROUP ET_MPI_MISC
#define ET_MPI_TOPO  ET_MPI_MISC
#define ET_MPI_TYPE  ET_MPI_MISC
#define ET_MPIC_SIZE 51000001
#define ET_MPIC_ROOT 51000002
#define ET_MPIC_COMM 51000003
#define ET_METRIC    42000000
#define ET_OTHER     70000000

/*
 * Paraver States
 */
#define ST_IDLE      0
#define ST_USER      1
#define ST_EMPTY     2
#define ST_MPIRECV   3
#define ST_MPISEND   4
#define ST_OMPSYNC   5
#define ST_MPITEST   6
#define ST_FORKJOIN  7
#define ST_MPIWAIT   8
#define ST_BLOCKED   9
#define ST_MPIISEND 10
#define ST_MPIIRECV 11
#define ST_IO       12
#define ST_MPICOLL  13
#define ST_TRCOFF   14
#define ST_MPIOTHER 15
#define ST_MPISDRV  16
#define ST_MPIRMA   17
#define ST_LAST     18

#include "paraver_mpitab.h"
#include "papi_evtab.h"

#define ELG_ID_MAX 2048
#define PV_COMM_BASE 91

static pv_mpi_tab_t elg2pv_tab[ELG_ID_MAX];

static int pvcmp(const void *v1, const void *v2) {
  return strcmp(((const char*)v1), ((pv_mpi_tab_t*)v2)->name);
}

static int papicmp(const void *v1, const void *v2) {
  return strcmp(((const char*)v1), ((papi_ev_tab_t*)v2)->name);
}

static void add_mapping(int elgid,
                        const char* pvname, int pvid, int pvtype, int pvstate) {
  if ( elgid >= ELG_ID_MAX ) {
    elg_error_msg("ERROR: epilog id for %s too large", pvname);
  } else {
    elg2pv_tab[elgid].name  = pvname;
    elg2pv_tab[elgid].id    = pvid;
    elg2pv_tab[elgid].type  = pvtype;
    elg2pv_tab[elgid].state = pvstate;
  }
}

static elg_ui4 numrec = 0;
static elg_d8  lasttime = 0.0;
static int     rmatag = 101010;
static FILE*   pv_tmp;
static FILE*   pv_trc;
static FILE*   pv_cfg;
static FILE*   pv_row;
static int*    metric_tab;
static elg_ui8** metric_lastval;
static elg_d8* state_entry;
static int*    last_state;
static int     no_mpi_states = 0;
static char*   pv_cfgfile;
static char*   pv_rowfile;
static char*   pv_tmpfile;
static char*   pv_trcfile;

static void write_paraver_rowfile() {
  int m, i, t;
  int numnodes = 0;
  Group* g;
  
  fprintf(pv_row, "##########################\n");
  fprintf(pv_row, "# NAMES CONFIGURATION FILE\n");
  fprintf(pv_row, "##########################\n\n");

  fprintf(pv_row, "LEVEL SYSTEM   SIZE %d\n", nummachines);
  for (m=0; m<nummachines; ++m) {
    fprintf(pv_row, "%s\n", machine[m].name);
    numnodes += machine[m].num_member;
  }
  fprintf(pv_row, "\n");
  fprintf(pv_row, "##########################\n\n");

  fprintf(pv_row, "LEVEL NODE     SIZE %d\n", numnodes);
  for (m=0; m<nummachines; ++m) {
    for (i=0; i<machine[m].num_member; ++i) {
      g = machine[m].member + i;
      fprintf(pv_row, "%s\n", g->name);
    }
  }
  fprintf(pv_row, "\n");
  fprintf(pv_row, "##########################\n\n");

  fprintf(pv_row, "LEVEL CPU      SIZE %d\n", totallocs);
  for (i=0; i<totallocs; ++i) {
    fprintf(pv_row, "PROCESSOR %d:%d\n", loctab[i].node->id, i);
  }
  fprintf(pv_row, "\n");
  fprintf(pv_row, "##########################\n\n");

  fprintf(pv_row, "LEVEL WORKLOAD SIZE 1\n");
  fprintf(pv_row, "Workload (1 application)\n");
  fprintf(pv_row, "\n");
  fprintf(pv_row, "##########################\n\n");

  fprintf(pv_row, "LEVEL APPL     SIZE 1\n");
  if ( act_omp == -1 ) {
    fprintf(pv_row, "MPI application\n");
  } else if ( totalranks == 1 ) {
    fprintf(pv_row, "OpenMP application\n");
  } else {
    fprintf(pv_row, "MPI+OpenMP application\n");
  }
  fprintf(pv_row, "\n");
  fprintf(pv_row, "##########################\n\n");

  fprintf(pv_row, "LEVEL TASK     SIZE %d\n", totalranks);
  if ( totalranks == 1 ) {
    fprintf(pv_row, "OpenMP task\n");
  } else {
    for (i=0; i<totalranks; ++i) {
      fprintf(pv_row, "MPI Rank %d\n", i);
    }
  }
  fprintf(pv_row, "\n");
  fprintf(pv_row, "##########################\n\n");

  fprintf(pv_row, "LEVEL THREAD   SIZE %d\n", totallocs);
  if ( ! isthreaded ) {
    for (i=0; i<totallocs; i++) {
      fprintf(pv_row, "MPI Rank %d\n", i);
    }
  } else {
    for (i=0; i<totallocs; i++) {
      for (t=0; t<totallocs; t++) {
        if ( i == locmap[t] ) {
          fprintf(pv_row, "%s\n", loctab[t].thrd->name);
          break;
        }
      }
    }
  }
}

static const char* const cfg_default =
  "DEFAULT_OPTIONS\n"
  "\n"
  "LEVEL               THREAD\n"
  "UNITS               NANOSEC\n"
  "LOOK_BACK           100\n"
  "SPEED               1\n"
  "FLAG_ICONS          ENABLED\n"
  "NUM_OF_STATE_COLORS 129\n"
  "YMAX_SCALE          128\n"
  "\n"
  "DEFAULT_SEMANTIC\n"
  "\n"
  "THREAD_FUNC          State As Is\n"
  "\n";

static const char* const cfg_states1 =
  "STATES\n"
  "0    Idle\n"
  "1    Running\n"
  "2    Not created\n"
  "3    Waiting a message\n"
  "4    Blocking Send\n"
  "5    Thd. Synchr.\n"
  "6    Test/Probe\n"
  "7    Sched. and Fork/Join\n"
  "8    Wait/WaitAll\n"
  "9    Blocked\n"
  "10    Immediate Send\n"
  "11    Immediate Receive\n"
  "12    I/O\n"
  "13    Group Communication\n"
  "14    Tracing Disabled\n"
  "15    Others\n"
  "16    MPI_SendRecv\n"
  "17    MPI2 RMA\n";

static const char* const cfg_states2 =
  "\n"
  "STATES_COLOR\n"
  "0    {117,195,255}\n"
  "1    {0,0,255}\n"
  "2    {255,255,255}\n"
  "3    {255,0,0}\n"
  "4    {255,0,174}\n"
  "5    {174,0,0}\n"
  "6    {0,255,0}\n"
  "7    {255,255,0}\n"
  "8    {235,0,0}\n"
  "9    {0,162,0}\n"
  "10    {255,0,255}\n"
  "11    {100,100,177}\n"
  "12    {172,174,41}\n"
  "13    {255,144,26}\n"
  "14    {2,255,177}\n"
  "15    {192,224,0}\n"
  "16    {255,0,255}\n"
  "17    {42,42,42}\n"
  "18    {202,202,202}\n"
  "19    {0,0,255}\n"
  "20    {0,0,128}\n"
  "21    {0,255,0}\n"
  "22    {0,128,0}\n"
  "23    {255,255,0}\n"
  "24    {255,128,0}\n"
  "25    {255,0,0}\n"
  "26    {128,0,0}\n"
  "\n";

static const char* const cfg_gradient =
  "GRADIENT_COLOR\n"
  "0    {0,255,2}\n"
  "1    {0,244,13}\n"
  "2    {0,232,25}\n"
  "3    {0,220,37}\n"
  "4    {0,209,48}\n"
  "5    {0,197,60}\n"
  "6    {0,185,72}\n"
  "7    {0,173,84}\n"
  "8    {0,162,95}\n"
  "9    {0,150,107}\n"
  "10    {0,138,119}\n"
  "11    {0,127,130}\n"
  "12    {0,115,142}\n"
  "13    {0,103,154}\n"
  "14    {0,91,166}\n"
  "\n"
  "GRADIENT_NAMES\n"
  "0    Gradient 0\n"
  "1    Grad. 1/MPI Events\n"
  "2    Grad. 2/OMP Events\n"
  "3    Grad. 3/OMP locks\n"
  "4    Grad. 4/User func\n"
  "5    Grad. 5/User Events\n"
  "6    Grad. 6/General Events\n"
  "7    Grad. 7/Hardware Counters\n"
  "8    Grad. 8/Other Events\n"
  "9    Gradient 9\n"
  "10    Gradient 10\n"
  "11    Gradient 11\n"
  "12    Gradient 12\n"
  "13    Gradient 13\n"
  "14    Gradient 14\n"
  "\n";

static char* cfg_basic_events =
  "EVENT_TYPE\n"
  "6    %d    Application\n"
  "6    %d    Flushing Traces\n"
  "VALUES\n"
  "1      Begin\n"
  "0      End\n"
  "\n"
  "EVENT_TYPE\n"
  "6    %d    Tracing\n"
  "VALUES\n"
  "1      Disabled\n"
  "0      Enabled\n"
  "\n";

static char* cfg_para_events =
  "EVENT_TYPE\n"
  "2    %d    Parallel (OMP)\n"
  "VALUES\n"
  "0      close\n"
  "1      DO (open)\n"
  "2      SECTIONS (open)\n"
  "3      REGION (open)\n"
  "\n";

static char* cfg_coll_events =
  "EVENT_TYPE\n"
  "1    %d    Size in MPI Global OP\n"
  "1    %d    Root in MPI Global OP\n"
  "1    %d    Communicator in MPI Global OP\n"
  "\n";

static char* cfg_eventtype =
  "EVENT_TYPE\n"
  "%d    %d    %s\n"
  "VALUES\n"
  "0      End\n";

static void write_paraver_mpi_cfg(int type, const char* desc) {
  int i;

  fprintf(pv_cfg, cfg_eventtype, 1, type, desc);
  for (i=0; i<ELG_ID_MAX; i++) {
    if ( elg2pv_tab[i].type == type ) {
      fprintf(pv_cfg, "%-3d    %s\n", elg2pv_tab[i].id, elg2pv_tab[i].name);
    }
  }
  fprintf(pv_cfg, "\n");
}

static void write_paraver_cfgfile() {
  int a, i, firstid;

  fprintf(pv_cfg, cfg_default);
  fprintf(pv_cfg, cfg_states1);
  /* map each non-default activity to a state */
  for (a=0; a<numact; a++) {
    if ( acttab[a].id == ELG_NO_ID ) break;
    if ( strcmp(acttab[a].name, "MPI") == 0 ) continue;
    if ( strcmp(acttab[a].name, "OMP") == 0 ) continue;
    if ( strcmp(acttab[a].name, "USR") == 0 ) continue;
    if ( strcmp(acttab[a].name, "ELG") == 0 ) continue;
    if ( strcmp(acttab[a].name, "IDLE") == 0 ) continue;
    if ( strcmp(acttab[a].name, "PREG") == 0 ) continue;
    if ( strcmp(acttab[a].name, "OMP-SYNC") == 0 ) continue;
    fprintf(pv_cfg, "%d    %s\n", ST_LAST + acttab[a].id, acttab[a].name);
  }
  fprintf(pv_cfg, cfg_states2);
  fprintf(pv_cfg, cfg_gradient);

  /* Basic event types: Application, Flushing, Tracing */
  fprintf(pv_cfg, cfg_basic_events, ET_APPLIC, ET_FLUSH, ET_TRACING);

  /* OpenMP event types */
  if ( act_omp != -1 ) {
    fprintf(pv_cfg, cfg_eventtype, 2, ET_PREGION, "OpenMP Regions");
    /* OpenMP constructs and RTL functions */
    firstid = 0;
    for (i=0; i<numstate; i++) {
      int elgid = statetab[i].id;
      const char* elgname = statetab[i].name;

      if ( statetab[i].act == act_omp ) {
        int pvid = ++firstid;
	int locking = (statetab[i].type == ELG_FUNCTION) &&
	              (strstr(elgname, "_lock") != 0);
	fprintf(pv_cfg, "%d      %s\n", pvid, elgname);
	add_mapping(elgid, elgname, pvid, ET_PREGION,
	            locking ? ST_OMPSYNC :ST_USER);
      }
    }
    /* OpenMP barriers */
    for (i=0; i<numstate; i++) {
      int elgid = statetab[i].id;
      const char* elgname = statetab[i].name;

      if ( statetab[i].act == act_ompsync ) {
        int pvid = ++firstid;
	fprintf(pv_cfg, "%d      %s\n", pvid, elgname);
	add_mapping(elgid, elgname, pvid, ET_PREGION, ST_OMPSYNC);
      }
    }
    fprintf(pv_cfg, "\n");
    /* OpenMP Parallel region type */
    add_mapping(state_pregion, "Parallel Region", 3, ET_PARALLEL, ST_FORKJOIN);
    fprintf(pv_cfg, cfg_para_events, ET_PARALLEL);
  }

  /* User function events */
  fprintf(pv_cfg, cfg_eventtype, 4, ET_USER, "User Functions");
  firstid = 0;
  for (i=0; i<numstate; i++) {
    if ( statetab[i].act == act_usr ) {
      int scl = statetab[i].sclid;
      int elgid = statetab[i].id;
      int pvid = ++firstid;

      fprintf(pv_cfg, "%d      %s", pvid, statetab[i].name);
      if ( scl != -1 ) {
        fprintf(pv_cfg, " (%s:%d)", filetab[scltab[scl].fid], scltab[scl].line);
      }
      fprintf(pv_cfg, "\n");
      add_mapping(elgid, statetab[i].name, pvid, ET_USER, ST_USER);
    }
  }
  fprintf(pv_cfg, "\n");

  /* setup convert MPI epilog -> pv */
  for (i=0; i<numstate; i++) {
    if ( statetab[i].act == act_mpi ) {
      int elgid = statetab[i].id;
      const char* elgname = statetab[i].name;
      pv_mpi_tab_t* entry = bsearch(elgname, pv_mpi_tab,
	 sizeof(pv_mpi_tab)/sizeof(pv_mpi_tab_t), sizeof(pv_mpi_tab_t), pvcmp);
      if ( entry ) {
	add_mapping(elgid, elgname, entry->id, entry->type,
	            no_mpi_states ? ST_USER : entry->state);
      } else {
	elg_cntl_msg("WARNING: no paraver id for %s", elgname);
	add_mapping(elgid, elgname, elgid,
	            is_mpi_collective(elgname) ? ET_MPI_COLL : ET_MPI_MISC,
	            no_mpi_states ? ST_USER : (
	            is_mpi_collective(elgname) ? ST_MPICOLL : ST_MPIOTHER));
      }
    }
  }

  /* MPI event types */
  write_paraver_mpi_cfg(ET_MPI_P2P, "MPI Point-to-point");
  write_paraver_mpi_cfg(ET_MPI_COLL, "MPI Collective Comm");
  fprintf(pv_cfg, cfg_coll_events, ET_MPIC_SIZE, ET_MPIC_ROOT, ET_MPIC_COMM);
  write_paraver_mpi_cfg(ET_MPI_MISC, "MPI Other");
  write_paraver_mpi_cfg(ET_MPI_RMA, "MPI One-sided");

  /* HW counter Metrics event types */
  if ( maxmet != -1 ) {
    int m = 0;
    int t;
    metric_tab = x_malloc(nummet*sizeof(int));
    metric_lastval = x_malloc(nummet*sizeof(elg_ui8*));

    fprintf(pv_cfg, "EVENT_TYPE\n");
    for (i=0; i<nummet; i++) {
      if ( mettab[i].name ) {
        papi_ev_tab_t* entry = bsearch(mettab[i].name, papi_ev_tab,
	                           sizeof(papi_ev_tab)/sizeof(papi_ev_tab_t),
				   sizeof(papi_ev_tab_t), papicmp);
	metric_tab[m] = entry ? ET_METRIC+entry->id : ET_METRIC+256+i;
        fprintf(pv_cfg, "7    %d    %s (%s)\n", metric_tab[m],
	        mettab[i].descr, mettab[i].name);
	metric_lastval[m] = x_malloc(totallocs*sizeof(elg_ui8));
	for (t=0; t<totallocs; t++) metric_lastval[m][t] = 0;
	m++;
      }
    }
    fprintf(pv_cfg, "\n");
  }

  /* non-default activity event types */
  for (a=0; a<numact; a++) {
    int aid = acttab[a].id;
    if ( aid == ELG_NO_ID ) break;
    if ( strcmp(acttab[a].name, "MPI") == 0 ) continue;
    if ( strcmp(acttab[a].name, "OMP") == 0 ) continue;
    if ( strcmp(acttab[a].name, "USR") == 0 ) continue;
    if ( strcmp(acttab[a].name, "ELG") == 0 ) continue;
    if ( strcmp(acttab[a].name, "IDLE") == 0 ) continue;
    if ( strcmp(acttab[a].name, "PREG") == 0 ) continue;
    if ( strcmp(acttab[a].name, "OMP-SYNC") == 0 ) continue;

    fprintf(pv_cfg, cfg_eventtype, 8, ET_OTHER+aid, acttab[a].name);
    for (i=0; i<numstate; i++) {
      if ( statetab[i].act == aid ) {
        int scl = statetab[i].sclid;
        int elgid = statetab[i].id;
        int pvid = elgid;
        fprintf(pv_cfg, "%d      %s", pvid, statetab[i].name);
        if ( scl != -1 ) {
          fprintf(pv_cfg, " (%s:%d)", filetab[scltab[scl].fid], scltab[scl].line);
        }
        fprintf(pv_cfg, "\n");
        add_mapping(elgid, statetab[i].name, pvid, ET_OTHER+aid, ST_LAST+aid);
      }
    }
    fprintf(pv_cfg, "\n");
  }
}

static void write_paraver_trcheader(struct stat* statbuf) {
  char creation[24];
  struct tm* tmbuf;
  elg_ui8 duration = lasttime * 1.0e9;
  int m, n, c, t;
  Group* node;
  Group* task;
  int comma = 0;
  int numnodes = 0;
  int cpusperprocess = 0;

  /*basic header*/
  tmbuf = localtime(&statbuf->st_mtime);
  strftime(creation, 24, "(%d/%m/%y at %H:%M)", tmbuf);
  fprintf(pv_trc, "#Paraver %s:%llu_ns:", creation, (long long unsigned) duration);

  /*resource info*/
  for (m=0; m<nummachines; ++m) numnodes += machine[m].num_member;

  comma = 0;
  fprintf(pv_trc, "%d(", numnodes);
  for (m=0; m<nummachines; ++m) {
    for (n=0; n<machine[m].num_member; ++n) {
      node = machine[m].member + n;
      cpusperprocess = node->member[0].num_member;
      fprintf(pv_trc, "%s%d", comma?",":"", node->num_member*cpusperprocess);
      comma = 1;
    }
  }
  fprintf(pv_trc, "):");

  /*application list*/
  comma = 0;
  fprintf(pv_trc, "1:%d(", totalranks);
  for (m=0; m<nummachines; ++m) {
    for (n=0; n<machine[m].num_member; ++n) {
      node = machine[m].member + n;
      for (t=0; t<node->num_member; ++t) {
        task = node->member + t;
        fprintf(pv_trc, "%s%d:%d", comma?",":"", task->num_member, node->id+1);
        comma = 1;
      }
    }
  }
  fprintf(pv_trc, ")");

  /*communicators*/
  fprintf(pv_trc, ",%d\n", maxcomm+1);
  for (c=0; c<maxcomm+1; c++) {
    int j, nelem = 0;
    for (j=0; j<(commtab[c].numbyte*8); j++) {
        if ( (1 << (j % 8)) & commtab[c].bits[j/8] ) nelem++;
    }
    fprintf(pv_trc, "c:1:%d:%d", PV_COMM_BASE+c, nelem);
    for (j=0; j<(commtab[c].numbyte*8); j++) {
        if ( (1 << (j % 8)) & commtab[c].bits[j/8] ) {
	  fprintf(pv_trc, ":%d", j+1);
	}
    }
    fprintf(pv_trc, "\n");
  }
}

/*
 * Definition Records
 */

void conv_write_definition_records() {
  int i;

  elg_cntl_msg("Generating paraver row file %s ...", pv_rowfile);
  write_paraver_rowfile();
  elg_cntl_msg("Generating paraver config file %s ...", pv_cfgfile);
  write_paraver_cfgfile();

  elg_cntl_msg("Generating paraver trace file %s ...", pv_trcfile);
  state_entry = x_malloc(totallocs * sizeof(elg_d8));
  for (i=0; i<totallocs; i++) state_entry[i] = 0.0;
  last_state = x_malloc(totallocs * sizeof(int));
  for (i=0; i<totallocs; i++) last_state[i] = ST_EMPTY;
}

/*
 * EPILOG read callbacks
 */

/* -- Helpers -- */

#define PV_STATE 1
#define PV_EVENT 2
#define PV_COMM  3

static void genWriteLocation(elg_ui4 lid) {
  Group* proc = loctab[lid].proc;
  int rank = proc->id + 1;
  Group* thrd = loctab[lid].thrd;
  int tid = thrd ? (thrd - proc->member) : 0;
  fprintf(pv_tmp, ":%d:1:%d:%d", rank, rank, tid+1);
}

static void genWriteTimestamp(elg_d8 time) {
  elg_ui8 tstamp = time * 1.e9;
  fprintf(pv_tmp, ":%llu", (long long unsigned) tstamp);
}

static void genWriteRecordHeader(int type, elg_ui4 lid, elg_d8 time) {
  fprintf(pv_tmp, "%d", type);
  genWriteLocation(lid);
  genWriteTimestamp(time);
}

static void genWriteState(int state, elg_ui4 lid, elg_d8 time) {
  if ( state != last_state[lid] ) {
    genWriteRecordHeader(PV_STATE, lid, state_entry[lid]);
    genWriteTimestamp(time);
    fprintf(pv_tmp, ":%d\n", last_state[lid]);
    state_entry[lid] = time;
    last_state[lid] = state;
  }
}

static void genWriteENTER(elg_ui4 lid, elg_d8 time, elg_ui4 rid) {
  stk_push(lid, rid, time);
  genWriteRecordHeader(PV_EVENT, lid, time);
  fprintf(pv_tmp, ":%d:%d\n", elg2pv_tab[rid].type, elg2pv_tab[rid].id);

  /*enter next state*/
  genWriteState(elg2pv_tab[rid].state, lid, time);
}

static void genWriteCOUNTER(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[]) {
  int i;

  if ( metc ) {
    genWriteRecordHeader(PV_EVENT, lid, time);
    for (i=0; i<metc; i++) {
      elg_ui8 val = metv[i] - metric_lastval[i][lid];
      metric_lastval[i][lid] = metv[i];
      fprintf(pv_tmp, ":%d:%llu", metric_tab[i], (long long unsigned) val);
    }
    fprintf(pv_tmp, "\n");
  }
  lasttime = time;
}

static void genWriteLEAVE(elg_ui4 lid, elg_d8 time) {
  StackNode *n = stk_pop(lid);
  genWriteRecordHeader(PV_EVENT, lid, time);
  fprintf(pv_tmp, ":%d:%d\n", elg2pv_tab[n->reg].type, 0);

  /*enter next state*/
  n = stk_top(lid);
  if (n->reg == ELG_NO_ID) {
    genWriteState(ST_IDLE, lid, time);
  } else {
    genWriteState(elg2pv_tab[n->reg].state, lid, time);
  }
}

static void genWriteCOLLLEAVE(int w, elg_ui4 lid, elg_d8 time,
       elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent, elg_ui4 recvd) {
  StackNode *n = stk_pop(lid);

  rlid = ( rlid != ELG_NO_ID ) ? locmap[rlid] + 1 : 0;

  /*write parameter for coll enter*/
  genWriteRecordHeader(PV_EVENT, lid, n->tim);
  if ( rlid ) fprintf(pv_tmp, ":%d:%d", ET_MPIC_ROOT, rlid);
  fprintf(pv_tmp, ":%d:%d:%d:%d\n", ET_MPIC_COMM, PV_COMM_BASE+cid,
                                    ET_MPIC_SIZE, sent+recvd);

  /*write collexit*/
  genWriteRecordHeader(PV_EVENT, lid, time);
  fprintf(pv_tmp, ":%d:%d", elg2pv_tab[n->reg].type, 0);
  if ( rlid ) fprintf(pv_tmp, ":%d:0", ET_MPIC_ROOT);
  fprintf(pv_tmp, ":%d:0:%d:0\n", ET_MPIC_COMM, ET_MPIC_SIZE);

  /*enter next state*/
  n = stk_top(lid);
  if (n->reg == ELG_NO_ID) {
    genWriteState(ST_IDLE, lid, time);
  } else {
    genWriteState(elg2pv_tab[n->reg].state, lid, time);
  }
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
    genWriteENTER(lid, time, state_user);
  }
  genWriteENTER(lid, time, rid);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_ENTER_CS(elg_ui4 lid, elg_d8 time, elg_ui4 csid,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  if ( need_usr && stk_top(lid)->reg == ELG_NO_ID ) {
    genWriteENTER(lid, time, state_user);
  }
  stk_push(lid, csitetab[csid].erid, time);
  genWriteENTER(lid, time, csitetab[csid].erid);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_EXIT(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  lid = locmap[lid];
  genWriteLEAVE(lid, time);
  genWriteCOUNTER(lid, time, metc, metv);
}

/* -- MPI-1 -- */

void elg_readcb_MPI_SEND(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 cid,
       elg_ui4 tag, elg_ui4 sent, void* userdata) {
  StackNode *n;
  lid = locmap[lid];
  dlid = locmap[dlid];
  n = stk_top(lid); 
  AddMessage(lid, dlid, tag, cid, sent, n->tim, time);
  lasttime = time;
}

void elg_readcb_MPI_RECV(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 cid,
       elg_ui4 tag, void* userdata) {
  StackNode *n;
  Message* send;

  lid = locmap[lid];
  slid = locmap[slid];
  n = stk_top(lid);
  if ( (send = FindMessage(slid, lid, tag, cid)) == 0 ) {
    elg_warning("WARNING: cannot find matching send"
                ": #%u: %.6f %d MPI_RECV from=%d t=%d c=%d",
		numrec, time, lid, slid, tag, cid);
  } else {
    genWriteRecordHeader(PV_COMM, slid, send->etime);
    genWriteTimestamp(send->stime);
    genWriteLocation(lid);
    genWriteTimestamp(n->tim);
    genWriteTimestamp(time);
    fprintf(pv_tmp, ":%d:%d\n", send->len, tag);
  }
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
  lasttime = time;
}

void elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
  lasttime = time;
}

void elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{ 
  lasttime = time;
}

void elg_readcb_MPI_CANCELLED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
  lasttime = time;
}

/* -- MPI-2 One-sided Operations -- */

void elg_readcb_MPI_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  dlid = locmap[dlid];
  */
  lasttime = time;
}

void elg_readcb_MPI_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  slid = locmap[slid];
  */
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
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  dlid = locmap[dlid];
  */
  lasttime = time;
}

void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  slid = locmap[slid];
  */
  lasttime = time;
}

void elg_readcb_MPI_WINEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, elg_ui4 cid, elg_ui1 synex,
       void* userdata) {
  lid = locmap[lid];
  genWriteLEAVE(lid, time);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_MPI_WINCOLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 wid, void* userdata) {
  lid = locmap[lid];
  genWriteCOLLLEAVE(1, lid, time, ELG_NO_ID, wintab[wid], 0, 0);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_MPI_WIN_LOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, elg_ui1 ltype, void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  llid = locmap[llid];
  */
  lasttime = time;
}

void elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
       elg_ui4 wid, void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  llid = locmap[llid];
  */
  lasttime = time;
}

/* -- Generic One-sided Operations -- */

void elg_readcb_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  dlid = locmap[dlid];
  */
  lasttime = time;
}

void elg_readcb_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  slid = locmap[slid];
  */
  lasttime = time;
}

void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  dlid = locmap[dlid];
  */
  lasttime = time;
}

void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  slid = locmap[slid];
  */
  lasttime = time;
}

void elg_readcb_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
  lid = locmap[lid];
  genWriteCOLLLEAVE(genmpiglop,
                    lid, time, rlid, cid, sent, recvd);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  */
  lasttime = time;
}

void elg_readcb_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  */
  lasttime = time;
}

/* -- OpenMP -- */

void elg_readcb_OMP_FORK(elg_ui4 lid, elg_d8 time, void* userdata) {
  lid = locmap[lid];
  genWriteENTER(lid, time, state_pregion);
  lasttime = time;
}

void elg_readcb_OMP_JOIN(elg_ui4 lid, elg_d8 time, void* userdata) {
  lid = locmap[lid];
  genWriteLEAVE(lid, time);
  lasttime = time;
}

void elg_readcb_OMP_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  */
  lasttime = time;
}

void elg_readcb_OMP_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid,
       void* userdata) {
  /* NOT USED YET FOR Paraver
  lid = locmap[lid];
  */
  lasttime = time;
}

void elg_readcb_OMP_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  StackNode *n;

  lid = locmap[lid];
  n = stk_top(lid);
  genWriteCOLLLEAVE(genompglop && is_glop(n->reg), lid, time,
                    ELG_NO_ID, loctab[lid].proc->id, 0, 0);
  genWriteCOUNTER(lid, time, metc, metv);
}

/* -- EPILOG Internal -- */

void elg_readcb_LOG_OFF(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  lid = locmap[lid];
  genWriteRecordHeader(PV_EVENT, lid, time);
  fprintf(pv_tmp, ":%d:1\n", ET_TRACING);
  genWriteCOUNTER(lid, time, metc, metv);

  /*enter next state*/
  genWriteState(ST_TRCOFF, lid, time);
}

void elg_readcb_LOG_ON(elg_ui4 lid, elg_d8 time, elg_ui1 metc, elg_ui8 metv[],
       void* userdata) {
  StackNode *n = stk_top(lid);

  lid = locmap[lid];
  genWriteRecordHeader(PV_EVENT, lid, time);
  fprintf(pv_tmp, ":%d:0\n", ET_TRACING);
  genWriteCOUNTER(lid, time, metc, metv);

  /*enter next state*/
  genWriteState(elg2pv_tab[n->reg].state, lid, time);
}

void elg_readcb_ENTER_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  genWriteRecordHeader(PV_EVENT, lid, time);
  fprintf(pv_tmp, ":%d:1\n", ET_FLUSH);
  genWriteCOUNTER(lid, time, metc, metv);
}

void elg_readcb_EXIT_TRACING(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], void* userdata) {
  lid = locmap[lid];
  genWriteRecordHeader(PV_EVENT, lid, time);
  fprintf(pv_tmp, ":%d:0\n", ET_FLUSH);
  genWriteCOUNTER(lid, time, metc, metv);
}

/*
 * M A I N
 */

void conv_setup() {
  genWriteRecordHeader(PV_EVENT, 0, 0.0);
  fprintf(pv_tmp, ":%d:1\n", ET_APPLIC);
}

static char* change_suffix(char *infile, char* suffix) {
  char* outfile = strdup(infile);
  char* dot = strrchr(outfile, '.');
  *(dot + 1) = suffix[0];
  *(dot + 2) = suffix[1];
  *(dot + 3) = suffix[2];
  return outfile;
}

int main(int argc, char **argv) {
  char*   infile;
  int     ch;
  int     eflag = 0;
  struct stat statbuf;
  elg_ui4 limrec = 0xFFFFFFFF;
  ElgRCB* handle = NULL;
  MYDATA  md = { 0, 0, 0, 0 };

  addidle = 0;

  while ((ch = getopt(argc, argv, "hmn:r:")) != EOF) {
    switch (ch) {
    case 'm': no_mpi_states = 1;
              break;
    case 'n': limrec = atol(optarg);
              break;
    case 'r': rmatag = atol(optarg);
              break;
    case 'h':
    case '?': eflag = 1;
              break;
    }
  }

  if ((argc-optind) == 1) {
    infile  = epk_get_elgfilename(argv[optind]);
    pv_tmpfile = change_suffix(infile, "tmp");
    pv_trcfile = change_suffix(infile, "prv");
    pv_cfgfile = change_suffix(infile, "pcf");
    pv_rowfile = change_suffix(infile, "row");
  } else {
    eflag = 1;
  }

  if ( eflag ) {
    fprintf(stderr, "Usage:   %s [-i] [-n #] [-r #] "
                    "(<infile>.elg | <experiment_archive>)\n", argv[0]);
    fprintf(stderr, "Options  -m do not generate MPI related states\n");
    fprintf(stderr, "         -n stop converting after # records\n");
    fprintf(stderr, "         -r use # as tag for RMA operations\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  handle = elg_read_open(infile);
  if ( ! handle ) exit(EXIT_FAILURE);
  stat(infile, &statbuf);

  if ( (pv_tmp = fopen(pv_tmpfile, "w")) == NULL ) {
    perror(pv_tmpfile);
    exit(EXIT_FAILURE);
  }
  if ( (pv_trc = fopen(pv_trcfile, "w")) == NULL ) {
    perror(pv_trcfile);
    exit(EXIT_FAILURE);
  }
  if ( (pv_cfg = fopen(pv_cfgfile, "w")) == NULL ) {
    perror(pv_cfgfile);
    exit(EXIT_FAILURE);
  }
  if ( (pv_row = fopen(pv_rowfile, "w")) == NULL ) {
    perror(pv_rowfile);
    exit(EXIT_FAILURE);
  }

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
    int l;
    for (l=0; l<totallocs; l++) {
      if ( state_entry[l] != lasttime ) {
        genWriteRecordHeader(PV_STATE, l, state_entry[l]);
        genWriteTimestamp(lasttime);
        fprintf(pv_tmp, ":%d\n", ST_IDLE);
      }
    }
  }
  genWriteRecordHeader(PV_EVENT, 0, lasttime+1.0e-9);
  fprintf(pv_tmp, ":%d:0\n", ET_APPLIC);

  if ( nummsg ) {
    elg_warning("WARNING: %d messages left in message queue", nummsg);
  }

  write_paraver_trcheader(&statbuf);
  elg_read_close(handle);
  fclose(pv_tmp);
  fclose(pv_trc);
  fclose(pv_cfg);
  fclose(pv_row);
  return 0;
}
