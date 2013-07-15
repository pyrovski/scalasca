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
#include <string.h>

#include "elg_error.h"
#include "epk_vector.h"


/*--- Header file documentation -------------------------------------------*/

/**
 * @addtogroup EPIK_utils_containers
 * @{
 */
/**
 * @file  epk_vector.h
 * @brief Dynamic array data structure
 *
 * This file provides type definitions and function prototypes for a generic
 * dynamic array data structure. Although its programming interface tries to
 * resemble the interface of the C++ STL class template @c vector, this
 * implementation can only store @c void pointers as its elements due to the
 * lacking template support in C.
 *
 * Nevertheless, the epk_vector module follows an object-oriented style. That
 * is, each function (except the two @c create functions) takes a pointer to
 * an instance of type @ref EpkVector as their first argument, which is the
 * object (i.e., the data structure) they operate on.
 *
 * @note This module uses the @c assert() macro to check various conditions
 *       (especially the values of given parameters) at runtime, which can
 *       cause a performance penalty.
 */
/** @} */


/*
 *---------------------------------------------------------------------------
 * EpkVector
 *---------------------------------------------------------------------------
 */

#define DEFAULT_CAPACITY     16


/*--- Type definitions ----------------------------------------------------*/

/* Actual dynamic array data type */
struct EpkVector_struct
{
  void** items;        /* Field elements */
  size_t capacity;     /* Total field size */
  size_t size;         /* Number of entries used */
};


/*--- Construction & destruction ------------------------------------------*/

/**
 * Creates and returns an empty instance of EpkVector with an initial
 * capacity of zero elements. If the memory allocation request can not
 * be fulfilled, an error message is printed and the program is aborted.
 *
 * @return Pointer to new instance
 */
EpkVector* epk_vector_create()
{
  EpkVector* instance;

  /* Create data structure */
  instance = (EpkVector*)calloc(1, sizeof(EpkVector));
  if (!instance)
    elg_error();

  return instance;
}


/**
 * Creates and returns an instance of EpkVector with the given initial
 * @a capacity.  If the memory allocation request can not be fulfilled,
 * an error message is printed and the program is aborted.
 *
 * @param capacity Initial capacity
 *
 * @return Pointer to new instance
 */
EpkVector* epk_vector_create_size(size_t capacity)
{
  EpkVector* instance;

  /* Create data structure and reserve memory for entries */
  instance = epk_vector_create();
  epk_vector_reserve(instance, capacity);

  return instance;
}


/**
 * Destroys the given @a instance of EpkVector and releases the allocated
 * memory.
 *
 * @note Similar to the destructor of C++ STL vector's, this call does not
 *       free the memory occupied by the elements since only pointers are
 *       stored. This has to be done separately.
 *
 * @param instance Object to be freed
 */
void epk_vector_free(EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Release data structure */
  free(instance->items);
  free(instance);
}


/*--- Size operations -----------------------------------------------------*/

/**
 * Returns the actual number of elements stored in the given EpkVector
 * @a instance.
 *
 * @param instance Queried object
 *
 * @return Number of elements stored
 */
size_t epk_vector_size(const EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  return instance->size;
}


/**
 * Returns whether the given EpkVector @a instance is empty.
 *
 * @param instance Queried object
 *
 * @return Non-zero value if instance if empty; zero otherwise
 */
int epk_vector_empty(const EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  return (instance->size == 0);
}


/*--- Capacity operations -------------------------------------------------*/

/**
 * Returns the current capacity of the given EpkVector @a instance.
 *
 * @param instance Queried object
 *
 * @return Current capacity
 */
size_t epk_vector_capacity(const EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  return instance->capacity;
}


/**
 * Resizes the EpkVector @a instance to provide the given @a capacity. If
 * the memory reallocation request can not be fulfilled, an error message
 * is printed and the program is aborted.
 *
 * @note It is only possible to increase the capacity of an dynamic array.
 *       If the current capacity is equal to or larger than the requested
 *       capacity, the function call has no effect.
 *
 * @param instance Object to be resized
 * @param capacity New capacity
 */
void epk_vector_reserve(EpkVector* instance, size_t capacity)
{
  /* Validate arguments */
  assert(instance);

  /* Eventually resize dynamic array */
  if (instance->capacity < capacity) {
    instance->capacity = capacity;

    instance->items = (void**)realloc(instance->items,
                                      instance->capacity * sizeof(void*));
    if (!instance->items)
      elg_error();
  }
}


