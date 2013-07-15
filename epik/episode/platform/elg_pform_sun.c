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
#include <unistd.h>
#include <string.h>

/* platform specific initialization */
void elg_pform_init() {}

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
  return 0;
}

/* local or global wall-clock time in seconds */
double elg_pform_wtime() {
  return (double) gethrtime() * 1.0e-9;
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
  return sysconf(_SC_NPROCESSORS_ONLN);
}

/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}
