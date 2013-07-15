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

#include "elg_topol.h"
#include <stdio.h>
#include "elg_pform.h"
#include "elg_defs.h"
#include "epk_conf.h"
#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>


#define BGP_GROUP_ON_NODEBOARD

static double elg_clockspeed=1.0e-6/850.0;

static _BGP_Personality_t mybgp;

/* platform specific initialization */
void elg_pform_init() {
  Kernel_GetPersonality(&mybgp, sizeof(_BGP_Personality_t));
  elg_clockspeed = 1.0e-6/(double)BGP_Personality_clockMHz(&mybgp);
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
  return ( _bgp_GetTimeBase() * elg_clockspeed );
}

/* platform name */
const char*  elg_pform_name() {
  return epk_get(EPK_MACHINE_NAME);
}

/* number of nodes */
int elg_pform_num_nodes() {
  int nn = BGP_Personality_numComputeNodes(&mybgp);

#ifdef BGP_GROUP_ON_NODEBOARD
  return (nn - 1) / 32 + 1;
#else
  return nn * Kernel_ProcessCount();
#endif
}

/* unique numeric SMP-node identifier */
long elg_pform_node_id() {
#ifdef BGP_GROUP_ON_NODEBOARD
  _BGP_UniversalComponentIdentifier uci;
  uci.UCI = mybgp.Kernel_Config.UniversalComponentIdentifier;
  /* use upper part of UCI (upto NodeCard, ignore lower 14bits)
   * but only use the 13 bits (1FFF) that describe row,col,mp,nc */
  return ((uci.UCI>>14)&0x1FFF);
#else
  return ( BGP_Personality_psetNum(&mybgp) *
           BGP_Personality_psetSize(&mybgp) +
           BGP_Personality_rankInPset(&mybgp)) * Kernel_ProcessCount()
           + Kernel_PhysicalProcessorID();
#endif
}

#if 0 /* unused */
static void bgp_getLocString(const _BGP_Personality_t *p, char *buf) {
   _BGP_UniversalComponentIdentifier uci;
  uci.UCI = p->Kernel_Config.UniversalComponentIdentifier;
  
  if ((uci.ComputeCard.Component == _BGP_UCI_Component_ComputeCard) ||
      (uci.IOCard.Component == _BGP_UCI_Component_IOCard)) {

    unsigned row = uci.ComputeCard.RackRow;
    unsigned col = uci.ComputeCard.RackColumn;
    unsigned mp  = uci.ComputeCard.Midplane;
    unsigned nc  = uci.ComputeCard.NodeCard;
    unsigned pc  = uci.ComputeCard.ComputeCard;
    unsigned asic = uci.UCI & 1;
    const char *asicname = (asic ? "U01" : "U11");
    if (row == 0xf)
      sprintf(buf, "Rxx-Mx-N%x-J%02d-%s", nc, pc, asicname);
    else
      sprintf(buf, "R%x%x-M%d-N%x-J%02d-%s", row, col, mp, nc, pc, asicname);
  }
}
#endif

static void bgp_getNodeidString(const _BGP_Personality_t *p, char *buf) {
   _BGP_UniversalComponentIdentifier uci;
  uci.UCI = p->Kernel_Config.UniversalComponentIdentifier;

  if ((uci.ComputeCard.Component == _BGP_UCI_Component_ComputeCard) ||
      (uci.IOCard.Component == _BGP_UCI_Component_IOCard)) {

    unsigned row = uci.ComputeCard.RackRow;
    unsigned col = uci.ComputeCard.RackColumn;
    unsigned mp  = uci.ComputeCard.Midplane;
    unsigned nc  = uci.ComputeCard.NodeCard;
    
    if (row == 0xff)
      sprintf(buf, "Rxx-Mx-N%x", nc);
    else
      sprintf(buf, "R%x%x-M%d-N%x", row, col, mp, nc);
  } else {
    sprintf(buf, "R?\?-M?-N?");
  }
}

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
#ifdef BGP_GROUP_ON_NODEBOARD
  static char buf[BGPPERSONALITY_MAX_LOCATION];
  bgp_getNodeidString(&mybgp, buf);
  return buf;
#else
  static char node[128];
  unsigned x = BGP_Personality_xCoord(&mybgp);
  unsigned y = BGP_Personality_yCoord(&mybgp);
  unsigned z = BGP_Personality_zCoord(&mybgp);

  sprintf(node, "node-%03d-%03d-%03d-%d", x, y, z, Kernel_PhysicalProcessorID());
  return node;
#endif
}

/* number of CPUs */
int elg_pform_num_cpus() {
#ifdef BGP_GROUP_ON_NODEBOARD
  return 32 * Kernel_ProcessCount();
#else
  return 1;
#endif
}


/* return core id on which thread tid is running */
int elg_pform_hw_core_id(int tid) {
    return tid;
}


