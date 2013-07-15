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

#include <assert.h>
#include <stdlib.h>

#include "elg_error.h"
#include "epk_hashtab.h"


/*--- Header file documentation -------------------------------------------*/

/**
 * @addtogroup EPIK_utils_containers
 * @{
 */
/**
 * @file  epk_hashtab.h
 * @brief Hash table data structure
 *
 * This file provides type definitions and function prototypes for a generic
 * hash table data structure. The elements of such a hash table are stored in
 * no particular order as a key/value pair, where both items are represented
 * by @c void pointers (see EpkHashEntry).
 *
 * In addition, this module defines an associated iterator data type
 * EpkHashIter, which allows convenient traversal of all entries stored in
 * the hash table.
 *
 * The epk_hashtab module follows an object-oriented style. That is, each
 * function (except the two @a create functions) takes a pointer to an
 * instance of either type @ref EpkHashTab or @ref EpkHashIter as their first
 * argument, which is the object (i.e., the data structure) they operate on.
 *
 * @note This module uses the @c assert() macro to check various conditions
 *       (especially the values of given parameters) at runtime, which can
 *       cause a performance penalty.
 */
/** @} */

/*
 *---------------------------------------------------------------------------
 * EpkHashTab
 *---------------------------------------------------------------------------
 */

/*--- Type definitions ----------------------------------------------------*/

typedef struct ListItem_struct ListItem;

/* Collision list entry */
struct ListItem_struct
{
  EpkHashEntry entry;     /* Table entry (key, value) */
  ListItem*    next;      /* Pointer to next entry */
};

/* Actual hash table data type */
struct EpkHashTab_struct
{
  ListItem**    table;       /* Field elements */
  size_t        tabsize;     /* Number of field elements */
  size_t        size;        /* Number of items stored */
  epk_ht_hash_f hash;        /* Hashing function */
  epk_ht_kcmp_f kcmp;        /* Comparison function */
};


/*--- Construction & destruction ------------------------------------------*/

/**
 * Creates and returns an instance of EpkHashTab. Besides the @a size of the
 * hash table, pointers to a hashing function as well as a key comparison
 * function have to be provided. If the memory allocation request can not be
 * fulfilled, an error message is printed and the program is aborted.
 *
 * @param size     Size of the hash table
 * @param hashfunc Hashing function
 * @param kcmpfunc Key comparison function
 *
 * @return Pointer to new instance
 */
EpkHashTab* epk_hashtab_create_size(size_t        size,
                                    epk_ht_hash_f hashfunc,
                                    epk_ht_kcmp_f kcmpfunc)
{
  EpkHashTab* instance;

  /* Validate arguments */
  assert(size > 0 && hashfunc && kcmpfunc);

  /* Create hash table data structure */
  instance = (EpkHashTab*)malloc(sizeof(EpkHashTab));
  if (!instance)
    elg_error();

  instance->table = (ListItem**)calloc(size, sizeof(ListItem*));
  if (!instance->table)
    elg_error();

  /* Initialization */
  instance->tabsize = size;
  instance->size    = 0;
  instance->hash    = hashfunc;
  instance->kcmp    = kcmpfunc;

  return instance;
}


/**
 * Destroys the given @a instance of EpkHashTab and releases the allocated
 * memory.
 *
 * @note Similar to the @ref EpkVector data structure, this call does not
 *       free the memory occupied by the elements, i.e., keys and values,
 *       since only pointers are stored. This has to be done separately.
 *
 * @param instance Object to be freed
 */
void epk_hashtab_free(EpkHashTab* instance)
{
  size_t index;

  /* Validate arguments */
  assert(instance);

  /* Release data structure */
  for (index = 0; index < instance->tabsize; ++index) {
    ListItem* item = instance->table[index];
    while (item) {
      ListItem* next = item->next;
      free(item);
      item = next;
    }
  }
  free(instance->table);
  free(instance);
}


/*--- Size operations -----------------------------------------------------*/

/**
 * Returns the actual number of elements stored in the given EpkHashTab
 * @a instance.
 *
 * @param instance Queried object
 *
 * @return Number of elements stored
 */
size_t epk_hashtab_size(const EpkHashTab* instance)
{
  /* Validate arguments */
  assert(instance);

  return instance->size;
}


/**
 * Returns whether the given EpkHashTab @a instance is empty.
 *
 * @param instance Queried object
 *
 * @return Non-zero value if instance if empty; zero otherwise
 */
int epk_hashtab_empty(const EpkHashTab* instance)
{
  /* Validate arguments */
  assert(instance);

  return (instance->size == 0);
}


/*--- Inserting & removing elements ---------------------------------------*/

/**
 * Inserts the given (@a key,@a value) pair into the EpkHashTab @a instance.
 * In addition, an index hint (e.g., returned by a preceding call of
 * epk_hashtab_find()) can be specified. If the index should be (re-)calculated
 * instead, @c NULL can be passed.
 *
 * This function also has to allocate memory for internal data structures. If
 * this memory allocation request can not be fulfilled, an error message is
 * printed and the program is aborted.
 *
 * @param instance Object in which the key/value pair should be inserted
 * @param key      Unique key
 * @param value    Associated value
 * @param index    Memory location where an index hint is stored (ignored if
 *                 @c NULL)
 */
