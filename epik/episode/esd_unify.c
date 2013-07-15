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
#include "elg_readcb.h"
#include "epk_hashtab.h"
#include "epk_vector.h"
#include "esd_unify.h"


/*--- Header file documentation -------------------------------------------*/

/**
 * @addtogroup EPISODE_unify
 * @{
 */
/**
 * @file  esd_unify.h
 * @brief Static program entity unification routines
 *
 * This file contains type definitions and function prototypes needed for the
 * unification of per-process EPILOG definition records for static program
 * entities, such as instrumented source code regions or MPI communicators.
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

/* Default sizes of data structures */
#define STRINGS_HASH         5077
#define STRINGS_ARRAY         512
#define MACHS_ARRAY             4
#define NODES_ARRAY            64
#define PROCESS_ARRAY          16
#define THREADS_ARRAY          16
#define FILES_ARRAY            16
#define LOCS_ARRAY           1024
#define REGIONS_ARRAY        1024
#define GROUPS_ARRAY            8
#define COMS_ARRAY             16
#define CSITES_ARRAY           16
#define CPATHS_ARRAY          256
#define METRICS_ARRAY          16
#define TOPOLOGIES_ARRAY        4
#define WINDOWS_ARRAY          16


/*--- Type definitions ----------------------------------------------------*/

/*
 * "Base class" for entities which can be uniquely identified using a single
 * identifier.
 */
typedef struct
{
  elg_ui4 id;     /* Identifier */
} UnifySingleId;

/*
 * "Base class" for entities which can only be uniquely identified using two
 * identifiers.
 */
typedef struct
{
  elg_ui4 id1;     /* First ID */
  elg_ui4 id2;     /* Second ID */
} UnifyDoubleId;

/*
 * Structure storing the data of an MPI group record.
 */
typedef struct
{
  elg_ui4  id;      /* Global group id */
  elg_ui1  mode;    /* Storage mode */
  elg_ui4  size;    /* Group size */
  elg_ui4* ranks;   /* Rank array (local |-> global rank) */
} UnifyMpiGroup;

/* 
 * Structure for storing the data of an MPI communicator reference record.
 */
typedef struct
{
  elg_ui4 id;        /* Global communicator ID */
  elg_ui4 groupid;   /* Global identifier of corresponding MPI group */
} UnifyMpiCom;

/*
 * Structure for storing how often an MPI communicator is used to define
 * an RMA window. It is "derived" from UnifySingleId.
 */
typedef struct
{
  elg_ui4 comid;     /* Global communicator ID */
  elg_ui4 count;     /* Number of occurrences */
} ComCount;


/*--- Function prototypes -------------------------------------------------*/

/* Hash functions */
static size_t hash_string(const char* key);

/* Unification functions */
static void unify_string(EsdUnifyData* data, char* key, elg_ui4 local_id);

/* Comparisons  */
static int single_id_cmp(const UnifySingleId* key, const UnifySingleId* entry);
static int double_id_cmp(const UnifyDoubleId* key, const UnifyDoubleId* entry);
static int loc_cmp      (const UnifyLoc*      key, const UnifyLoc*      entry);
static int region_cmp   (const UnifyRegion*   key, const UnifyRegion*   entry);
static int csite_cmp    (const UnifyCsite*    key, const UnifyCsite*    entry);
static int group_cmp    (const UnifyMpiGroup* key, const UnifyMpiGroup* entry);
static int cart_cmp     (const UnifyCart*     key, const UnifyCart*     entry);

/* Memory management */
static void strhash_free(EpkHashEntry*  entry);
static void cpath_free  (UnifyCpath*    entry);
static void group_free  (UnifyMpiGroup* entry);
static void cart_free   (UnifyCart*     entry);
static void coord_free  (CartCoord*     entry);

/* File I/O */
static size_t map_size (const EsdUnifyData* instance, esd_unify_map_t map_id);
static void   write_map(const EsdUnifyData* instance, esd_unify_map_t map_id,
                        ElgOut* mapfile);


/*
 *---------------------------------------------------------------------------
 * EsdUnify
 *---------------------------------------------------------------------------
 */

/*--- Construction & destruction ------------------------------------------*/

/** 
 * @brief  Creates and returns an new instance of type EsdUnifyData.
 *
 * Creates and returns an new instance of type EsdUnifyData. If the
 * memory allocation request can not be fulfilled, an error message is
 * printed and the program is aborted.
 *
 * @return Pointer to new instance
 */
EsdUnifyData* esd_unify_create()
{
  EsdUnifyData* instance;

  /* Create data structure */
  instance = (EsdUnifyData*)malloc(sizeof(EsdUnifyData));
  if (!instance)
    elg_error();

  /* Initialize temporary data */
  instance->tmp_count  = 0;
  instance->tmp_string = NULL;
  instance->comcnt     = epk_vector_create_size(WINDOWS_ARRAY);

  instance->mode       = ESD_UNIFY_FULL;
  instance->rank       = -1;

  /* Initialize global data */
  instance->str2id  = epk_hashtab_create_size(STRINGS_HASH,
                                              (epk_ht_hash_f)&hash_string,
                                              (epk_ht_kcmp_f)&strcmp);
  instance->topmap  = epk_idmap_create(EPK_IDMAP_SPARSE, TOPOLOGIES_ARRAY);

  instance->strings = epk_vector_create_size(STRINGS_ARRAY);
  instance->machs   = epk_vector_create_size(MACHS_ARRAY);
  instance->nodes   = epk_vector_create_size(NODES_ARRAY);
  instance->procs   = epk_vector_create_size(PROCESS_ARRAY);
  instance->thrds   = epk_vector_create_size(THREADS_ARRAY);
  instance->locs    = epk_vector_create_size(LOCS_ARRAY);
  instance->files   = epk_vector_create_size(FILES_ARRAY);
  instance->regions = epk_vector_create_size(REGIONS_ARRAY);
  instance->csites  = epk_vector_create_size(CSITES_ARRAY);
  instance->cpaths  = epk_vector_create_size(CPATHS_ARRAY);
  instance->cproots = epk_vector_create();
  instance->metrics = epk_vector_create_size(METRICS_ARRAY);
  instance->groups  = epk_vector_create_size(GROUPS_ARRAY);
  instance->coms    = epk_vector_create_size(COMS_ARRAY);
  instance->tops    = epk_vector_create_size(TOPOLOGIES_ARRAY);
  instance->wins    = epk_vector_create_size(WINDOWS_ARRAY);

  /* Initialize per-process data */
  instance->maps[ESD_UNIFY_STRMAP  ] = epk_idmap_create(EPK_IDMAP_DENSE,  STRINGS_ARRAY);
  instance->maps[ESD_UNIFY_MACHMAP ] = epk_idmap_create(EPK_IDMAP_SPARSE, MACHS_ARRAY);
  instance->maps[ESD_UNIFY_NODEMAP ] = epk_idmap_create(EPK_IDMAP_SPARSE, NODES_ARRAY);
  instance->maps[ESD_UNIFY_LOCMAP  ] = epk_idmap_create(EPK_IDMAP_DENSE,  LOCS_ARRAY);
  instance->maps[ESD_UNIFY_FILEMAP ] = epk_idmap_create(EPK_IDMAP_DENSE,  FILES_ARRAY);
  instance->maps[ESD_UNIFY_REGMAP  ] = epk_idmap_create(EPK_IDMAP_DENSE,  REGIONS_ARRAY);
  instance->maps[ESD_UNIFY_CSITEMAP] = epk_idmap_create(EPK_IDMAP_DENSE,  CSITES_ARRAY);
  instance->maps[ESD_UNIFY_CPATHMAP] = epk_idmap_create(EPK_IDMAP_DENSE,  CPATHS_ARRAY);
  instance->maps[ESD_UNIFY_GROUPMAP] = epk_idmap_create(EPK_IDMAP_SPARSE, GROUPS_ARRAY);
  instance->maps[ESD_UNIFY_COMMAP  ] = epk_idmap_create(EPK_IDMAP_DENSE,  COMS_ARRAY);
  instance->maps[ESD_UNIFY_WINMAP  ] = epk_idmap_create(EPK_IDMAP_DENSE,  WINDOWS_ARRAY);
  instance->maps[ESD_UNIFY_TOPMAP  ] = epk_idmap_create(EPK_IDMAP_DENSE,  TOPOLOGIES_ARRAY);

  instance->offcnt    = 0;
  instance->ltime[0]  = 0.0;
  instance->ltime[1]  = 1.0;
  instance->offset[0] = 0.0;
  instance->offset[1] = 0.0;

  instance->ntypes   = 0;
  instance->evtypes  = NULL;
  instance->evcounts = NULL;

  instance->dist_coms = epk_vector_create_size(COMS_ARRAY);
  instance->com_root  = 0;
  instance->com_self  = 0;

  return instance;
}


