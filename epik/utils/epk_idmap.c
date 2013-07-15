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

#include "elg_defs.h"
#include "elg_error.h"
#include "epk_idmap.h"


/*--- Header file documentation -------------------------------------------*/

/**
 * @addtogroup EPIK_utils_containers
 * @{
 */
/**
 * @file  epk_idmap.h
 * @brief Identifier mapping data structure
 *
 * This file provides type definitions and function prototypes for an
 * identifier mapping data structure which is used to store mapping tables
 * for converting local into global identifiers.
 *
 * This mapping data structure can operate in two different modes (see @ref
 * epk_idmap_mode_t): A dense mapping can be used if the local identifiers
 * are consecutively enumerated from 0 to N-1. In this case, only the global
 * identifier are stored in the table at the corresponding entry, leading to
 * compact storage and fast look-up. By contrast, if the local identifiers
 * can consist of arbitrary numbers, a sparse mapping is necessary. Here,
 * (local_id, global_id) tuples are stored, which requires a more complicated
 * look-up procedure.
 *
 * @note This module uses the @c assert() macro to check various conditions
 *       (especially the values of given parameters) at runtime, which can
 *       cause a performance penalty.
 */
/** @} */


/*
 *---------------------------------------------------------------------------
 * Local stuff
 *---------------------------------------------------------------------------
 */

/*--- Function prototypes -------------------------------------------------*/

static int binary_search(const EpkIdMap* instance,
                               elg_ui4   local_id,
                               size_t*   index);


/*
 *---------------------------------------------------------------------------
 * EpkIdMap
 *---------------------------------------------------------------------------
 */

/*--- Type definitions ----------------------------------------------------*/

/* Actual ID mapping data structure */
struct EpkIdMap_struct
{
  epk_idmap_mode_t mode;         /* Mapping mode */
  elg_ui4*         items;        /* Field elements */
  size_t           capacity;     /* Total field size */
  size_t           size;         /* Number of entries used */
};


/*--- Construction & destruction ------------------------------------------*/

/**
 * Creates and returns a new instance of EpkIdMap with the given @a mode
 * and initial @a capacity.  If the memory allocation request can not be
 * fulfilled, an error message is printed and the program is aborted.
 *
 * @param mode     Mapping mode
 * @param capacity Initial capacity
 *
 * @return Pointer to new instance
 */
EpkIdMap* epk_idmap_create(epk_idmap_mode_t mode, size_t capacity)
{
  EpkIdMap* instance;

  /* Validate arguments */
  assert((EPK_IDMAP_DENSE == mode || EPK_IDMAP_SPARSE == mode) && capacity > 0);

  /* Double capacity if sparse mapping requested */
  if (EPK_IDMAP_SPARSE == mode)
    capacity *= 2;

  /* Create data structures */
  instance = (EpkIdMap*)malloc(sizeof(EpkIdMap));
  if (!instance)
    elg_error();

  instance->items = (elg_ui4*)malloc(capacity * sizeof(elg_ui4));
  if (!instance->items)
    elg_error();

  /* Initialization */
  instance->mode     = mode;
  instance->capacity = capacity;
  instance->size     = 0;

  return instance;
}


/**
 * Destroys the given @a instance of EpkIdMap and releases the allocated
 * memory.
 *
 * @param instance Object to be freed
 */
void epk_idmap_free(EpkIdMap* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Release data structures */
  free(instance->items);
  free(instance);
}


/*--- Size operations -----------------------------------------------------*/

/**
 * Returns the actual number of entries stored in the given mapping table
 * @a instance.
 *
 * @param instance Queried object
 *
 * @return Number of entries
 */
size_t epk_idmap_size(const EpkIdMap* instance)
{
  /* Validate arguments */
  assert(instance);

  if (EPK_IDMAP_SPARSE == instance->mode)
    return instance->size / 2;

  return instance->size;
}


/**
 * Returns the identifier mapping mode (dense/sparse) used for the given
 * mapping table @a instance.
 *
 * @param instance Queried object
 *
 * @return Identifier mapping mode
 */
epk_idmap_mode_t epk_idmap_mode(const EpkIdMap* instance)
{
  /* Validate argument */
  assert(instance);

  return instance->mode;
}


/*--- Clear mapping table -------------------------------------------------*/

/**
 * Removes all entries in the given mapping table @a instance. It can be
 * used, e.g., to reuse an mapping table object for new input data.
 *
 * @param instance Object to remove entries from
 */
