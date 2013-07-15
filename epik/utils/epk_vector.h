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

#ifndef _EPK_VECTOR_H
#define _EPK_VECTOR_H


#include <stddef.h>


#ifdef __cplusplus
#  define EXTERN extern "C"
#else
#  define EXTERN extern 
#endif


/*--- Type definitions ----------------------------------------------------*/

/** Opaque data structure representing an dynamic array. */
typedef struct EpkVector_struct EpkVector;

/**
 * Pointer-to-function type describing comparison functions that can be used
 * with epk_vector_find() or epk_vector_lower_bound(). It has to return 0 if
 * @a value equals @a item, a negative value if @a value is less than @a item,
 * or a positive value if @a value is greater than @a item.
 *
 * @param value Searched value
 * @param item  Current array element
 *
 * @return Zero if @a value is equal to @a item, a negative value if @a value
 *         @< @a item, and a positive value if @a value @> @a item.
 */
typedef int (*epk_vec_cmp_f)(const void* value, const void* item);

/**
 * Pointer-to-function type describing unary processing functions that can
 * be used with epk_vector_foreach(). Here, the current array element will
 * be passed as parameter @a item.
 *
 * @param item Current array element
 */
typedef void (*epk_vec_proc_f)(void* item);


/*
 *---------------------------------------------------------------------------
 * EpkVector
 *---------------------------------------------------------------------------
 */

/** @name Dynamic array functions
 ** @{ */

/* Construction & destruction */
EXTERN EpkVector* epk_vector_create     ();
EXTERN EpkVector* epk_vector_create_size(size_t     capacity);
EXTERN void       epk_vector_free       (EpkVector* instance);

/* Size operations */
EXTERN size_t     epk_vector_size (const EpkVector* instance);
EXTERN int        epk_vector_empty(const EpkVector* instance);

/* Capacity operations */
EXTERN size_t     epk_vector_capacity(const EpkVector* instance);
EXTERN void       epk_vector_reserve (      EpkVector* instance,
                                            size_t     capacity);
EXTERN void       epk_vector_resize  (      EpkVector* instance,
                                            size_t     size);

/* Element access */
EXTERN void*      epk_vector_at   (const EpkVector* instance,
                                         size_t     index);
EXTERN void       epk_vector_set  (      EpkVector* instance,
                                         size_t     index,
                                         void*      item);

EXTERN void*      epk_vector_front(const EpkVector* instance);
EXTERN void*      epk_vector_back (const EpkVector* instance);

/* Iterator functions */
EXTERN void**     epk_vector_begin(const EpkVector* instance);
EXTERN void**     epk_vector_end  (const EpkVector* instance);

/* Inserting & removing elements */
EXTERN void       epk_vector_push_back(EpkVector* instance,
                                       void*      item);
EXTERN void       epk_vector_pop_back (EpkVector* instance);
EXTERN void       epk_vector_insert   (EpkVector* instance,
                                       size_t     index,
                                       void*      item);
EXTERN void       epk_vector_erase    (EpkVector* instance,
                                       size_t     index);
EXTERN void       epk_vector_clear    (EpkVector* instance);

/* Algorithms */
EXTERN int        epk_vector_find       (const EpkVector*     instance,
                                         const void*          value,
                                               epk_vec_cmp_f  cmpfunc,
                                               size_t*        index);
EXTERN int        epk_vector_lower_bound(const EpkVector*     instance,
                                         const void*          value,
                                               epk_vec_cmp_f  cmpfunc,
                                               size_t*        index);
EXTERN int        epk_vector_upper_bound(const EpkVector*     instance,
                                         const void*          value,
                                               epk_vec_cmp_f  cmpfunc,
                                               size_t*        index);
EXTERN void       epk_vector_foreach    (const EpkVector*     instance,
                                               epk_vec_proc_f procfunc);
EXTERN void       epk_vector_sort       (const EpkVector*     instance,
                                               epk_vec_cmp_f  cmpfunc);

/** @} */


#endif   /* !_EPK_VECTOR_H */