/**
 * Destroys the given @a instance of EsdUnifyData and releases the allocated
 * memory.
 *
 * @param instance Object to be freed
 */
void esd_unify_free(EsdUnifyData* instance)
{
  int i;

  /* Validate arguments */
  assert(instance);

  /* Clear unification data */
  esd_unify_clear_data(instance);

  /* Release temporary data */
  epk_vector_free(instance->comcnt);

  /* Release global data */
  epk_hashtab_foreach(instance->str2id, &strhash_free);
  epk_hashtab_free(instance->str2id);
  epk_idmap_free(instance->topmap);

  epk_vector_free(instance->strings);
  epk_vector_free(instance->machs);
  epk_vector_free(instance->nodes);
  epk_vector_free(instance->procs);
  epk_vector_free(instance->thrds);
  epk_vector_free(instance->locs);
  epk_vector_free(instance->files);
  epk_vector_free(instance->regions);
  epk_vector_free(instance->csites);
  epk_vector_free(instance->cpaths);
  epk_vector_free(instance->cproots);
  epk_vector_free(instance->metrics);
  epk_vector_free(instance->groups);
  epk_vector_free(instance->coms);
  epk_vector_free(instance->tops);
  epk_vector_free(instance->wins);

  /* Release per-process data */
  for (i = 0; i < ESD_UNIFY_NUM_MAPS; ++i)
    epk_idmap_free(instance->maps[i]);

  if (instance->evtypes)
    free(instance->evtypes);
  if (instance->evcounts)
    free(instance->evcounts);

  epk_vector_foreach(instance->dist_coms, &free);
  epk_vector_free(instance->dist_coms);

  free(instance);
}


/*--- Unification ---------------------------------------------------------*/

/**
 * Processes @a size bytes of the local definition record data provided
 * in the given memory @a buffer. The unified definition data is collected
 * in the unification data object @a instance. In addition, the per-process
 * data (mapping tables etc.) is stored. Depending on the unification @a
 * mode, location and topology data is just aggregated (ESD_UNIFY_PARTIAL)
 * or really unified to generate identifier mappings (ESD_UNIFY_FULL). The
 * @a rank parameter should either be set to the global rank of the process
 * whose data is being processed or to -1 for partially unified data from
 * multiple processes.
 *
 * In case of an error (e.g., if the buffer does not contain EPILOG data),
 * an error message is printed and the program is aborted.
 *
 * @param instance Unification data object
 * @param buffer   Memory buffer containing the definition record data
 * @param size     Size of buffer in bytes
 * @param rank     Rank of data being processed (or -1 for partially unified
 *                 data)
 * @param mode     Unification mode
 */
void esd_unify_buffer(      EsdUnifyData*    instance,
                      const buffer_t         buffer,
                            size_t           size,
                            int              rank,
                            esd_unify_mode_t mode)
{
  ElgRCB* handle;

  /* Validate arguments */
  assert(instance && buffer);

  /* Set unification mode & rank */
  instance->mode = mode;
  instance->rank = rank;

  /* Process buffer */
  handle = elg_read_open_buffer(buffer, size);
  if (!handle)
    elg_error();
  while (elg_read_next_def(handle, instance))
    ;
  elg_read_close(handle);
}


/*--- Unification data management -----------------------------------------*/

/**
 * Clears the unification data stored in the unification data object
 * @a instance. It should be used in conjunction with esd_unify_clear_maps()
 * to also clear the per-process identifier mappings.
 *
 * @param instance Unification data object
 */
void esd_unify_clear_data(EsdUnifyData* instance)
{
  /* Validate arguments */
  assert(instance);

  /* Clear temporary data */
  instance->mode = ESD_UNIFY_FULL;
  instance->rank = -1;

  /* Clear global unification data */
  epk_hashtab_foreach(instance->str2id, &strhash_free);
  epk_hashtab_free(instance->str2id);
  instance->str2id  = epk_hashtab_create_size(STRINGS_HASH,
                                              (epk_ht_hash_f)&hash_string,
                                              (epk_ht_kcmp_f)&strcmp);
  epk_idmap_clear(instance->topmap);

  epk_vector_clear(instance->strings);
  epk_vector_foreach(instance->machs, &free);
  epk_vector_clear(instance->machs);
  epk_vector_foreach(instance->nodes, &free);
  epk_vector_clear(instance->nodes);
  epk_vector_foreach(instance->procs, &free);
  epk_vector_clear(instance->procs);
  epk_vector_foreach(instance->thrds, &free);
  epk_vector_clear(instance->thrds);
  epk_vector_foreach(instance->locs, &free);
  epk_vector_clear(instance->locs);
  epk_vector_foreach(instance->files, &free);
  epk_vector_clear(instance->files);
  epk_vector_foreach(instance->regions, &free);
  epk_vector_clear(instance->regions);
  epk_vector_foreach(instance->csites, &free);
  epk_vector_clear(instance->csites);
  epk_vector_foreach(instance->cpaths, (epk_vec_proc_f)&cpath_free);
  epk_vector_clear(instance->cpaths);
  epk_vector_clear(instance->cproots);
  epk_vector_foreach(instance->metrics, &free);
  epk_vector_clear(instance->metrics);
  epk_vector_foreach(instance->groups, (epk_vec_proc_f)&group_free);
  epk_vector_clear(instance->groups);
  epk_vector_foreach(instance->coms, (epk_vec_proc_f)&free);
  epk_vector_clear(instance->coms);
  epk_vector_foreach(instance->tops, (epk_vec_proc_f)&cart_free);
  epk_vector_clear(instance->tops);
  epk_vector_foreach(instance->wins, &free);
  epk_vector_clear(instance->wins);
}


/*--- Identifier mapping table management ---------------------------------*/

/**
 * Returns one of the per-process identifier mapping tables stored in the
 * unification data object @a instance. The requested table is specified
 * by @a map_id.
 *
 * @param instance Unification data object
 * @param map_id   Requested mapping table
 *
 * @return Per-process identifier mapping table
 */
EpkIdMap* esd_unify_get_map(const EsdUnifyData*   instance,
                                  esd_unify_map_t map_id)
{
  /* Validate arguments */
  assert(ESD_UNIFY_NUM_MAPS > map_id);

  return instance->maps[map_id];
}


/**
 * Clears the per-process identifier mapping tables stored in the unification
 * data object @a instance, except for the MPI communicator reference mapping.
 *
 * @param instance Unification data object
 *
 * @note This function does not modify/delete any global unification data.
 */
void esd_unify_clear_maps(EsdUnifyData* instance)
{
  int i;

  /* Validate arguments */
  assert(instance);

  /* Forget per-process data... */
  for (i = 0; i < ESD_UNIFY_NUM_MAPS; ++i)
    if (i != ESD_UNIFY_COMMAP)
      epk_idmap_clear(instance->maps[i]);

  instance->offcnt    = 0;
  instance->ltime[0]  = 0.0;
  instance->ltime[1]  = 1.0;
  instance->offset[0] = 0.0;
  instance->offset[1] = 0.0;

  free(instance->evtypes);
  free(instance->evcounts);

  instance->ntypes   = 0;
  instance->evtypes  = NULL;
  instance->evcounts = NULL;

  epk_vector_foreach(instance->comcnt, &free);
  epk_vector_clear(instance->comcnt);
}


/*--- Determine output file sizes -----------------------------------------*/

/**
 * Determines the size of the definition data stored by the given
 * @a instance of EsdUnifyData when written to a file/memory buffer
 * in EPILOG format. Depending on the unification @a mode, location and
 * topology information is included (ESD_UNIFY_FULL) or not
 * (ESD_UNIFY_PARTIAL).
 *
 * @param instance Unification data object
 * @param mode     Unification mode
 *
 * @return Size of the definition data in EPILOG format in bytes
 */
