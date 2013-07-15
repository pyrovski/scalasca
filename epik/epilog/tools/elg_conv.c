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
/* EPILOG Converter                                                          */
/*                                                                           */
/*===========================================================================*/


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "elg_readcb.h"
#include "elg_error.h"
#include "elg_conv.h"

/*
 * Utility module
 */

static const char* mpi_colls[] = {
  /* must be alphabetically sorted ! */
  "MPI_Allgather", "MPI_Allgatherv", "MPI_Allreduce",
  "MPI_Alltoall", "MPI_Alltoallv", "MPI_Alltoallw",
  "MPI_Barrier", "MPI_Bcast", "MPI_Exscan",
  "MPI_Gather", "MPI_Gatherv", "MPI_Reduce", "MPI_Reduce_scatter",
  "MPI_Scan", "MPI_Scatter", "MPI_Scatterv",
  "MPI_Win_create", "MPI_Win_fence", "MPI_Win_free"
};

static int mycmp(const void *v1, const void *v2) {
  return strcasecmp(*((const char**)v1), *((const char**)v2));
}

int is_mpi_collective(const char* str) {
  return (bsearch(&str, mpi_colls, sizeof(mpi_colls)/sizeof(char *),
                  sizeof(char *), mycmp) != 0);
}

void *x_malloc_impl(size_t size, char* file, int line) {
  void *r = malloc(size);
  if ( r == NULL ) elg_error_msg("Out of memory at %s:%d", file, line);
  return r;
}

void *x_realloc_impl(void *ptr, size_t size, char* file, int line) {
  void *r = realloc(ptr, size);
  if ( r == NULL ) elg_error_msg("Out of memory at %s:%d", file, line);
  return r;
}

off_t   wbytes = 0;
int     genompglop = 1;
int     writeOMP   = 0;

/*
 * Region Stack Module
 */

RegStack mystack;

void stk_init(int totallocs) {
  int i;
  StackNode *n;

  mystack.vals = x_malloc(totallocs * STACK_SIZ * sizeof(StackNode));
  mystack.idxs = x_malloc(totallocs * sizeof(StackNode *));
  for (i=0; i<totallocs; i++) {
    n = mystack.idxs[i] = mystack.vals + (i*STACK_SIZ);
    n->reg = ELG_NO_ID;
    n->tim = -1.0;
  }
}

void stk_push(elg_ui4 l, elg_ui4 r, elg_d8 t) {
  StackNode *n = ++mystack.idxs[l];
  if ( n >= mystack.vals + ((l+1)*STACK_SIZ) )
    elg_error_msg("Region stack overflow l=%d r=%d t=%g", l, r, t);
  n->reg = r;
  n->tim = t;
}

StackNode* stk_pop(elg_ui4 l) {
  if ( mystack.idxs[l] <= mystack.vals + (l*STACK_SIZ) )
    elg_error_msg("Region stack underflow l=%d", l);
  return mystack.idxs[l]--;
}

StackNode* stk_top(elg_ui4 l) {
  if ( mystack.idxs[l] < mystack.vals + (l*STACK_SIZ) )
    elg_error_msg("Region stack underflow l=%d", l);
  return mystack.idxs[l];
}

/*
 * String Table Module
 */

int numstring = 0;
char** stringtab = 0;

static void str_add(int strid, char* str) {
  if ( (strid+1) > numstring ) {
    numstring += 100;
    stringtab = x_realloc(stringtab, numstring * sizeof(char *));
  }
  stringtab[strid] = str;
}

/*
 * File Table Module
 */

int numfile = 0;
char** filetab = 0;

static void file_add(int fid, const char*str) {
  int i;

  if ( (fid+1) > numfile ) {
    numfile += 20;
    filetab = x_realloc(filetab, numfile * sizeof(char *));
    for (i=(numfile-20); i<numfile; i++) filetab[i] = 0;
  }
  filetab[fid] = strdup(str);
}

/*
 * SCL Table Module
 */

int numscl = 0;
int sclid = 0;
SCL* scltab = 0;

