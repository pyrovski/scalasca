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

#ifndef ELG_DVECTOR_H
#define ELG_DVECTOR_H

#include <stdlib.h>
#include <assert.h>

typedef struct {
  int size;
  int capacity;
  int chunk;
  void* data;
} Dvector;

#define dvec_create(vec, type, chunksize) \
{ \
  vec.size     = 0; \
  vec.capacity = chunksize; \
  vec.chunk    = chunksize; \
  vec.data     = malloc(chunksize * sizeof(type)); \
  assert(vec.data); \
}

#define dvec_insert(vec, type, idx, val) \
{ \
  while ( (idx) >= vec.capacity ) { \
    vec.capacity += vec.chunk; \
    vec.data = realloc(vec.data, vec.capacity * sizeof(type)); \
    assert(vec.data); \
  } \
  { \
    type* dv = (type*) vec.data; \
    dv[idx] = val; \
    if ( (idx) >= vec.size ) vec.size = (idx) + 1; \
  } \
}

#define dvec_get(vec, type, idx) (*(((type*) vec.data) + (idx)))

#define dvec_pushback(vec, type, val) dvec_insert(vec, type, vec.size, val)

#define dvec_getidx(vec, type, val, idx) \
{ \
  type* dv = (type*) vec.data; \
  int i; \
  idx = -1; \
  for (i=0; i<vec.size; i++) if ( dv[i] == val ) { idx = i; break; } \
} 

#define dvec_zero(vec, chunksize) \
{ \
  vec.size     = 0; \
  vec.capacity = 0; \
  vec.chunk    = chunksize; \
  vec.data     = 0; \
}

#endif