size_t esd_unify_size_defs(const EsdUnifyData* instance, esd_unify_mode_t mode)
{
  void** ptr;
  void** end;
  size_t strsize = 0;
  size_t topsize = 0;
  size_t locsize = 0;
  size_t grpsize = 0;

  ptr   = epk_vector_begin(instance->strings);
  end   = epk_vector_end(instance->strings);
  while (ptr != end) {
    char** entry  = (char**)ptr;
    int    length = strlen(*entry) + 1;
    if (length <= 250)
      strsize += 2 + 5 + length;
    else
      strsize += 2 + 5 + 250 + (length - 250) +
                 ((length - 250 + 254) / 255) * 2;
    ptr++;
  }

  ptr   = epk_vector_begin(instance->groups);
  end   = epk_vector_end(instance->groups);
  while (ptr != end) {
    UnifyMpiGroup* entry = (UnifyMpiGroup*)(*ptr);

    if (entry->mode & ELG_GROUP_WORLD || entry->mode & ELG_GROUP_SELF)
      grpsize += 11;
    else if (entry->size < 62)
      grpsize += 11 + entry->size * 4;
    else
      grpsize += 11 + (61 * 4) +
                 (entry->size + 1) / 63 * 2 +
                 (entry->size - 61) * 4;
    ptr++;
  }

  if (mode == ESD_UNIFY_FULL) {
    ptr   = epk_vector_begin(instance->tops);
    end   = epk_vector_end(instance->tops);
    while (ptr != end) {
      UnifyCart* entry = (UnifyCart*)(*ptr);
      // In case someone EVER changes topology format again, adjust here.
      topsize += 2 +  // type, len
	4 + // id
	4 + // name
	4 + // cid
	1 + // ndims
	entry->ndims * 9 + // dim_sizes + period + name
	epk_vector_size(entry->coords) * (11 + entry->ndims * 4);
      ptr++;
    }

    locsize = epk_vector_size(instance->locs) * 22;
  }

  return 10 +
         strsize +
         grpsize +
         topsize +
         locsize +
         epk_vector_size(instance->machs)   * 14 +
         epk_vector_size(instance->nodes)   * 26 +
         epk_vector_size(instance->procs)   * 10 +
         epk_vector_size(instance->thrds)   * 14 +
         epk_vector_size(instance->files)   * 10 +
         epk_vector_size(instance->regions) * 27 +
         epk_vector_size(instance->csites)  * 22 +
         epk_vector_size(instance->cpaths)  * 22 +
         epk_vector_size(instance->metrics) * 17 +
         epk_vector_size(instance->coms)    * 10 +
         epk_vector_size(instance->wins)    * 10;
}


/**
 * Determines the size of the identifier mapping data stored by the given
 * @a instance of EsdUnifyData when written to a file/memory buffer in
 * EPILOG format.
 *
 * @param instance Unification data object
 *
 * @return Size of the mapping data in EPILOG format in bytes
 */
size_t esd_unify_size_maps(const EsdUnifyData* instance)
{
  size_t offsize   = 0;
  size_t locsize   = 0;
  size_t regsize   = 0;
  size_t csitesize = 0;
  size_t cpathsize = 0;
  size_t groupsize = 0;
  size_t comsize   = 0;
  size_t winsize   = 0;
  size_t cntsize   = 0;
  size_t count;

  if (instance->offcnt == 2)
    offsize = 36;

  locsize   = map_size(instance, ESD_UNIFY_LOCMAP);
  regsize   = map_size(instance, ESD_UNIFY_REGMAP);
  csitesize = map_size(instance, ESD_UNIFY_CSITEMAP);
  groupsize = map_size(instance, ESD_UNIFY_GROUPMAP);
  comsize   = map_size(instance, ESD_UNIFY_COMMAP);
  winsize   = map_size(instance, ESD_UNIFY_WINMAP);

  if (instance->ntypes > 0) {
    count   = (instance->ntypes + 61) / 62;
    /* Includes EVENT_TYPES and EVENT_COUNTS records */
    cntsize = (count * 12) + instance->ntypes + (instance->ntypes * 4);
  }

  return 10 +      /* EPILOG header */
          6 +      /* MAP_SECTION record */
         offsize + /* Two OFFSET records */
         locsize +
         regsize +
         csitesize +
         cpathsize +
         groupsize +
         comsize +
         winsize +
         cntsize;
}


/*--- File I/O ------------------------------------------------------------*/

/**
 * Writes the unified definition data collected in the given EsdUnifyData
 * @a instance to the EPILOG output stream @a deffile. Depending on the
 * unification @a mode, location and topology information is either written
 * (ESD_UNIFY_FULL) or not (ESD_UNIFY_PARTIAL).
 *
 * @param instance Object containing the unified definition data
 * @param deffile  EPILOG output stream
 * @param mode     Unification mode
 */
void esd_unify_write_defs(EsdUnifyData*    instance,
                          ElgOut*          deffile,
                          esd_unify_mode_t mode)
{
  void** ptr;
  void** end;
  size_t index;

  /* Validate arguments */
  assert(instance && deffile);

  /* Write string definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->strings);
  end   = epk_vector_end(instance->strings);
  while (ptr != end) {
    char** entry = (char**)ptr;
    ElgOut_write_string(deffile, index++, *entry);
    ptr++;
  }

  /* Write machine definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->machs);
  end   = epk_vector_end(instance->machs);
  while (ptr != end) {
    UnifyMach* entry = (UnifyMach*)(*ptr);
    ElgOut_write_MACHINE(deffile, index++, entry->nodec, entry->nameid);
    ptr++;
  }

  /* Write node definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->nodes);
  end   = epk_vector_end(instance->nodes);
  while (ptr != end) {
    UnifyNode* entry = (UnifyNode*)(*ptr);
    ElgOut_write_NODE(deffile, index++, entry->machid, entry->cpuc,
                      entry->nameid, entry->clockrt);
    ptr++;
  }

  /* Write process definitions */
  ptr   = epk_vector_begin(instance->procs);
  end   = epk_vector_end(instance->procs);
  while (ptr != end) {
    UnifyProc* entry = (UnifyProc*)(*ptr);
    ElgOut_write_PROCESS(deffile, entry->id, entry->nameid);
    ptr++;
  }

  /* Write thread definitions */
  ptr   = epk_vector_begin(instance->thrds);
  end   = epk_vector_end(instance->thrds);
  while (ptr != end) {
    UnifyThrd* entry = (UnifyThrd*)(*ptr);
    ElgOut_write_THREAD(deffile, entry->id, entry->pid, entry->nameid);
    ptr++;
  }

  /* Write location definitions */
  if (mode == ESD_UNIFY_FULL) {
    index = 0;
    ptr   = epk_vector_begin(instance->locs);
    end   = epk_vector_end(instance->locs);
    while (ptr != end) {
      UnifyLoc* entry = (UnifyLoc*)(*ptr);
      ElgOut_write_LOCATION(deffile, index++, entry->machid, entry->nodeid,
                            entry->procid, entry->thrdid);
      ptr++;
    }
  }

  /* Write file definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->files);
  end   = epk_vector_end(instance->files);
  while (ptr != end) {
    elg_ui4* entry = (elg_ui4*)(*ptr);
    ElgOut_write_FILE(deffile, index++, *entry);
    ptr++;
  }

  /* Write region definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->regions);
  end   = epk_vector_end(instance->regions);
  while (ptr != end) {
    UnifyRegion* entry = (UnifyRegion*)(*ptr);
    ElgOut_write_REGION(deffile, index++, entry->nameid, entry->fileid,
                        entry->begln, entry->endln, entry->descid, entry->type);
    ptr++;
  }

  /* Write call site definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->csites);
  end   = epk_vector_end(instance->csites);
  while (ptr != end) {
    UnifyCsite* entry = (UnifyCsite*)(*ptr);
    ElgOut_write_CALL_SITE(deffile, index++, entry->fileid, entry->lineno,
                           entry->e_regid, entry->l_regid);
    ptr++;
  }

  /* Write call path definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->cpaths);
  end   = epk_vector_end(instance->cpaths);
  while (ptr != end) {
    UnifyCpath* entry = (UnifyCpath*)(*ptr);
    ElgOut_write_CALL_PATH(deffile, index++, entry->rid, entry->ppid,
                           entry->order);
    ptr++;
  }

  /* Write metric definitions */
  ptr   = epk_vector_begin(instance->metrics);
  end   = epk_vector_end(instance->metrics);
  while (ptr != end) {
    UnifyMetric* entry = (UnifyMetric*)(*ptr);
    ElgOut_write_METRIC(deffile, entry->id, entry->nameid, entry->descid,
                        entry->type, entry->mode, entry->ival);
    ptr++;
  }

  /* Write MPI group definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->groups);
  end   = epk_vector_end(instance->groups);
  while (ptr != end) {
    UnifyMpiGroup* entry = (UnifyMpiGroup*)(*ptr);
    ElgOut_write_MPI_GROUP(deffile, index++, entry->mode,
                           entry->size, entry->ranks);
    ptr++;
  }

  /* Write MPI communicator definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->coms);
  end   = epk_vector_end(instance->coms);
  while (ptr != end) {
    UnifyMpiCom* entry = (UnifyMpiCom*)(*ptr);
    ElgOut_write_MPI_COMM_REF(deffile, index++, entry->groupid);
    ptr++;
  }

  /* Write topology definitions */
  if (mode == ESD_UNIFY_FULL) {
    index = 0;
    ptr   = epk_vector_begin(instance->tops);
    end   = epk_vector_end(instance->tops);
    while (ptr != end) {
      void**     coord_ptr;
      void**     coord_end;
      UnifyCart* entry = (UnifyCart*)(*ptr);
      ElgOut_write_CART_TOPOLOGY(deffile, index, entry->tnid, entry->comid, entry->ndims,
                                 entry->nlocs, entry->period, entry->dimids);

      coord_ptr = epk_vector_begin(entry->coords);
      coord_end = epk_vector_end(entry->coords);
      while (coord_ptr != coord_end) {
        CartCoord* centry = (CartCoord*)(*coord_ptr);
        ElgOut_write_CART_COORDS(deffile, index, centry->locid, entry->ndims,
                                 centry->coord);
        coord_ptr++;
      }

      index++;
      ptr++;
    }
  }

  /* Write RMA window definitions */
  index = 0;
  ptr   = epk_vector_begin(instance->wins);
  end   = epk_vector_end(instance->wins);
  while (ptr != end) {
    UnifyWin* entry = (UnifyWin*)(*ptr);
    ElgOut_write_MPI_WIN(deffile, index++, entry->comid);
    ptr++;
  }
}


