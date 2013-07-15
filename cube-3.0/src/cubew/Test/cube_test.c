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
/**
* \file cube_test.c
  \brief A small example of using "libcubew3.a".
  One creates a cube file "example.cube", defined structure of metrics, call tree, machine, cartesian topology
   and fills teh severity matrix.
  At the end the file will be written on the disk.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "cube.h"

#define NDIMS 2

char* cubefile = "example.cube"; ///< A cube file

/// Start point of this example.
int main(int argc, char* argv[])
{
    FILE* fp = fopen(cubefile, "w");
    cube_t* cube=cube_create();
    if (!cube) {
        fprintf(stderr,"cube_create failed!\n");
        exit(1);
    }

    /* generate header */
    cube_def_mirror(cube, "http://icl.cs.utk.edu/software/kojak/");
    cube_def_attr(cube, "description", "a simple example");
    cube_def_attr(cube, "experiment time", "November 1st, 2004");

    if (fp == NULL) {
        fprintf(stderr,"Error opening file %s: %s\n", cubefile, strerror(errno));
        exit(2);
    } else
        printf("Test file %s being generated ...\n", cubefile);

    /* define metric tree */
    cube_metric *met0, *met1, *met2;
    met0 = cube_def_met(cube, "Time", "Uniq_name1", "", "sec", "",
                "@mirror@patterns-2.1.html#execution",
                "root node", NULL);
    met1 = cube_def_met(cube, "User time", "Uniq_name2", "", "sec", "",
                "http://www.cs.utk.edu/usr.html",
                "2nd level", met0);
    met2 = cube_def_met(cube, "System time", "Uniq_name3", "", "sec", "",
                "http://www.cs.utk.edu/sys.html",
                "2nd level", met0);

    /* define call tree */
    char* mod = "/ICL/CUBE/example.c";
    cube_region *regn0, *regn1, *regn2;
    regn0 = cube_def_region(cube, "main", 21, 100, "", "1st level", mod);
    regn1 = cube_def_region(cube, "<<init>> foo", 1, 10, "", "2nd level", mod);
    regn2 = cube_def_region(cube, "<<loop>> bar", 11, 20, "", "2nd \"level\"", mod);

    cube_cnode *cnode0, *cnode1, *cnode2;
    cnode0 = cube_def_cnode_cs(cube, regn0, mod, 21, NULL);
    cnode1 = cube_def_cnode_cs(cube, regn1, mod, 60, cnode0);
    cnode2 = cube_def_cnode_cs(cube, regn2, mod, 80, cnode0);

    /* define location tree */
    cube_machine* mach  = cube_def_mach(cube, "MSC<<juelich>>", "");
    cube_node*    node  = cube_def_node(cube, "Athena <<\"JUROPA\">>", mach);
    cube_process* proc0 = cube_def_proc(cube, "Process << benchmark>> 0", 0, node);
    cube_process* proc1 = cube_def_proc(cube, "Process 1<<gauss integrations benchmark>>", 1, node);
    cube_thread*  thrd0 = cube_def_thrd(cube, "Thread 0 <<master \"scheduler\">>", 0, proc0);
    cube_thread*  thrd1 = cube_def_thrd(cube, "Thread 0 <<master \"scheduler\">>", 0, proc1);
    cube_thread*  thrd2 = cube_def_thrd(cube, "Thread 1 <<worker \"solver\">>", 1, proc1);

    /* define first topology */
    long dimv0[NDIMS] = { 5, 5 };
    int periodv0[NDIMS] = { 1, 0 };


    cube_cartesian* cart0 = cube_def_cart(cube, NDIMS, dimv0, periodv0);
    //cube_cart_set_name(cart0,"First");
    long coordv[NDIMS] = { 0, 0};
    cube_def_coords(cube, cart0, thrd1, coordv);

    char * namedims[NDIMS]={"First Dimension","Second Dimension"};

    /* define second topology*/
    cube_cart_set_namedims(cart0,namedims);
    long dimv1[4]    = { 3, 3, 3, 3 };
    int  periodv1[4] = { 1, 0, 0, 0 };
    cube_cartesian* cart1 = cube_def_cart(cube, 4, dimv1, periodv1);

    long coordv0[4] = { 0, 1, 0, 0 };
    long coordv1[4] = { 1, 0, 0 ,0 };
    cube_def_coords(cube, cart1, thrd0, coordv0);
    cube_def_coords(cube, cart1, thrd1, coordv1);
    cube_cart_set_name(cart1,"Second");

    /* define third topology*/
    long dimv2[14]    = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
    int  periodv2[14] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cube_cartesian* cart2 = cube_def_cart(cube, 14, dimv2, periodv2);

    long coordv2[14] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 };
    long coordv3[14] = { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    cube_def_coords(cube, cart2, thrd0, coordv2);
    cube_def_coords(cube, cart2, thrd1, coordv3);
    cube_cart_set_name(cart2,"Third");


    /* generate definitions */
    fflush(fp);
    cube_write_def(cube, fp);
    fflush(fp);
    /* set severities */
    cube_set_sev(cube, met0, cnode0, thrd0, 4);
    cube_set_sev(cube, met0, cnode0, thrd1, 0);
    cube_set_sev(cube, met0, cnode1, thrd0, 5);
    cube_set_sev(cube, met0, cnode1, thrd1, 9);
    cube_set_sev(cube, met1, cnode0, thrd0, 2);
    cube_set_sev(cube, met1, cnode0, thrd1, 1);
    /* unset severities default to zero */
    cube_set_sev(cube, met1, cnode1, thrd1, 3);

    /* generate severities */
    cube_write_sev_matrix(cube, fp);

    if (fclose(fp)) {
        fprintf(stderr,"Error closing file %s: %s\n", cubefile, strerror(errno));
    } else
        printf("Test file %s complete.\n", cubefile);

    
    cube_free(cube);


    return 0;
}
