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
#include "epk_pform_topol.h"

#if (defined (ELG_MPI) || defined (ELG_OMPI)) 
#  include <mpi.h>
#  include <mpi-ext.h>

static int rank = 0;
static int num_dims = 0;
static const char* host = 0;
#endif

#define FAILURE		 1


/* hardware topology */
EPIK_TOPOL * EPIK_Pform_hw_topol() {

#if (defined (ELG_MPI) || defined (ELG_OMPI)) 
  EPIK_TOPOL * myt;
  int xsize, ysize, zsize;
  int rc;

  host = elg_pform_node_name();
  PMPI_Comm_rank(MPI_COMM_WORLD, &rank);

  rc = FJMPI_Topology_get_dimension(&num_dims);
  if (MPI_SUCCESS != rc) {
    fprintf(stderr, "[%s] FJMPI_Topology_get_dimension ERROR\n", host);
    MPI_Abort(MPI_COMM_WORLD, FAILURE);
  }

  rc = FJMPI_Topology_get_shape(&xsize, &ysize, &zsize);
  if (MPI_SUCCESS != rc) {
    fprintf(stderr, "[%s] FJMPI_Topology_get_shape ERROR\n", host);
    MPI_Abort(MPI_COMM_WORLD, FAILURE);
  }

  myt=EPIK_Cart_create("Fujitsu FJMPI Topology", num_dims);

  switch(num_dims) { 
    case 1: EPIK_Cart_add_dim(myt, xsize, 1, "x");
	    break;
    case 2: EPIK_Cart_add_dim(myt, xsize, 1, "x");
            EPIK_Cart_add_dim(myt, ysize, 1, "y");
	    break;
    case 3: EPIK_Cart_add_dim(myt, xsize, 1, "x");
            EPIK_Cart_add_dim(myt, ysize, 1, "y");
            EPIK_Cart_add_dim(myt, zsize, 1, "z");
	    break;
    default:
      fprintf(stderr, "[%s] Dimension size ERROR\n", host);
      MPI_Abort(MPI_COMM_WORLD, FAILURE);
  }
  return myt;
#else
  return 0;
#endif
}

void EPIK_Pform_hw_topol_getcoords(EPIK_TOPOL * myt) {

#if (defined (ELG_MPI) || defined (ELG_OMPI)) 
  int x, y, z;
  int rc;

  switch(num_dims) { 
    case 1:
      rc = FJMPI_Topology_rank2x(rank, &x);
      EPIK_Cart_set_coords(myt, x);
      break;
    case 2:
      rc = FJMPI_Topology_rank2xy(rank, &x, &y);
      EPIK_Cart_set_coords(myt, x, y);
      break;
    case 3:
      rc = FJMPI_Topology_rank2xyz(rank, &x, &y, &z);
      EPIK_Cart_set_coords(myt, x, y, z);
      break;
    default:
      fprintf(stderr, "[%s] Dimension size ERROR\n", host);
      MPI_Abort(MPI_COMM_WORLD, FAILURE);
  }

  if (MPI_SUCCESS != rc) {
    fprintf(stderr, "[%s] Map rank to coordinates ERROR\n", host);
    MPI_Abort(MPI_COMM_WORLD, FAILURE);
  }

#endif
}
