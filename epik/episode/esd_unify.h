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

#ifndef _ESD_UNIFY_H
#define _ESD_UNIFY_H


#include <stddef.h>
#include <stdio.h>

#include "elg_rw.h"
#include "elg_impl.h"
#include "epk_idmap.h"
#include "epk_vector.h"
#include "epk_hashtab.h"


#ifdef __cplusplus
#  define EXTERN extern "C"
#else
#  define EXTERN extern 
#endif


/*--- Type definitions ----------------------------------------------------*/

/** Opaque data structure storing global and per-process unification data. */
typedef struct EsdUnifyData_struct EsdUnifyData;

/**
 * Enumeration type defining symbolic names for the different per-process
 * identifier mapping tables handled by EsdUnifyData.
 */
typedef enum {
  ESD_UNIFY_STRMAP,       /**< String ID mapping table */
  ESD_UNIFY_MACHMAP,      /**< Machine ID mapping table */
  ESD_UNIFY_NODEMAP,      /**< Node ID mapping table */
  ESD_UNIFY_LOCMAP,       /**< Location ID mapping table */
  ESD_UNIFY_FILEMAP,      /**< File ID mapping table */
  ESD_UNIFY_REGMAP,       /**< Region ID mapping table */
  ESD_UNIFY_CSITEMAP,     /**< Call site ID mapping table */
  ESD_UNIFY_CPATHMAP,     /**< Call-path ID mapping table */
  ESD_UNIFY_GROUPMAP,     /**< MPI group ID mapping table */
  ESD_UNIFY_COMMAP,       /**< MPI communicator ID mapping table */
  ESD_UNIFY_WINMAP,       /**< RMA window ID mapping table */
  ESD_UNIFY_TOPMAP,       /**< Cartesian topology ID mapping table */
  ESD_UNIFY_NUM_MAPS      /**< Total number of mapping tables */
} esd_unify_map_t;

/**
 * Enumeration type defining symbolic names for the different unification
 * modes.
 */
typedef enum {
  ESD_UNIFY_FULL,         /**< Full unification, incl. locations & topologies */
  ESD_UNIFY_PARTIAL       /**< Partial unification */
} esd_unify_mode_t;

/*
 *---------------------------------------------------------------------------
 * EsdUnify
 *---------------------------------------------------------------------------
 */

/** @name Unification data handling
 ** @{ */

/* Construction & destruction */
EXTERN EsdUnifyData* esd_unify_create();
EXTERN void          esd_unify_free  (EsdUnifyData* instance);

/* Unification */
EXTERN void          esd_unify_buffer(      EsdUnifyData*    instance,
                                      const buffer_t         buffer,
                                            size_t           size,
                                            int              rank,
                                            esd_unify_mode_t mode);

/* Unification data management */
EXTERN void          esd_unify_clear_data(EsdUnifyData* instance);

/* Identifier mapping table management */
EXTERN EpkIdMap*     esd_unify_get_map   (const EsdUnifyData*   instance,
                                                esd_unify_map_t map_id);
EXTERN void          esd_unify_clear_maps(      EsdUnifyData*   instance);

/* Determine output file sizes */
EXTERN size_t        esd_unify_size_defs(const EsdUnifyData*    instance,
                                               esd_unify_mode_t mode);
EXTERN size_t        esd_unify_size_maps(const EsdUnifyData*    instance);


/* File I/O */
EXTERN void          esd_unify_write_defs(EsdUnifyData*    instance,
                                          ElgOut*          deffile,
                                          esd_unify_mode_t mode);
EXTERN void          esd_unify_write_maps(EsdUnifyData*    instance,
                                          int              rank,
                                          ElgOut*          mapfile);

/* Data serialization */
EXTERN void          esd_unify_serialize_defs(EsdUnifyData*    instance,
                                              buffer_t*        buffer,
                                              size_t*          size,
                                              esd_unify_mode_t mode);
EXTERN void          esd_unify_serialize_maps(EsdUnifyData*    instance,
                                              int              rank,
                                              buffer_t*        buffer,
                                              size_t*          size);

