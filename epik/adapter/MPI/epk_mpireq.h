/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  epk_mpireg.h
 *
 * @brief Request handling for MPI adapter
 */

#ifndef _EPK_MPIREQ_H
#define _EPK_MPIREQ_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"
#include "mpi.h"

/* 
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - MPI request management
 *
 *-----------------------------------------------------------------------------
 */

enum EpkReqFlags {
  ERF_NONE          = 0x00,
  ERF_SEND          = 0x01,
  ERF_RECV          = 0x02,
  ERF_IS_PERSISTENT = 0x10,
  ERF_DEALLOCATE    = 0x20,
  ERF_IS_ACTIVE     = 0x40,
  ERF_ANY_TAG       = 0x80,
  ERF_ANY_SRC       = 0x100,
  ERF_CAN_CANCEL    = 0x200
};

struct EpkRequest {
  MPI_Request request;
  unsigned flags;
  int tag;
  int dest;
  int bytes;
  MPI_Datatype datatype;
  MPI_Comm comm;
  elg_ui4 id;
};

EXTERN void epk_request_finalize();
EXTERN elg_ui4 epk_get_request_id(); 
EXTERN void epk_request_create( MPI_Request request, 
                                unsigned flags,
                                int tag, 
                                int dest, 
                                int bytes,
                                MPI_Datatype datatype,
                                MPI_Comm comm,
                                elg_ui4 id);
EXTERN struct EpkRequest* epk_request_get(MPI_Request request);
EXTERN void epk_request_free(struct EpkRequest* req);
EXTERN void epk_check_request(struct EpkRequest* req, MPI_Status *status);
EXTERN void epk_save_request_array(MPI_Request *arr_req, int arr_req_size);
EXTERN struct EpkRequest* epk_saved_request_get(int i);

#endif