static int scl_add(elg_ui4 fid, elg_ui4 line) {
  if ( fid == ELG_NO_ID || line == ELG_NO_NUM ) return -1;

  if ( (sclid+1) > numscl ) {
    numscl += 20;
    scltab = x_realloc(scltab, numscl * sizeof(SCL));
  }
  scltab[sclid].fid  = fid;
  scltab[sclid].line = line;
  return sclid++;
}

/*
 * Activity Table Module
 */

char* rtypenames[] = {
  "UNKNOWN", "FUNCTION", "LOOP", "USER_REGION",
  "FUNCTION_COLL_BARRIER", "FUNCTION_COLL_ONE2ALL",
  "FUNCTION_COLL_ALL2ONE", "FUNCTION_COLL_ALL2ALL", "FUNCTION_COLL_OTHER",
  "?", "?",
  "OMP_PARALLEL", "OMP_LOOP", "OMP_SECTIONS", "OMP_SECTION",
  "OMP_WORKSHARE", "OMP_SINGLE", "OMP_MASTER", "OMP_CRITICAL",
  "OMP_ATOMIC", "OMP_BARRIER", "OMP_IBARRIER", "OMP_FLUSH",
  "OMP_CRITICAL_SBLOCK", "OMP_SINGLE_SBLOCK"
};

int numact = 0;
Activity* acttab = 0;
elg_ui4 maxact = 0;

int act_elg = -1;
int act_omp = -1;
int act_ompsync = -1;
int act_preg = -1;
int act_idle = -1;
int act_usr = -1;
int act_mpi = -1;
int need_usr = 0;
int is_pattern_trace = 0;

static void act_add(elg_ui4 id, const char* s) {
  int i;

  /*search table*/
  for (i=0; i<numact; i++) {
    if ( acttab[i].id == id ) {
      /*already added*/
      return;
    } else if ( acttab[i].id == ELG_NO_ID ) {
      /*empty slot; add*/
      acttab[i].id = id;
      acttab[i].name = s;
      if ( id > maxact ) maxact = id;
      if ( strcmp(s, "OMP") == 0 ) act_omp = id;
      else if ( strcmp(s, "USR") == 0 ) act_usr = id;
      else if ( strcmp(s, "MPI") == 0 ) act_mpi = id;
      else if ( strstr(s, "SEVERITY") != 0 ) is_pattern_trace = 1;
      return;
    }
  }

  /*table full; resize and add*/
  numact += 10;
  acttab = x_realloc(acttab, numact * sizeof(Activity));
  for (i=numact-10; i<numact; i++) acttab[i].id = ELG_NO_ID;
  acttab[numact-10].id = id;
  acttab[numact-10].name = s;
  if ( id > maxact ) maxact = id;
  if ( strcmp(s, "OMP") == 0 ) act_omp = id;
  else if ( strcmp(s, "USR") == 0 ) act_usr = id;
  else if ( strcmp(s, "MPI") == 0 ) act_mpi = id;
  else if ( strstr(s, "SEVERITY") != 0 ) is_pattern_trace = 1;
}

static void act_setup() {
  act_elg = maxact + 1;
  act_add(act_elg, "ELG");
  if ( act_usr == -1 && !is_pattern_trace ) {
    act_usr = maxact + 1;
    act_add(act_usr, "USR");
    need_usr = 1;
  }
  if ( act_omp != -1 ) {
    act_idle = maxact + 1;
    act_add(act_idle, "IDLE");
    if ( ! writeOMP ) {
      act_preg = maxact + 1;
      act_add(act_preg, "PREG");
      act_ompsync = maxact + 1;
      act_add(act_ompsync, "OMP-SYNC");
    }
  }
}

/*
 * Callsite Table Module
 */

int numcsite = 0;
Csite* csitetab = 0;

static void csite_add(elg_ui4 csid, elg_ui4 erid, int sclid) {
  int i;

  if ( (csid + 1) > numcsite ) {
    /*table full; resize*/
    numcsite += 20;
    csitetab = x_realloc(csitetab, numcsite * sizeof(Csite));
    for (i=(numcsite-20); i<numcsite; i++) csitetab[i].erid = ELG_NO_ID;
  }
  csitetab[csid].erid  = erid;
  csitetab[csid].sclid = sclid;
}

/*
 * State Table Module
 */

int numstate = 0;
int capstate = 0;
State* statetab = 0;
elg_ui4 maxstate = 0;