/* MPI group and communicator management */
EXTERN elg_ui4       esd_unify_add_group(EsdUnifyData* instance,
                                         elg_ui4       group_id,
                                         elg_ui1       mode,
                                         elg_ui4       size,
                                         elg_ui4*      ranks);
EXTERN void          esd_unify_add_comm (EsdUnifyData* instance,
                                         elg_ui4       comm_id,
                                         elg_ui4       group_id);

/** @} */

/*--- Type definitions ----------------------------------------------------*/

/*
 * Structure for storing the data of an unified machine record. It is
 * "derived" from UnifySingleId.
 */
typedef struct
{
  elg_ui4 id;          /* Local machine ID */
  elg_ui4 nodec;       /* Number of nodes */
  elg_ui4 nameid;      /* Global name string ID */
} UnifyMach;

/*
 * Structure for storing the data of an unified node record. It is
 * "derived" from UnifyDoubleId, since a node identifier is local
 * to a machine.
 */
typedef struct
{
  elg_ui4 nameid;      /* Global name string ID */
  elg_ui4 machid;      /* Global machine ID */
  elg_ui4 id;          /* Local node ID */
  elg_ui4 cpuc;        /* Number of CPUs */
  elg_d8  clockrt;     /* Clock rate */
} UnifyNode;

/*
 * Structure for storing the data of an unified process record. It is
 * "derived" from UnifySingleId.
 */
typedef struct
{
  elg_ui4 id;         /* Local process ID */
  elg_ui4 nameid;     /* Global name string ID */
} UnifyProc;

/*
 * Structure for storing the data of an unified thread record. It is
 * "derived" from UnifyDoubleId, since thread identifiers are local
 * to their corresponding process.
 */
typedef struct
{
  elg_ui4 id;         /* Local thread ID */
  elg_ui4 pid;        /* Local process ID */
  elg_ui4 nameid;     /* Global name string ID */
} UnifyThrd;

/* Structure for storing the data of an unified location record. */
typedef struct
{
  elg_ui4 machid;     /* Global machine ID */
  elg_ui4 nodeid;     /* Global node ID */
  elg_ui4 procid;     /* Local process ID */
  elg_ui4 thrdid;     /* Local thread ID */
} UnifyLoc;

/* Structure for storing the data of an unified source code region record. */
typedef struct
{
  elg_ui4 nameid;     /* Global name string ID */
  elg_ui4 fileid;     /* Global file ID */
  elg_ui4 begln;      /* Begin line number */
  elg_ui4 endln;      /* End line number */
  elg_ui4 descid;     /* Global description string ID */
  elg_ui1 type;       /* Region type */
} UnifyRegion;

/* Structure for storing the data of an unified call site record. */
typedef struct
{
  elg_ui4 fileid;      /* Global file ID */
  elg_ui4 lineno;      /* Line number */
  elg_ui4 e_regid;     /* Global ID of region to be entered */
  elg_ui4 l_regid;     /* Global ID of region to be left */
} UnifyCsite;

/* Structure for storing the data of an unified call-path record. */
typedef struct
{
  elg_ui4    rid;          /* Global region ID */
  elg_ui4    ppid;         /* Global ID of parent call-path */
  elg_ui4    id;           /* Global call-path ID (needed because records are stored in multiple lists) */
  elg_ui8    order;        /* Node order specifier */
  EpkVector* children;     /* List of child nodes */
} UnifyCpath;

/*
 * Structure for storing the data of an unified hardware counter metric
 * record. It is "derived" from UnifySingleId.
 */
typedef struct
{
  elg_ui4 id;         /* Global metric ID */
  elg_ui4 nameid;     /* Global name string ID */
  elg_ui4 descid;     /* Global description string ID */
  elg_ui1 type;       /* Metric type */
  elg_ui1 mode;       /* Metric mode */
  elg_ui1 ival;       /* Time interval */
} UnifyMetric;

