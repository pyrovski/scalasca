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
#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>

static  _BGP_Personality_t mybgp;  

EPIK_TOPOL * EPIK_Pform_hw_topol() {
  EPIK_TOPOL * myt;

  Kernel_GetPersonality(&mybgp, sizeof(_BGP_Personality_t));

  myt=EPIK_Cart_create("Blue Gene/P Hardware Topology",4);

  EPIK_Cart_add_dim(myt, (elg_ui4)BGP_Personality_xSize(&mybgp),
                         BGP_Personality_isTorusX(&mybgp),"X");
  EPIK_Cart_add_dim(myt, (elg_ui4)BGP_Personality_ySize(&mybgp),
                         BGP_Personality_isTorusY(&mybgp),"Y");
  EPIK_Cart_add_dim(myt, (elg_ui4)BGP_Personality_zSize(&mybgp),
                         BGP_Personality_isTorusZ(&mybgp),"Z");
  EPIK_Cart_add_dim(myt, 4, ELG_FALSE, "Core");   /* BlueGene/P quad-core */

  return myt;
}

void EPIK_Pform_hw_topol_getcoords(EPIK_TOPOL * myt) {
  EPIK_Cart_set_coords(myt,
                       BGP_Personality_xCoord(&mybgp),
                       BGP_Personality_yCoord(&mybgp),
                       BGP_Personality_zCoord(&mybgp),
                       Kernel_PhysicalProcessorID();
                      );
}
