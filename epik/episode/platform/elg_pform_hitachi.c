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
#include <sys/time.h>
#include "elg_pform.h"
#include "epk_conf.h"
#include "lrz.h"

static double elg_time_base = 0.0;

void elg_pform_init() {
  struct timeval tp;
  gettimeofday(&tp, 0);
  elg_time_base = tp.tv_sec;
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
  struct timeval tp;
  gettimeofday(&tp, 0);
  return (tp.tv_sec - elg_time_base ) + (tp.tv_usec * 1.0e-6);
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
  int coord[3];
  getxyz(coord);
  return (coord[0]*100 + coord[1]*10 + coord[2]);
}

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
  static char node[20];
  int coord[3];
  getxyz(coord);
  sprintf(node, "node%d.%d.%d", coord[0], coord[1], coord[2]);
  return node;
}

/* number of CPUs */
int elg_pform_num_cpus() {
  return 8;
}


/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}