/**
 * Resizes the EpkVector @a instance to provide the given @a size. Newly
 * created entries will be initialized with @c NULL. If the memory
 * reallocation request can not be fulfilled, an error message is printed
 * and the program is aborted.
 *
 * @note It is only possible to increase the size of an dynamic array.
 *       If the current size is equal to or larger than the requested
 *       size, the function call has no effect.
 *
 * @param instance Object to be resized
 * @param size     New size
 */
void epk_vector_resize(EpkVector* instance, size_t size)
{
  size_t old_size;

  /* Validate arguments */
  assert(instance);

  /* Nothing to do? */
  if (instance->size >= size)
    return;

  /* Save old size */
  old_size = instance->size;

  /* Eventually resize dynamic array */
  if (instance->capacity < size) {
    instance->capacity = size;

    instance->items = (void**)realloc(instance->items,
                                      instance->capacity * sizeof(void*));
    if (!instance->items)
      elg_error();
  }

  /* Update size & initialize new entries */
  instance->size = size;
  memset(&instance->items[old_size], 0, (size - old_size) * sizeof(void*));
}


/*--- Element access ------------------------------------------------------*/

/**
 * Returns the element stored at entry @a index in the given EpkVector
 * @a instance.
 *
 * @param instance Queried object
 * @param index    Index of the element
 *
 * @return Requested element
 */
void* epk_vector_at(const EpkVector* instance, size_t index)
{
  /* Validate arguments */
  assert(instance && index < instance->size);

  return instance->items[index];
}


/**
 * Stores the given @a item at position @a index in the EpkVector @a instance.
 *
 * @param instance Object to be modified
 * @param index    Index where the item should be stored
 * @param item     Item to be stored
 */
void epk_vector_set(EpkVector* instance, size_t index, void* item)
{
  /* Validate arguments */
  assert(instance && index < instance->size);

  instance->items[index] = item;
}


/**
 * Returns the first element stored in the given EpkVector @a instance.
 *
 * @param instance Queried object
 *
 * @return First element
 */
void* epk_vector_front(const EpkVector* instance)
{
  /* Validate arguments */
  assert(instance && instance->size > 0);

  return instance->items[0];
}


/**
 * Returns the last element stored in the given EpkVector @a instance.
 *
 * @param instance Queried object
 *
 * @return Last element
 */
void* epk_vector_back(const EpkVector* instance)
{
  /* Validate arguments */
  assert(instance && instance->size > 0);

  return instance->items[instance->size - 1];
}


/*--- Iterator functions --------------------------------------------------*/

/**
 * Returns an "iterator" for (i.e., an pointer to) the first element stored
 * in the given EpkVector @a instance.
 *
 * @param instance Queried object
 *
 * @return Iterator for first element
 */
void** epk_vector_begin(const EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  return instance->items;
}


/**
 * Returns an "iterator" for (i.e., an pointer to) the position after the
 * last element stored in the given EpkVector @a instance.
 *
 * @param instance Queried object
 *
 * @return Iterator for the position after the last element
 */
void** epk_vector_end(const EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  return (instance->items + instance->size);
}


/*--- Inserting & removing elements ---------------------------------------*/

/**
 * Appends the given @a item (which can also be a @c NULL pointer) at the
 * end of the EpkVector @a instance. If the current capacity does not suffice,
 * the data structure is automatically resized. If this memory reallocation
 * request can not be fulfilled, an error message is printed and the program
 * is aborted.
 *
 * @param instance Object to which the item should be appended
 * @param item     Item to append
 */
void epk_vector_push_back(EpkVector* instance, void* item)
{
  /* Validate arguments */
  assert(instance);

  /* Eventually resize dynamic array */
  if (instance->size == instance->capacity)
    epk_vector_reserve(instance, (instance->capacity == 0)
                                    ? DEFAULT_CAPACITY
                                    : (instance->capacity * 2));

  /* Append item */
  instance->items[instance->size] = item;
  instance->size++;
}


/**
 * Inserts the given @a item (which can also be a @c NULL pointer) at the
 * given position @a index in the EpkVector @a instance. If the current
 * capacity does not suffice, the data structure is automatically resized.
 * If this memory reallocation request can not be fulfilled, an error message
 * is printed and the program is aborted.
 *
 * @param instance Object to which the item should be inserted
 * @param index    Index where item should be inserted
 * @param item     Item to insert
 */
