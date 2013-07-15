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
#include "elg_error.h"
#include "elg_topol.h"
#include "epik_topol.h"
#include "esd_def.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "elg_types.h"
#include "esd_proc.h"
#include "elg_thrd.h"
#include "esd_gen.h"

#define elgf_cart_create_U ELGf_CART_CREATE
#define elgf_cart_create_L elgf_cart_create
#define elgf_cart_coords_U ELGf_CART_COORDS
#define elgf_cart_coords_L elgf_cart_coords

#include "epk_fwrapper_def.h"

EXTERN EsdProc* proc;

static EPIK_TOPOL * internal_c_topology;

void elg_cart_create ( elg_ui4 dim0, elg_ui4 dim1, elg_ui4 dim2,
                       elg_ui1 period0, elg_ui1 period1, elg_ui1 period2 )
{
    if (internal_c_topology == 0) {
       if ( dim0 > 0 && dim1 == 0 && dim2 == 0 ) {
         internal_c_topology =  EPIK_Cart_create("C User Topology", 1);
         EPIK_Cart_add_dim(internal_c_topology, dim0, period0, "x");
       } else if ( dim0 > 0 && dim1 > 0 && dim2 == 0 ) {
         internal_c_topology =  EPIK_Cart_create("C User Topology", 2);
         EPIK_Cart_add_dim(internal_c_topology, dim0, period0, "x");
         EPIK_Cart_add_dim(internal_c_topology, dim1, period1, "y");
       } else {
         internal_c_topology =  EPIK_Cart_create("C User Topology", 3);
         EPIK_Cart_add_dim(internal_c_topology, dim0, period0, "x");
         EPIK_Cart_add_dim(internal_c_topology, dim1, period1, "y");
         EPIK_Cart_add_dim(internal_c_topology, dim2, period2, "z");
       }
       EPIK_Cart_commit(internal_c_topology);
     } else {
       elg_error_msg("C user topology already defined");
     }
}

void elg_cart_coords ( elg_ui4 coord0, elg_ui4 coord1, elg_ui4 coord2 )
{
  if (internal_c_topology != 0) {
    switch ( internal_c_topology->num_dims ) {
      case 1: EPIK_Cart_set_coords(internal_c_topology, coord0);
              break;
      case 2: EPIK_Cart_set_coords(internal_c_topology, coord0, coord1);
              break;
      case 3: EPIK_Cart_set_coords(internal_c_topology, coord0, coord1, coord2);
              break;
  }
    EPIK_Cart_coords_commit(internal_c_topology);
  } else {
    elg_error_msg("C user topology not yet defined");
  }
}

static EPIK_TOPOL *internal_for_topology = 0; /* Global EPILOG Fortran topology <=3d */

#define FSUB FSUB1
#include "elg_ftopol.c"

#undef FSUB
#define FSUB FSUB2
#include "elg_ftopol.c"

#undef FSUB
#define FSUB FSUB3
#include "elg_ftopol.c"

#undef FSUB
#define FSUB FSUB4
#include "elg_ftopol.c"

