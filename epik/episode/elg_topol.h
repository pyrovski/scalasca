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

#ifndef _ELG_TOPOL_H
#define _ELG_TOPOL_H


#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"


/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Interface for defining topologies
 *
 *-----------------------------------------------------------------------------
 */

/* elg_cart_create() defines the user cartesian topology
   the first three parameters define number of processes
   in each dimension and the next three parameters define
   the periodicity in each dimension */

EXTERN void elg_cart_create ( elg_ui4 dim0, elg_ui4 dim1, elg_ui4 dim2,
                              elg_ui1 period0, elg_ui1 period1, elg_ui1 period2
                            );

/* elg_cart_coords()  */
EXTERN void elg_cart_coords ( elg_ui4 coord0, elg_ui4 coord1, elg_ui4 coord2 );

#endif