int state_tracing = -1;
int state_off = -1;
int state_pregion = -1;
int state_idle = -1;
int state_user = -1;

#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
elg_ui4 state_mpi_init = 0xFFFFFFD6; /*-42*/
elg_ui4 state_mpi_fin  = 0xFFFFFFD6; /*-42*/
#endif

static
void state_add(elg_ui4 act, elg_ui4 id, const char* s, elg_ui1 t, int sclid) {
  if (numstate == capstate) {
    /*table full; resize*/
    capstate += 100;
    statetab = x_realloc(statetab, capstate * sizeof(State));
  }
  statetab[numstate].act   = act;
  statetab[numstate].id    = id;
  statetab[numstate].name  = s;
  statetab[numstate].sclid = sclid;
  statetab[numstate].type  = t;
  if ( id > maxstate ) maxstate = id;
  numstate++;

#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
  if ( strcmp(s, "MPI_Init") == 0 )
    state_mpi_init = id;
  else if ( strcmp(s, "MPI_Finalize") == 0 )
    state_mpi_fin = id;
#endif
}

#ifndef FLUSH_TIMESTAMPS_ALWAYS_CORRECT
int is_mpi_init_fin(elg_ui4 r) {
  return (r == state_mpi_init || r == state_mpi_fin );
}
#endif

static void state_setup() {
  int i;

  state_tracing = maxstate + 1;
  state_add(act_elg, state_tracing, "tracing", ELG_UNKNOWN, -1);
  state_off = state_tracing + 1;
  state_add(act_elg, state_off, "off", ELG_UNKNOWN, -1);
  if ( act_omp != -1 ) {
    state_idle = state_tracing + 2;
    state_add(act_idle, state_idle, "idle", ELG_UNKNOWN, -1);
    if ( !writeOMP ) {
      state_pregion = state_tracing + 3;
      state_add(act_preg, state_pregion, "parallel region",
                ELG_USER_REGION, -1);
    }
  }
  if ( need_usr ) {
    state_user = state_tracing + 3 + (1 - writeOMP);
    state_add(act_usr, state_user, "user", ELG_UNKNOWN, -1);
  }

  for (i=0; i<numstate; i++) {
    /*move openmp barriers to separate activity OMP-SYNC*/
    if ( !writeOMP &&
       ( strncmp(statetab[i].name, "!$omp barrier", 13) == 0 ||
         strncmp(statetab[i].name, "!$omp ibarrier", 14) == 0 )
       ) {
      statetab[i].act = act_ompsync;
    }
  }
}

/*
 * GlobalOp Table Module
 */

int numglop = 0;
int capglop = 0;
Glop* gloptab = 0;

static void glop_add(elg_ui4 id, const char* s, elg_ui1 r) {
  if (numglop == capglop) {
    /*table full; resize*/
    capglop += 10;
    gloptab = x_realloc(gloptab, capglop * sizeof(Glop));
  }
  gloptab[numglop].id    = id;
  gloptab[numglop].name  = s;
  gloptab[numglop].rtype = r;
  numglop++;
}

static int glopcmp(const void *v1, const void *v2) {
  return ((Glop*)v1)->id - ((Glop*)v2)->id;
}

int is_glop(elg_ui4 id) {
  Glop key;
  key.id = id;
  if (numglop)
    return (bsearch(&key, gloptab, numglop, sizeof(Glop), glopcmp) != 0);
  else
    return 0;
}

/*
 * Groups Module
 */

int nummachines = 0;
Group machine[MAXMACHINE];
const char *group_descr[] = { "machine", "node", "process", "thread" };

/* debug
void group_print(int level, Group *g) {
  int i;
  fprintf(stderr, "%s %d: \"%s\" nelem=%d\n",
          group_descr[level], g->id, g->name, g->num_member);
  if ( g->member ) {
    for (i=0; i<g->num_member; ++i) {
      group_print(level+1, g->member + i);
    }
  }
}
*/

unsigned int grpid = 10000000;
int totallocs = 0;
int totalranks = 0;
int *locmap = 0;
int *locremap = 0;

int isthreaded  = 0;
int numlocs = 0;
Location* loctab = 0;
unsigned int samplegroup = 0;