void epk_hashtab_insert(EpkHashTab* instance,
                        void*       key,
                        void*       value,
                        size_t*     index)
{
  ListItem* item;
  size_t    hashval;

  /* Validate arguments */
  assert(instance && key);

  /* Eventually calculate hash value */
  if (index)
    hashval = *index;
  else
    hashval = instance->hash(key) % instance->tabsize;

  /* Create new item */
  item = (ListItem*)malloc(sizeof(ListItem));
  if (!item)
    elg_error();

  /* Initialize item */
  item->entry.key   = key;
  item->entry.value = value;
  item->next        = instance->table[hashval];

  /* Add item to hash table */
  instance->table[hashval] = item;
  instance->size++;
}


/*--- Algorithms ----------------------------------------------------------*/

/**
 * Searches for the an hash table entry with the specified @a key in the
 * given EpkHashTab @a instance, using the binary key comparison function
 * @a cmpfunc (see @ref epk_ht_kcmp_f for implementation details). If a
 * matching item could be found, a pointer to the key/value pair is returned.
 * In addition, if @a index is non-@c NULL, the hash table index is stored in
 * the memory location pointed to by @a index, which can be used as an index
 * hint for an subsequent call to epk_hashtab_insert(). Otherwise, i.e., if no
 * matching item could be found, this function returns @c NULL.
 *
 * @param instance Object in which the item is searched
 * @param key      Unique key to search for
 * @param index    Memory location where index of matching item is stored
 *                 (ignored if @c NULL)
 *
 * @return Pointer to hash table entry if matching item cound be found;
 *         @c NULL otherwise
 */
EpkHashEntry* epk_hashtab_find(const EpkHashTab* instance,
                               const void*       key,
                                     size_t*     index)
{
  ListItem* item;
  size_t    hashval;

  /* Validate arguments */
  assert(instance && key);

  /* Calculate hash value */
  hashval = instance->hash(key) % instance->tabsize;
  if (index)
    *index = hashval;

  /* Search element */
  item = instance->table[hashval];
  while (item) {
    if (0 == instance->kcmp(key, item->entry.key))
      return &item->entry;

    item = item->next;
  }

  return NULL;
}


/**
 * Calls the unary processing function @a procfunc for each element of
 * the given EpkHashTab @a instance.
 *
 * @param instance Object whose entries should be processed
 * @param procfunc Unary processing function
 */
void epk_hashtab_foreach(const EpkHashTab* instance, epk_ht_proc_f procfunc)
{
  EpkHashIter*  iter;
  EpkHashEntry* entry;

  /* Validate arguments */
  assert(instance && procfunc);

  /* Execute function for each entry */
  iter  = epk_hashiter_create(instance);
  entry = epk_hashiter_first(iter);
  while (entry) {
    procfunc(entry);
    entry = epk_hashiter_next(iter);
  }
  epk_hashiter_free(iter);
}


/*
 *---------------------------------------------------------------------------
 * EpkHashIter
 *---------------------------------------------------------------------------
 */

/*--- Type definitions ----------------------------------------------------*/

/* Actual hash table iterator data structure */
struct EpkHashIter_struct
{
  const EpkHashTab* hashtab;     /* corresponding hash table */
        size_t      index;       /* current field index      */
        ListItem*   item;        /* current item             */
};


/*--- Construction & destruction ------------------------------------------*/

/**
 * Creates and returns an iterator for the given EpkHashTab @a instance. If
 * the memory allocation request can not be fulfilled, an error message is
 * printed and the program is aborted.
 *
 * @return Pointer to new instance
 */
EpkHashIter* epk_hashiter_create(const EpkHashTab* hashtab)
{
  EpkHashIter* instance;

  /* Validate arguments */
  assert(hashtab);

  /* Create iterator */
  instance = (EpkHashIter*)malloc(sizeof(EpkHashIter));
  if (!instance)
    elg_error();

  /* Initialization */
  instance->hashtab = hashtab;
  instance->index   = 0;
  instance->item    = NULL;

  return instance;
}


/**
 * Destroys the given @a instance of EpkHashIter and releases the allocated
 * memory.
 *
 * @param instance Object to be freed
 */
void epk_hashiter_free(EpkHashIter* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Release iterator */
  free(instance);
}


/*--- Element access ------------------------------------------------------*/

/**
 * Returns a pointer to the first entry of the hash table which is
 * associated to the given iterator @a instance. If the hash table is
 * empty, @c NULL is returned.
 *
 * @param instance Iterator object
 *
 * @return Pointer to first entry of the hash table or @c NULL if empty
 */
EpkHashEntry* epk_hashiter_first(EpkHashIter* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Hash table empty? */
  if (0 == instance->hashtab->size)
    return NULL;

  /* Reset iterator */
  instance->index = 0;
  instance->item  = NULL;

  /* Search first list entry */
  while (instance->hashtab->table[instance->index] == NULL &&
         instance->index < instance->hashtab->tabsize)
    instance->index++;
  instance->item = instance->hashtab->table[instance->index];

  return &instance->item->entry;
}


/**
 * Returns a pointer to the next entry of the hash table which is
 * associated to the given iterator @a instance. If no next entry is
 * available, @c NULL is returned.
 *
 * @param instance Iterator object
 *
 * @return Pointer to next entry of the hash table or @c NULL if unavailable
 */
EpkHashEntry* epk_hashiter_next(EpkHashIter* instance)
{
  /* Validate arguments */
  assert(instance);

  /* No more entries? */
  if (instance->item == NULL)
    return NULL;

  /* Search next entry */
  instance->item = instance->item->next;
  while (instance->item == NULL) {
    instance->index++;
    if (instance->index == instance->hashtab->tabsize)
      return NULL;

    instance->item = instance->hashtab->table[instance->index];
  }

  return &instance->item->entry;
}