void epk_vector_insert(EpkVector* instance, size_t index, void* item)
{
  /* Validate arguments */
  assert(instance && index <= instance->size);

  /* Eventually resize dynamic array */
  if (instance->size == instance->capacity)
    epk_vector_reserve(instance, (instance->capacity == 0)
                                    ? DEFAULT_CAPACITY
                                    : (instance->capacity * 2));

  /* Insert item */
  memmove(&instance->items[index + 1], &instance->items[index],
          (instance->size - index) * sizeof(void*));
  instance->items[index] = item;
  instance->size++;
}


/**
 * Removes the last element stored in the given EpkVector @a instance.
 * However, it does not release the memory occupied by the item itself.
 *
 * @param instance Object from which the item should be removed
 */
void epk_vector_pop_back(EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Remove last item */
  instance->size--;
}


/**
 * Removes the element stored at entry @a index in the given EpkVector
 * @a instance. However, it does not release the memory occupied by the
 * item itself.
 *
 * @param instance Object from which the item should be removed
 * @param index    Index of item to remove
 */
void epk_vector_erase(EpkVector* instance, size_t index)
{
  /* Validate arguments */
  assert(instance && index < instance->size);

  /* Remove element */
  instance->size--;
  memmove(&instance->items[index], &instance->items[index + 1],
          (instance->size - index) * sizeof(void*));
}


/**
 * Removes all elements stored in the given EpkVector @a instance. However,
 * it does not release the memory occupied by the items themselves.
 *
 * @param instance Object to remove items from
 */
void epk_vector_clear(EpkVector* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Forget everything... */
  instance->size = 0;
}


/*--- Algorithms ----------------------------------------------------------*/

/**
 * Searches for the first occurence of @a value in the given EpkVector
 * @a instance, using the binary comparison function @a cmpfunc (see
 * @ref epk_vec_cmp_f for implementation details). If a matching item could
 * be found, a non-zero value is returned. In addition, if @a index is
 * non-@c NULL, the index of the corresponding entry is stored in the memory
 * location pointed to by @a index. Otherwise, i.e., if no matching item
 * could be found, this function returns zero.
 *
 * @param instance Object in which the item is searched
 * @param value    Item to search for
 * @param cmpfunc  Binary comparison function
 * @param index    Memory location where index of matching item is stored
 *                 (ignored if @c NULL)
 *
 * @return Non-zero if matching item cound be found; zero otherwise
 */
int epk_vector_find(const EpkVector*    instance,
                    const void*         value,
                          epk_vec_cmp_f cmpfunc,
                          size_t*       index)
{
  size_t i;

  /* Validate arguments */
  assert(instance && cmpfunc);

  /* Search item using comparison function */
  for (i = 0; i < instance->size; ++i) {
    if (0 == cmpfunc(value, instance->items[i])) {
      if (index)
        *index = i;
      return 1;
    }
  }

  return 0;
}


/**
 * Determines the index of the first element that has a value greater than or
 * equal to the given @a value in the EpkVector @a instance, using the binary
 * comparison function @a cmpfunc (see @ref epk_vec_cmp_f for implementation
 * details). If a matching item could be found, a non-zero value is returned.
 * In addition, if @a index is non-@c NULL, the index of the corresponding
 * entry is stored in the memory location pointed to by @a index. Otherwise,
 * i.e., if no matching item could be found, this function returns zero.
 *
 * @note For this algorithm to work correctly, the elements in the given
 *       EpkVector instance have to be sorted in ascending order with respect
 *       to the binary comparison function @a cmpfunc.
 *
 * @param instance Object in which the item is searched
 * @param value    Item to search for
 * @param cmpfunc  Binary comparison function
 * @param index    Memory location where index of matching item is stored
 *                 (ignored if @c NULL)
 *
 * @return Non-zero if matching item cound be found; zero otherwise
 */
int epk_vector_lower_bound(const EpkVector*    instance,
                           const void*         value,
                                 epk_vec_cmp_f cmpfunc,
                                 size_t*       index)
{
  size_t left;
  size_t size;

  /* Validate arguments */
  assert(instance && cmpfunc);

  /* Perform lower bound search */
  left = 0;
  size = instance->size;
  while (size > 0) {
    size_t half = size / 2;

    if (cmpfunc(value, instance->items[left + half]) > 0) {
      left += half + 1;
      size -= half + 1;
    } else {
      size = half;
    }
  }

  /* Store lower bound */
  if (index)
    *index = left;

  return (left < instance->size && 0 == cmpfunc(value, instance->items[left]));
}