static void machine_setup() {
  int m, n, p, l, numnode, numproc;
  Group *mach, *node;

  /*fix real number of nodes and procs*/
  for (m=0; m<nummachines; m++) {
    mach = machine + m;
    numnode = 0;
    for (n=0; n<mach->num_member; n++) {
      node = mach->member + n;
      if ( node->id == -1 ) break;
      numnode++;
      numproc = 0;
      for (p=0; p<node->num_member; p++) {
        if ( node->member[p].id == -1 ) break;
	/*fix non-threaded: remove unnecessary 1-element thread list*/
	if ( ! isthreaded ) node->member[p].member = 0 ;
        numproc++;
      }
      node->num_member = numproc;
    }
    mach->num_member = numnode;
  }

  /*fix non-threaded: remove invalid thread pointers*/
  if ( ! isthreaded ) {
    for (l=0; l<totallocs; l++) {
      loctab[l].thrd = 0;
    }
  }
}

static
void loc_add(elg_ui4 lid, elg_ui4 mid, elg_ui4 nid, elg_ui4 pid, elg_ui4 tid) {
  int i;
  Group *node = &(machine[mid].member[nid - machine[mid].min_id]);
  Group *proc = 0;
  Group *thrd = 0;
  Group *old = 0;
  elg_ui4 cpuc = node->num_member;

  /*count mpi ranks*/
  if ( tid == 0 ) ++totalranks;

  /*do we have thread locations? */
  if ( (tid != 0) || (act_omp != -1) ) isthreaded = 1;

  /*more processes than CPUs found -> get more space for proc's*/
  if ( pid >= cpuc ) {
    old = node->member;
    node->member = x_realloc(node->member, (pid+1) * sizeof(Group));
    node->num_member = pid+1;
    for (i=cpuc; i<(pid+1); ++i) {
      node->member[i].id = -1;
    }

    /*if realloc moved block -> fix old pointers into block*/
    if ( old != node->member ) {
      for (i=0; i<lid; ++i) {
        if ( loctab[i].node == node ) {
          /*we changed this node -> fix proc pointer here*/
          int p = loctab[i].proc - old;
          loctab[i].proc = node->member + p;
        }
      }
    }
  }

  for (i=0; i<node->num_member; i++) {
    proc = node->member + i;
    if ( proc->id == -1 ) {
      /*not found; add*/
      proc->id = pid;
      proc->name = x_malloc(32 * sizeof(char));
      sprintf(proc->name, "process %u", pid);
      proc->num_member = tid+1;
      proc->member = x_malloc(MAX_THREADS * sizeof(Group));
      for (i=0; i<MAX_THREADS; ++i) {
        proc->member[i].id = -1;
      }
      thrd = proc->member + tid;
      break;
    } else if ( proc->id == pid ) {
      /*found; update num threads*/
      if ( (tid+1) > proc->num_member )
        proc->num_member = tid+1;
      thrd = proc->member + tid;
      break;
    }
  }
  if ( thrd ) {
    thrd->id = lid;
    thrd->name = 0;
    thrd->member = 0;
    thrd->num_member = 0;
  }

  /*update loctab*/
  if ( (lid+1) > numlocs ) {
    numlocs += 32;
    loctab = x_realloc(loctab, numlocs * sizeof(Location));
  }
  loctab[lid].mach = machine + mid;
  loctab[lid].node = node;
  loctab[lid].proc = proc;
  loctab[lid].thrd = thrd;
  if ( lid+1 > totallocs ) totallocs = lid+1;
}

