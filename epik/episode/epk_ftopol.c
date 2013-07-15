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

/* Topology "object" constructor. Allocates memory for the structures to be added later. Sets name */
elg_ui1 FSUB(epikf_cart_create)(const char* name, elg_ui4* num_dims, int namelen)
{
    /* I cannot use the topology pointers, so I create an array of pointers here */
    /* Returns to the fortran code an index of given topology. */

    /* Fortran strings come without the null terminator */
    char c_name[namelen+1];
    strncpy(c_name, name, namelen);
    c_name[namelen]='\0';

    /* Grows fortran topologies' array and stores a topology pointer on it */
    fortran_topologies_array=realloc(fortran_topologies_array, (fortran_topologies_defined+1)*(sizeof(EPIK_TOPOL *)));
    fortran_topologies_array[fortran_topologies_defined]=EPIK_Cart_create(c_name,(elg_ui1)*num_dims); /* Fortran passes integer arguments as integer(4) */
    fortran_topologies_defined++;
    return fortran_topologies_defined;
}

/* Creates the definition record for the topology. From here on topology should not be changed anymore. */
void FSUB(epikf_cart_commit)(elg_ui4* topology_index)
{
    EPIK_Cart_commit(fortran_topologies_array[(*topology_index)-1]);
}

/* Adds a new dimension to a topology, with an optional name */
void FSUB(epikf_cart_add_dim)(elg_ui4* topology_index, elg_ui4* size, elg_ui4* periodic, const char* name, int namelen)
{
    /* Fortran strings come without the null terminator */
    char c_name[namelen+1];
    strncpy(c_name, name, namelen);
    c_name[namelen]='\0';

    EPIK_Cart_add_dim(fortran_topologies_array[(*topology_index)-1], *size, (elg_ui1)*periodic, c_name); /* FORTRAN passes period as integer(4) */
}

/* Releases memory */
void FSUB(epikf_cart_free)(elg_ui4* topology_index)
{
    EPIK_Cart_free(fortran_topologies_array[(*topology_index)-1]);
}
/* Receives a number of arguments to set coordinates */
void FSUB(epikf_cart_set_coords)(elg_ui4* topology_index, elg_ui4* coords)
{
    int i;
    /* As now, epik_cart_set_coords uses a variable list of arguments, cannot call that from here */
    for( i=0; i<(fortran_topologies_array[(*topology_index)-1]->num_dims_defined) ; i++) {
        fortran_topologies_array[(*topology_index)-1]->coords[i]=coords[i];
    }
}

/* Creates the definition record for the coordinates. It's done by each thread or process */
void FSUB(epikf_cart_coords_commit)(elg_ui4* topology_index)
{
    EPIK_Cart_coords_commit(fortran_topologies_array[(*topology_index)-1]);
}