/* Structure storing the data of a distributed MPI communicator record. */
typedef struct
{
  elg_ui4 comid;    /* Communicator ID unique to root process */
  elg_ui4 root;     /* Global rank of root process */
  elg_ui4 lcomid;   /* Local communicator ID on process */
  elg_ui4 lrank;    /* Local rank of process in this communicator */
  elg_ui4 size;     /* Size of communicator */
} UnifyComDist;

/*
 * Structure for storing the data of an unified cartesian topology record.
 * It is "derived" from UnifySingleId.
 */ 
typedef struct
{
  elg_ui4    topid;      /* Global topology ID */
  elg_ui4    tnid;       /* Global topology name ID */
  elg_ui4    comid;      /* Global communicator ID */
  elg_ui4    ndims;      /* Number of dimensions */
  elg_ui4*   nlocs;      /* Number of locations per dimension */
  elg_ui1*   period;     /* Periodicity per location */
  EpkVector* coords;     /* Coordinates */
  elg_ui4*   dimids;     /* Dimension names' ids */
} UnifyCart;

/*
 * Structure for storing the data of an unified RMA window record. It is
 * "derived" from UnifyDoubleId.
 */
typedef struct
{
  elg_ui4 seqno;     /* Sequence number */
  elg_ui4 comid;     /* Global communicator ID */
} UnifyWin;

/*
 * Structure for storing the data of unified cartesian coordinate records.
 * It is "derived" from UnifySingleId.
 */
typedef struct
{
  elg_ui4  locid;     /* Global location ID */
  elg_ui4* coord;     /* Coordinate */
} CartCoord;


/*--- Type definitions ----------------------------------------------------*/

/* Actual data structure storing temporary and unification results. */
struct EsdUnifyData_struct
{
  /*--- Temporary data ---*/
  elg_ui4     tmp_id;         /* Temporary identifier */
  elg_ui4     tmp_count;      /* Temporary count */
  char*       tmp_string;     /* Temporary string buffer */
  EpkVector*  comcnt;         /* Temporary vector to count how many times a
                                 window is created for a certain communicator */

  esd_unify_mode_t mode;      /* Current unification mode */
  int              rank;      /* current rank of process being unified */

  /*--- Global unification data ---*/
  EpkHashTab* str2id;         /* String |-> Global string ID */
  EpkIdMap*   topmap;         /* Global communicator ID |-> Global topology ID */

  EpkVector*  strings;        /* Unified strings */
  EpkVector*  machs;          /* Unified machines */
  EpkVector*  nodes;          /* Unified nodes */
  EpkVector*  procs;          /* Unified processes */
  EpkVector*  thrds;          /* Unified threads */
  EpkVector*  locs;           /* Unified locations */
  EpkVector*  files;          /* Unified files */
  EpkVector*  regions;        /* Unified regions */
  EpkVector*  csites;         /* Unified call sites */
  EpkVector*  cpaths;         /* Unified call-paths */
  EpkVector*  cproots;        /* Root nodes of unified call-paths */
  EpkVector*  metrics;        /* Unified metrics */
  EpkVector*  groups;         /* Unified MPI groups */
  EpkVector*  coms;           /* Unified MPI communicators */
  EpkVector*  tops;           /* Unified topologies */
  EpkVector*  wins;           /* Unified RMA windows */

  /*--- Per-process unification data ---*/
  EpkIdMap*   maps[ESD_UNIFY_NUM_MAPS];     /* Identifier mapping tables */

  elg_ui1     offcnt;         /* Number of time synchronization measurements */
  elg_d8      ltime[2];       /* Local time at start/end of measurement */
  elg_d8      offset[2];      /* Time offset at start/end of measurement */

  elg_ui4     ntypes;         /* Number of event types/counts */
  elg_ui1*    evtypes;        /* Array containing event types */
  elg_ui4*    evcounts;       /* Array containing event counts */

  EpkVector*  dist_coms;      /* Distributed communicator records */
  elg_ui4     com_root;       /* Number of communicators where process is root */
  elg_ui4     com_self;       /* Number of COMM_SELF-like communicators */
};


#endif   /* !_ESD_UNIFY_H */
