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

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "elg_pform.h"
#include "epk_conf.h"
#include <unistd.h>

#ifdef USE_SWITCH_CLOCK
#include <swclock.h>
static swclock_handle_t elg_swclk;
#define NUMRETRY 100
#endif

static double elg_time_base = 0.0;

/* platform specific initialization */
void elg_pform_init() {
#ifdef USE_SWITCH_CLOCK
  int i;
  for (i=0; i<NUMRETRY; i++) {
    if ( (elg_swclk = swclockInit()) != 0 ) break;
  }
#else
  timebasestruct_t t;
  read_real_time(&t, TIMEBASE_SZ);
  time_base_to_time(&t, TIMEBASE_SZ);
  elg_time_base = t.tb_high - (t.tb_high & 0xFFFF);
#endif
}

/* MPI event notification */
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
#ifdef USE_SWITCH_CLOCK
  return 1;
#else
  return 0;
#endif
}

/* local or global wall-clock time in seconds */
double elg_pform_wtime() {
#ifdef USE_SWITCH_CLOCK
  int i;
  double t;
  for (i=0; i<NUMRETRY; i++) {
    if ( (t = swclockReadSec(elg_swclk)) != -1.0 ) return t;
  }
  return -1.0;
#else
  timebasestruct_t t;
  read_real_time(&t, TIMEBASE_SZ);
  time_base_to_time(&t, TIMEBASE_SZ);
  return (t.tb_high - elg_time_base) + (t.tb_low * 1.0e-9);
#endif
}

/* platform name */
const char*  elg_pform_name() {
  return epk_get(EPK_MACHINE_NAME);
}

/* number of nodes */
int elg_pform_num_nodes() {
  return 1;
}

/* unique numeric SMP-node identifier */
long elg_pform_node_id() {
  return gethostid();
}

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
  static char node[64];
  char *dlmt;
  gethostname(node, 64);
  if ( (dlmt = strchr(node, '.')) != NULL ) *dlmt = '\0';
  else node[63] = '\0';
  return node;
}

/* number of CPUs */
int elg_pform_num_cpus() {
  return sysconf(_SC_NPROCESSORS_CONF);
}


/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}