/**
 * Writes the identifier mapping data collected in the given EsdUnifyData
 * @a instance to the EPILOG output stream @a mapfile using the given @a
 * rank number.
 *
 * @param instance Object containing the unified definition data
 * @param rank     Rank number written to the header.
 * @param mapfile  EPILOG output stream
 */
void esd_unify_write_maps(EsdUnifyData* instance, int rank, ElgOut* mapfile)
{
  /* Validate arguments */
  assert(instance && mapfile);

  /* Write section to mapping file */
  ElgOut_write_MAP_SECTION(mapfile, rank);

  if (instance->offcnt == 2) {
    ElgOut_write_OFFSET(mapfile, instance->ltime[0], instance->offset[0]);
    ElgOut_write_OFFSET(mapfile, instance->ltime[1], instance->offset[1]);
  } else if (instance->offcnt == 1) {
    elg_warning("Only one ELG_OFFSET record found -- discarded");
  }

  write_map(instance, ESD_UNIFY_LOCMAP,   mapfile);
  write_map(instance, ESD_UNIFY_REGMAP,   mapfile);
  write_map(instance, ESD_UNIFY_CSITEMAP, mapfile);
  write_map(instance, ESD_UNIFY_GROUPMAP, mapfile);
  write_map(instance, ESD_UNIFY_COMMAP,   mapfile);
  write_map(instance, ESD_UNIFY_WINMAP,   mapfile);

  if (instance->ntypes > 0) {
    ElgOut_write_EVENT_TYPES(mapfile, instance->ntypes, instance->evtypes);
    ElgOut_write_EVENT_COUNTS(mapfile, instance->ntypes, instance->evcounts);
  }
}


/*--- Data serialization --------------------------------------------------*/

/**
 * Serialized the definition data held by the @a instance of EsdUnifyData
 * to a memory buffer. A buffer of appropriate size if implicitly created
 * and its size and a pointer to it stored in @a buffer and @a size,
 * respectively. Depending on the unification @a mode, location and
 * topology data is either serialized (ESD_UNIFY_FULL) or not
 * (ESD_UNIFY_PARTIAL).
 *
 * @param instance Unification data object
 * @param buffer   Address of a pointer where the memory location of the
 *                 buffer is stored
 * @param size     Address of a pointer where the size of the buffer is
 *                 stored
 * @param mode     Unification mode
 */
void esd_unify_serialize_defs(EsdUnifyData*    instance,
                              buffer_t*        buffer,
                              size_t*          size,
                              esd_unify_mode_t mode)
{
  ElgOut*  writer;

  /* Allocate buffer */
  *size   = esd_unify_size_defs(instance, mode);
  *buffer = (buffer_t)malloc(*size);
  if (!*buffer)
    elg_error_msg("Failed to malloc serialization buffer");

  /* Serialize definition data */
  writer = ElgOut_open_buffer(*buffer, *size, ELG_BYTE_ORDER);
  esd_unify_write_defs(instance, writer, mode);
  ElgOut_close(writer);
}


/**
 * Serialized the mapping data held by the @a instance of EsdUnifyData
 * to a memory buffer. A buffer of appropriate size if implicitly created
 * and its size and a pointer to it stored in @a buffer and @a size,
 * respectively. To correctly initialize the header for the process-specific
 * mapping data, the corresponding MPI @a rank needs to be given as well.
 *
 * @param instance Unification data object
 * @param rank     Rank to which the data corresponds
 * @param buffer   Address of a pointer where the memory location of the
 *                 buffer is stored
 * @param size     Address of a pointer where the size of the buffer is
 *                 stored
 */
void esd_unify_serialize_maps(EsdUnifyData* instance,
                              int           rank,
                              buffer_t*     buffer,
                              size_t*       size)
{
  ElgOut*  writer;

  /* Allocate buffer */
  *size   = esd_unify_size_maps(instance);
  *buffer = (buffer_t)malloc(*size);
  if (!*buffer)
    elg_error_msg("Failed to malloc serialization buffer");

  /* Serialize mapping data */
  writer = ElgOut_open_buffer(*buffer, *size, ELG_BYTE_ORDER);
  esd_unify_write_maps(instance, rank, writer);
  ElgOut_close(writer);
}


/*--- MPI group & communicator management ---------------------------------*/

/**
 * Adds the definition data of an MPI group to the given unification data
 * object @a instance and returns a unique identifier for that particular
 * group. If the same group has already been defined before, no operation
 * is performed (i.e., the data is merged).
 *
 * @param instance Unification data object
 * @param group_id Local MPI group identifier
 * @param mode     Storage mode of rank information
 * @param size     Group size (i.e., number of ranks)
 * @param ranks    Rank information (local |-@> global rank mapping)
 *
 * @return Unique identifier for the group
 */
elg_ui4 esd_unify_add_group(EsdUnifyData* instance,
                            elg_ui4       group_id,
                            elg_ui1       mode,
                            elg_ui4       size,
                            elg_ui4*      ranks)
{
  UnifyMpiGroup* entry;
  size_t         global_id;

  /* Construct new entry */
  entry = (UnifyMpiGroup*)malloc(sizeof(UnifyMpiGroup));
  if (!entry)
    elg_error();
  entry->mode  = mode;
  entry->size  = size;
  entry->ranks = NULL;
  if (size > 0) {
    entry->ranks = (elg_ui4*)malloc(size * sizeof(elg_ui4));
    if (!entry->ranks)
      elg_error();
    memcpy(entry->ranks, ranks, size * sizeof(elg_ui4));
  }

  /* Group not yet defined? */
  if (epk_vector_find(instance->groups, entry,
      (epk_vec_cmp_f)&group_cmp, &global_id) == 0) {
    /* Determine global ID */
    global_id = epk_vector_size(instance->groups);

    epk_vector_push_back(instance->groups, entry);
  } else {
    group_free(entry);
  }

  return global_id;
}


