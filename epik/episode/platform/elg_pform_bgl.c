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

#include "elg_pform.h"
#include "elg_defs.h"
#include "elg_topol.h"
#include "epk_conf.h"
#include <bglpersonality.h>
#include <rts.h>

#define BGL_GROUP_ON_NODEBOARD

static double elg_clockspeed=1.0e-6/700.0;

static BGLPersonality mybgl;

/* platform specific initialization */
void elg_pform_init() {
  rts_get_personality(&mybgl, sizeof(BGLPersonality));
  elg_clockspeed = 1.0/(double)BGLPersonality_clockHz(&mybgl);
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
  return ( rts_get_timebase() * elg_clockspeed );
}

/* platform name */
const char*  elg_pform_name() {
  return epk_get(EPK_MACHINE_NAME);
}

/* number of nodes */
int elg_pform_num_nodes() {
  int nn = BGLPersonality_numPsets(&mybgl) *
           BGLPersonality_numNodesInPset(&mybgl);

#ifdef BGL_GROUP_ON_NODEBOARD
  return (nn - 1) / 32 + 1;
#else
  if ( BGLPersonality_virtualNodeMode(&mybgl) )
    return nn * 2;
  else
    return nn;
#endif
}

/* unique numeric SMP-node identifier */
long elg_pform_node_id() {
#ifdef BGL_GROUP_ON_NODEBOARD
  return ((mybgl.location >> 6) & 0x1fff);
#else
  if ( BGLPersonality_virtualNodeMode(&mybgl) )
    return ( BGLPersonality_psetNum(&mybgl) *
           BGLPersonality_numNodesInPset(&mybgl) +
           BGLPersonality_rankInPset(&mybgl)) * 2
           + rts_get_processor_id();
  else
    return BGLPersonality_psetNum(&mybgl) *
           BGLPersonality_numNodesInPset(&mybgl) +
           BGLPersonality_rankInPset(&mybgl);
#endif
}

static void bgl_getLocString(const BGLPersonality *p, char *buf) {
  unsigned row = (p->location >> 15) & 0xf;
  unsigned col = (p->location >> 11) & 0xf;
  unsigned mp  = (p->location >> 10) & 1;
  unsigned nc  = (p->location >> 6) & 0xf;
  unsigned pc  = (p->location >> 1) & 0x1f;
  unsigned asic = p->location & 1;
  const char *asicname = (asic ? "U01" : "U11");
  if (row == 0xff)
    sprintf(buf, "Rxx-Mx-N%x-J%02d-%s", nc, pc, asicname);
  else
    sprintf(buf, "R%x%x-M%d-N%x-J%02d-%s", row, col, mp, nc, pc, asicname);
}

static void bgl_getNodeidString(const BGLPersonality *p, char *buf) {
  unsigned row = (p->location >> 15) & 0xf;
  unsigned col = (p->location >> 11) & 0xf;
  unsigned mp  = (p->location >> 10) & 1;
  unsigned nc  = (p->location >> 6) & 0xf;
  if (row == 0xff)
    sprintf(buf, "Rxx-Mx-N%x", nc);
  else
    sprintf(buf, "R%x%x-M%d-N%x", row, col, mp, nc);
}

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
#ifdef BGL_GROUP_ON_NODEBOARD
  static char buf[BGLPERSONALITY_MAX_LOCATION];
  bgl_getNodeidString(&mybgl, buf);
  return buf;
#else
  static char node[128];
  unsigned x = BGLPersonality_xCoord(&mybgl);
  unsigned y = BGLPersonality_yCoord(&mybgl);
  unsigned z = BGLPersonality_zCoord(&mybgl);

  sprintf(node, "node-%03d-%03d-%03d-%d", x, y, z, rts_get_processor_id());

  /* -- BGL internal location string
  static char buf[BGLPERSONALITY_MAX_LOCATION];
  BGLPersonality_getLocationString(&mybgl, buf);
  -- */
  return node;              
#endif
}

/* number of CPUs */
int elg_pform_num_cpus() {
#ifdef BGL_GROUP_ON_NODEBOARD
  if ( BGLPersonality_virtualNodeMode(&mybgl) )
    return 64;
  else
    return 32;
#else
  return 1;
#endif
}


/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}
