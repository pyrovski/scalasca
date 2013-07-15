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

#ifndef _EPIK_TOPOL_H
#define _EPIK_TOPOL_H


#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"


/*
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing, Investigating, and Logging)
 *
 *  - Interface for defining topologies
 *
 *-----------------------------------------------------------------------------
 */


/* Generic topology */
typedef struct {
  elg_ui4 topid;
  elg_ui1 num_dims;         /* Number of dimensions */
  elg_ui4* dim_sizes;       /* Size of every dimension */
  elg_ui1* periods;         /* periodicity */
  elg_ui4* coords;          /* Coordinates of this given process  */
  char *topo_name;          /* Topology name.  */
  char **dim_names;         /* Dimension names. */

  /*private*/
  elg_ui1 num_dims_defined; /* Number of dimensions defined by add_dim() */
  elg_ui1 shape_committed;   /* Was it already send to epik buffer? */
} EPIK_TOPOL;

/*
 * Topology constructor
 */
EXTERN EPIK_TOPOL * EPIK_Cart_create(char *name, elg_ui1 num_dims);

/*
 * Topology destructor
 */
EXTERN void EPIK_Cart_free ( EPIK_TOPOL * top ) ;

/*
 * Add a new named dimension to a topology
 */
EXTERN void EPIK_Cart_add_dim ( EPIK_TOPOL * top,
                              unsigned int size, elg_ui1 periodic, char* name );

/* 
 * Commit Topology
 */
EXTERN void EPIK_Cart_commit       ( EPIK_TOPOL * top );

/*
 * Set coordinates for caller process/thread
 * 2nd to Nth argmument is coordinate for dimension 0 to N-2
 */
EXTERN void EPIK_Cart_set_coords ( EPIK_TOPOL * top, ... );

/* 
 * Commit Coordinates
 */
EXTERN void EPIK_Cart_coords_commit ( EPIK_TOPOL * top );


/*
 * Macros for epik user code
 */
#ifdef EPIK
  #define EPIK_CART_CREATE(topology_index, name, num_dims)	        EPIK_TOPOL *topology_index = EPIK_Cart_create(name,num_dims)
  #define EPIK_CART_FREE(topology_index)			                EPIK_Cart_free(topology_index)
  #define EPIK_CART_ADD_DIM(topology_index, size, periodic, name)   EPIK_Cart_add_dim(topology_index,size,periodic,name)
  #define EPIK_CART_COMMIT(topology_index)                          EPIK_Cart_commit(topology_index)
  #define EPIK_CART_SET_COORDS(topology_index, ...)                 EPIK_Cart_set_coords(topology_index, __VA_ARGS__)
  #define EPIK_CART_GET_COORDS(topology_index)                      EPIK_Cart_get_coords(topology_index)
  #define EPIK_CART_COORDS_COMMIT(topology_index)                   EPIK_Cart_coords_commit(topology_index)
#else
  #define EPIK_CART_CREATE(topology_index, name, num_dims)
  #define EPIK_CART_FREE(topology_index)
  #define EPIK_CART_ADD_DIM(topology_index, size, periodic, name)
  #define EPIK_CART_COMMIT(topology_index)
  #define EPIK_CART_SET_COORDS(topology_index, ...)
  #define EPIK_CART_GET_COORDS(topology_index)
  #define EPIK_CART_COORDS_COMMIT(topology_index)
#endif

#endif
