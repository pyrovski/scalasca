/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file epk_mpireg.c
 *
 * @brief Registration of MPI regions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "epk_mpireg.h"
#include "epk_conf.h"

#include "elg_error.h"

#include "esd_def.h"
#include "epk_defs_mpi.h"

/**
 * MPI operation event type
 */
typedef struct
{
  char*   name;
  elg_ui4 type;
} EpkMPItype;

/** MPI point-to-point function types
 * @note contents must be alphabetically sorted */
static const EpkMPItype epk_mpi_pt2pt[] = {
  { "MPI_Bsend",            EPK_MPI_TYPE__SEND                                              },
  { "MPI_Ibsend",           EPK_MPI_TYPE__SEND                                              },
  { "MPI_Irsend",           EPK_MPI_TYPE__SEND                                              },
  { "MPI_Isend",            EPK_MPI_TYPE__SEND                                              },
  { "MPI_Issend",           EPK_MPI_TYPE__SEND                                              },
  { "MPI_Recv",             EPK_MPI_TYPE__RECV                                              },
  { "MPI_Rsend",            EPK_MPI_TYPE__SEND                                              },
  { "MPI_Send",             EPK_MPI_TYPE__SEND                                              },
  { "MPI_Sendrecv",         EPK_MPI_TYPE__SEND + EPK_MPI_TYPE__RECV                         },
  { "MPI_Sendrecv_replace", EPK_MPI_TYPE__SEND + EPK_MPI_TYPE__RECV                         },
  { "MPI_Ssend",            EPK_MPI_TYPE__SEND                                              },
  { "MPI_Start",            EPK_MPI_TYPE__SEND                                              },
  { "MPI_Startall",         EPK_MPI_TYPE__SEND                                              },
  { "MPI_Test",             EPK_MPI_TYPE__RECV                                              },
  { "MPI_Testall",          EPK_MPI_TYPE__RECV                                              },
  { "MPI_Testany",          EPK_MPI_TYPE__RECV                                              },
  { "MPI_Testsome",         EPK_MPI_TYPE__RECV                                              },
  { "MPI_Wait",             EPK_MPI_TYPE__RECV                                              },
  { "MPI_Waitall",          EPK_MPI_TYPE__RECV                                              },
  { "MPI_Waitany",          EPK_MPI_TYPE__RECV                                              },
  { "MPI_Waitsome",         EPK_MPI_TYPE__RECV                                              },
};

/** MPI collective function types
 * @note contents must be alphabetically sorted */
