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

#include "epk_pform_topol.h"
#include <spi/include/kernel/location.h>
#include <stdio.h>

static Personality_t pers;

EPIK_TOPOL * EPIK_Pform_hw_topol() {
  uint64_t Nflags;
  char procname[128];
  int procid, core, hwthread, namelen;
  elg_ui4 Anodes, Bnodes, Cnodes, Dnodes, Enodes;

  elg_ui1 Atorus, Btorus, Ctorus, Dtorus, Etorus;
  Kernel_GetPersonality(&pers, sizeof(pers));
  Anodes = (elg_ui4)pers.Network_Config.Anodes; 
  Bnodes = (elg_ui4)pers.Network_Config.Bnodes; 
  Cnodes = (elg_ui4)pers.Network_Config.Cnodes; 
  Dnodes = (elg_ui4)pers.Network_Config.Dnodes; 
  Enodes = (elg_ui4)pers.Network_Config.Enodes; 
  Nflags = (elg_ui4)pers.Network_Config.NetFlags;

  if (Nflags & ND_ENABLE_TORUS_DIM_A) Atorus = 1; else Atorus = 0;
  if (Nflags & ND_ENABLE_TORUS_DIM_B) Btorus = 1; else Btorus = 0;
  if (Nflags & ND_ENABLE_TORUS_DIM_C) Ctorus = 1; else Ctorus = 0;
  if (Nflags & ND_ENABLE_TORUS_DIM_D) Dtorus = 1; else Dtorus = 0;
  if (Nflags & ND_ENABLE_TORUS_DIM_E) Etorus = 1; else Etorus = 0;

  EPIK_TOPOL * myt;
  myt=EPIK_Cart_create("BGQ Hardware Topology",7);
  EPIK_Cart_add_dim(myt, Anodes , Atorus, "A");
  EPIK_Cart_add_dim(myt, Bnodes , Btorus, "B");
  EPIK_Cart_add_dim(myt, Cnodes , Ctorus, "C");
  EPIK_Cart_add_dim(myt, Dnodes , Dtorus, "D");
  EPIK_Cart_add_dim(myt, Enodes , Etorus, "E");
  EPIK_Cart_add_dim(myt, 16, 0, "CoreID");
  EPIK_Cart_add_dim(myt, 4, 0, "HWThrd");

  return myt;
}

void EPIK_Pform_hw_topol_getcoords(EPIK_TOPOL * myt) {
  int procid, core, hwthread;
  elg_ui4 Acoord, Bcoord, Ccoord, Dcoord, Ecoord;

  Acoord = (elg_ui4)pers.Network_Config.Acoord;
  Bcoord = (elg_ui4)pers.Network_Config.Bcoord;
  Ccoord = (elg_ui4)pers.Network_Config.Ccoord;
  Dcoord = (elg_ui4)pers.Network_Config.Dcoord;
  Ecoord = (elg_ui4)pers.Network_Config.Ecoord;
  procid = Kernel_ProcessorID(); // 0-63
  core = Kernel_ProcessorCoreID(); // 0-15
  hwthread = Kernel_ProcessorThreadID(); // 0-3

  EPIK_Cart_set_coords(myt,
                       Acoord, Bcoord, Ccoord, Dcoord, Ecoord,
                       core, hwthread
                      );
}
