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
#include "elg_error.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <papi.h>

#ifndef ELG_PROCDIR 
#  define ELG_PROCDIR "/proc/"
#endif


static unsigned int elg_cpu_count=0;


void elg_pform_init()
{
  FILE   *cpuinfofp;
  char   line[1024];

  /* Determine number of CPUs */
  if ((cpuinfofp = fopen (ELG_PROCDIR "cpuinfo", "r")) == NULL) 
    elg_error_msg("Cannot open file %s: %s\n", ELG_PROCDIR "cpuinfo",
                  strerror(errno));
 
  while (fgets(line, sizeof (line), cpuinfofp))
  {
    if (!strncmp("processor", line, 9))
      elg_cpu_count++;
  }
 
  fclose(cpuinfofp);

  /* Initialize PAPI */
  if (PAPI_NOT_INITED == PAPI_is_initialized())
    if (PAPI_VER_CURRENT != PAPI_library_init(PAPI_VER_CURRENT))
      elg_error_msg("PAPI library initialization failed!");
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
  return PAPI_get_real_usec() * 1.0e-6;
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
