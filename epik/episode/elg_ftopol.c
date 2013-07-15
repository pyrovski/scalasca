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


/*
 * These functions can be called directly by fortran code. The fortran interface
 * for them is described in elg_ftopol.inc
 *
 * Old interface for <=3d topologies.
 */

/*
 * This function receives up to 3 dimentions with their periodicity. It allocates
 * space for then and already create the definition record. From then on, the topology
 * should not be changed, as such changes will never get recorded. The new interface
 * allows one to create the topology and commit it separately. 
 */
void FSUB(elgf_cart_create)( elg_ui4* dim0,
			     elg_ui4* dim1, 
			     elg_ui4* dim2,
			     elg_ui4* period0, 
			     elg_ui4* period1, 
			     elg_ui4* period2 )
{
  if (internal_for_topology == 0) {
    if ( *dim0 > 0 && *dim1 == 0 && *dim2 == 0 ) {
      internal_for_topology =  EPIK_Cart_create("Fortran User Topology",1);
      EPIK_Cart_add_dim(internal_for_topology, *dim0, *period0, "x");
    } else if ( *dim0 > 0 && *dim1 > 0 && *dim2 == 0 ) {
      internal_for_topology =  EPIK_Cart_create("Fortran User Topology",2);
      EPIK_Cart_add_dim(internal_for_topology, *dim0, *period0, "x");
      EPIK_Cart_add_dim(internal_for_topology, *dim1, *period1, "y");
    } else {
      internal_for_topology =  EPIK_Cart_create("Fortran User Topology",3);
      EPIK_Cart_add_dim(internal_for_topology, *dim0, *period0, "x");
      EPIK_Cart_add_dim(internal_for_topology, *dim1, *period1, "y");
      EPIK_Cart_add_dim(internal_for_topology, *dim2, *period2, "z");
    }
    EPIK_Cart_commit(internal_for_topology);
  } else {
    elg_error_msg("Fortran user topology already defined");
  }
}

/*
 * Old version of for recording the coordinates of a process or thread. The
 * same remarks made to elgf_cart_create apply.
 */
void FSUB(elgf_cart_coords)( elg_ui4* coord0,
			     elg_ui4* coord1, 
			     elg_ui4* coord2 )
{
  if (internal_for_topology != 0) {
    switch ( internal_for_topology->num_dims ) {
    case 1: EPIK_Cart_set_coords(internal_for_topology, *coord0);
            break;
    case 2: EPIK_Cart_set_coords(internal_for_topology, *coord0, *coord1);
            break;
    case 3: EPIK_Cart_set_coords(internal_for_topology, *coord0, *coord1, *coord2);
            break;
    }
    EPIK_Cart_coords_commit(internal_for_topology);
  } else {
    elg_error_msg("Fortran user topology not yet defined");
  }
}
