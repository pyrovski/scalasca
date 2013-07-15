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

#ifndef _EPK_HASHTAB_H
#define _EPK_HASHTAB_H


#include <stddef.h>


#ifdef __cplusplus
#  define EXTERN extern "C"
#else
#  define EXTERN extern
#endif


/*--- Type definitions ----------------------------------------------------*/

/** Data structure representing key/value pairs stored in the hash table. */
typedef struct
{
  void* key;       /**< Unique key */
  void* value;     /**< Stored value */
} EpkHashEntry;

/** Opaque data structure representing a hash table. */
typedef struct EpkHashTab_struct EpkHashTab;

/** Opaque data structure representing a hash table iterator. */
typedef struct EpkHashIter_struct EpkHashIter;

/**
 * Pointer-to-function type describing hashing functions. The function has to
 * compute an integral index value for the given @a key. If the computed index
 * is larger than the size of the hash table, it will be automatically adjusted
 * using modulo arithmetics.
 *
 * @param key Key value
 *
 * @return entry Computed hash table index
 */
typedef size_t (*epk_ht_hash_f)(const void* key);

/**
 * Pointer-to-function type describing key comparison functions. It has to
 * return 0 if the given @a key equals the key of the current item (@a item_key)
 * or a non-zero value otherwise.
 *
 * @param key      Key value to compare
 * @param item_key Key value of the current item
 */
typedef int (*epk_ht_kcmp_f)(const void* key , const void* item_key);

/**
 * Pointer-to-function type describing unary processing functions that can
 * be used with epk_hashtab_foreach(). Here, the current key/value pair will
 * be passed as parameter @a entry.
 *
 * @param Hash table entry
 */
typedef void (*epk_ht_proc_f)(EpkHashEntry* entry);


/*
 *---------------------------------------------------------------------------
 * EpkHashTab
 *---------------------------------------------------------------------------
 */

/** @name Hash table functions
 ** @{ */

/* Construction & destruction */
EXTERN EpkHashTab*   epk_hashtab_create_size(size_t         size,
                                             epk_ht_hash_f  hashfunc,
                                             epk_ht_kcmp_f  kcmpfunc);
EXTERN void          epk_hashtab_free       (EpkHashTab*    instance);

/* Size operations */
EXTERN size_t        epk_hashtab_size (const EpkHashTab* instance);
EXTERN int           epk_hashtab_empty(const EpkHashTab* instance);

/* Inserting & removing elements */
EXTERN void          epk_hashtab_insert(EpkHashTab* instance,
                                        void*       key,
                                        void*       value,
                                        size_t*     index);

/* Algorithms */
EXTERN EpkHashEntry* epk_hashtab_find   (const EpkHashTab* instance,
                                         const void*       key,
                                               size_t*     index);
EXTERN void          epk_hashtab_foreach(const EpkHashTab*   instance,
                                               epk_ht_proc_f procfunc);

/** @} */


/*
 *---------------------------------------------------------------------------
 * EpkHashIter
 *---------------------------------------------------------------------------
 */

/** @name Iterator functions
 ** @{ */

/* Construction & destruction */
EXTERN EpkHashIter*  epk_hashiter_create(const EpkHashTab*  hashtab);
EXTERN void          epk_hashiter_free  (      EpkHashIter* instance);

/* Element access */
EXTERN EpkHashEntry* epk_hashiter_first(EpkHashIter* instance);
EXTERN EpkHashEntry* epk_hashiter_next (EpkHashIter* instance);

/** @} */


#endif   /* !_EPK_HASHTAB_H */