/**
 * Adds the definition data of an MPI communicator to the given unification
 * data object @a instance. It is assumed that the communicator data is
 * already globalized.
 *
 * @param instance Unification data object
 * @param comm_id  Global MPI communicator identifier
 * @param group_id Global MPI group identifier.
 */
void esd_unify_add_comm(EsdUnifyData* instance,
                        elg_ui4       comm_id,
                        elg_ui4       group_id)
{
  UnifyMpiCom* entry;

  /* Construct new entry */
  entry = (UnifyMpiCom*)malloc(sizeof(UnifyMpiCom));
  if (!entry)
    elg_error();
  entry->id      = comm_id;
  entry->groupid = group_id;

  /* It is assumed that each MPI communicator is only defined once. */

  epk_vector_push_back(instance->coms, entry);
}


/*
 *---------------------------------------------------------------------------
 * Local functions
 *---------------------------------------------------------------------------
 */

/*--- Hash functions ------------------------------------------------------*/

/*
 * Returns a hash value for the given string @a key.
 */
size_t hash_string(const char* key)
{
  size_t hash;

  /* Validate arguments */
  assert(key);

  /* Calculate hash value */
  hash = 5381;
  while (*key) {
    hash = hash * 33 + *key;
    key++;
  }

  return hash;
}


/*--- Unification functions -----------------------------------------------*/

/*
 * Helper function for the unification of strings. It searches for an existing
 * definition of the string @a key in the unfication data object @a data and
 * eventually adds it. In addition, an appropriate mapping from the given
 * @a local_id to its global identifier is created.
 */
void unify_string(EsdUnifyData* data, char* key, elg_ui4 local_id)
{
  EpkHashEntry* entry;
  size_t        index;
  elg_ui4*      global_id;

  /* String not yet defined? */
  entry = epk_hashtab_find(data->str2id, key, &index);
  if (!entry) {
    /* Create new entry */
    global_id = (elg_ui4*)malloc(sizeof(elg_ui4));
    if (!global_id)
      elg_error();

    /* Determine global ID */
    *global_id = epk_vector_size(data->strings);
    
    /* Insert string into tables */
    epk_hashtab_insert(data->str2id, key, global_id, &index);
    epk_vector_push_back(data->strings, key);
  } else {
    /* Retrieve global ID */
    global_id = (elg_ui4*)entry->value;

    /* Release temporary string */
    free(key);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_STRMAP], local_id, *global_id);
}


/*--- Comparisons ---------------------------------------------------------*/

/*
 * Compares two instances of type UnifySingleId (or a derived type) for
 * equality by comparing the IDs. Returns 0 if the IDs are equal, -1 if
 * @a key is less than @a entry and 1 of @a key is greater than @a entry.
 */
int single_id_cmp(const UnifySingleId* key, const UnifySingleId* entry)
{
  if (key->id < entry->id)
    return -1;
  else if (key->id > entry->id)
    return 1;
  else
    return 0;
}


/*
 * Compares two instances of type UnifyDoubleId (or a derived type) for
 * equality by comparing the IDs. Returns 0 if both IDs are equal and
 * 1 otherwise.
 */
int double_id_cmp(const UnifyDoubleId* key, const UnifyDoubleId* entry)
{
  return !(key->id1 == entry->id1 &&
           key->id2 == entry->id2);
}


/*
 * Compares the two given location definitions for equality. Returns 0 if
 * they are equal and 1 otherwise.
 */
int loc_cmp(const UnifyLoc* key, const UnifyLoc* entry)
{
  return !(key->machid == entry->machid && 
           key->nodeid == entry->nodeid && 
           key->procid == entry->procid && 
           key->thrdid == entry->thrdid);
}


/*
 * Compares the two given region definitions for equality. Returns 0 if
 * they are equal and 1 otherwise.
 */
int region_cmp(const UnifyRegion* key, const UnifyRegion* entry)
{
  return !(key->nameid == entry->nameid &&
           key->fileid == entry->fileid &&
           key->begln  == entry->begln  &&
           key->endln  == entry->endln);
}


/*
 * Compares the two given call site definitions for equality. Returns 0 if
 * they are equal and 1 otherwise.
 */
int csite_cmp(const UnifyCsite* key, const UnifyCsite* entry)
{
  return !(key->fileid  == entry->fileid  &&
           key->lineno  == entry->lineno  &&
           key->e_regid == entry->e_regid &&
           key->l_regid == entry->l_regid);
}


/*
 * Compares the two given MPI group definitions for equality. Returns
 * 0 if they are equal and non-zero otherwise.
 */
int group_cmp(const UnifyMpiGroup* key, const UnifyMpiGroup* entry)
{
  /* Compare storage mode and group size */
  if (key->mode != entry->mode || key->size != entry->size)
    return 1;

  /* Special handling for SELF */
  if (key->mode & ELG_GROUP_SELF)
    return 0;

  /* Compare ranks array */
  return memcmp(key->ranks, entry->ranks, key->size * sizeof(elg_ui4));
}


/*
 * Compares the two given Cartesian topology definitions for equality. Returns
 * 0 if they are equal and 1 otherwise.
 */
int cart_cmp(const UnifyCart* key, const UnifyCart* entry)
{
  if (key->comid != entry->comid)
    return 1;

  /* Only compare topology IDs if it is not an MPI topology */
  if (ELG_NO_ID == key->comid)
    return !(key->topid == entry->topid);

  return 0;
}


/*--- Memory management ---------------------------------------------------*/

/*
 * Releases the memory occupied by the given string hash table entry.
 */
void strhash_free(EpkHashEntry* entry)
{
  free(entry->key);
  free(entry->value);
}


/*
 * Releases the memory occupied by the given call path definition entry.
 */
void cpath_free(UnifyCpath* entry)
{
  if (entry->children)
    epk_vector_free(entry->children);
  free(entry);
}


/*
 * Releases the memory occupied by the given MPI group definition entry.
 */
void group_free(UnifyMpiGroup* entry)
{
  if (entry->ranks)
    free(entry->ranks);
  free(entry);
}


/*
 * Releases the memory occupied by the given cartesian topology definition
 * entry.
 */
void cart_free(UnifyCart* entry)
{
  /* Release coordinates */
  epk_vector_foreach(entry->coords, (epk_vec_proc_f)&coord_free);
  epk_vector_free(entry->coords);

  /* Release data structure */
  free(entry->nlocs);
  free(entry->period);
  free(entry->dimids);
  free(entry);
}


/*
 * Releases the memory occupied by the given cartesian coordinate definition
 * entry.
 */
void coord_free(CartCoord* entry)
{
  free(entry->coord);
  free(entry);
}


/*--- File I/O ------------------------------------------------------------*/

/*
 * Calculates the memory/file size requirements of the identifier mapping
 * table with ID @a map_id of the unification data object @a instance when
 * written to an EPILOG output stream.
 */
size_t map_size(const EsdUnifyData* instance, esd_unify_map_t map_id)
{
  EpkIdMap* map;
  size_t    count;
  size_t    result = 0;

  /* Determine map size */
  map   = instance->maps[map_id];
  count = epk_idmap_size(map);
  if (EPK_IDMAP_SPARSE == epk_idmap_mode(map))
    count *= 2;
  if (count > 0) {
    if (count <= 62)
      result = 8 + count * 4;
    else
      result = 8 + 62 * 4 + (count / 63) * 2 + (count - 62) * 4;
  }

  return result;
}


/*
 * Writes the mapping stored in the identifier mapping table with ID
 * @a map_id of the unification data object @a instance to the EPILOG output
 * stream @a mapfile.
 */
void write_map(const EsdUnifyData* instance, esd_unify_map_t map_id,
               ElgOut* mapfile)
{
  EpkIdMap* map;
  elg_ui1   type;
  elg_ui1   mode;
  size_t    count;
  elg_ui4*  data;

  /* Determine map */
  map = esd_unify_get_map(instance, map_id);

  /* Convert mapping table ID */
  switch (map_id) {
    case ESD_UNIFY_LOCMAP:
      type = ELG_LOCATION;
      break;
    case ESD_UNIFY_REGMAP:
      type = ELG_REGION;
      break;
    case ESD_UNIFY_CSITEMAP:
      type = ELG_CALL_SITE;
      break;
    case ESD_UNIFY_GROUPMAP:
      type = ELG_MPI_GROUP;
      break;
    case ESD_UNIFY_COMMAP:
      type = ELG_MPI_COMM;
      break;
    case ESD_UNIFY_WINMAP:
      type = ELG_MPI_WIN;
      break;
    default:
      elg_error_msg("Unknown mapping table ID");
  }

  /* Convert mapping mode */
  mode = epk_idmap_mode(map) == EPK_IDMAP_DENSE ? ELG_MAP_DENSE
                                                : ELG_MAP_SPARSE;

  /* Query data */
  data = (elg_ui4*)epk_idmap_data(map, &count);

  /* Write record */
  if (count > 0)
    ElgOut_write_IDMAP(mapfile, type, mode, count, data);
}


