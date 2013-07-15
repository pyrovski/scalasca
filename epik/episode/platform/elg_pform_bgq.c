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
#include <firmware/include/personality.h>
#include <spi/include/kernel/process.h>
#include <spi/include/kernel/location.h>
#ifdef __GNUC__
#include <hwi/include/bqc/A2_inlines.h>   // for GetTimebase()
#endif
#include <hwi/include/common/uci.h>

#define BGQ_GROUP_ON_NODEBOARD

static double elg_clockspeed=1.0e-6/DEFAULT_FREQ_MHZ;

static int torus_size[6];
static int torus_coord[6];
static int torus_joined[6];

static Personality_t mybgq;

/* platform specific initialization */
void elg_pform_init() {
  uint64_t netflags;

  Kernel_GetPersonality(&mybgq, sizeof(Personality_t));
  elg_clockspeed = 1.0e-6/(double)(mybgq.Kernel_Config.FreqMHz);
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
#ifdef __IBMC__
  return ( __mftb() * elg_clockspeed );
#elif defined __GNUC__
  return ( GetTimeBase() * elg_clockspeed );
#else
#error "Platform BGQ: cannot determine timebase"
#endif
}

/* platform name */
const char*  elg_pform_name() {
  return epk_get(EPK_MACHINE_NAME);
}

/* number of nodes */
int elg_pform_num_nodes() {
  return ( torus_size[0] * torus_size[1] * torus_size[2] *
           torus_size[3] * torus_size[4]);
}

/* unique numeric SMP-node identifier */
long elg_pform_node_id() {
  BG_UniversalComponentIdentifier uci = mybgq.Kernel_Config.UCI;
  /* use upper part of UCI (26bit; upto ComputeCard; ignore lower 38bit)
   * but only use the 20 bits (FFFFF) that describe row,col,mp,nb,cc */
  return ((uci>>38)&0xFFFFF);
}

/* unique string SMP-node identifier */
const char* elg_pform_node_name() {
  static char buf[48];
  elg_ui4 Acoord, Bcoord, Ccoord, Dcoord, Ecoord;
  Acoord = (elg_ui4)mybgq.Network_Config.Acoord;
  Bcoord = (elg_ui4)mybgq.Network_Config.Bcoord;
  Ccoord = (elg_ui4)mybgq.Network_Config.Ccoord;
  Dcoord = (elg_ui4)mybgq.Network_Config.Dcoord;
  Ecoord = (elg_ui4)mybgq.Network_Config.Ecoord;

  BG_UniversalComponentIdentifier uci = mybgq.Kernel_Config.UCI;
  unsigned int row, col, mp, nb, cc;
  bg_decodeComputeCardOnNodeBoardUCI(uci, &row, &col, &mp, &nb, &cc);
  sprintf(buf, "R%x%x-M%d-N%02x-J%02x <%d,%d,%d,%d,%d>", row, col, mp, nb, cc,
          Acoord, Bcoord, Ccoord, Dcoord, Ecoord);
  return buf;
}

/* number of CPUs */
int elg_pform_num_cpus() {
  return 64;
}

int elg_pform_hw_core_id(int tid) {
  return Kernel_ProcessorID();
}

/* helper function for BG/Q sionlib support */
int elg_pform_bridge() {
  int bridge;
  int factor = 1;
  bridge  = mybgq.Network_Config.cnBridge_E;
  factor *= mybgq.Network_Config.Enodes;
  bridge += mybgq.Network_Config.cnBridge_D*factor;
  factor *= mybgq.Network_Config.Dnodes;
  bridge += mybgq.Network_Config.cnBridge_C*factor;
  factor *= mybgq.Network_Config.Cnodes;
  bridge += mybgq.Network_Config.cnBridge_B*factor;
  factor *= mybgq.Network_Config.Bnodes;
  bridge += mybgq.Network_Config.cnBridge_A*factor;
  return bridge;  
}