/**
 * Determines the index of the first element that has a value greater than the
 * given @a value in the EpkVector @a instance, using the binary comparison
 * function @a cmpfunc (see @ref epk_vec_cmp_f for implementation details). If
 * a matching item could be found, a non-zero value is returned. In addition,
 * if @a index is non-@c NULL, the index of the corresponding entry is stored
 * in the memory location pointed to by @a index. Otherwise, i.e., if no
 * matching item could be found, this function returns zero.
 *
 * @note For this algorithm to work correctly, the elements in the given
 *       EpkVector instance have to be sorted in ascending order with respect
 *       to the binary comparison function @a cmpfunc.
 *
 * @param instance Object in which the item is searched
 * @param value    Item to search for
 * @param cmpfunc  Binary comparison function
 * @param index    Memory location where index of matching item is stored
 *                 (ignored if @c NULL)
 *
 * @return Non-zero if matching item cound be found; zero otherwise
 */
int epk_vector_upper_bound(const EpkVector*    instance,
                           const void*         value,
                                 epk_vec_cmp_f cmpfunc,
                                 size_t*       index)
{
  size_t left;
  size_t size;

  /* Validate arguments */
  assert(instance && cmpfunc);

  /* Perform upper bound search */
  left = 0;
  size = instance->size;
  while (size > 0) {
    size_t half = size / 2;

    if (cmpfunc(value, instance->items[left + half]) < 0) {
      size = half;
    } else {
      left += half + 1;
      size -= half + 1;
    }
  }

  /* Store upper bound */
  if (index)
    *index = left;

  return (left > 0 && 0 == cmpfunc(value, instance->items[left - 1]));
}


/**
 * Calls the unary processing function @a procfunc for each element of
 * the given EpkVector @a instance.
 *
 * @param instance Object whose entries should be processed
 * @param procfunc Unary processing function
 */
void epk_vector_foreach(const EpkVector* instance, epk_vec_proc_f procfunc)
{
  size_t index;

  /* Validate arguments */
  assert(instance && procfunc);

  /* Execute function for each entry */
  for (index = 0; index < instance->size; ++index)
    procfunc(instance->items[index]);
}


/** @internal
 * Sorts the elements in the index interval [@a lower, @a upper] of the given
 * EpkVector @a instance according to the binary comparison function @a cmpfunc
 * (see @ref epk_vec_cmp_f for implementation details) using the Quicksort
 * algorithm.
 *
 * @param instance Object in which the item is searched
 * @param lower    Lower bound of the index interval
 * @param upper    Upper bound of the index interval
 * @param cmpfunc  Binary comparison function
 */
static void epk_sort(const EpkVector*    instance,
                           size_t        lower,
                           size_t        upper,
                           epk_vec_cmp_f cmpfunc)
{
  size_t left  = lower;
  size_t right = upper;
  void*  pivot = instance->items[(left + right) / 2];
  
  while (left < right) {
    /* Find item less/greater than the pivot element */
    while (cmpfunc(instance->items[left], pivot) < 0 && left < upper)
      left++;
    while (cmpfunc(instance->items[right], pivot) > 0 && right > lower)
      right--;

    /* Swap elements */
    if (left < right) {
      void* tmp = instance->items[left];
      instance->items[left] = instance->items[right];
      instance->items[right] = tmp;
    }
  }

  /* Recursion */
  if (lower < left)
    epk_sort(instance, lower, left, cmpfunc);
  if (left + 1 < upper)
    epk_sort(instance, left + 1, upper, cmpfunc);
}


/**
 * Sorts the elements of the given EpkVector @a instance according to the
 * binary comparison function @a cmpfunc (see @ref epk_vec_cmp_f for
 * implementation details).
 *
 * @param instance Object in which the item is searched
 * @param cmpfunc  Binary comparison function
 */
void epk_vector_sort(const EpkVector* instance, epk_vec_cmp_f cmpfunc)
{
  /* Validate arguments */
  assert(instance && cmpfunc);

  if (instance->size > 1)
    epk_sort(instance, 0, instance->size - 1, cmpfunc);
}