static void cpuname_setup() {
  int i, t;
  Group *proc;

  locmap = x_malloc(totallocs * sizeof(int));
  locremap = x_malloc(totallocs * sizeof(int));
  if ( ! isthreaded ) {
    /*plain MPI: no reordering, default names OK*/
    for (i=0; i<totallocs; i++) {
      locmap[i] = i;
      locremap[i] = i;
      if (nummachines > 1) {
	sprintf(loctab[i].proc->name, "process %d:%d", loctab[i].mach->id,
                                                       loctab[i].proc->id);
        sprintf(loctab[i].lstr, "%d:%d", loctab[i].mach->id,
	                                 loctab[i].proc->id);
      } else {
        sprintf(loctab[i].lstr, "%d", loctab[i].proc->id);
      }
    }
  } else {
    char mstr[8];

    /*reorder threads correctly*/
    t = 0;
    while ( t < totallocs ) {
      proc = loctab[t].proc;
      for (i=0; i<proc->num_member; i++) {
        locmap[proc->member[i].id] = t++;
      }
    }

    /*create better naming*/
    for (i=0; i<totallocs; i++) {
      if (nummachines > 1) {
	sprintf(mstr, "%d:", loctab[i].mach->id);
      } else {
	mstr[0] = '\0';
      }
      loctab[i].thrd->name = x_malloc(32 * sizeof(char));
      if ( loctab[i].proc->member[0].id == i ) {
	/*master thread*/
	/*sprintf(loctab[i].thrd->name, "process %d", locmap[i]);*/
	sprintf(loctab[i].thrd->name, "process %s%d", mstr, loctab[i].proc->id);
	sprintf(loctab[i].lstr, "%s%d:%d:0", mstr, loctab[i].node->id,
					     loctab[i].proc->id);
      } else {
	/*slave thread*/
	int tid = (loctab[i].thrd - loctab[i].proc->member);
	/*sprintf(loctab[i].thrd->name, "thread %d:%d",
		locmap[loctab[i].proc->member[0].id], tid);*/
	sprintf(loctab[i].thrd->name, "thread %s%d:%d", mstr,
                                      loctab[i].proc->id, tid);
        sprintf(loctab[i].lstr, "%s%d:%d:%d", mstr, loctab[i].node->id,
					      loctab[i].proc->id, tid);
      }
    }

    /*write new name in sorted order*/
    for (i=0; i<totallocs; i++) {
      for (t=0; t<totallocs; t++) {
        if ( i == locmap[t] ) {
	  locremap[i] = t;
	  break;
	}
      }
    }
  }
}

/*
 * Communicator Table Module
 */

int numcomm = 0;
int maxcomm = 0;
Comm* commtab = 0;

static void comm_add(elg_ui4 cid, elg_ui1 mode, elg_ui4 grpc, elg_ui1 *v) {
  int i;

  if ( (cid+1) > numcomm ) {
    numcomm += 20;
    commtab = x_realloc(commtab, numcomm * sizeof(Comm));
    for (i=(numcomm-20); i<numcomm; i++) {
      commtab[i].mode    = 0;
      commtab[i].numbyte = 0;
    }
  }
  commtab[cid].mode    = mode;
  commtab[cid].numbyte = grpc;
  commtab[cid].bits    = v;

  if ( cid > maxcomm ) maxcomm = cid;
}

unsigned int get_comm_loc(int l) {
  if ( ! isthreaded ) {
    /*plain MPI: location==comm member id*/
    return l;
  } else {
    int i;
    for (i=0; i<totallocs; i++) {
      if ( loctab[i].proc->id == l && loctab[i].proc->member[0].id == i )
        return locmap[i];
    }
  }
  return -1;
}

/*
 * MPI group table (new scheme)
 */

int numMpiGroups = 0;
int maxMpiGroups = 0;
MpiGroup* mpiGroupTab;

static void mpiGroup_add(elg_ui4 gid, elg_ui1 mode, elg_ui4 grpc, elg_ui4* grpv) {
  int i;

  if ( (gid+1) > numMpiGroups ) {
    numMpiGroups += 20;
    mpiGroupTab = x_realloc(mpiGroupTab, numMpiGroups * sizeof(MpiGroup));
    for (i=(numMpiGroups-20); i<numMpiGroups; i++) {
      mpiGroupTab[i].numranks = 0;
      mpiGroupTab[i].mode     = 0;
    }
  }
  mpiGroupTab[gid].mode     = mode;
  mpiGroupTab[gid].numranks = grpc;
  mpiGroupTab[gid].ranks    = grpv;

  if ( gid > maxMpiGroups ) maxMpiGroups = gid;
}

/*
 * MPI communicator table (new scheme)
 */

int numMpiComms = 0;
int maxMpiComms = 0;
MpiComm* mpiCommTab;

