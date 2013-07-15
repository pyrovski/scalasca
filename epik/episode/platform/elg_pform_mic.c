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


#include "elg_defs.h"
#include "elg_pform.h"
#include "epk_pform_topol.h"
#include "elg_topol.h"
#include "epk_conf.h"
#include "elg_error.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
# if defined(__ia64__)
#   include <asm/intrinsics.h>
# endif

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
#ifndef ELG_CPUFREQ
#  define ELG_CPUFREQ "/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq"
#endif

static elg_ui8       elg_cycles_per_sec=1;
static unsigned char elg_cpu_has_tsc=0;
static unsigned int  elg_cpu_count=0;

elg_ui8 elg_pform_cpuinfo()
{
  FILE   *cpuinfofp;
  char   line[1024];
  char   *token;
  
  /* size corresponding to elg_cycles_per_sec */
  elg_ui8 hz = 0;
  
  /* check special file */
  if ((cpuinfofp = fopen (ELG_CPUFREQ, "r")) && fgets(line, sizeof(line), cpuinfofp)) {
    fclose(cpuinfofp);
    hz = 1000*atoll(line);
    elg_cntl_msg("Got %llu cycles_per_sec from %s", hz, ELG_CPUFREQ);
    return hz;
  }
  
  if ((cpuinfofp = fopen (ELG_PROCDIR "cpuinfo", "r")) == NULL) 
    elg_error_msg("Cannot open file %s: %s\n", ELG_PROCDIR "cpuinfo",
                  strerror(errno));
  
  while (fgets(line, sizeof (line), cpuinfofp))
  {
    if (!strncmp("processor", line, 9))
      elg_cpu_count++;
#if defined(__ia64__)
    if (!strncmp("itc MHz", line, 7))
#else
    if (!strncmp("cpu MHz", line, 7))
#endif
    {
      strtok(line, ":");
      
      hz = strtol((char*) strtok(NULL, " \n"), (char**) NULL, 0) * 1e6;
    }
    if (!strncmp("timebase", line, 8))
    {
      strtok(line, ":");
      
      hz = strtol((char*) strtok(NULL, " \n"), (char**) NULL, 0);
    }
    if (!strncmp("flags", line, 5))
    { 
      strtok(line, ":");
      while ( (token = (char*) strtok(NULL, " \n"))!=NULL)
      {
        if (strcmp(token,"tsc")==0)
        {
          elg_cpu_has_tsc=1;
        } else if (strcmp(token,"constant_tsc")==0)
        {
          elg_cpu_has_tsc=2;
        }
      }
    }
  }
  
  fclose(cpuinfofp);
  elg_cntl_msg("Got %llu cycles_per_sec from %s", hz, ELG_PROCDIR "cpuinfo");
  return hz;
}

void elg_pform_init()
{
#ifdef USE_POSIX_CLOCK
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  elg_time_base = tp.tv_sec - (tp.tv_sec & 0xFF);
  elg_cntl_msg("USE_POSIX_CLOCK: time_base = %.09f", elg_time_base);

#elif USE_GETTIMEOFDAY
  struct timeval tp;
  gettimeofday(&tp, 0);
  elg_time_base = tp.tv_sec - (tp.tv_sec & 0xFFFF);
  elg_cntl_msg("USE_GETTIMEOFDAY: time_base = %.09f", elg_time_base);

#else

  char* env = getenv("ESD_CLOCK_HZ");
  if (env && (atoll(env) > 0)) {
      elg_cycles_per_sec = atoll(env);
      elg_cntl_msg("Got %llu cycles_per_sec from ESD_CLOCK_HZ", elg_cycles_per_sec);
#ifdef USE_CLOCK_HZ
  } else if (USE_CLOCK_HZ > 0) {
      elg_cycles_per_sec = USE_CLOCK_HZ;
      elg_cntl_msg("Got %llu cycles_per_sec from USE_CLOCK_HZ", elg_cycles_per_sec);
#endif
  } else {
      elg_cycles_per_sec = elg_pform_cpuinfo();
  }

  if (elg_cycles_per_sec == 0) {
      elg_error_msg("Failed to determine valid value for cycles_per_sec");
  }
#endif
}

void elg_pform_mpi_init()
{
}

void elg_pform_mpi_finalize()
{
} 

const char* elg_pform_gdir()
{
  return epk_get(ELG_PFORM_GDIR);
}

const char* elg_pform_ldir()
{
  return epk_get(ELG_PFORM_LDIR);
}


int elg_pform_is_gclock()
{
  return 0;
}

double elg_pform_wtime()
{
#ifdef USE_POSIX_CLOCK
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
  clock_value = __getReg(_IA64_REG_AR_ITC);
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
const char*  elg_pform_name()
{
  return epk_get(EPK_MACHINE_NAME);
}

int  elg_pform_num_nodes()
{
  return 1;
}

long elg_pform_node_id()
{
  return gethostid(); 
}

const char* elg_pform_node_name()
{
  static char node[64];
  char *dlmt;
  gethostname(node, 64);
  if ( (dlmt = strchr(node, '.')) != NULL ) *dlmt = '\0';
  else node[63] = '\0';
  return node;
}

int elg_pform_num_cpus()
{
   return elg_cpu_count;
}
                                                                                

/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}