/*
 *---------------------------------------------------------------------------
 * EPILOG read callbacks
 *---------------------------------------------------------------------------
 */

void elg_readcb_ALL(elg_ui1 type,         /* record type */
		    elg_ui1 length,       /* record length */
                    void*   userdata)
{
  /* Nothing to be done... */
}


void elg_readcb_STRING(elg_ui4     strid,        /* string identifier */
		       elg_ui1     cntc,         /* number of continuation records */
		       const char* str,          /* string */
                       void*       userdata) 
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;

  /* Safety check */
  if (data->tmp_count > 0)
    elg_error_msg("ELG_STRING_CNT record expected!");

  /* No continuation records? */
  if (cntc == 0) {
    char* key;

    /* Duplicate string */
    key = strdup(str);
    if (!key)
      elg_error();

    /* Try to unify string */
    unify_string(data, key, strid);  
  } else {
    elg_ui1 maxlen;

    /* Save temporary data */
    data->tmp_id     = strid;
    data->tmp_count  = cntc;
    data->tmp_string = (char*)malloc((cntc + 1) * 255 * sizeof(char));
    if (!data->tmp_string)
      elg_error();

    maxlen = 255 - sizeof(elg_ui4) - sizeof(elg_ui1);
    strncpy(data->tmp_string, str, maxlen);
    data->tmp_string[maxlen] = '\0';
  }
}


void elg_readcb_STRING_CNT(const char* str,          /* continued string */
                           void*       userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;

  /* Safety check */
  if (data->tmp_count == 0)
    elg_error_msg("Unexpected ELG_STRING_CNT record!");

  /* Concatenate substrings */
  strncat(data->tmp_string, str, 255);

  /* Eventually try to unify string */
  data->tmp_count--;
  if (data->tmp_count == 0) {
    unify_string(data, data->tmp_string, data->tmp_id);
    data->tmp_string = NULL;
  }
}


void elg_readcb_MACHINE(elg_ui4 mid,          /* machine identifier */
		        elg_ui4 nodec,        /* number of nodes */
		        elg_ui4 mnid,         /* machine name identifier */
                        void*   userdata) 
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  size_t        global_id;

  /* Machine not yet defined? */
  if (epk_vector_find(data->machs, &mid,
                      (epk_vec_cmp_f)&single_id_cmp, &global_id) == 0) {
    UnifyMach* entry;

    /* Determine global ID */
    global_id = epk_vector_size(data->machs);

    /* Create new machine definition record */
    entry = (UnifyMach*)malloc(sizeof(UnifyMach));
    if (!entry)
      elg_error();
    entry->id     = mid;
    entry->nodec  = 0;
    entry->nameid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], mnid);

    epk_vector_push_back(data->machs, entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_MACHMAP], mid, global_id);
}


void elg_readcb_NODE(elg_ui4 nid,          /* node identifier */
		     elg_ui4 mid,          /* machine identifier */
		     elg_ui4 cpuc,         /* number of CPUs */
		     elg_ui4 nnid,         /* node name identifer */
		     elg_d8  cr,           /* number of clock cycles/sec */
                     void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyNode*    entry;
  size_t        global_id;

  /* Construct new entry */
  entry = (UnifyNode*)malloc(sizeof(UnifyNode));
  if (!entry)
      elg_error();
  entry->id      = nid;
  entry->machid  = epk_idmap_get(data->maps[ESD_UNIFY_MACHMAP], mid);
  entry->cpuc    = cpuc;
  entry->nameid  = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], nnid);
  entry->clockrt = cr;

  /* Node not yet defined? */
  if (epk_vector_find(data->nodes, entry,
                      (epk_vec_cmp_f)&double_id_cmp, &global_id) == 0) {
    /* Determine global ID */
    global_id = epk_vector_size(data->nodes);

    /* Adjust node count for machine */
    ((UnifyMach*)epk_vector_at(data->machs, entry->machid))->nodec++;

    epk_vector_push_back(data->nodes, entry);
  } else {
    free(entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_NODEMAP], nid, global_id);
}


void elg_readcb_PROCESS(elg_ui4 pid,          /* process identifier */
		        elg_ui4 pnid,         /* process name identifier */
                        void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;

  /* Process not yet defined? */
  if (epk_vector_find(data->procs, &pid,
                      (epk_vec_cmp_f)&single_id_cmp, NULL) == 0) {
    UnifyProc* entry;

    /* Create new process definition record */
    entry = (UnifyProc*)malloc(sizeof(UnifyProc));
    if (!entry)
      elg_error();
    entry->id     = pid;
    entry->nameid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], pnid);

    epk_vector_push_back(data->procs, entry);
  }
}


void elg_readcb_THREAD(elg_ui4 tid,          /* thread identifier */
		       elg_ui4 pid,          /* process identifier */
		       elg_ui4 tnid,         /* thread name identifier */
                       void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyThrd*    entry;

  /* Create new thread definition record */
  entry = (UnifyThrd*)malloc(sizeof(UnifyThrd));
  if (!entry)
    elg_error();
  entry->id     = tid;
  entry->pid    = pid;
  entry->nameid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], tnid);

  /* Thread not yet defined? */
  if (epk_vector_find(data->thrds, entry,
                      (epk_vec_cmp_f)&double_id_cmp, NULL) == 0)
    epk_vector_push_back(data->thrds, entry);
  else
    free(entry);
}


void elg_readcb_LOCATION(elg_ui4 lid,          /* location identifier */
			 elg_ui4 mid,          /* machine identifier */
			 elg_ui4 nid,          /* node identifier */
			 elg_ui4 pid,          /* process identifier */
			 elg_ui4 tid,          /* thread identifier */
                         void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyLoc*     entry;
  size_t        global_id;

  /* Construct new entry */
  entry = (UnifyLoc*)malloc(sizeof(UnifyLoc));
  if (!entry)
      elg_error();
  entry->machid = epk_idmap_get(data->maps[ESD_UNIFY_MACHMAP], mid);
  entry->nodeid = epk_idmap_get(data->maps[ESD_UNIFY_NODEMAP], nid);
  entry->procid = pid;
  entry->thrdid = tid;

  /* Location not yet defined? */
  if (data->mode == ESD_UNIFY_PARTIAL ||
      epk_vector_find(data->locs, entry, 
                      (epk_vec_cmp_f)&loc_cmp, &global_id) == 0) { 
    global_id = epk_vector_size(data->locs); 

    epk_vector_push_back(data->locs, entry);
  } else {
    free(entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_LOCMAP], lid, global_id);
}


void elg_readcb_FILE(elg_ui4 fid,          /* file identifier */
		     elg_ui4 fnid,         /* file name identifier */
                     void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  elg_ui4       name;
  size_t        global_id;

  /* Map file name */
  name = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], fnid);

  /* File not yet defined? */
  if (epk_vector_find(data->files, &name,
                      (epk_vec_cmp_f)&single_id_cmp, &global_id) == 0) {
    elg_ui4* entry;

    /* Determine global ID */
    global_id = epk_vector_size(data->files);

    /* Create new entry */
    entry = (elg_ui4*)malloc(sizeof(elg_ui4));
    if (!entry)
      elg_error();
    *entry = name;

    epk_vector_push_back(data->files, entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_FILEMAP], fid, global_id);
}