static void mpiComm_add(elg_ui4 cid, elg_ui4 gid) {
  int i;

  if ( (cid+1) > numMpiComms ) {
    numMpiComms += 20;
    mpiCommTab = x_realloc(mpiCommTab, numMpiComms * sizeof(MpiComm));
    for (i=(numMpiComms-20); i<numMpiComms; i++)
      mpiCommTab[i].group = ELG_NO_ID;
  }
  mpiCommTab[cid].group = gid;

  if ( cid > maxMpiComms ) maxMpiComms = cid;
}

/*
 * Metrics table
 */

int nummet = 0;
int maxmet = -1;
Metric* mettab = 0;

static
void metric_add(elg_ui4 metid, const char *metname, const char *metdescr,
                elg_ui1 metdtype, elg_ui1 metmode, elg_ui1 metiv) {
  int i;

  if ( (metid+1) > nummet ) {
    nummet += 5;
    mettab = x_realloc(mettab, nummet * sizeof(Metric));
    for (i=(nummet-5); i<nummet; i++) mettab[i].name = 0;
  }
  mettab[metid].name  = metname;
  mettab[metid].descr = metdescr;
  mettab[metid].type  = metdtype;
  mettab[metid].mode  = metmode;
  mettab[metid].iv    = metiv;
  if ((int)metid > maxmet) maxmet = metid;
}

/*
 * Windows
 */

int numwin = 0;
int* wintab = 0;

static void win_add(int wid, int cid) {
  int i;

  if ( (wid+1) > numwin ) {
    numwin += 20;
    wintab = x_realloc(wintab, numwin * sizeof(int));
    for (i=(numwin-20); i<numwin; i++) wintab[i] = -1;
  }
  wintab[wid] = cid;
}


/*
 * Topplogy Table Module
 */

int numtopol = 0;
Topology** topoltab = 0;

static
void topol_add(elg_ui4 topid, elg_ui4 tnid, elg_ui4 cid, elg_ui1 ndims, elg_ui4 dimv[], elg_ui1 periodv[], elg_ui4 dimids[]) {
  int i;

  if ( (topid+1) > numtopol ) {
    numtopol += 5;
    topoltab = x_realloc(topoltab, numtopol * sizeof(Topology*));
    for (i=(numtopol-5); i<numtopol; i++) topoltab[i] = 0;
  }
  topoltab[topid] = x_malloc(sizeof(Topology));
  topoltab[topid]->cid = cid;
  topoltab[topid]->ndims=ndims;
  topoltab[topid]->sizes = calloc(ndims,sizeof(elg_ui4));
  topoltab[topid]->periods = calloc(ndims,sizeof(elg_ui1));

  topoltab[topid]->numlocs = 1;  // Total number of locations
  for (i=0; i<ndims; i++) {
	  	topoltab[topid]->numlocs *= dimv[i];
	  	topoltab[topid]->sizes[i]=dimv[i];
	  	topoltab[topid]->periods[i]=periodv[i];

  }
  topoltab[topid]->coords=calloc(topoltab[topid]->numlocs,sizeof(elg_ui4));
}

static
void tcoord_add(elg_ui4 topid, elg_ui4 lid, elg_ui1 ndims, elg_ui4 coordv[]) {
	memcpy(topoltab[topid]->coords,coordv,sizeof(coordv));
}

/*
 * EPILOG read callbacks
 */

int addidle = 1;
int genmpiglop = 1;

static elg_ui1 length;

void elg_readcb_ALL(elg_ui1 t, elg_ui1 l, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  md->done += (length + 2);
  md->type = t;
  length = l;
}

void elg_readcb_STRING(elg_ui4 strid, elg_ui1 cntc, const char* str,
                       void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  if ( cntc ) {
    md->numcont = cntc;
    md->strpos  = malloc((250 + md->numcont*255) * sizeof(char));
    str_add(strid, md->strpos);
    strncpy(md->strpos, str, 250);
    md->strpos += 250;
  } else {
    str_add(strid, strdup(str));
  }
}

void elg_readcb_STRING_CNT(const char* str, void* userdata) {
  MYDATA* md = (MYDATA*) userdata;
  md->numcont--;
  if ( md->numcont ) {
    strncpy(md->strpos, str, 255);
    md->strpos += 255;
  } else {
    strcpy(md->strpos, str);
  }
}

