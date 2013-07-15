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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "elg_pform.h"
#include "elg_error.h"
#include "epk_conf.h"

#ifdef USE_MMTIMER
# include <errno.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/ioctl.h>
# include <sn/mmtimer.h>
# include <sys/mman.h>

  static volatile unsigned long *mmdev_timer_addr;
  static double mmdev_secs_per_tick;
#else
# include <time.h>

  static double elg_time_base = 0.0;
#endif

/* platform specific initialization */
void elg_pform_init() {
#ifdef USE_MMTIMER
  int fd;
  unsigned long femtosecs_per_tick = 0;
  int offset;

  if((fd = open(MMTIMER_FULLNAME, O_RDONLY)) == -1) {
    elg_error_msg("Failed to open " MMTIMER_FULLNAME);
  }

  if ((offset = ioctl(fd, MMTIMER_GETOFFSET, 0)) == -ENOSYS) {
    elg_error_msg("Cannot get mmtimer offset");
  }

  if ((mmdev_timer_addr = mmap(0, getpagesize(), PROT_READ, MAP_SHARED, fd, 0))
       == NULL) {
    elg_error_msg("Cannot mmap mmtimer");
  }
  mmdev_timer_addr += offset;

  ioctl(fd, MMTIMER_GETRES, &femtosecs_per_tick);
  mmdev_secs_per_tick = 1e-15 * femtosecs_per_tick;

  close(fd);
#else
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  elg_time_base = tp.tv_sec - (tp.tv_sec & 0xFF);
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
  return 1;
}

/* local or global wall-clock time in seconds */
double elg_pform_wtime() {
#ifdef USE_MMTIMER
  return mmdev_secs_per_tick * (*mmdev_timer_addr);
#else
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  return (tp.tv_sec - elg_time_base) + (tp.tv_nsec * 1.0e-9);
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
