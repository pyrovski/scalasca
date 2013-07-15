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
* \file cartesian.c
 \brief Defines a types and functions to deal with cartesian topology.
*/
#include <stdlib.h>
#include <string.h>
#include "cartesian.h"
#include "machine.h"
#include "vector.h"
#include "services.h"

/**
* Maps a thread and its three-dimenasional coordinates.
*/
typedef struct map_thrd {
  cube_thread* key;
  long int * value;
} map_thrd;



/**
* Allocates memory for the array with definition of coordnates for every thread.
*/
cube_cartesian* cube_cart_create(cube_cartesian* this) {
  if (this == NULL) {
    ALLOC(this, 1, cube_cartesian);
  }
  this->name = NULL;
  return this;
}

/**
* Allocates memory for the arrays for dimensions and set the values for them.
*/
int cube_cart_init(cube_cartesian* this, long int ndims, long int* dim, int* period) {
  int i=0, locs=1;
  this->ndims = ndims;
  ALLOC(this->dim, ndims, long int);
  ALLOC(this->period, ndims, int);
  ALLOC(this->namedims,ndims, char *);
  if ((this->dim == NULL) || (this->period == NULL)) return 1;
  for (i = 0; i < ndims; i++) {
    locs *= dim[i];
    this->dim[i] = dim[i];
    this->period[i] = period[i];
  }


  ALLOC(this->thrd2coord, locs, map_thrd);

  for (i=0;i<locs;i++) {
    ALLOC(this->thrd2coord[i].value,ndims,long int);
  }

  return (this->thrd2coord == NULL);
}

/**
* Releases allocated memory
*/
void cube_cart_free(cube_cartesian* this) {
  if (this != NULL)    {
      int i = 0, locs = 1;
      for (i = 0; i < this->ndims; i++)
	{
	  locs *= this->dim[i];
	  free(this->namedims[i]);	  
	} 
      
    free(this->dim);
    free(this->namedims);
    free(this->period);
      for (i = 0; i < locs; i++)
	{
	  free (this->thrd2coord[i].value);
	}

    free(this->thrd2coord);
    free(this->name);
    free(this);
  }
}

/**
* Set for given thread "thrd" in the cartesian topology "this" coordinates "coord"
*/

void cube_cart_def_coords(cube_cartesian* this, cube_thread* thrd, long int *coord) {
  int i = 0;
  int pos    = coord[0];
  int factor = this->dim[0];
  for (i = 1; i < this->ndims; i++) {
    pos    += coord[i] * factor;
    factor *= this->dim[i];
  }
  map_thrd* m = &this->thrd2coord[pos];

  m->key = thrd;
  for (i = 0; i < this->ndims; i++) {
    m->value[i] = coord[i];
  }
}


/**
   Sets the name of cartesian topology.
 */
void
cube_cart_set_name( cube_cartesian* cart,
                    const char*     name )
{
    if ( cart->name != NULL )
    {
        free( cart->name );
    }
    cart->name = cubew_strdup( name );
}

/**
   Sets the name of the dimensions
 */
void
cube_cart_set_namedims( cube_cartesian* cart,
			char**    _namedims )
{
  int i=0;
  for(i=0;i<cart->ndims;i++) {
    cart->namedims[i]= cubew_strdup(_namedims[i]);
  }
  
}


/**
* Writes topology in XML format.
*/
void cube_cart_writeXML(cube_cartesian* this, FILE* fp) {
  int i = 0, j = 0, k = 0;
  int locs = 1;
  char* indent = "    ";
  char* per;
  
  fprintf( fp, "%s  <cart ",  indent );
  if ( this->name != NULL && (strlen(this->name)>0) )
	  {
	    char* _name =   ( char* )services_escape_to_xml( this->name );
	    fprintf( fp, "name=\"%s\" ", _name );
	    free(_name);
      }
  fprintf( fp, "ndims=\"%u\">\n", this->ndims );

  for (i = 0; i < this->ndims; i++) {
    locs *= this->dim[i];
    if (this->period[i] == 0) per = "false";
    else per = "true";
    fprintf(fp, "%s    <dim ",indent);
    if(this->namedims[i]!=NULL) {
      char *dimName=(char* )services_escape_to_xml(this->namedims[i]);
      fprintf(fp,"name=\"%s\" ",dimName);
      free(dimName);
    }
    fprintf(fp,"size=\"%ld\" periodic=\"%s\"/>\n", this->dim[i], per);
  }

  for (j = 0; j < locs; j++) {
    map_thrd* m = &this->thrd2coord[j];
    cube_thread* t = m->key;
    if (t) {
      fprintf(fp, "%s    <coord thrdId=\"%d\">", indent, cube_thread_get_id(t));
      for (k = 0; k < this->ndims; k++) {
        if (k != 0) fprintf(fp, " ");
        fprintf(fp, "%ld", m->value[k]);
      }
      fprintf(fp, "</coord>\n");
    }
  }
  fprintf(fp, "%s  </cart>\n", indent);
}