void elg_readcb_MACHINE(elg_ui4 mid, elg_ui4 nodec, elg_ui4 mnid,
                        void* userdata) {
  int i;

  if ( mid >= MAXMACHINE ) {
    elg_error_msg("Too many machines: more than %d", MAXMACHINE);
  }
  nummachines++;

  if ( mnid == ELG_NO_ID ) {
    machine[mid].name = x_malloc(16 * sizeof(char));
    sprintf(machine[mid].name, "machine %u", mid);
  } else {
    machine[mid].name = stringtab[mnid];
  }
  machine[mid].id   = mid;
  machine[mid].min_id = -1;
  machine[mid].num_member = nodec;
  machine[mid].member     = x_malloc(nodec * sizeof(Group));
  for (i=0; i<nodec; ++i) {
    machine[mid].member[i].id = -1;
  }
}

void elg_readcb_NODE(elg_ui4 nid, elg_ui4 mid, elg_ui4 cpuc, elg_ui4 nnid,
                     elg_d8 cr, void* userdata) {
  int i;

  elg_ui4 nodec = machine[mid].num_member;
  int min_id    = machine[mid].min_id;
  int local_nid;
  Group *node;

  if ( min_id == -1 ) {
    /* first node of this machine */
    min_id = machine[mid].min_id = nid;
  }
  local_nid = nid - min_id;

  if ( local_nid >= nodec ) {
    elg_error_msg("Too many nodes for machine %d: more than %d", mid, nodec);
  }

  node = &(machine[mid].member[local_nid]);
  if ( nnid == ELG_NO_ID ) {
    node->name = x_malloc(16 * sizeof(char));
    sprintf(node->name, "node %u", mid);
  } else {
    node->name = stringtab[nnid];
  }
  node->id   = nid;
  node->min_id     = -1;
  node->num_member = cpuc;
  node->member     = x_malloc(cpuc * sizeof(Group));
  for (i=0; i<cpuc; ++i) {
    node->member[i].id = -1;
  }
}

void elg_readcb_PROCESS(elg_ui4 pid, elg_ui4 pnid, void* userdata) {
  /* NOT YET USED */
}

void elg_readcb_THREAD(elg_ui4 tid, elg_ui4 pid, elg_ui4 tnid, void* userdata) {
  /* NOT YET USED */
}

void elg_readcb_LOCATION(elg_ui4 lid, elg_ui4 mid, elg_ui4 nid, elg_ui4 pid,
                         elg_ui4 tid, void* userdata) {
  loc_add(lid, mid, nid, pid, tid);
}

void elg_readcb_FILE(elg_ui4 fid, elg_ui4 fnid, void* userdata) {
  file_add(fid, stringtab[fnid]);
}

void elg_readcb_REGION(elg_ui4 rid, elg_ui4 rnid, elg_ui4 fid,
                       elg_ui4 begln, elg_ui4 endln, elg_ui4 rdid,
                       elg_ui1 rtype, void* userdata) {
  act_add(rdid, stringtab[rdid]);
  state_add(rdid, rid, stringtab[rnid], rtype, scl_add(fid, begln));
  switch ( rtype ) {
    case ELG_FUNCTION:
      /* only needed for conversion of old traces */
      if ( is_mpi_collective(stringtab[rnid]) ) {
        glop_add(rid, stringtab[rnid], ELG_FUNCTION_COLL_OTHER);
      }
      break;

    case ELG_FUNCTION_COLL_BARRIER:
    case ELG_FUNCTION_COLL_ONE2ALL:
    case ELG_FUNCTION_COLL_ALL2ONE:
    case ELG_FUNCTION_COLL_ALL2ALL:
    case ELG_FUNCTION_COLL_OTHER:
      glop_add(rid, stringtab[rnid], rtype);
      break;

    case ELG_OMP_BARRIER:
    case ELG_OMP_IBARRIER:
      if ( genompglop ) {
	glop_add(rid, stringtab[rnid], ELG_FUNCTION_COLL_BARRIER);
      }
      break;
  }
}