void elg_readcb_REGION(elg_ui4 rid,          /* region identifier */
		       elg_ui4 rnid,         /* region name identifier */
		       elg_ui4 fid,          /* source file identifier */
		       elg_ui4 begln,        /* begin line number */
		       elg_ui4 endln,        /* end line number */
		       elg_ui4 rdid,         /* region description identifier */
		       elg_ui1 rtype,        /* region type */
                       void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyRegion*  entry;
  size_t        global_id;

  /* Construct new entry */
  entry = (UnifyRegion*)malloc(sizeof(UnifyRegion));
  if (!entry)
      elg_error();
  entry->nameid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], rnid);
  entry->fileid = epk_idmap_get(data->maps[ESD_UNIFY_FILEMAP], fid);
  entry->begln  = begln;
  entry->endln  = endln;
  entry->descid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], rdid);
  entry->type   = rtype;

  /* Region not yet defined? */
  if (epk_vector_find(data->regions, entry,
                      (epk_vec_cmp_f)&region_cmp, &global_id) == 0) {
    /* Determine global ID */
    global_id = epk_vector_size(data->regions);

    epk_vector_push_back(data->regions, entry);
  } else {
    free(entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_REGMAP], rid, global_id);
}


void elg_readcb_CALL_SITE(elg_ui4 csid,         /* call site identifier */
			  elg_ui4 fid,          /* source file identifier */
			  elg_ui4 lno,          /* line number */
			  elg_ui4 erid,         /* region identifer to be entered */
			  elg_ui4 lrid,         /* region identifer to be left */
                          void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyCsite*   entry;
  size_t        global_id;

  /* Construct new entry */
  entry = (UnifyCsite*)malloc(sizeof(UnifyCsite));
  if (!entry)
      elg_error();
  entry->fileid  = epk_idmap_get(data->maps[ESD_UNIFY_FILEMAP], fid);
  entry->lineno  = lno;
  entry->e_regid = epk_idmap_get(data->maps[ESD_UNIFY_REGMAP], erid);
  entry->l_regid = epk_idmap_get(data->maps[ESD_UNIFY_REGMAP], lrid);

  /* Call site not yet defined? */
  if (epk_vector_find(data->csites, entry,
                      (epk_vec_cmp_f)&csite_cmp, &global_id) == 0) {
    /* Determine global ID */
    global_id = epk_vector_size(data->csites);

    epk_vector_push_back(data->csites, entry);
  } else {
    free(entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_CSITEMAP], csid, global_id);
}


void elg_readcb_CALL_PATH(elg_ui4 cpid,         /* call-path identifier */
			  elg_ui4 rid,          /* node region identifier */
			  elg_ui4 ppid,         /* parent call-path identifier */
			  elg_ui8 order,        /* node order specifier */
                          void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  EpkVector*    list;
  UnifyCpath*   entry;
  size_t        index;
  size_t        global_id;

  /* Construct new entry */
  entry = (UnifyCpath*)malloc(sizeof(UnifyCpath));
  if (!entry)
    elg_error();
  entry->rid  = epk_idmap_get(data->maps[ESD_UNIFY_REGMAP], rid);
  entry->ppid = epk_idmap_get(data->maps[ESD_UNIFY_CPATHMAP], ppid);

  /* Root call-path? => Search in root list */
  /* Otherwise search in parents child list */
  if (ELG_NO_ID == entry->ppid)
    list = data->cproots;
  else {
    UnifyCpath* parent = (UnifyCpath*)epk_vector_at(data->cpaths, entry->ppid);

    /* Eventually create child list */
    if (!parent->children)
      parent->children = epk_vector_create();

    list = parent->children;
  }

  /* Call-path not yet defined? */
  if (epk_vector_find(list, entry,
                      (epk_vec_cmp_f)&double_id_cmp, &index) == 0) {
    /* Determine global ID */
    global_id = epk_vector_size(data->cpaths);

    /* Initialize remaining fields */
    entry->id       = global_id;
    entry->order    = order;
    entry->children = NULL;

    /* Add item to call-path vector & root/child list */
    epk_vector_push_back(data->cpaths, entry);
    epk_vector_push_back(list, entry);
  } else {
    UnifyCpath* exist = epk_vector_at(list, index);

    /* Determine global ID & update order attribute */
    global_id = exist->id;
    if (exist->order > order)
      exist->order = order;

    free(entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_CPATHMAP], cpid, global_id);
}


void elg_readcb_METRIC(elg_ui4 metid,        /* metric identifier */
		       elg_ui4 metnid,       /* metric name identifier */
		       elg_ui4 metdid,       /* metric description identifier */
		       elg_ui1 metdtype,     /* metric data type */
		       elg_ui1 metmode,      /* metric mode */
		       elg_ui1 metiv,        /* time interval referenced */
                       void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyMetric*  entry;

  /* 
   * It is assumed that each metric carries its global
   * identifier.
   *
   * Also, it is assumed that the metric identifiers are
   * already enumerated from 0 to n-1.
   */

  /* Create new metric definition record */
  entry = (UnifyMetric*)malloc(sizeof(UnifyMetric));
  if (!entry)
    elg_error();
  entry->id     = metid;
  entry->nameid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], metnid);
  entry->descid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], metdid);
  entry->type   = metdtype;
  entry->mode   = metmode;
  entry->ival   = metiv;

  /* Metric not yet defined? */
  if (epk_vector_find(data->metrics, entry,
                      (epk_vec_cmp_f)&single_id_cmp, NULL) == 0)
    epk_vector_push_back(data->metrics, entry);
  else
    free(entry);
}


void elg_readcb_MPI_GROUP(elg_ui4 gid,         /* group identifier */
                          elg_ui1 mode,        /* mode flags */
                          elg_ui4 grpc,        /* number of ranks in group */
                          elg_ui4 grpv[],      /* local |-> global rank mapping */
                          void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  elg_ui4       global_id;

  /* Store entry */
  global_id = esd_unify_add_group(data, gid, mode, grpc, grpv);

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_GROUPMAP], gid, global_id);
}


void elg_readcb_MPI_COMM_DIST(elg_ui4 cid,    /* communicator id unique to root */
                              elg_ui4 root,   /* global rank of root process */
                              elg_ui4 lcid,   /* local communicator id on process */
                              elg_ui4 lrank,  /* local rank of process */
                              elg_ui4 size,   /* size of communicator */
                              void* userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyComDist* entry;

  /* Only process ELG_MPI_COMM_DIST records in partial unification mode */
  if (ESD_UNIFY_PARTIAL != data->mode)
    return;

  /* Create new distributed communicator definition record */
  entry = (UnifyComDist*)malloc(sizeof(UnifyComDist));
  if (!entry)
    elg_error();
  entry->comid  = cid;
  entry->root   = root;
  entry->lcomid = lcid;
  entry->lrank  = lrank;
  entry->size   = size;

  /* Count COMM_SELF-like communicators */
  if (1 == entry->size)
    ++data->com_self;
  /* Count communicators where process is root */
  else if (data->rank == entry->root)
    ++data->com_root;

  /* Store entry */
  epk_vector_push_back(data->dist_coms, entry);

}


void elg_readcb_MPI_COMM_REF(elg_ui4 cid,      /* communicator identifier */
                             elg_ui4 gid,      /* group identifier */
                             void* userdata)
{
  /* Nothing to be done. This should never be called... */
}


void elg_readcb_MPI_COMM(elg_ui4 cid,          /* communicator identifier */
                         elg_ui1 mode,         /* mode flags */
                         elg_ui4 grpc,         /* size of bitstring in bytes */
                         elg_ui1 grpv[],       /* bitstring defining the group */
                         void*   userdata)
{
  /* Nothing to be done. This should never be called... */
}


