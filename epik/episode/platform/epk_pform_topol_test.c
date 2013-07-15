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

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include "elg_pform.h"

int main(int argc, char *argv[]) {
  int rank, numranks,i;
  EPIK_TOPOL * topology;
  char buf[1024];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  elg_pform_init();
  topology = EPIK_Pform_hw_topol();
  if ( rank==0 ) {
    printf("platform    : %s\n", elg_pform_name());
    printf("Topology name: %s\n", (const char *)EPIK_Cart_get_toponame(topology));
    printf("nodes x CPUs: %u x %u\n", elg_pform_num_nodes(), elg_pform_num_cpus());
    if ( topology ) {
      if(topology->num_dims>0) {
	for(i=0;i<topology->num_dims;i++) {
	  printf("topology    : ");
	  printf("%u%c ",topology->dim_sizes[i], (topology->periods[i] ? '*' : ' '));
	} 
	printf("\n");
      } else { printf("No dimensions set!"); }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);

  if ( topology ) {

    sprintf(buf, "%s [%lu] R%05d <", elg_pform_node_name(), elg_pform_node_id(), rank);
    for(i=0; i<topology->num_dims;i++) {
      sprintf(buf,"%5u",topology->coords[i]);
    } 
    if(i>0) sprintf(buf,">\n"); else sprintf(buf,"\n");

  } else {
    sprintf(buf, "%s [%lu] R%05d", elg_pform_node_name(), elg_pform_node_id(),
	    rank);
  }
  puts(buf);

  MPI_Finalize();
  return 0;
}
