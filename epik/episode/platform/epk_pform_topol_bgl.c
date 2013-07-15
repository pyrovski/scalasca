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
#include <bglpersonality.h>
#include <rts.h>

EPIK_TOPOL * EPIK_Pform_hw_topol() {
	BGLPersonality bgl;
	EPIK_TOPOL * myt;

	myt=EPIK_Cart_create("Blue Gene/L Hardware Topology",3);
	EPIK_Cart_add_dim(myt, (elg_ui4)BGLPersonality_xSize(&bgl),
                               BGLPersonality_isTorusX(&bgl), "X");
	EPIK_Cart_add_dim(myt, (elg_ui4)BGLPersonality_ySize(&bgl),
                               BGLPersonality_isTorusY(&bgl), "Y");
	EPIK_Cart_add_dim(myt, (elg_ui4)BGLPersonality_zSize(&bgl),
                               BGLPersonality_isTorusZ(&bgl), "Z");

	EPIK_Cart_set_coords(myt,
	                     BGLPersonality_xCoord(&bgl),
                             BGLPersonality_yCoord(&bgl),
                             BGLPersonality_zCoord(&bgl)
                            );
	return myt;
}

void EPIK_Pform_hw_topol_getcoords(EPIK_TOPOL * myt) {
}