void elg_readcb_CART_TOPOLOGY(elg_ui4 topid,         /* topology id */
			      elg_ui4 tnid,          /* topology name id */
			      elg_ui4 cid,           /* communicator id */
			      elg_ui1 ndims,         /* number of dimensions */
			      elg_ui4 dimv[],        /* number of processes in each dim */  
			      elg_ui1 periodv[],     /* periodicity in each dim */
			      elg_ui4 dimids[],      /* dimension names' ids */
			      void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  UnifyCart*    entry;
  size_t        index;

  /* Only process ELG_CART_TOPOLOGY records in partial unification mode */
  if (ESD_UNIFY_PARTIAL != data->mode)
    return;

  /* Construct new entry */
  entry = (UnifyCart*)malloc(sizeof(UnifyCart));
  if (!entry)
    elg_error();
  entry->topid = topid;
  entry->tnid = epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], tnid); // Needs to copy unified string id
  entry->comid = cid;

  /* Topology not yet defined? */
  if (epk_vector_find(data->tops, entry,
                      (epk_vec_cmp_f)&cart_cmp, &index) == 0) {
    elg_ui4 ncoords;
    elg_ui4 i;

    /* Calculate total number of coordinates */
    ncoords = 1;
    for (i = 0; i < ndims; ++i)
      ncoords *= dimv[i];

    /* Update topology entry */
    entry->topid = epk_vector_size(data->tops);
    entry->ndims = ndims;
    entry->nlocs = (elg_ui4*)malloc(ndims * sizeof(elg_ui4));
    if (!entry->nlocs)
      elg_error();
    memcpy(entry->nlocs, dimv, ndims * sizeof(elg_ui4));
    entry->period = (elg_ui1*)malloc(ndims * sizeof(elg_ui1));
    if (!entry->period)
      elg_error();
    memcpy(entry->period, periodv, ndims * sizeof(elg_ui1));
    entry->coords = epk_vector_create_size(ncoords);
    entry->dimids = (elg_ui4*)malloc(ndims * sizeof(elg_ui4));
    if (!entry->dimids)
      elg_error();

    for(i=0;i<ndims;i++) {
      entry->dimids[i]=epk_idmap_get(data->maps[ESD_UNIFY_STRMAP], dimids[i]); // unified string ids
    }

    epk_vector_push_back(data->tops, entry);
  } else {
    free(entry);
    entry = epk_vector_at(data->tops, index);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_TOPMAP], topid, entry->topid);
}


void elg_readcb_CART_COORDS(elg_ui4 topid,        /* topology id */
			    elg_ui4 lid,          /* location id */
			    elg_ui1 ndims,        /* number of dimensions */
			    elg_ui4 coordv[],     /* coordinates in each dimension */
			    void*   userdata)
{
  EsdUnifyData* data   = (EsdUnifyData*)userdata;
  elg_ui4       gtopid;
  elg_ui4       glid;
  UnifyCart*    cart;
  CartCoord*    entry;
  size_t        index;

  /* Only process ELG_CART_COORDS records in partial unification mode */
  if (ESD_UNIFY_PARTIAL != data->mode)
    return;

  /* Search topology */
  gtopid = epk_idmap_get(data->maps[ESD_UNIFY_TOPMAP], topid);
  if (epk_vector_find(data->tops, &gtopid,
                      (epk_vec_cmp_f)&single_id_cmp, &index) == 0)
    elg_error_msg("ELG_CART_COORDS record refers to undefined topology!");

  /* Retrieve topology */
  cart = epk_vector_at(data->tops, index);

  /* Search whether coordinate already exists */
  glid  = epk_idmap_get(data->maps[ESD_UNIFY_LOCMAP], lid);
  index = epk_vector_size(cart->coords);
  if (data->mode == ESD_UNIFY_PARTIAL ||
      epk_vector_lower_bound(cart->coords, &glid,
                             (epk_vec_cmp_f)&single_id_cmp, &index) == 0) {
    /* Create new coordinate entry */
    entry = (CartCoord*)malloc(sizeof(CartCoord));
    if (!entry)
      elg_error();
    entry->locid = glid;
    entry->coord = (elg_ui4*)malloc(ndims * sizeof(elg_ui4));
    if (!entry->coord)
      elg_error();
    memcpy(entry->coord, coordv, ndims * sizeof(elg_ui4));

    /* Store coordinate entry in topology */
    epk_vector_insert(cart->coords, index, entry);
  } else {
    entry = epk_vector_at(cart->coords, index);

    /* Compare coordinates and warn if different */
    if (memcmp(coordv, entry->coord, ndims * sizeof(elg_ui4)))
      elg_warning("Additional coordinate definition for location!");
  }
}


void elg_readcb_OFFSET(elg_d8 ltime,        /* local time */
		       elg_d8 offset,       /* offset to global time */
                       void*  userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;

  /* Sanity check */
  if (data->offcnt == 2)
    elg_error_msg("Unexpected ELG_OFFSET record!");

  /* Store offset data */
  data->ltime [data->offcnt] = ltime;
  data->offset[data->offcnt] = offset;
  data->offcnt++;
}


void elg_readcb_NUM_EVENTS(elg_ui4 eventc,       /* number of event records */
                           void*   userdata)
{
  /* Nothing to be done... */
}


void elg_readcb_EVENT_TYPES(elg_ui4 ntypes,     /* number of event types */
                            elg_ui1 typev[],    /* vector of event types */
                            void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;

  /* Sanity checks */
  if (data->ntypes > 0 && data->ntypes != ntypes)
    elg_error_msg("Inconsistent ELG_EVENT_TYPES record!");
  if (data->evtypes)
    elg_error_msg("Unexpected ELG_EVENT_TYPES record!");

  /* Store type data */
  data->ntypes  = ntypes;
  data->evtypes = (elg_ui1*)malloc(ntypes * sizeof(elg_ui1));
  memcpy(data->evtypes, typev, ntypes * sizeof(elg_ui1));
}


void elg_readcb_EVENT_COUNTS(elg_ui4 ntypes,    /* number of event types */
                             elg_ui4 countv[],  /* vector of type counts */
                             void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;

  /* Sanity checks */
  if (data->ntypes > 0 && data->ntypes != ntypes)
    elg_error_msg("Inconsistent ELG_EVENT_COUNTS record!");
  if (data->evcounts)
    elg_error_msg("Unexpected ELG_EVENT_COUNTS record!");

  /* Store type data */
  data->ntypes   = ntypes;
  data->evcounts = (elg_ui4*)malloc(ntypes * sizeof(elg_ui4));
  memcpy(data->evcounts, countv, ntypes * sizeof(elg_ui4));
}


void elg_readcb_MAP_SECTION(elg_ui4 rank,   /* rank number the section relates to */
                            void* userdata)
{
  /* Nothing to be done... */
}


void elg_readcb_MAP_OFFSET(elg_ui4 rank,   /* rank number the offset relates to */
                           elg_ui4 offset, /* file offset in mapping file */
                           void* userdata )
{
  /* Nothing to be done... */
}

void elg_readcb_IDMAP(elg_ui1 type,     /* object type to be mapped */
                      elg_ui1 mode,     /* mapping mode (dense/sparse) */
                      elg_ui4 count,    /* number of entries */
                      elg_ui4 mapv[],   /* vector of mappings */
                      void* userdata)
{
  /* Nothing to be done... */
}


void elg_readcb_LAST_DEF(void* userdata)
{
  /* Nothing to be done... */
}


void elg_readcb_MPI_WIN(elg_ui4 wid,          /* window identifier */
                        elg_ui4 cid,          /* communicator identifier */
                        void*   userdata)
{
  EsdUnifyData* data = (EsdUnifyData*)userdata;
  ComCount*     counter;
  UnifyWin*     entry;
  size_t        index;
  size_t        global_id;

  /* Create counter entry */
  counter = (ComCount*)malloc(sizeof(ComCount));
  if (!counter)
    elg_error();
  if (ESD_UNIFY_FULL == data->mode)
    counter->comid = epk_idmap_get(data->maps[ESD_UNIFY_COMMAP], cid);
  else
    counter->comid = cid;
  counter->count = 0;

  /* Search counter entry */
  if (epk_vector_find(data->comcnt, counter,
                      (epk_vec_cmp_f)&single_id_cmp, &index) == 0) {
    epk_vector_push_back(data->comcnt, counter);
  } else {
    free(counter);
    counter = (ComCount*)epk_vector_at(data->comcnt, index);
    counter->count++;
  }

  /* Create RMA window description record */
  entry = (UnifyWin*)malloc(sizeof(UnifyWin));
  if (!entry)
    elg_error();
  entry->seqno = counter->count;
  entry->comid = counter->comid;

  /* RMA window not yet defined? */
  if (epk_vector_find(data->wins, entry,
                      (epk_vec_cmp_f)&double_id_cmp, &global_id) == 0) {
    /* Determine global ID */
    global_id = epk_vector_size(data->wins);

    epk_vector_push_back(data->wins, entry);
  } else {
    free(entry);
  }

  /* Add mapping */
  epk_idmap_add(data->maps[ESD_UNIFY_WINMAP], wid, global_id);
}


/* Include empty callback functions for event records */
#include "elg_readcb_dummies.c.gen"
