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
#include "elg_topol.h"
#include "epk_conf.h"
#include "elg_error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(__LIBCATAMOUNT__)
#include <catamount/dclock.h>
#include <catamount/data.h>

/* platform specific initialization */
void elg_pform_init() {}
#else

/*
 * By default, cycle counters are used
 * Undef one of the following if you want other timer
 */

#ifndef USE_POSIX_CLOCK
#undef USE_POSIX_CLOCK
#endif
#ifndef USE_GETTIMEOFDAY
#undef USE_GETTIMEOFDAY
#endif

#if defined(USE_POSIX_CLOCK) || defined(USE_GETTIMEOFDAY)
#include <time.h>
#include <sys/time.h>
static double elg_time_base = 0.0;
#endif

#ifndef ELG_PROCDIR 
#  define ELG_PROCDIR "/proc/"
#endif

static elg_ui8       elg_cycles_per_sec=1;
static unsigned char elg_cpu_has_tsc=0;
static unsigned int  elg_cpu_count=0;

void elg_pform_init()
{
  FILE   *cpuinfofp;
  char   line[1024];
  char   *token;
  
  /* size corresponding to elg_cycles_per_sec */
  elg_ui8 hz;
  
#ifdef USE_POSIX_CLOCK
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  elg_time_base = tp.tv_sec - (tp.tv_sec & 0xFF);
#endif

#ifdef USE_GETTIMEOFDAY
  struct timeval tp;
  gettimeofday(&tp, 0);
  elg_time_base = tp.tv_sec - (tp.tv_sec & 0xFFFF);
#endif

  if ((cpuinfofp = fopen (ELG_PROCDIR "cpuinfo", "r")) == NULL) 
    elg_error_msg("Cannot open file %s: %s\n", ELG_PROCDIR "cpuinfo",
                  strerror(errno));
  
  while (fgets(line, sizeof (line), cpuinfofp))
  {
    if (!strncmp("processor", line, 9))
      elg_cpu_count++;
    if (!strncmp("cpu MHz", line, 7))
    {
      strtok(line, ":");
      
      hz = strtol((char*) strtok(NULL, " \n"), (char**) NULL, 0) * 1e6;
      elg_cycles_per_sec = hz;
    }
    if (!strncmp("timebase", line, 8))
    {
      strtok(line, ":");
      
      hz = strtol((char*) strtok(NULL, " \n"), (char**) NULL, 0);
      elg_cycles_per_sec = hz;
    }
    if (!strncmp("flags", line, 5))
    { 
      strtok(line, ":");
      while ( (token = (char*) strtok(NULL, " \n"))!=NULL)
        if (strcmp(token,"tsc")==0)
        {
          elg_cpu_has_tsc=1;
          break;
        }
    }
  }
  
  fclose(cpuinfofp);
}
#endif

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

double elg_pform_wtime()
{
#if defined(__LIBCATAMOUNT__)
  return dclock();

#elif defined(USE_POSIX_CLOCK)
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  return (tp.tv_sec - elg_time_base) + (tp.tv_nsec * 1.0e-9);

#elif defined(USE_GETTIMEOFDAY)
  struct timeval tp;
  gettimeofday(&tp, 0);
  return (tp.tv_sec - elg_time_base ) + (tp.tv_usec * 1.0e-6);

#else
  /* fast assembler codes to access the cycle counter */
  elg_ui8 clock_value;

#ifdef __powerpc__
  unsigned int Low, HighB, HighA;

  do {
    asm volatile ("mftbu %0" : "=r"(HighB));
    asm volatile ("mftb %0" : "=r"(Low));
    asm volatile ("mftbu %0" : "=r"(HighA));
  } while (HighB != HighA);
  clock_value = ((unsigned long long)HighA<<32) | ((unsigned long long)Low);
#elif defined(__ia64__)
  /* ... ITC */
  asm volatile ("mov %0=ar.itc" : "=r"(clock_value));
#else
  /* ... TSC */
  {
    elg_ui4 low = 0;
    elg_ui4 high = 0;

    asm volatile ("rdtsc" : "=a" (low), "=d" (high));

    clock_value = ((elg_ui8)high << 32) + low;
  }
#endif
  
  return (double) (clock_value) / (double) elg_cycles_per_sec;
#endif
}

/* platform name */
const char*  elg_pform_name() {
  return epk_get(EPK_MACHINE_NAME);
}

/* number of nodes */
int elg_pform_num_nodes() {
#if defined(__LIBCATAMOUNT__)
  return _my_nnodes; /* only defined for Catamount */
#else
  return 1;
#endif
}

/* unique numeric SMP-node identifier */
#if defined(__LIBCATAMOUNT__)
long elg_pform_node_id() {
  return _my_pnid; /* only defined for Catamount */
}
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
long elg_pform_node_id() {
  static long nid = ELG_NO_ID;
  if (nid == ELG_NO_ID) {
      char buf[16];
      ssize_t bytes;
      int fd = open("/proc/cray_xt/nid", O_RDONLY);
      if (fd < 0) elg_error_msg("/proc/cray_xt/nid open error");
      bytes = read(fd, buf, 16);
      if (bytes <= 0) elg_error_msg("/proc/cray_xt/nid read error");
      close(fd);
      nid = atoi(buf);
  }
  return nid;  
}
#endif

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
  static char node[16];
  sprintf(node, "node%ld", elg_pform_node_id());
  return node;              
}

/* number of CPUs */
int elg_pform_num_cpus() {
  return elg_cpu_count;
}

/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}

