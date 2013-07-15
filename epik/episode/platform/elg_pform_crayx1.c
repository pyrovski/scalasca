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

#include "elg_pform.h"
#include "epk_conf.h"

#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <intrinsics.h>
#include <sys/types.h>
#include <sys/sv2/apteamctl.h>

#ifndef USE_RTC_TIMER
#undef USE_RTC_TIMER  /* -- low-overhead rtc timer code does not work yet
                            with Fortran codes -- */
#endif

static double elg_time_base = 0.0;
#ifdef USE_RTC_TIMER
static double elg_rtc_base = 0.0;
static double elg_clockspeed = 1.0;
#pragma omp threadprivate(elg_time_base,elg_rtc_base)
#endif

static long elg_nodeid = -1L;
static int elg_mspmode = 1;

/* platform specific initialization */
void elg_pform_init() {
  struct timeval tp;
  static ApTeam_t app;
  int i;

#ifdef USE_RTC_TIMER
  elg_clockspeed = 1.0/sysconf(_SC_SV2_USER_TIME_RATE);
# pragma omp parallel
  {
    gettimeofday(&tp, 0);
    elg_rtc_base = _rtc();
    elg_time_base = (tp.tv_sec & 0xFFFF) + (tp.tv_usec * 1.0e-6);
  }
#else
  gettimeofday(&tp, 0);
  elg_time_base = tp.tv_sec - (tp.tv_sec & 0xFFFF);
#endif

  if (apteamctl(ApTeam_Status, 0, 0, &app) == 1) {
    elg_mspmode = (app.flags & APTEAM_MSP);
    app.pes = malloc(app.pecount * sizeof(ApPe_t));
    if (apteamctl(ApTeam_Status, 0, 0, &app) == 1) {
      for (i=0; i<_num_pes(); i++) {
        if (_my_pe() == app.pes[i].lpe) elg_nodeid = (long)app.pes[i].place;
      }
    }
  }
}

void elg_pform_mpi_init() {}

void elg_pform_mpi_finalize() {} 

/* directory of global file system  */
const char* elg_pform_gdir() {
  return epk_get(ELG_PFORM_GDIR);
}

/* directory of local file system  */
const char* elg_pform_ldir() {
  return epk_get(ELG_PFORM_LDIR);
}

/* is a global clock provided ? */ 
int elg_pform_is_gclock() {
  return 0;
}

/* local or global wall-clock time in seconds */
double elg_pform_wtime() {
#ifdef USE_RTC_TIMER
  return ( elg_time_base + ( _rtc() - elg_rtc_base ) * elg_clockspeed );
#else
  struct timeval tp;
  gettimeofday(&tp, 0);
  return (tp.tv_sec - elg_time_base ) + (tp.tv_usec * 1.0e-6);
#endif
}

/* platform name */
const char*  elg_pform_name() {
  return epk_get(EPK_MACHINE_NAME);
}

/* number of nodes */
int elg_pform_num_nodes() {
  int n = 0, i;
  int maxnodes = apteamctl(ApMgmt_GetNumnodes, 0, 0, NULL);
  ApNodeStat_t *ndlist = malloc(maxnodes*sizeof(ApNodeStat_t));
  apteamctl(ApMgmt_GetNodeStatus, 0, 0, (ApTeam_t*) ndlist);
  for (i=0; i<maxnodes; i++) {
    if (ndlist[i].ssp_count ) n++;
  }
  return n;
}

/* unique numeric SMP-node identifier */
long elg_pform_node_id() {
  return elg_nodeid;
}

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
  static char node[20];
  sprintf(node, "node%03d", elg_nodeid);
  return node;              
}

/* number of CPUs */
int elg_pform_num_cpus() {
  if (elg_mspmode)
    return 4;
  else
    return 16;
}

/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}