void elg_readcb_CALL_SITE(elg_ui4 csid, elg_ui4 fid, elg_ui4 lno,
                          elg_ui4 erid, elg_ui4 lrid, void* userdata) {
  csite_add(csid, erid, scl_add(fid, lno));
}

void elg_readcb_CALL_PATH(elg_ui4 cpid, elg_ui4 rid, elg_ui4 ppid,
                          elg_ui8 order, void* userdata) {
  /* NOT YET USED */
}

void elg_readcb_METRIC(elg_ui4 metid, elg_ui4 metnid, elg_ui4 metdid,
                       elg_ui1 metdtype, elg_ui1 metmode, elg_ui1 metiv,
                       void* userdata) {
  metric_add(metid, stringtab[metnid],
             (metdid==ELG_NO_ID ? 0L : stringtab[metdid]),
             metdtype, metmode, metiv);
}

void elg_readcb_MPI_GROUP(elg_ui4 gid, elg_ui1 mode, elg_ui4 grpc,
                          elg_ui4 grpv[], void* userdata) {
  int i;

  elg_ui4* group = x_malloc(grpc * sizeof(elg_ui4));
  for (i=0; i<grpc; i++) group[i] = grpv[i];
  mpiGroup_add(gid, mode, grpc, group);
}

void elg_readcb_MPI_COMM_DIST(elg_ui4 cid, elg_ui4 root, elg_ui4 lcid,
                              elg_ui4 lrank, elg_ui4 size, 
                              void* userdata ) {
  fprintf(stderr, "Unexpected ELG_MPI_COMM_DIST record!");
  exit(EXIT_FAILURE);
}

void elg_readcb_MPI_COMM_REF(elg_ui4 cid, elg_ui4 gid, void* userdata) {
  mpiComm_add(cid, gid);
}

void elg_readcb_MPI_COMM(elg_ui4 cid, elg_ui1 mode, elg_ui4 grpc, elg_ui1 grpv[],
                         void* userdata) {
  int i;

  elg_ui1* comm = x_malloc(grpc * sizeof(elg_ui1));
  for (i=0; i<grpc; i++) comm[i] = grpv[i];
  comm_add(cid, mode, grpc, comm);
}

void elg_readcb_CART_TOPOLOGY(elg_ui4 topid, elg_ui4 tnid, elg_ui4 cid, elg_ui1 ndims,
                              elg_ui4 dimv[], elg_ui1 periodv[], elg_ui4 dimids[],
                              void* userdata) {
  topol_add(topid, tnid, cid, ndims, dimv, periodv, dimids);
}

void elg_readcb_CART_COORDS(elg_ui4 topid, elg_ui4 lid, elg_ui1 ndims,
			    elg_ui4 coordv[], void* userdata) {
  tcoord_add(topid, lid, ndims, coordv);
}

void elg_readcb_OFFSET(elg_d8 ltime, elg_d8 offset, void* userdata) {
  /* NOT USED IN GLOBAL EPILOG TRACES */
}

void elg_readcb_NUM_EVENTS(elg_ui4 eventc, void* userdata) {
  /* NOT USED YET */
}

void elg_readcb_EVENT_TYPES(elg_ui4 ntypes, elg_ui1 typev[], void* userdata) {
  /* NOT USED YET */
}

void elg_readcb_EVENT_COUNTS(elg_ui4 ntypes, elg_ui4 countv[], void* userdata) {
  /* NOT USED YET */
}

void elg_readcb_MAP_SECTION(elg_ui4 rank, void* userdata ) {
  /* NOT USED YET */
}

void elg_readcb_MAP_OFFSET(elg_ui4 rank, elg_ui4 offset, void* userdata ) {
  /* NOT USED YET */
}

void elg_readcb_IDMAP(elg_ui1 type, elg_ui1 mode, elg_ui4 count,
                      elg_ui4 mapv[], void* userdata ) {
  /* NOT USED YET */
}

void elg_readcb_MPI_WIN( elg_ui4 wid, elg_ui4 cid, void* userdata) {
  win_add(wid, cid);
}

void elg_readcb_LAST_DEF(void* userdata) {
  act_setup();
  state_setup();
  machine_setup();
  cpuname_setup();
  conv_write_definition_records();
  stk_init(totallocs);
  conv_setup();
}