void epk_idmap_clear(EpkIdMap* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Forget everything... */
  instance->size = 0;
}


/*--- Adding & querying identifiers ---------------------------------------*/

/**
 * Adds the given mapping from @a local_id to @a global_id to the mapping
 * table @a instance. If the current capacity does not suffice, the data
 * structure is automatically resized. If this memory reallocation request
 * can not be fulfilled, an error message is printed and the program is
 * aborted.
 *
 * @note If the mapping table operates in dense mapping mode, the parameter
 *       @a local_id has to correspond to the next entry in the mapping table.
 *
 * @param instance  Object to add the mapping to
 * @param local_id  Local identifier
 * @param global_id Global identifier
 */
void epk_idmap_add(EpkIdMap* instance, elg_ui4 local_id, elg_ui4 global_id)
{
  /* Validate arguments */
  assert(instance);
  if (EPK_IDMAP_DENSE == instance->mode)
    assert(local_id == instance->size);

  /* Eventually resize dynamic array */
  if (instance->size == instance->capacity) {
    instance->capacity *= 2;
    instance->items     = (elg_ui4*)realloc(instance->items,
                                            instance->capacity * sizeof(elg_ui4));
    if (!instance->items)
      elg_error();
  }

  /* Add identifier */
  if (EPK_IDMAP_DENSE == instance->mode) {
    instance->items[instance->size++] = global_id;
  } else {
    size_t index;

    if (binary_search(instance, local_id, &index)) {
      /* In theory, this should never happen... */
      instance->items[index + 1] = global_id;
    } else {
      /* Move entries */
      memmove(&instance->items[index + 2], &instance->items[index],
              (instance->size - index) * sizeof(elg_ui4));

      /* Insert mapping */
      instance->items[index    ] = local_id;
      instance->items[index + 1] = global_id;
      instance->size += 2;
    }
  }
}


/**
 * Returns the global identifier for the given @a local_id stored in the
 * mapping table @a instance. If no entry can be found, ELG_NO_ID is returned.
 *
 * @param instance Queried object
 * @param local_id Local identifier
 *
 * @return Corresponding global identifier or ELG_NO_ID
 */
elg_ui4 epk_idmap_get(const EpkIdMap* instance, elg_ui4 local_id)
{
  /* Validate arguments */
  assert(instance);
  if (EPK_IDMAP_DENSE == instance->mode)
    assert(local_id < instance->size || local_id == ELG_NO_ID);

  /* Dense mapping mode? */
  if (EPK_IDMAP_DENSE == instance->mode) {
    if (ELG_NO_ID == local_id)
      return ELG_NO_ID;

    return instance->items[local_id];
  } else {
    size_t index;

    /* Search local ID */
    if (binary_search(instance, local_id, &index))
      return instance->items[index + 1];

    return ELG_NO_ID;
  }
}


/*--- Accessing raw data --------------------------------------------------*/

/**
 * Returns an pointer to the raw data of the given mapping table @a instance.
 * In addition, the total number of IDs is stored at the location pointed to
 * by @a count. For a dense mapping, this number is equal to the number of
 * entries, whereas for a sparse mapping it is twice as much, since each
 * entry consists of an local and an global identifier (in this order).
 */
const elg_ui4* epk_idmap_data(const EpkIdMap* instance, size_t* count)
{
  /* Validate arguments */
  assert(instance && count);

  /* Store number of ID entries */
  *count = instance->size;

  /* Return data */
  return instance->items;
}


/*
 *---------------------------------------------------------------------------
 * Local functions
 *---------------------------------------------------------------------------
 */

/*
 * Performs a binary search in @a instance (which must be a sparse mapping
 * table) for the given @a local_id. Returns 1 if the ID could be found and
 * stores the corresponding table index at the memory location pointed to by
 * @a index. Otherwise, it returns 0 and the insertion location is stored at
 * the location pointed to by @a index.
 */
static int binary_search(const EpkIdMap* instance,
                               elg_ui4   local_id,
                               size_t*   index)
{
  long left  = 0;
  long right = ((long)instance->size / 2) - 1;

  while (left <= right) {
    long mid = (left + right) & ~1;   /* mid = [(left + right) / 2] * 2 */

    if (instance->items[mid] < local_id) {
      left = (mid / 2) + 1;
    } else if (instance->items[mid] > local_id) {
      right = (mid / 2) - 1;
    } else {
      *index = (size_t)mid;
      return 1;
    }
  }

  *index = (size_t)(left * 2);
  return 0;
}
