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
#include "elg_defs.h"
#include "epk_conf.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

static double elg_clockspeed=1.0;

/* platform specific initialization */
void elg_pform_init() {
  elg_clockspeed = 1.0/sysconf(_SC_CLK_TCK);
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
  return 1;
}

/* local or global wall-clock time in seconds */
double elg_pform_wtime() {
  return ( rtclock() * elg_clockspeed );
}

/* platform name */
const char*  elg_pform_name() {
  return epk_get(EPK_MACHINE_NAME);
}

/* number of nodes */
int elg_pform_num_nodes() {
  return sysconf(_SC_CRAY_MAXPES);
}

/* unique numeric SMP-node identifier */
long elg_pform_node_id() {
  return sysconf(_SC_CRAY_LPE);
}

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
  static char node[20];
  sprintf(node, "node%d",sysconf(_SC_CRAY_LPE));
  return node;              
}

/* number of CPUs */
int elg_pform_num_cpus() {
  return 1;
}

/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}