static const EpkMPItype epk_mpi_colls[] = {
  { "MPI_Allgather",            EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Allgatherv",           EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Allreduce",            EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Alltoall",             EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Alltoallv",            EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Alltoallw",            EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Barrier",              EPK_COLL_TYPE__BARRIER                                 },
  { "MPI_Bcast",                EPK_COLL_TYPE__ONE2ALL                                 },
  { "MPI_Cart_create",          EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Cart_sub",             EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Comm_create",          EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Comm_dup",             EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Comm_free",            EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Comm_split",           EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Exscan",               EPK_COLL_TYPE__PARTIAL                                 },
  { "MPI_Finalize",             EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Gather",               EPK_COLL_TYPE__ALL2ONE                                 },
  { "MPI_Gatherv",              EPK_COLL_TYPE__ALL2ONE                                 },
  { "MPI_Init",                 EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Init_thread",          EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Reduce",               EPK_COLL_TYPE__ALL2ONE                                 },
  { "MPI_Reduce_scatter",       EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Reduce_scatter_block", EPK_COLL_TYPE__ALL2ALL                                 },
  { "MPI_Scan",                 EPK_COLL_TYPE__PARTIAL                                 },
  { "MPI_Scatter",              EPK_COLL_TYPE__ONE2ALL                                 },
  { "MPI_Scatterv",             EPK_COLL_TYPE__ONE2ALL                                 },
  { "MPI_Win_create",           EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Win_fence",            EPK_COLL_TYPE__IMPLIED                                 },
  { "MPI_Win_free",             EPK_COLL_TYPE__IMPLIED                                 }
};

/** Region IDs of MPI functions */
int epk_mpi_regid[EPK__MPI_NUMFUNCS + 1];

/** Mapping of string keys to enabling group IDs
 * @note The keys need to be alphabetically sorted, to enable binary
 *       search on the data structure.
 */
static const EpkMPItype epk_mpi_enable_groups[] = {
  { "ALL",       EPK_MPI_ENABLED_ALL                     },
  { "CG",        EPK_MPI_ENABLED_CG                      },
  { "COLL",      EPK_MPI_ENABLED_COLL                    },
  { "DEFAULT",   EPK_MPI_ENABLED_DEFAULT                 },
  { "ENV",       EPK_MPI_ENABLED_ENV                     },
  { "ERR",       EPK_MPI_ENABLED_ERR                     },
  { "EXT",       EPK_MPI_ENABLED_EXT                     },
  { "IO",        EPK_MPI_ENABLED_IO                      },
  { "MISC",      EPK_MPI_ENABLED_MISC                    },
  { "P2P",       EPK_MPI_ENABLED_P2P                     },
  { "PERF",      EPK_MPI_ENABLED_PERF                    },
  { "RMA",       EPK_MPI_ENABLED_RMA                     },
  { "SPAWN",     EPK_MPI_ENABLED_SPAWN                   },
  { "TOPO",      EPK_MPI_ENABLED_TOPO                    },
  { "TYPE",      EPK_MPI_ENABLED_TYPE                    },
  { "XNONBLOCK", EPK_MPI_ENABLED_XNONBLOCK               },
  { "XREQTEST",  EPK_MPI_ENABLED_XREQTEST                }
};

/** Bit vector for runtime measurement wrapper enabling/disabling */
elg_ui4 epk_mpi_enabled = 0;

static int epk_mycmp(const void* v1,
                     const void* v2);

/**
 * Enable measurement for specific subgroups of MPI functions
 */
void epk_mpi_enable_init()
{
  char* token = NULL;
  int   len   = strlen(epk_get(EPK_MPI_ENABLED)) + 1;
  char* conf  = malloc(sizeof (char) * len);

  strncpy(conf, epk_get(EPK_MPI_ENABLED), len);
  token = strtok(conf, ":");
  while (token != NULL)
  {
    /* check if token is a valid flag */
    EpkMPItype* match =
      bsearch(token, (EpkMPItype*)epk_mpi_enable_groups,
              sizeof (epk_mpi_enable_groups) / sizeof (EpkMPItype),
              sizeof (EpkMPItype), epk_mycmp);

    if (match)
    {
      /* enable event generation for function group */
      epk_mpi_enabled |= match->type;
      elg_cntl_msg("Enabled event generation for MPI group: %s",
                   token);
    }
    else
    {
      /* token is not a valid MPI function group */
      elg_warning("Unknown MPI function group ignored: %s", token);
    }
    token = strtok(NULL, ":");
  }
} /* epk_mpi_enable_init */

/**
 * Register MPI functions and initialize data structures
 */
void epk_mpi_register()
{
  elg_ui4 fid = esd_def_file("MPI");

  epk_mpi_enable_init();

  #if defined(HAS_MPI_ABORT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_ABORT] =
      esd_def_region("MPI_Abort",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ACCUMULATE) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_ACCUMULATE] =
      esd_def_region("MPI_Accumulate",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ADD_ERROR_CLASS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ADD_ERROR_CLASS] =
      esd_def_region("MPI_Add_error_class",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ADD_ERROR_CODE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ADD_ERROR_CODE] =
      esd_def_region("MPI_Add_error_code",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ADD_ERROR_STRING) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ADD_ERROR_STRING] =
      esd_def_region("MPI_Add_error_string",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ADDRESS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_ADDRESS] =
      esd_def_region("MPI_Address",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ALLGATHER) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_ALLGATHER] =
      esd_def_region("MPI_Allgather",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_ALLGATHERV) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_ALLGATHERV] =
      esd_def_region("MPI_Allgatherv",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_ALLOC_MEM) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_ALLOC_MEM] =
      esd_def_region("MPI_Alloc_mem",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ALLREDUCE) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_ALLREDUCE] =
      esd_def_region("MPI_Allreduce",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_ALLTOALL) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_ALLTOALL] =
      esd_def_region("MPI_Alltoall",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_ALLTOALLV) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_ALLTOALLV] =
      esd_def_region("MPI_Alltoallv",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_ALLTOALLW) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_ALLTOALLW] =
      esd_def_region("MPI_Alltoallw",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_ATTR_DELETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_ATTR_DELETE] =
      esd_def_region("MPI_Attr_delete",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ATTR_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_ATTR_GET] =
      esd_def_region("MPI_Attr_get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ATTR_PUT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_ATTR_PUT] =
      esd_def_region("MPI_Attr_put",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_BARRIER) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_BARRIER] =
      esd_def_region("MPI_Barrier",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_BARRIER);
  }
  #endif
  #if defined(HAS_MPI_BCAST) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_BCAST] =
      esd_def_region("MPI_Bcast",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ONE2ALL);
  }
  #endif
  #if defined(HAS_MPI_BSEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_BSEND] =
      esd_def_region("MPI_Bsend",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_BSEND_INIT) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_BSEND_INIT] =
      esd_def_region("MPI_Bsend_init",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_BUFFER_ATTACH) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_BUFFER_ATTACH] =
      esd_def_region("MPI_Buffer_attach",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_BUFFER_DETACH) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_BUFFER_DETACH] =
      esd_def_region("MPI_Buffer_detach",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CANCEL) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_CANCEL] =
      esd_def_region("MPI_Cancel",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CART_COORDS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CART_COORDS] =
      esd_def_region("MPI_Cart_coords",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CART_CREATE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CART_CREATE] =
      esd_def_region("MPI_Cart_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CART_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CART_GET] =
      esd_def_region("MPI_Cart_get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CART_MAP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CART_MAP] =
      esd_def_region("MPI_Cart_map",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CART_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CART_RANK] =
      esd_def_region("MPI_Cart_rank",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CART_SHIFT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CART_SHIFT] =
      esd_def_region("MPI_Cart_shift",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CART_SUB)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CART_SUB] =
      esd_def_region("MPI_Cart_sub",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CARTDIM_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_CARTDIM_GET] =
      esd_def_region("MPI_Cartdim_get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_CLOSE_PORT) && !defined(NO_MPI_SPAWN) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_CLOSE_PORT] =
      esd_def_region("MPI_Close_port",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_ACCEPT) && !defined(NO_MPI_SPAWN)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_COMM_ACCEPT] =
      esd_def_region("MPI_Comm_accept",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_MISC)
  {
    epk_mpi_regid[EPK__MPI_COMM_C2F] =
      esd_def_region("MPI_Comm_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_CALL_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_ERR)
  {
    epk_mpi_regid[EPK__MPI_COMM_CALL_ERRHANDLER] =
      esd_def_region("MPI_Comm_call_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_COMPARE] =
      esd_def_region("MPI_Comm_compare",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_CONNECT) && !defined(NO_MPI_SPAWN)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_COMM_CONNECT] =
      esd_def_region("MPI_Comm_connect",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_CREATE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_CREATE] =
      esd_def_region("MPI_Comm_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_CREATE_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_ERR)
  {
    epk_mpi_regid[EPK__MPI_COMM_CREATE_ERRHANDLER] =
      esd_def_region("MPI_Comm_create_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_CREATE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_COMM_CREATE_KEYVAL] =
      esd_def_region("MPI_Comm_create_keyval",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_DELETE_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_COMM_DELETE_ATTR] =
      esd_def_region("MPI_Comm_delete_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_DISCONNECT) && !defined(NO_MPI_SPAWN) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_COMM_DISCONNECT] =
      esd_def_region("MPI_Comm_disconnect",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_DUP)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_DUP] =
      esd_def_region("MPI_Comm_dup",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_MISC)
  {
    epk_mpi_regid[EPK__MPI_COMM_F2C] =
      esd_def_region("MPI_Comm_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_FREE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_FREE] =
      esd_def_region("MPI_Comm_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_FREE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_COMM_FREE_KEYVAL] =
      esd_def_region("MPI_Comm_free_keyval",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_GET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_COMM_GET_ATTR] =
      esd_def_region("MPI_Comm_get_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_GET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_ERR)
  {
    epk_mpi_regid[EPK__MPI_COMM_GET_ERRHANDLER] =
      esd_def_region("MPI_Comm_get_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_GET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_COMM_GET_NAME] =
      esd_def_region("MPI_Comm_get_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_GET_PARENT) && !defined(NO_MPI_SPAWN) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_COMM_GET_PARENT] =
      esd_def_region("MPI_Comm_get_parent",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_GROUP)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_GROUP] =
      esd_def_region("MPI_Comm_group",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_JOIN) && !defined(NO_MPI_SPAWN)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_COMM_JOIN] =
      esd_def_region("MPI_Comm_join",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_RANK] =
      esd_def_region("MPI_Comm_rank",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_REMOTE_GROUP)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_REMOTE_GROUP] =
      esd_def_region("MPI_Comm_remote_group",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_REMOTE_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_REMOTE_SIZE] =
      esd_def_region("MPI_Comm_remote_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_SET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_COMM_SET_ATTR] =
      esd_def_region("MPI_Comm_set_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_SET_ERRHANDLER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_ERR)
  {
    epk_mpi_regid[EPK__MPI_COMM_SET_ERRHANDLER] =
      esd_def_region("MPI_Comm_set_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_SET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_COMM_SET_NAME] =
      esd_def_region("MPI_Comm_set_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_SIZE] =
      esd_def_region("MPI_Comm_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_SPAWN) && !defined(NO_MPI_SPAWN)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_COMM_SPAWN] =
      esd_def_region("MPI_Comm_spawn",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_SPAWN_MULTIPLE) && !defined(NO_MPI_SPAWN)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_COMM_SPAWN_MULTIPLE] =
      esd_def_region("MPI_Comm_spawn_multiple",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_SPLIT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_SPLIT] =
      esd_def_region("MPI_Comm_split",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_COMM_TEST_INTER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_COMM_TEST_INTER] =
      esd_def_region("MPI_Comm_test_inter",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_DIMS_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_DIMS_CREATE] =
      esd_def_region("MPI_Dims_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_DIST_GRAPH_CREATE) && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_DIST_GRAPH_CREATE] =
      esd_def_region("MPI_Dist_graph_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_DIST_GRAPH_CREATE_ADJACENT) && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_DIST_GRAPH_CREATE_ADJACENT] =
      esd_def_region("MPI_Dist_graph_create_adjacent",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_DIST_GRAPH_NEIGHBORS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_DIST_GRAPH_NEIGHBORS] =
      esd_def_region("MPI_Dist_graph_neighbors",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_DIST_GRAPH_NEIGHBORS_COUNT) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_DIST_GRAPH_NEIGHBORS_COUNT] =
      esd_def_region("MPI_Dist_graph_neighbors_count",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ERRHANDLER_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ERRHANDLER_CREATE] =
      esd_def_region("MPI_Errhandler_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ERRHANDLER_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ERRHANDLER_FREE] =
      esd_def_region("MPI_Errhandler_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ERRHANDLER_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ERRHANDLER_GET] =
      esd_def_region("MPI_Errhandler_get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ERRHANDLER_SET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ERRHANDLER_SET] =
      esd_def_region("MPI_Errhandler_set",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ERROR_CLASS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ERROR_CLASS] =
      esd_def_region("MPI_Error_class",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ERROR_STRING) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ERR)
  {
    epk_mpi_regid[EPK__MPI_ERROR_STRING] =
      esd_def_region("MPI_Error_string",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_EXSCAN) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_EXSCAN] =
      esd_def_region("MPI_Exscan",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_OTHER);
  }
  #endif
  #if defined(HAS_MPI_FILE_C2F) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO_MISC)
  {
    epk_mpi_regid[EPK__MPI_FILE_C2F] =
      esd_def_region("MPI_File_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_CALL_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO_ERR)
  {
    epk_mpi_regid[EPK__MPI_FILE_CALL_ERRHANDLER] =
      esd_def_region("MPI_File_call_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_CLOSE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_CLOSE] =
      esd_def_region("MPI_File_close",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_CREATE_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO_ERR)
  {
    epk_mpi_regid[EPK__MPI_FILE_CREATE_ERRHANDLER] =
      esd_def_region("MPI_File_create_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_DELETE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_DELETE] =
      esd_def_region("MPI_File_delete",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_F2C) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO_MISC)
  {
    epk_mpi_regid[EPK__MPI_FILE_F2C] =
      esd_def_region("MPI_File_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_AMODE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_AMODE] =
      esd_def_region("MPI_File_get_amode",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_ATOMICITY) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_ATOMICITY] =
      esd_def_region("MPI_File_get_atomicity",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_BYTE_OFFSET) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_BYTE_OFFSET] =
      esd_def_region("MPI_File_get_byte_offset",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO_ERR)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_ERRHANDLER] =
      esd_def_region("MPI_File_get_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_GROUP) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_GROUP] =
      esd_def_region("MPI_File_get_group",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_INFO) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_INFO] =
      esd_def_region("MPI_File_get_info",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_POSITION) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_POSITION] =
      esd_def_region("MPI_File_get_position",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_POSITION_SHARED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_POSITION_SHARED] =
      esd_def_region("MPI_File_get_position_shared",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_SIZE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_SIZE] =
      esd_def_region("MPI_File_get_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_TYPE_EXTENT) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_TYPE_EXTENT] =
      esd_def_region("MPI_File_get_type_extent",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_GET_VIEW) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_GET_VIEW] =
      esd_def_region("MPI_File_get_view",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_IREAD) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_IREAD] =
      esd_def_region("MPI_File_iread",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_IREAD_AT) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_IREAD_AT] =
      esd_def_region("MPI_File_iread_at",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_IREAD_SHARED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_IREAD_SHARED] =
      esd_def_region("MPI_File_iread_shared",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_IWRITE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_IWRITE] =
      esd_def_region("MPI_File_iwrite",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_IWRITE_AT) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_IWRITE_AT] =
      esd_def_region("MPI_File_iwrite_at",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_IWRITE_SHARED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_IWRITE_SHARED] =
      esd_def_region("MPI_File_iwrite_shared",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_OPEN) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_OPEN] =
      esd_def_region("MPI_File_open",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_PREALLOCATE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_PREALLOCATE] =
      esd_def_region("MPI_File_preallocate",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ] =
      esd_def_region("MPI_File_read",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_ALL) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_ALL] =
      esd_def_region("MPI_File_read_all",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_ALL_BEGIN) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_ALL_BEGIN] =
      esd_def_region("MPI_File_read_all_begin",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_ALL_END) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_ALL_END] =
      esd_def_region("MPI_File_read_all_end",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_AT) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_AT] =
      esd_def_region("MPI_File_read_at",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_AT_ALL) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_AT_ALL] =
      esd_def_region("MPI_File_read_at_all",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_AT_ALL_BEGIN) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_AT_ALL_BEGIN] =
      esd_def_region("MPI_File_read_at_all_begin",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_AT_ALL_END) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_AT_ALL_END] =
      esd_def_region("MPI_File_read_at_all_end",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_ORDERED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_ORDERED] =
      esd_def_region("MPI_File_read_ordered",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_ORDERED_BEGIN) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_ORDERED_BEGIN] =
      esd_def_region("MPI_File_read_ordered_begin",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_ORDERED_END) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_ORDERED_END] =
      esd_def_region("MPI_File_read_ordered_end",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_READ_SHARED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_READ_SHARED] =
      esd_def_region("MPI_File_read_shared",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SEEK) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_SEEK] =
      esd_def_region("MPI_File_seek",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SEEK_SHARED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_SEEK_SHARED] =
      esd_def_region("MPI_File_seek_shared",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SET_ATOMICITY) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_SET_ATOMICITY] =
      esd_def_region("MPI_File_set_atomicity",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SET_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO_ERR)
  {
    epk_mpi_regid[EPK__MPI_FILE_SET_ERRHANDLER] =
      esd_def_region("MPI_File_set_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SET_INFO) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_SET_INFO] =
      esd_def_region("MPI_File_set_info",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SET_SIZE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_SET_SIZE] =
      esd_def_region("MPI_File_set_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SET_VIEW) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_SET_VIEW] =
      esd_def_region("MPI_File_set_view",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_SYNC) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_SYNC] =
      esd_def_region("MPI_File_sync",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE] =
      esd_def_region("MPI_File_write",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_ALL) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_ALL] =
      esd_def_region("MPI_File_write_all",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_ALL_BEGIN) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_ALL_BEGIN] =
      esd_def_region("MPI_File_write_all_begin",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_ALL_END) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_ALL_END] =
      esd_def_region("MPI_File_write_all_end",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_AT) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_AT] =
      esd_def_region("MPI_File_write_at",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_AT_ALL) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_AT_ALL] =
      esd_def_region("MPI_File_write_at_all",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_AT_ALL_BEGIN) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_AT_ALL_BEGIN] =
      esd_def_region("MPI_File_write_at_all_begin",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_AT_ALL_END) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_AT_ALL_END] =
      esd_def_region("MPI_File_write_at_all_end",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_ORDERED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_ORDERED] =
      esd_def_region("MPI_File_write_ordered",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_ORDERED_BEGIN) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_ORDERED_BEGIN] =
      esd_def_region("MPI_File_write_ordered_begin",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_ORDERED_END) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_ORDERED_END] =
      esd_def_region("MPI_File_write_ordered_end",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FILE_WRITE_SHARED) && !defined(NO_MPI_IO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_FILE_WRITE_SHARED] =
      esd_def_region("MPI_File_write_shared",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FINALIZE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ENV)
  {
    epk_mpi_regid[EPK__MPI_FINALIZE] =
      esd_def_region("MPI_Finalize",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FINALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ENV)
  {
    epk_mpi_regid[EPK__MPI_FINALIZED] =
      esd_def_region("MPI_Finalized",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_FREE_MEM) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_FREE_MEM] =
      esd_def_region("MPI_Free_mem",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GATHER) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_GATHER] =
      esd_def_region("MPI_Gather",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ONE);
  }
  #endif
  #if defined(HAS_MPI_GATHERV) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_GATHERV] =
      esd_def_region("MPI_Gatherv",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ONE);
  }
  #endif
  #if defined(HAS_MPI_GET) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_GET] =
      esd_def_region("MPI_Get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GET_ADDRESS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_GET_ADDRESS] =
      esd_def_region("MPI_Get_address",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GET_COUNT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_GET_COUNT] =
      esd_def_region("MPI_Get_count",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GET_ELEMENTS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_GET_ELEMENTS] =
      esd_def_region("MPI_Get_elements",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GET_PROCESSOR_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_GET_PROCESSOR_NAME] =
      esd_def_region("MPI_Get_processor_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GET_VERSION) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_GET_VERSION] =
      esd_def_region("MPI_Get_version",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GRAPH_CREATE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_GRAPH_CREATE] =
      esd_def_region("MPI_Graph_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GRAPH_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_GRAPH_GET] =
      esd_def_region("MPI_Graph_get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GRAPH_MAP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_GRAPH_MAP] =
      esd_def_region("MPI_Graph_map",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GRAPH_NEIGHBORS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_GRAPH_NEIGHBORS] =
      esd_def_region("MPI_Graph_neighbors",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GRAPH_NEIGHBORS_COUNT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_GRAPH_NEIGHBORS_COUNT] =
      esd_def_region("MPI_Graph_neighbors_count",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GRAPHDIMS_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TOPO)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_GRAPHDIMS_GET] =
      esd_def_region("MPI_Graphdims_get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GREQUEST_COMPLETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_GREQUEST_COMPLETE] =
      esd_def_region("MPI_Grequest_complete",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GREQUEST_START) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_GREQUEST_START] =
      esd_def_region("MPI_Grequest_start",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_MISC)
  {
    epk_mpi_regid[EPK__MPI_GROUP_C2F] =
      esd_def_region("MPI_Group_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_COMPARE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_COMPARE] =
      esd_def_region("MPI_Group_compare",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_DIFFERENCE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_DIFFERENCE] =
      esd_def_region("MPI_Group_difference",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_EXCL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_EXCL] =
      esd_def_region("MPI_Group_excl",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_MISC)
  {
    epk_mpi_regid[EPK__MPI_GROUP_F2C] =
      esd_def_region("MPI_Group_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_FREE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_FREE] =
      esd_def_region("MPI_Group_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_INCL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_INCL] =
      esd_def_region("MPI_Group_incl",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_INTERSECTION)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_INTERSECTION] =
      esd_def_region("MPI_Group_intersection",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_RANGE_EXCL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_RANGE_EXCL] =
      esd_def_region("MPI_Group_range_excl",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_RANGE_INCL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_RANGE_INCL] =
      esd_def_region("MPI_Group_range_incl",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_RANK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_RANK] =
      esd_def_region("MPI_Group_rank",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_SIZE] =
      esd_def_region("MPI_Group_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_TRANSLATE_RANKS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_TRANSLATE_RANKS] =
      esd_def_region("MPI_Group_translate_ranks",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_GROUP_UNION)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_GROUP_UNION] =
      esd_def_region("MPI_Group_union",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_IBSEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_IBSEND] =
      esd_def_region("MPI_Ibsend",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_C2F] =
      esd_def_region("MPI_Info_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_CREATE] =
      esd_def_region("MPI_Info_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_DELETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_DELETE] =
      esd_def_region("MPI_Info_delete",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_DUP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_DUP] =
      esd_def_region("MPI_Info_dup",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_F2C] =
      esd_def_region("MPI_Info_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_FREE] =
      esd_def_region("MPI_Info_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_GET] =
      esd_def_region("MPI_Info_get",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_GET_NKEYS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_GET_NKEYS] =
      esd_def_region("MPI_Info_get_nkeys",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_GET_NTHKEY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_GET_NTHKEY] =
      esd_def_region("MPI_Info_get_nthkey",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_GET_VALUELEN) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_GET_VALUELEN] =
      esd_def_region("MPI_Info_get_valuelen",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INFO_SET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_INFO_SET] =
      esd_def_region("MPI_Info_set",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INIT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ENV)
  {
    epk_mpi_regid[EPK__MPI_INIT] =
      esd_def_region("MPI_Init",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INIT_THREAD)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ENV)
  {
    epk_mpi_regid[EPK__MPI_INIT_THREAD] =
      esd_def_region("MPI_Init_thread",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INITIALIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ENV)
  {
    epk_mpi_regid[EPK__MPI_INITIALIZED] =
      esd_def_region("MPI_Initialized",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INTERCOMM_CREATE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_INTERCOMM_CREATE] =
      esd_def_region("MPI_Intercomm_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_INTERCOMM_MERGE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG)
  {
    epk_mpi_regid[EPK__MPI_INTERCOMM_MERGE] =
      esd_def_region("MPI_Intercomm_merge",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_IPROBE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_IPROBE] =
      esd_def_region("MPI_Iprobe",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_IRECV) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_IRECV] =
      esd_def_region("MPI_Irecv",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_IRSEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_IRSEND] =
      esd_def_region("MPI_Irsend",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_IS_THREAD_MAIN) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ENV)
  {
    epk_mpi_regid[EPK__MPI_IS_THREAD_MAIN] =
      esd_def_region("MPI_Is_thread_main",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ISEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_ISEND] =
      esd_def_region("MPI_Isend",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_ISSEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_ISSEND] =
      esd_def_region("MPI_Issend",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_KEYVAL_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_KEYVAL_CREATE] =
      esd_def_region("MPI_Keyval_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_KEYVAL_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_CG) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_CG_EXT)
  {
    epk_mpi_regid[EPK__MPI_KEYVAL_FREE] =
      esd_def_region("MPI_Keyval_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_LOOKUP_NAME) && !defined(NO_MPI_SPAWN) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_LOOKUP_NAME] =
      esd_def_region("MPI_Lookup_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_OP_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_OP_C2F] =
      esd_def_region("MPI_Op_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_OP_COMMUTATIVE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_OP_COMMUTATIVE] =
      esd_def_region("MPI_Op_commutative",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_OP_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_OP_CREATE] =
      esd_def_region("MPI_Op_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_OP_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_OP_F2C] =
      esd_def_region("MPI_Op_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_OP_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_OP_FREE] =
      esd_def_region("MPI_Op_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_OPEN_PORT) && !defined(NO_MPI_SPAWN) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_OPEN_PORT] =
      esd_def_region("MPI_Open_port",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PACK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_PACK] =
      esd_def_region("MPI_Pack",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PACK_EXTERNAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_PACK_EXTERNAL] =
      esd_def_region("MPI_Pack_external",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PACK_EXTERNAL_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_PACK_EXTERNAL_SIZE] =
      esd_def_region("MPI_Pack_external_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PACK_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_PACK_SIZE] =
      esd_def_region("MPI_Pack_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PCONTROL) && !defined(NO_MPI_PERF) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_PERF)
  {
    epk_mpi_regid[EPK__MPI_PCONTROL] =
      esd_def_region("MPI_Pcontrol",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PROBE) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_PROBE] =
      esd_def_region("MPI_Probe",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PUBLISH_NAME) && !defined(NO_MPI_SPAWN) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_PUBLISH_NAME] =
      esd_def_region("MPI_Publish_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_PUT) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_PUT] =
      esd_def_region("MPI_Put",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_QUERY_THREAD) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_ENV)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_ENV)
  {
    epk_mpi_regid[EPK__MPI_QUERY_THREAD] =
      esd_def_region("MPI_Query_thread",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_RECV) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_RECV] =
      esd_def_region("MPI_Recv",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_RECV_INIT) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_RECV_INIT] =
      esd_def_region("MPI_Recv_init",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_REDUCE) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_REDUCE] =
      esd_def_region("MPI_Reduce",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ONE);
  }
  #endif
  #if defined(HAS_MPI_REDUCE_LOCAL) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_REDUCE_LOCAL] =
      esd_def_region("MPI_Reduce_local",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_REDUCE_SCATTER) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_REDUCE_SCATTER] =
      esd_def_region("MPI_Reduce_scatter",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_REDUCE_SCATTER_BLOCK) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_REDUCE_SCATTER_BLOCK] =
      esd_def_region("MPI_Reduce_scatter_block",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ALL2ALL);
  }
  #endif
  #if defined(HAS_MPI_REGISTER_DATAREP) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_IO)
  {
    epk_mpi_regid[EPK__MPI_REGISTER_DATAREP] =
      esd_def_region("MPI_Register_datarep",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_REQUEST_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_REQUEST_C2F] =
      esd_def_region("MPI_Request_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_REQUEST_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_REQUEST_F2C] =
      esd_def_region("MPI_Request_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_REQUEST_FREE) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_REQUEST_FREE] =
      esd_def_region("MPI_Request_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_REQUEST_GET_STATUS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_REQUEST_GET_STATUS] =
      esd_def_region("MPI_Request_get_status",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_RSEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_RSEND] =
      esd_def_region("MPI_Rsend",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_RSEND_INIT) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_RSEND_INIT] =
      esd_def_region("MPI_Rsend_init",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_SCAN) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_SCAN] =
      esd_def_region("MPI_Scan",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_OTHER);
  }
  #endif
  #if defined(HAS_MPI_SCATTER) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_SCATTER] =
      esd_def_region("MPI_Scatter",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ONE2ALL);
  }
  #endif
  #if defined(HAS_MPI_SCATTERV) && !defined(NO_MPI_COLL)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_COLL)
  {
    epk_mpi_regid[EPK__MPI_SCATTERV] =
      esd_def_region("MPI_Scatterv",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION_COLL_ONE2ALL);
  }
  #endif
  #if defined(HAS_MPI_SEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_SEND] =
      esd_def_region("MPI_Send",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_SEND_INIT) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_SEND_INIT] =
      esd_def_region("MPI_Send_init",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_SENDRECV) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_SENDRECV] =
      esd_def_region("MPI_Sendrecv",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_SENDRECV_REPLACE) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_SENDRECV_REPLACE] =
      esd_def_region("MPI_Sendrecv_replace",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_SIZEOF) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_SIZEOF] =
      esd_def_region("MPI_Sizeof",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_SSEND) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_SSEND] =
      esd_def_region("MPI_Ssend",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_SSEND_INIT) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_SSEND_INIT] =
      esd_def_region("MPI_Ssend_init",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_START) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_START] =
      esd_def_region("MPI_Start",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_STARTALL) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_STARTALL] =
      esd_def_region("MPI_Startall",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_STATUS_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_STATUS_C2F] =
      esd_def_region("MPI_Status_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_STATUS_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_MISC)
  {
    epk_mpi_regid[EPK__MPI_STATUS_F2C] =
      esd_def_region("MPI_Status_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_STATUS_SET_CANCELLED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_STATUS_SET_CANCELLED] =
      esd_def_region("MPI_Status_set_cancelled",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_STATUS_SET_ELEMENTS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_STATUS_SET_ELEMENTS] =
      esd_def_region("MPI_Status_set_elements",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TEST) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_TEST] =
      esd_def_region("MPI_Test",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TEST_CANCELLED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_TEST_CANCELLED] =
      esd_def_region("MPI_Test_cancelled",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TESTALL) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_TESTALL] =
      esd_def_region("MPI_Testall",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TESTANY) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_TESTANY] =
      esd_def_region("MPI_Testany",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TESTSOME) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_TESTSOME] =
      esd_def_region("MPI_Testsome",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TOPO_TEST) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TOPO)
  {
    epk_mpi_regid[EPK__MPI_TOPO_TEST] =
      esd_def_region("MPI_Topo_test",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_C2F) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_MISC)
  {
    epk_mpi_regid[EPK__MPI_TYPE_C2F] =
      esd_def_region("MPI_Type_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_COMMIT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_COMMIT] =
      esd_def_region("MPI_Type_commit",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CONTIGUOUS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CONTIGUOUS] =
      esd_def_region("MPI_Type_contiguous",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_DARRAY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_DARRAY] =
      esd_def_region("MPI_Type_create_darray",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_F90_COMPLEX) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_F90_COMPLEX] =
      esd_def_region("MPI_Type_create_f90_complex",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_F90_INTEGER) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_F90_INTEGER] =
      esd_def_region("MPI_Type_create_f90_integer",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_F90_REAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_F90_REAL] =
      esd_def_region("MPI_Type_create_f90_real",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_HINDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_HINDEXED] =
      esd_def_region("MPI_Type_create_hindexed",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_HVECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_HVECTOR] =
      esd_def_region("MPI_Type_create_hvector",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_INDEXED_BLOCK) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_INDEXED_BLOCK] =
      esd_def_region("MPI_Type_create_indexed_block",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_EXT)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_KEYVAL] =
      esd_def_region("MPI_Type_create_keyval",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_RESIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_RESIZED] =
      esd_def_region("MPI_Type_create_resized",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_STRUCT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_STRUCT] =
      esd_def_region("MPI_Type_create_struct",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_CREATE_SUBARRAY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_CREATE_SUBARRAY] =
      esd_def_region("MPI_Type_create_subarray",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_DELETE_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_EXT)
  {
    epk_mpi_regid[EPK__MPI_TYPE_DELETE_ATTR] =
      esd_def_region("MPI_Type_delete_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_DUP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_DUP] =
      esd_def_region("MPI_Type_dup",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_EXTENT] =
      esd_def_region("MPI_Type_extent",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_F2C) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_MISC)
  {
    epk_mpi_regid[EPK__MPI_TYPE_F2C] =
      esd_def_region("MPI_Type_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_FREE] =
      esd_def_region("MPI_Type_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_FREE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_EXT)
  {
    epk_mpi_regid[EPK__MPI_TYPE_FREE_KEYVAL] =
      esd_def_region("MPI_Type_free_keyval",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_GET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_EXT)
  {
    epk_mpi_regid[EPK__MPI_TYPE_GET_ATTR] =
      esd_def_region("MPI_Type_get_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_GET_CONTENTS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_GET_CONTENTS] =
      esd_def_region("MPI_Type_get_contents",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_GET_ENVELOPE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_GET_ENVELOPE] =
      esd_def_region("MPI_Type_get_envelope",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_GET_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_GET_EXTENT] =
      esd_def_region("MPI_Type_get_extent",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_GET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_EXT)
  {
    epk_mpi_regid[EPK__MPI_TYPE_GET_NAME] =
      esd_def_region("MPI_Type_get_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_GET_TRUE_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_GET_TRUE_EXTENT] =
      esd_def_region("MPI_Type_get_true_extent",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_HINDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_HINDEXED] =
      esd_def_region("MPI_Type_hindexed",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_HVECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_HVECTOR] =
      esd_def_region("MPI_Type_hvector",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_INDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_INDEXED] =
      esd_def_region("MPI_Type_indexed",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_LB) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_LB] =
      esd_def_region("MPI_Type_lb",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_MATCH_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_MATCH_SIZE] =
      esd_def_region("MPI_Type_match_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_SET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_EXT)
  {
    epk_mpi_regid[EPK__MPI_TYPE_SET_ATTR] =
      esd_def_region("MPI_Type_set_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_SET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE_EXT)
  {
    epk_mpi_regid[EPK__MPI_TYPE_SET_NAME] =
      esd_def_region("MPI_Type_set_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_SIZE] =
      esd_def_region("MPI_Type_size",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_STRUCT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_STRUCT] =
      esd_def_region("MPI_Type_struct",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_UB) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_UB] =
      esd_def_region("MPI_Type_ub",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_TYPE_VECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_TYPE_VECTOR] =
      esd_def_region("MPI_Type_vector",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_UNPACK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_UNPACK] =
      esd_def_region("MPI_Unpack",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_UNPACK_EXTERNAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_TYPE)
  {
    epk_mpi_regid[EPK__MPI_UNPACK_EXTERNAL] =
      esd_def_region("MPI_Unpack_external",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_UNPUBLISH_NAME) && !defined(NO_MPI_SPAWN) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_SPAWN)
  {
    epk_mpi_regid[EPK__MPI_UNPUBLISH_NAME] =
      esd_def_region("MPI_Unpublish_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WAIT) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_WAIT] =
      esd_def_region("MPI_Wait",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WAITALL) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_WAITALL] =
      esd_def_region("MPI_Waitall",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WAITANY) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_WAITANY] =
      esd_def_region("MPI_Waitany",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WAITSOME) && !defined(NO_MPI_P2P)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_P2P)
  {
    epk_mpi_regid[EPK__MPI_WAITSOME] =
      esd_def_region("MPI_Waitsome",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_C2F) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_MISC)
  {
    epk_mpi_regid[EPK__MPI_WIN_C2F] =
      esd_def_region("MPI_Win_c2f",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_CALL_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_ERR)
  {
    epk_mpi_regid[EPK__MPI_WIN_CALL_ERRHANDLER] =
      esd_def_region("MPI_Win_call_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_COMPLETE) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_COMPLETE] =
      esd_def_region("MPI_Win_complete",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_CREATE) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_CREATE] =
      esd_def_region("MPI_Win_create",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_CREATE_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_ERR)
  {
    epk_mpi_regid[EPK__MPI_WIN_CREATE_ERRHANDLER] =
      esd_def_region("MPI_Win_create_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_CREATE_KEYVAL) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_EXT)
  {
    epk_mpi_regid[EPK__MPI_WIN_CREATE_KEYVAL] =
      esd_def_region("MPI_Win_create_keyval",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_DELETE_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_EXT)
  {
    epk_mpi_regid[EPK__MPI_WIN_DELETE_ATTR] =
      esd_def_region("MPI_Win_delete_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_F2C) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_MISC)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_MISC)
  {
    epk_mpi_regid[EPK__MPI_WIN_F2C] =
      esd_def_region("MPI_Win_f2c",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_FENCE) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_FENCE] =
      esd_def_region("MPI_Win_fence",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_FREE) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_FREE] =
      esd_def_region("MPI_Win_free",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_FREE_KEYVAL) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_EXT)
  {
    epk_mpi_regid[EPK__MPI_WIN_FREE_KEYVAL] =
      esd_def_region("MPI_Win_free_keyval",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_GET_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_EXT)
  {
    epk_mpi_regid[EPK__MPI_WIN_GET_ATTR] =
      esd_def_region("MPI_Win_get_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_GET_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_ERR)
  {
    epk_mpi_regid[EPK__MPI_WIN_GET_ERRHANDLER] =
      esd_def_region("MPI_Win_get_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_GET_GROUP) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_GET_GROUP] =
      esd_def_region("MPI_Win_get_group",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_GET_NAME) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_EXT)
  {
    epk_mpi_regid[EPK__MPI_WIN_GET_NAME] =
      esd_def_region("MPI_Win_get_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_LOCK) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_LOCK] =
      esd_def_region("MPI_Win_lock",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_POST) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_POST] =
      esd_def_region("MPI_Win_post",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_SET_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_EXT)
  {
    epk_mpi_regid[EPK__MPI_WIN_SET_ATTR] =
      esd_def_region("MPI_Win_set_attr",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_SET_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_ERR)
  {
    epk_mpi_regid[EPK__MPI_WIN_SET_ERRHANDLER] =
      esd_def_region("MPI_Win_set_errhandler",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_SET_NAME) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA_EXT)
  {
    epk_mpi_regid[EPK__MPI_WIN_SET_NAME] =
      esd_def_region("MPI_Win_set_name",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_START) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_START] =
      esd_def_region("MPI_Win_start",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_TEST) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_TEST] =
      esd_def_region("MPI_Win_test",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_UNLOCK) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_UNLOCK] =
      esd_def_region("MPI_Win_unlock",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WIN_WAIT) && !defined(NO_MPI_RMA)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_RMA)
  {
    epk_mpi_regid[EPK__MPI_WIN_WAIT] =
      esd_def_region("MPI_Win_wait",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WTICK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_WTICK] =
      esd_def_region("MPI_Wtick",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif
  #if defined(HAS_MPI_WTIME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MINI) && !defined(NO_MPI_EXT)
  if (epk_mpi_enabled & EPK_MPI_ENABLED_EXT)
  {
    epk_mpi_regid[EPK__MPI_WTIME] =
      esd_def_region("MPI_Wtime",
                     fid,
                     ELG_NO_LNO,
                     ELG_NO_LNO,
                     "MPI",
                     ELG_FUNCTION);
  }
  #endif

  /** Artificial root for MPI-only experiments when no user-code
   * instrumenation is available */
  epk_mpi_regid[EPK_PARALLEL__MPI] =
    esd_def_region("PARALLEL",
                   ELG_NO_ID,
                   ELG_NO_LNO,
                   ELG_NO_LNO,
                   "EPIK",
                   ELG_UNKNOWN);
} /* epk_mpi_register */

/**
 * Compare function for binary search
 * @param v1 string to checked
 * @param v2 pointer to \p EpkMPItype structure
 * @return result is equivalent to \p strcasecmp result of two strings
 */
static int epk_mycmp(const void* v1,
                     const void* v2)
{
  return strcasecmp((char*)v1, ((EpkMPItype*)v2)->name);
}

/**
 * Check if region is flagged \a collective
 * @param str Region name
 * @return type of function if collective, 0 otherwise.
 */
elg_ui1 epk_is_mpi_collective(const char* str)
{
  EpkMPItype* match =
    bsearch(str,
            (EpkMPItype*)epk_mpi_colls,
            sizeof (epk_mpi_colls) / sizeof (EpkMPItype),
            sizeof (EpkMPItype),
            epk_mycmp);

  return match ? match->type : 0;
}

/**
 * Check if region is flagged \a point-to-point
 * @param str Region name
 * @return type of function if point-to-point, 0 otherwise.
 */
elg_ui1 epk_is_mpi_point2point(const char* str)
{
  EpkMPItype* match =
    bsearch(str,
            (EpkMPItype*)epk_mpi_pt2pt,
            sizeof (epk_mpi_pt2pt) / sizeof (EpkMPItype),
            sizeof (EpkMPItype),
            epk_mycmp);

  return match ? match->type : EPK_MPI_TYPE__NONE;
}

/**
 * Check if region is flagged either \a point-to-point or \a collective
 * @param  str Region name
 * @return type of function, if point-to-point or collective. 0,
 *         otherwise.
 */
elg_ui1 epk_mpi_eventtype(const char* str)
{
  elg_ui1 type = epk_is_mpi_point2point(str);

  if (type != EPK_MPI_TYPE__NONE)
  {
    return type;
  }

  type = (elg_ui1)epk_is_mpi_collective(str);
  switch (type)
  {
  case EPK_COLL_TYPE__BARRIER:
  case EPK_COLL_TYPE__ONE2ALL:
  case EPK_COLL_TYPE__ALL2ONE:
  case EPK_COLL_TYPE__ALL2ALL:
  case EPK_COLL_TYPE__PARTIAL:
    return EPK_MPI_TYPE__COLLECTIVE;
  default:
    break;
  }
  return EPK_MPI_TYPE__NONE;
}
