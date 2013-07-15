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

#ifndef _EPK_IDMAP_H
#define _EPK_IDMAP_H


#include <stddef.h>

#include "elg_types.h"


#ifdef __cplusplus
#  define EXTERN extern "C"
#else
#  define EXTERN extern 
#endif


/*--- Type definitions ----------------------------------------------------*/

/** Opaque data structure representing an ID mapping table. */
typedef struct EpkIdMap_struct EpkIdMap;

/**
 * Enumeration type defining the two differnt modes of an identifier mapping
 * table.
 */
typedef enum {
  EPK_IDMAP_DENSE,     /**< Dense mapping table */
  EPK_IDMAP_SPARSE     /**< Sparse mapping table */
} epk_idmap_mode_t;


/*
 *---------------------------------------------------------------------------
 * EpkIdMap
 *---------------------------------------------------------------------------
 */

/** @name Functions
 ** @{ */

/* Construction & destruction */
EXTERN EpkIdMap* epk_idmap_create(epk_idmap_mode_t mode,
                                  size_t           capacity);
EXTERN void      epk_idmap_free  (EpkIdMap*        instance);

/* Size operations */
EXTERN size_t epk_idmap_size(const EpkIdMap* instance);

/* Querying mapping mode */
EXTERN epk_idmap_mode_t epk_idmap_mode(const EpkIdMap* instance);

/* Clear mapping table */
EXTERN void epk_idmap_clear(EpkIdMap* instance);

/* Adding & querying identifiers */
EXTERN void    epk_idmap_add(      EpkIdMap* instance,
                                   elg_ui4   local_id,
                                   elg_ui4   global_id);
EXTERN elg_ui4 epk_idmap_get(const EpkIdMap* instance,
                                   elg_ui4   local_id);

/* Accessing raw data */
EXTERN const elg_ui4* epk_idmap_data(const EpkIdMap* instance,
                                           size_t*   count);

/** @} */


#endif   /* !_EPK_IDMAP_H */
