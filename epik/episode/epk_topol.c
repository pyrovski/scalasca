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
#include "epik_topol.h"
#include "esd_def.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdarg.h>
#include "elg_types.h"
#include "esd_proc.h"
#include "elg_thrd.h"
#include "esd_gen.h"

EXTERN EsdProc* proc;


/* Fortran Name mangling for Wrappers of topologies. See the end of this file */
#define elgf_cart_create_U ELGF_CART_CREATE
#define elgf_cart_create_L elgf_cart_create

#define elgf_cart_coords_U ELGF_CART_COORDS
#define elgf_cart_coords_L elgf_cart_coords

#define epikf_cart_create_U EPIKF_CART_CREATE
#define epikf_cart_create_L epikf_cart_create

#define epikf_cart_commit_U EPIKF_CART_COMMIT
#define epikf_cart_commit_L epikf_cart_commit

#define epikf_cart_add_dim_U EPIKF_CART_ADD_DIM
#define epikf_cart_add_dim_L epikf_cart_add_dim

#define epikf_cart_set_coords_U EPIKF_CART_SET_COORDS
#define epikf_cart_set_coords_L epikf_cart_set_coords

#define epikf_cart_coords_commit_U EPIKF_CART_COORDS_COMMIT
#define epikf_cart_coords_commit_L epikf_cart_coords_commit

#define epikf_cart_free_U EPIKF_CART_FREE
#define epikf_cart_free_L epikf_cart_free


#include "epk_fwrapper_def.h"


/* Topology "object" constructor. Allocates memory for the structures to be added later. Sets name */

EPIK_TOPOL * EPIK_Cart_create(char *name, elg_ui1 num_dims)
{
  EPIK_TOPOL * topology=calloc(1,sizeof(*topology));
  assert(topology!=0); /* Make sure calloc worked */
  topology->num_dims_defined=0;
  topology->shape_committed=0;
  free(topology->topo_name);
  topology->topo_name=strdup(name);
  topology->num_dims=num_dims;
  size_t s = sizeof(elg_ui4 *);
  topology->dim_sizes = calloc(num_dims, s);
  topology->coords = calloc(num_dims,s);
  topology->periods = calloc(num_dims,sizeof(elg_ui1 *));
  topology->dim_names = calloc(num_dims,sizeof(char *));
  return topology;
}

/* Adds a new dimension to a topology, with an optional name */
void EPIK_Cart_add_dim(EPIK_TOPOL * topology, unsigned int size, elg_ui1 periodic, char* name) {
  assert(topology!=0);
  assert(topology->num_dims_defined<=(topology->num_dims-1));
  assert(topology->shape_committed!=1); /* One cannot add things to a topology already recorded */
  topology->dim_sizes[topology->num_dims_defined] = (elg_ui4)size;
  topology->periods[topology->num_dims_defined] = periodic;
  if(name && name[0]) {
    topology->dim_names[topology->num_dims_defined]=strdup(name);
  } else {
    topology->dim_names[topology->num_dims_defined]=NULL;
  }
  topology->num_dims_defined++;
}

/* Creates the definition record for the topology. From here on topology should not be changed anymore. */
void EPIK_Cart_commit   ( EPIK_TOPOL * topology )
{
  assert(topology!=0);
  assert(topology->shape_committed==0);
  assert(topology->num_dims_defined==topology->num_dims); /* If one declares, but does not use all dimensions, you're gonna have a bad time */
  topology->shape_committed=1;
  esd_def_cart(ELG_NO_ID, topology);
}

/* Creates the definition record for the coordinates. It's done by each thread or process */
void EPIK_Cart_coords_commit    ( EPIK_TOPOL * topology )
{
  assert(topology!=0);
  assert(topology->shape_committed==1); /* Topology needs to be committed first */
  esd_def_coords(topology);
}

/* Releases memory */
void EPIK_Cart_free(EPIK_TOPOL * topology) {
  assert(topology!=0);  // Everybody hates double free
  int i;
  free(topology->dim_sizes);
  free(topology->coords);
  free(topology->periods);
  free(topology->topo_name);
  
  for(i=0;i<topology->num_dims;i++) {
    free(topology->dim_names[i]);
  }
  free(topology->dim_names);
  free(topology);
}

/* Receives a number of arguments to set coordinates */
void EPIK_Cart_set_coords(EPIK_TOPOL * topology, ...) {
  va_list arguments;

  assert(topology!=0);
  va_start (arguments, topology);
  int i;
  for(i=0;i<topology->num_dims_defined;i++) {
    topology->coords[i]=va_arg (arguments, elg_ui4);
  }
  va_end (arguments);
}

/* fortran_topologies_defined is a counter, where fortran code can use as the position on fortran_topologies_array */

static elg_ui1      fortran_topologies_defined = 0; /* total number of fortran topologies. Fortran code receives it upon creation of a new one. */
static EPIK_TOPOL **fortran_topologies_array   = 0; /* As FORTRAN code cannot call the topologies by their pointers, we keep an array of them, which is accessed by the index defined by fortran_topologies_defined */


/* These are the combinations of fortran names according to different compilers' needs. all uppercase, all lowercase, with underscore, etc */
#define FSUB FSUB1
#include "epk_ftopol.c"

#undef FSUB
#define FSUB FSUB2
#include "epk_ftopol.c"

#undef FSUB
#define FSUB FSUB3
#include "epk_ftopol.c"

#undef FSUB
#define FSUB FSUB4
#include "epk_ftopol.c"
