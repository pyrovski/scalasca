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
#include <unistd.h>
#include "elg_pform.h"

int main() {
  double t1, t2;
  int i;
  ELG_TOPOL* top;

  elg_pform_init();
  top = elg_pform_hw_topol();
  printf("platform            : %s\n", elg_pform_name());
  printf("nodes x CPUs        : %u x %u\n", elg_pform_num_nodes(), elg_pform_num_cpus());
  printf("node id             : %lu\n", elg_pform_node_id());
  printf("node name           : %s\n", elg_pform_node_name());
  if ( top ) {
    /*  printf("topology            : %u%c x %u%c x %u%c  [%u,%u,%u]\n", 
           top->xsize, (top->xperiod ? '*' : ' '),
           top->ysize, (top->yperiod ? '*' : ' '),
           top->zsize, (top->zperiod ? '*' : ' '),
           top->xcoord, top->ycoord, top->zcoord);*/
  }
  printf("global directory    : %s\n", elg_pform_gdir());
  printf("node local directory: %s\n", elg_pform_ldir());
  printf("global clock        : %d\n", elg_pform_is_gclock());
  printf("current time        : %16.9f\n", t1 = elg_pform_wtime());
  //  sleep(3);
  printf("current time        : %16.9f -> %16.9f\n",
                                           t2 = elg_pform_wtime(), (t2-t1));
  t1 = elg_pform_wtime();
  for (i=0; i<1000; i++) t2 = elg_pform_wtime();
  printf("per call            : %16.9f\n", (t2 - t1) / 1000.0);
  return 0;
}
