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
* \file cartesian.h
 \brief Declares a types and functions to deal with cartesian topology.
*/
#ifndef CUBEW_CARTESIAN_H
#define CUBEW_CARTESIAN_H

#ifdef __cplusplus
extern "C" {
#endif

  #include <stdio.h>
  #include "machine.h"
  #include "node.h"
  #include "process.h"
  #include "thread.h"

  typedef struct map_thrd cube_map_thrd;  ///< Declares a synonim for the general maping "map_thrd" as "cube_map_thrd"

/**
* Defines cartesian topology of the threads.
*/

  typedef struct cube_cartesian {
    char * name;                /* topology name */
    unsigned ndims;             /* Number of dimensions */
    char ** namedims;           /* in the case one decides to give names for the dimensions of a topology */
    long int* dim;              /* Array with dimensions. */
    int* period;                /* Period in ID in every dimension */
    cube_map_thrd* thrd2coord;  /* Thread -> (Coordinates) Mapping. */
  } cube_cartesian;

  cube_cartesian* cube_cart_create(cube_cartesian* cart);
  int  cube_cart_init(cube_cartesian* cart, long int ndims, long int* dim, int* period);
  void cube_cart_free(cube_cartesian* cart);
  void cube_cart_def_coords(cube_cartesian* cart, cube_thread* thrd, long int* coord);
  void cube_cart_writeXML(cube_cartesian* cart, FILE* fp);
  void cube_cart_set_name( cube_cartesian* cart, const char* name );
  void cube_cart_set_namedims( cube_cartesian* cart, char** _namedims );
#ifdef __cplusplus
}
#endif

#endif
