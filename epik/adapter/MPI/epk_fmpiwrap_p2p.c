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
 * @file  epk_fmpiwrap_p2p.c
 *
 * @brief Fortran interface wrappers for point-to-point communication
 */

#include <stdlib.h>
#include <mpi.h>

#include "elg_error.h"
#include "elg_fmpi_defs.h"
#include "epk_fmpiwrap.h"
#include "epk_defs_mpi.h"
#include "epk_fwrapper_def.h"

/* uppercase defines */
/** All uppercase name of MPI_Bsend */
#define MPI_Bsend_U MPI_BSEND
/** All uppercase name of MPI_Bsend_init */
#define MPI_Bsend_init_U MPI_BSEND_INIT
/** All uppercase name of MPI_Buffer_attach */
#define MPI_Buffer_attach_U MPI_BUFFER_ATTACH
/** All uppercase name of MPI_Buffer_detach */
#define MPI_Buffer_detach_U MPI_BUFFER_DETACH
/** All uppercase name of MPI_Cancel */
#define MPI_Cancel_U MPI_CANCEL
/** All uppercase name of MPI_Ibsend */
#define MPI_Ibsend_U MPI_IBSEND
/** All uppercase name of MPI_Iprobe */
#define MPI_Iprobe_U MPI_IPROBE
/** All uppercase name of MPI_Irecv */
#define MPI_Irecv_U MPI_IRECV
/** All uppercase name of MPI_Irsend */
#define MPI_Irsend_U MPI_IRSEND
/** All uppercase name of MPI_Isend */
#define MPI_Isend_U MPI_ISEND
/** All uppercase name of MPI_Issend */
#define MPI_Issend_U MPI_ISSEND
/** All uppercase name of MPI_Probe */
#define MPI_Probe_U MPI_PROBE
/** All uppercase name of MPI_Recv */
#define MPI_Recv_U MPI_RECV
/** All uppercase name of MPI_Recv_init */
#define MPI_Recv_init_U MPI_RECV_INIT
/** All uppercase name of MPI_Request_free */
#define MPI_Request_free_U MPI_REQUEST_FREE
/** All uppercase name of MPI_Rsend */
#define MPI_Rsend_U MPI_RSEND
/** All uppercase name of MPI_Rsend_init */
#define MPI_Rsend_init_U MPI_RSEND_INIT
/** All uppercase name of MPI_Send */
#define MPI_Send_U MPI_SEND
/** All uppercase name of MPI_Send_init */
#define MPI_Send_init_U MPI_SEND_INIT
/** All uppercase name of MPI_Sendrecv */
#define MPI_Sendrecv_U MPI_SENDRECV
/** All uppercase name of MPI_Sendrecv_replace */
#define MPI_Sendrecv_replace_U MPI_SENDRECV_REPLACE
/** All uppercase name of MPI_Ssend */
#define MPI_Ssend_U MPI_SSEND
/** All uppercase name of MPI_Ssend_init */
#define MPI_Ssend_init_U MPI_SSEND_INIT
/** All uppercase name of MPI_Start */
#define MPI_Start_U MPI_START
/** All uppercase name of MPI_Startall */
#define MPI_Startall_U MPI_STARTALL
/** All uppercase name of MPI_Test */
#define MPI_Test_U MPI_TEST
/** All uppercase name of MPI_Test_cancelled */
#define MPI_Test_cancelled_U MPI_TEST_CANCELLED
/** All uppercase name of MPI_Testall */
#define MPI_Testall_U MPI_TESTALL
/** All uppercase name of MPI_Testany */
#define MPI_Testany_U MPI_TESTANY
/** All uppercase name of MPI_Testsome */
#define MPI_Testsome_U MPI_TESTSOME
/** All uppercase name of MPI_Wait */
#define MPI_Wait_U MPI_WAIT
/** All uppercase name of MPI_Waitall */
#define MPI_Waitall_U MPI_WAITALL
/** All uppercase name of MPI_Waitany */
#define MPI_Waitany_U MPI_WAITANY
/** All uppercase name of MPI_Waitsome */
#define MPI_Waitsome_U MPI_WAITSOME

/* lowercase defines */
/** All lowercase name of MPI_Bsend */
#define MPI_Bsend_L mpi_bsend
/** All lowercase name of MPI_Bsend_init */
#define MPI_Bsend_init_L mpi_bsend_init
/** All lowercase name of MPI_Buffer_attach */
#define MPI_Buffer_attach_L mpi_buffer_attach
/** All lowercase name of MPI_Buffer_detach */
#define MPI_Buffer_detach_L mpi_buffer_detach
/** All lowercase name of MPI_Cancel */
#define MPI_Cancel_L mpi_cancel
/** All lowercase name of MPI_Ibsend */
#define MPI_Ibsend_L mpi_ibsend
/** All lowercase name of MPI_Iprobe */
#define MPI_Iprobe_L mpi_iprobe
/** All lowercase name of MPI_Irecv */
#define MPI_Irecv_L mpi_irecv
/** All lowercase name of MPI_Irsend */
#define MPI_Irsend_L mpi_irsend
/** All lowercase name of MPI_Isend */
#define MPI_Isend_L mpi_isend
/** All lowercase name of MPI_Issend */
#define MPI_Issend_L mpi_issend
/** All lowercase name of MPI_Probe */
#define MPI_Probe_L mpi_probe
/** All lowercase name of MPI_Recv */
#define MPI_Recv_L mpi_recv
/** All lowercase name of MPI_Recv_init */
#define MPI_Recv_init_L mpi_recv_init
/** All lowercase name of MPI_Request_free */
#define MPI_Request_free_L mpi_request_free
/** All lowercase name of MPI_Rsend */
#define MPI_Rsend_L mpi_rsend
/** All lowercase name of MPI_Rsend_init */
#define MPI_Rsend_init_L mpi_rsend_init
/** All lowercase name of MPI_Send */
#define MPI_Send_L mpi_send
/** All lowercase name of MPI_Send_init */
#define MPI_Send_init_L mpi_send_init
/** All lowercase name of MPI_Sendrecv */
#define MPI_Sendrecv_L mpi_sendrecv
/** All lowercase name of MPI_Sendrecv_replace */
#define MPI_Sendrecv_replace_L mpi_sendrecv_replace
/** All lowercase name of MPI_Ssend */
#define MPI_Ssend_L mpi_ssend
/** All lowercase name of MPI_Ssend_init */
#define MPI_Ssend_init_L mpi_ssend_init
/** All lowercase name of MPI_Start */
#define MPI_Start_L mpi_start
/** All lowercase name of MPI_Startall */
#define MPI_Startall_L mpi_startall
/** All lowercase name of MPI_Test */
#define MPI_Test_L mpi_test
/** All lowercase name of MPI_Test_cancelled */
#define MPI_Test_cancelled_L mpi_test_cancelled
/** All lowercase name of MPI_Testall */
#define MPI_Testall_L mpi_testall
/** All lowercase name of MPI_Testany */
#define MPI_Testany_L mpi_testany
/** All lowercase name of MPI_Testsome */
#define MPI_Testsome_L mpi_testsome
/** All lowercase name of MPI_Wait */
#define MPI_Wait_L mpi_wait
/** All lowercase name of MPI_Waitall */
#define MPI_Waitall_L mpi_waitall
/** All lowercase name of MPI_Waitany */
#define MPI_Waitany_L mpi_waitany
/** All lowercase name of MPI_Waitsome */
#define MPI_Waitsome_L mpi_waitsome

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if defined(HAS_MPI_WAITANY) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitany
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void FSUB(MPI_Waitany)(int*         count,
                       MPI_Request* array,
                       int*         index,
                       MPI_Status*  status,
                       int*         ierr)
{
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif
  *ierr = MPI_Waitany(*count, array, index, status);

  if ((*index != MPI_UNDEFINED) && (*index >= 0))
  {
    (*index)++;
  }
}

#endif

#if defined(HAS_MPI_WAITSOME) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitsome
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void FSUB(MPI_Waitsome)(int*         incount,
                        MPI_Request* array_of_requests,
                        int*         outcount,
                        int*         array_of_indices,
                        MPI_Status*  array_of_statuses,
                        int*         ierr)
{
  int i;

  #if defined(HAS_MPI_STATUSES_IGNORE)
  if (array_of_statuses == epk_mpif_statuses_ignore)
  {
    array_of_statuses = MPI_STATUSES_IGNORE;
  }
  #endif
  *ierr = MPI_Waitsome(*incount, array_of_requests, outcount,
                       array_of_indices, array_of_statuses);

  if (*outcount != MPI_UNDEFINED)
  {
    for (i = 0; i < *outcount; i++)
    {
      if (array_of_indices[i] >= 0)
      {
        array_of_indices[i]++;
      }
    }
  }
}

#endif

#if defined(HAS_MPI_TESTANY) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testany
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void FSUB(MPI_Testany)(int*         count,
                       MPI_Request* array_of_requests,
                       int*         index,
                       int*         flag,
                       MPI_Status*  status,
                       int*         ierr)
{
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif
  *ierr = MPI_Testany(*count, array_of_requests, index, flag, status);

  /* convert index to Fortran */
  if ((*ierr == MPI_SUCCESS)
      && *flag
      && (*index != MPI_UNDEFINED)
      && (*index >= 0))
  {
    (*index)++;
  }
}

#endif

#if defined(HAS_MPI_TESTSOME) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testsome
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void FSUB(MPI_Testsome)(int*         incount,
                        MPI_Request* array_of_requests,
                        int*         outcount,
                        int*         array_of_indices,
                        MPI_Status*  array_of_statuses,
                        int*         ierr)
{
  int i;

  #if defined(HAS_MPI_STATUSES_IGNORE)
  if (array_of_statuses == epk_mpif_statuses_ignore)
  {
    array_of_statuses = MPI_STATUSES_IGNORE;
  }
  #endif

  *ierr = MPI_Testsome(*incount,
                       array_of_requests,
                       outcount,
                       array_of_indices,
                       array_of_statuses);

  /* convert indices to Fortran */
  if ((*ierr == MPI_SUCCESS) && (*outcount != MPI_UNDEFINED))
  {
    for (i = 0; i < *outcount; i++)
    {
      if (array_of_indices[i] >= 0)
      {
        array_of_indices[i]++;
      }
    }
  }
}

#endif

#if defined(HAS_MPI_BSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Bsend
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Bsend)(void*         buf,
                     int*          count,
                     MPI_Datatype* datatype,
                     int*          dest,
                     int*          tag,
                     MPI_Comm*     comm,
                     int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Bsend(buf, *count, *datatype, *dest, *tag, *comm);
}

#endif
#if defined(HAS_MPI_BSEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Bsend_init
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Bsend_init)(void*         buf,
                          int*          count,
                          MPI_Datatype* datatype,
                          int*          dest,
                          int*          tag,
                          MPI_Comm*     comm,
                          MPI_Request*  request,
                          int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Bsend_init(buf,
                         *count,
                         *datatype,
                         *dest,
                         *tag,
                         *comm,
                         request);
}

#endif
#if defined(HAS_MPI_BUFFER_ATTACH) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Buffer_attach
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Buffer_attach)(void* buffer,
                             int*  size,
                             int*  ierr)
{
  *ierr = MPI_Buffer_attach(buffer, *size);
}

#endif
#if defined(HAS_MPI_BUFFER_DETACH) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Buffer_detach
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Buffer_detach)(void* buffer,
                             int*  size,
                             int*  ierr)
{
  *ierr = MPI_Buffer_detach(buffer, size);
}

#endif
#if defined(HAS_MPI_CANCEL) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Cancel
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Cancel)(MPI_Request* request,
                      int*         ierr)
{
  *ierr = MPI_Cancel(request);
}

#endif
#if defined(HAS_MPI_IBSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Ibsend
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Ibsend)(void*         buf,
                      int*          count,
                      MPI_Datatype* datatype,
                      int*          dest,
                      int*          tag,
                      MPI_Comm*     comm,
                      MPI_Request*  request,
                      int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Ibsend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

#endif
#if defined(HAS_MPI_IPROBE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Iprobe
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Iprobe)(int*        source,
                      int*        tag,
                      MPI_Comm*   comm,
                      int*        flag,
                      MPI_Status* status,
                      int*        ierr)
{
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Iprobe(*source, *tag, *comm, flag, status);
}

#endif
#if defined(HAS_MPI_IRECV) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Irecv
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Irecv)(void*         buf,
                     int*          count,
                     MPI_Datatype* datatype,
                     int*          source,
                     int*          tag,
                     MPI_Comm*     comm,
                     MPI_Request*  request,
                     int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Irecv(buf, *count, *datatype, *source, *tag, *comm, request);
}

#endif
#if defined(HAS_MPI_IRSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Irsend
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Irsend)(void*         buf,
                      int*          count,
                      MPI_Datatype* datatype,
                      int*          dest,
                      int*          tag,
                      MPI_Comm*     comm,
                      MPI_Request*  request,
                      int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Irsend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

#endif
#if defined(HAS_MPI_ISEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Isend
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Isend)(void*         buf,
                     int*          count,
                     MPI_Datatype* datatype,
                     int*          dest,
                     int*          tag,
                     MPI_Comm*     comm,
                     MPI_Request*  request,
                     int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Isend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

#endif
#if defined(HAS_MPI_ISSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Issend
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Issend)(void*         buf,
                      int*          count,
                      MPI_Datatype* datatype,
                      int*          dest,
                      int*          tag,
                      MPI_Comm*     comm,
                      MPI_Request*  request,
                      int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Issend(buf, *count, *datatype, *dest, *tag, *comm, request);
}

#endif
#if defined(HAS_MPI_PROBE) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Probe
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Probe)(int*        source,
                     int*        tag,
                     MPI_Comm*   comm,
                     MPI_Status* status,
                     int*        ierr)
{
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Probe(*source, *tag, *comm, status);
}

#endif
#if defined(HAS_MPI_RECV) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Recv
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Recv)(void*         buf,
                    int*          count,
                    MPI_Datatype* datatype,
                    int*          source,
                    int*          tag,
                    MPI_Comm*     comm,
                    MPI_Status*   status,
                    int*          ierr)
{
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Recv(buf, *count, *datatype, *source, *tag, *comm, status);
}

#endif
#if defined(HAS_MPI_RECV_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Recv_init
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Recv_init)(void*         buf,
                         int*          count,
                         MPI_Datatype* datatype,
                         int*          source,
                         int*          tag,
                         MPI_Comm*     comm,
                         MPI_Request*  request,
                         int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Recv_init(buf,
                        *count,
                        *datatype,
                        *source,
                        *tag,
                        *comm,
                        request);
}

#endif
#if defined(HAS_MPI_REQUEST_FREE) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Request_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Request_free)(MPI_Request* request,
                            int*         ierr)
{
  *ierr = MPI_Request_free(request);
}

#endif
#if defined(HAS_MPI_RSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Rsend
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Rsend)(void*         buf,
                     int*          count,
                     MPI_Datatype* datatype,
                     int*          dest,
                     int*          tag,
                     MPI_Comm*     comm,
                     int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Rsend(buf, *count, *datatype, *dest, *tag, *comm);
}

#endif
#if defined(HAS_MPI_RSEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Rsend_init
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Rsend_init)(void*         buf,
                          int*          count,
                          MPI_Datatype* datatype,
                          int*          dest,
                          int*          tag,
                          MPI_Comm*     comm,
                          MPI_Request*  request,
                          int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Rsend_init(buf,
                         *count,
                         *datatype,
                         *dest,
                         *tag,
                         *comm,
                         request);
}

#endif
#if defined(HAS_MPI_SEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Send
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Send)(void*         buf,
                    int*          count,
                    MPI_Datatype* datatype,
                    int*          dest,
                    int*          tag,
                    MPI_Comm*     comm,
                    int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Send(buf, *count, *datatype, *dest, *tag, *comm);
}

#endif
#if defined(HAS_MPI_SEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Send_init
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Send_init)(void*         buf,
                         int*          count,
                         MPI_Datatype* datatype,
                         int*          dest,
                         int*          tag,
                         MPI_Comm*     comm,
                         MPI_Request*  request,
                         int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Send_init(buf,
                        *count,
                        *datatype,
                        *dest,
                        *tag,
                        *comm,
                        request);
}

#endif
#if defined(HAS_MPI_SENDRECV) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Sendrecv
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Sendrecv)(void*         sendbuf,
                        int*          sendcount,
                        MPI_Datatype* sendtype,
                        int*          dest,
                        int*          sendtag,
                        void*         recvbuf,
                        int*          recvcount,
                        MPI_Datatype* recvtype,
                        int*          source,
                        int*          recvtag,
                        MPI_Comm*     comm,
                        MPI_Status*   status,
                        int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (sendbuf == epk_mpif_bottom)
  {
    sendbuf = MPI_BOTTOM;
  }
  #endif
  #if defined(HAS_MPI_BOTTOM)
  if (recvbuf == epk_mpif_bottom)
  {
    recvbuf = MPI_BOTTOM;
  }
  #endif
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Sendrecv(sendbuf,
                       *sendcount,
                       *sendtype,
                       *dest,
                       *sendtag,
                       recvbuf,
                       *recvcount,
                       *recvtype,
                       *source,
                       *recvtag,
                       *comm,
                       status);
}

#endif
#if defined(HAS_MPI_SENDRECV_REPLACE) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Sendrecv_replace
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Sendrecv_replace)(void*         buf,
                                int*          count,
                                MPI_Datatype* datatype,
                                int*          dest,
                                int*          sendtag,
                                int*          source,
                                int*          recvtag,
                                MPI_Comm*     comm,
                                MPI_Status*   status,
                                int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Sendrecv_replace(buf,
                               *count,
                               *datatype,
                               *dest,
                               *sendtag,
                               *source,
                               *recvtag,
                               *comm,
                               status);
}

#endif
#if defined(HAS_MPI_SSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Ssend
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Ssend)(void*         buf,
                     int*          count,
                     MPI_Datatype* datatype,
                     int*          dest,
                     int*          tag,
                     MPI_Comm*     comm,
                     int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Ssend(buf, *count, *datatype, *dest, *tag, *comm);
}

#endif
#if defined(HAS_MPI_SSEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Ssend_init
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Ssend_init)(void*         buf,
                          int*          count,
                          MPI_Datatype* datatype,
                          int*          dest,
                          int*          tag,
                          MPI_Comm*     comm,
                          MPI_Request*  request,
                          int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Ssend_init(buf,
                         *count,
                         *datatype,
                         *dest,
                         *tag,
                         *comm,
                         request);
}

#endif
#if defined(HAS_MPI_START) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Start)(MPI_Request* request,
                     int*         ierr)
{
  *ierr = MPI_Start(request);
}

#endif
#if defined(HAS_MPI_STARTALL) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Startall
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Startall)(int*         count,
                        MPI_Request* array_of_requests,
                        int*         ierr)
{
  *ierr = MPI_Startall(*count, array_of_requests);
}

#endif
#if defined(HAS_MPI_TEST) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Test
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Test)(MPI_Request* request,
                    int*         flag,
                    MPI_Status*  status,
                    int*         ierr)
{
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Test(request, flag, status);
}

#endif
#if defined(HAS_MPI_TEST_CANCELLED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Test_cancelled)(MPI_Status* status,
                              int*        flag,
                              int*        ierr)
{
  *ierr = MPI_Test_cancelled(status, flag);
}

#endif
#if defined(HAS_MPI_TESTALL) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Testall
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Testall)(int*         count,
                       MPI_Request* array_of_requests,
                       int*         flag,
                       MPI_Status*  array_of_statuses,
                       int*         ierr)
{
  #if defined(HAS_MPI_STATUSES_IGNORE)
  if (array_of_statuses == epk_mpif_statuses_ignore)
  {
    array_of_statuses = MPI_STATUSES_IGNORE;
  }
  #endif

  *ierr = MPI_Testall(*count, array_of_requests, flag, array_of_statuses);
}

#endif
#if defined(HAS_MPI_WAIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Wait
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Wait)(MPI_Request* request,
                    MPI_Status*  status,
                    int*         ierr)
{
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    status = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Wait(request, status);
}

#endif
#if defined(HAS_MPI_WAITALL) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Waitall
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Waitall)(int*         count,
                       MPI_Request* array_of_requests,
                       MPI_Status*  array_of_statuses,
                       int*         ierr)
{
  #if defined(HAS_MPI_STATUSES_IGNORE)
  if (array_of_statuses == epk_mpif_statuses_ignore)
  {
    array_of_statuses = MPI_STATUSES_IGNORE;
  }
  #endif

  *ierr = MPI_Waitall(*count, array_of_requests, array_of_statuses);
}

#endif

#else /* !NEED_F2C_CONV */

/**
 * Wrapper for a statically allocated request array that can be reused
 * in different calls. It will be enlarged automatically, when needed.
 * @ingroup utility_functions
 */
static MPI_Request* alloc_request_array(int count)
{
  static MPI_Request* local_req_arr      = 0;
  static int          local_req_arr_size = 0;

  if (local_req_arr_size == 0)
  {
    /* -- never used: initialize -- */
    local_req_arr      = malloc(2 * count * sizeof (MPI_Request));
    local_req_arr_size = 2 * count;
  }
  else
  if (count > local_req_arr_size)
  {
    /* -- not enough room: expand -- */
    local_req_arr =
      realloc(local_req_arr, count * sizeof (MPI_Request));
    local_req_arr_size = count;
  }
  return local_req_arr;
}

/**
 * Wrapper for a statically allocated status array that can be reused
 * in different calls. It will be enlarged automatically, when needed.
 * @ingroup utility_functions
 */
static MPI_Status* alloc_status_array(int count)
{
  static MPI_Status* local_stat_arr      = 0;
  static int         local_stat_arr_size = 0;

  if (local_stat_arr_size == 0)
  {
    /* -- never used: initialize -- */
    local_stat_arr      = malloc(2 * count * sizeof (MPI_Status));
    local_stat_arr_size = 2 * count;
  }
  else
  if (count > local_stat_arr_size)
  {
    /* -- not enough room: expand -- */
    local_stat_arr =
      realloc(local_stat_arr, count * sizeof (MPI_Status));
    local_stat_arr_size = count;
  }
  return local_stat_arr;
}

#if defined(HAS_MPI_WAIT) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Wait,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Wait)(MPI_Fint* request,
                    MPI_Fint* status,
                    MPI_Fint* ierr)
{
  MPI_Request lrequest;
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;

  lrequest = PMPI_Request_f2c(*request);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Wait(&lrequest, c_status_ptr);

  *request = PMPI_Request_c2f(lrequest);

  if (*ierr == MPI_SUCCESS)
  {
    #if defined(HAS_MPI_STATUS_IGNORE)
    if (status != epk_mpif_status_ignore)
    #endif
    {
      PMPI_Status_c2f(c_status_ptr, status);
    }
  }
} /* FSUB(MPI_Wait) */

#endif

#if defined(HAS_MPI_WAITALL) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Waitall)(MPI_Fint* count,
                       MPI_Fint  array_of_requests[],
                       MPI_Fint  array_of_statuses[][ELG_MPI_STATUS_SIZE],
                       MPI_Fint* ierr)
{
  int          i;
  MPI_Request* lrequest = NULL;
  MPI_Status*  c_status = NULL;

  if (*count > 0)
  {
    lrequest = alloc_request_array(*count);

    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_statuses_ignore)
    #endif
    {
      c_status = alloc_status_array(*count);
    }
    #if defined(HAS_MPI_STATUSES_IGNORE)
    else
    {
      c_status = MPI_STATUSES_IGNORE;
    }
    #endif

    for (i = 0; i < *count; i++)
    {
      lrequest[i] = PMPI_Request_f2c(array_of_requests[i]);
    }
  }

  *ierr = MPI_Waitall(*count, lrequest, c_status);

  for (i = 0; i < *count; i++)
  {
    array_of_requests[i] = PMPI_Request_c2f(lrequest[i]);
  }
  if (*ierr == MPI_SUCCESS)
  {
    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_statuses_ignore)
    #endif
    {
      for (i = 0; i < *count; i++)
      {
        PMPI_Status_c2f(&(c_status[i]), &(array_of_statuses[i][0]));
      }
    }
  }
} /* FSUB(MPI_Waitall) */

#endif

#if defined(HAS_MPI_WAITANY) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitany,
 * when handle conversion is needed.
 * @note C index has to be converted to Fortran index, only if it is not
 *       MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Waitany)(MPI_Fint* count,
                       MPI_Fint  array_of_requests[],
                       MPI_Fint* index,
                       MPI_Fint* status,
                       MPI_Fint* ierr)
{
  int          i;
  MPI_Request* lrequest = NULL;
  MPI_Status   c_status;
  MPI_Status*  c_status_ptr = &c_status;

  if (*count > 0)
  {
    lrequest = alloc_request_array(*count);
    for (i = 0; i < *count; i++)
    {
      lrequest[i] = PMPI_Request_f2c(array_of_requests[i]);
    }
  }

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Waitany(*count, lrequest, index, c_status_ptr);

  if ((*ierr == MPI_SUCCESS) && (*index != MPI_UNDEFINED))
  {
    if (*index >= 0)
    {
      array_of_requests[*index] = PMPI_Request_c2f(lrequest[*index]);

      /* See the description of waitany in the standard;
         the Fortran index ranges are from 1, not zero */
      (*index)++;
    }

    #if defined(HAS_MPI_STATUS_IGNORE)
    if (status != epk_mpif_status_ignore)
    #endif
    {
      PMPI_Status_c2f(&c_status, status);
    }
  }
} /* FSUB(MPI_Waitany) */

#endif

#if defined(HAS_MPI_WAITSOME) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitsome,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, only if the
 *       outcount is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Waitsome)(MPI_Fint* incount,
                        MPI_Fint  array_of_requests[],
                        MPI_Fint* outcount,
                        MPI_Fint  indices[],
                        MPI_Fint  array_of_statuses[][ELG_MPI_STATUS_SIZE],
                        MPI_Fint* ierr)
{
  int          i, j, found;
  MPI_Request* lrequest = NULL;
  MPI_Status*  c_status = NULL;

  /* get addresses of internal buffers, and convert input requests */
  if (*incount > 0)
  {
    lrequest = alloc_request_array(*incount);

    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_statuses_ignore)
    #endif
    {
      c_status = alloc_status_array(*incount);
    }
    #if defined(HAS_MPI_STATUSES_IGNORE)
    else
    {
      c_status = MPI_STATUSES_IGNORE;
    }
    #endif

    for (i = 0; i < *incount; i++)
    {
      lrequest[i] = PMPI_Request_f2c(array_of_requests[i]);
    }
  }

  /* call C wrapper function */
  *ierr = MPI_Waitsome(*incount, lrequest, outcount, indices, c_status);

  /* convert requests and indices back to fortran */
  if ((*ierr == MPI_SUCCESS) && (*outcount != MPI_UNDEFINED))
  {
    for (i = 0; i < *incount; i++)
    {
      if (i < *outcount)
      {
        if (indices[i] >= 0)
        {
          array_of_requests[indices[i]] =
            PMPI_Request_c2f(lrequest[indices[i]]);
        }
      }
      else
      {
        found = j = 0;
        while ((!found) && (j < *outcount))
        {
          if (indices[j++] == i)
          {
            found = 1;
          }
        }

        if (!found)
        {
          array_of_requests[i] = PMPI_Request_c2f(lrequest[i]);
        }
      }
    }

    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_statuses_ignore)
    #endif
    {
      for (i = 0; i < *outcount; i++)
      {
        PMPI_Status_c2f(&c_status[i], &(array_of_statuses[i][0]));
        /* See the description of waitsome in the standard;
           the Fortran index ranges are from 1, not zero */
        if (indices[i] >= 0)
        {
          indices[i]++;
        }
      }
    }
  }
} /* FSUB(MPI_Waitsome) */

#endif

#if defined(HAS_MPI_TEST) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Test,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Test)(MPI_Fint* request,
                    MPI_Fint* flag,
                    MPI_Fint* status,
                    MPI_Fint* ierr)
{
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;
  MPI_Request lrequest     = PMPI_Request_f2c(*request);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Test(&lrequest, flag, c_status_ptr);

  if (*ierr != MPI_SUCCESS)
  {
    return;
  }
  *request = PMPI_Request_c2f(lrequest);
  if (flag)
  {
    #if defined(HAS_MPI_STATUS_IGNORE)
    if (status != epk_mpif_status_ignore)
    #endif
    {
      PMPI_Status_c2f(&c_status, status);
    }
  }
} /* FSUB(MPI_Test) */

#endif

#if defined(HAS_MPI_TESTANY) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testany,
 * when handle conversion is needed.
 * @note C index has to be converted to Fortran index, only if it is not
 *       MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Testany)(MPI_Fint* count,
                       MPI_Fint  array_of_requests[],
                       MPI_Fint* index,
                       MPI_Fint* flag,
                       MPI_Fint* status,
                       MPI_Fint* ierr)
{
  int          i;
  MPI_Request* lrequest = NULL;
  MPI_Status   c_status;
  MPI_Status*  c_status_ptr = &c_status;

  if (*count > 0)
  {
    lrequest = alloc_request_array(*count);
    for (i = 0; i < *count; i++)
    {
      lrequest[i] = PMPI_Request_f2c(array_of_requests[i]);
    }
  }

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Testany(*count, lrequest, index, flag, c_status_ptr);

  if ((*ierr == MPI_SUCCESS) && (*index != MPI_UNDEFINED))
  {
    if (*flag && *index >= 0)
    {
      /* index may be MPI_UNDEFINED if all are null */
      array_of_requests[*index] = PMPI_Request_c2f(lrequest[*index]);

      /* See the description of waitany in the standard;
         the Fortran index ranges are from 1, not zero */
      (*index)++;
    }

    #if defined(HAS_MPI_STATUS_IGNORE)
    if (status != epk_mpif_status_ignore)
    #endif
    {
      PMPI_Status_c2f(&c_status, status);
    }
  }
} /* FSUB(MPI_Testany) */

#endif

#if defined(HAS_MPI_TESTALL) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testall,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Testall)(MPI_Fint* count,
                       MPI_Fint  array_of_requests[],
                       MPI_Fint* flag,
                       MPI_Fint  array_of_statuses[][ELG_MPI_STATUS_SIZE],
                       MPI_Fint* ierr)
{
  int          i;
  MPI_Request* lrequest = NULL;
  MPI_Status*  c_status = NULL;

  if (*count > 0)
  {
    lrequest = alloc_request_array(*count);

    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_statuses_ignore)
    #endif
    {
      c_status = alloc_status_array(*count);
    }
    #if defined(HAS_MPI_STATUSES_IGNORE)
    else
    {
      c_status = MPI_STATUSES_IGNORE;
    }
    #endif

    for (i = 0; i < *count; i++)
    {
      lrequest[i] = PMPI_Request_f2c(array_of_requests[i]);
    }
  }

  *ierr = MPI_Testall(*count, lrequest, flag, c_status);

  for (i = 0; i < *count; i++)
  {
    array_of_requests[i] = PMPI_Request_c2f(lrequest[i]);
  }
  if (*ierr == MPI_SUCCESS && *flag)
  {
    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_statuses_ignore)
    #endif
    {
      for (i = 0; i < *count; i++)
      {
        PMPI_Status_c2f(&(c_status[i]), &(array_of_statuses[i][0]));
      }
    }
  }
} /* FSUB(MPI_Testall) */

#endif

#if defined(HAS_MPI_TESTSOME) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testsome,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, only if the
 *       outcount is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Testsome)(MPI_Fint* incount,
                        MPI_Fint  array_of_requests[],
                        MPI_Fint* outcount,
                        MPI_Fint  indices[],
                        MPI_Fint  array_of_statuses[][ELG_MPI_STATUS_SIZE],
                        MPI_Fint* ierr)
{
  int          i, j, found;
  MPI_Request* lrequest = NULL;
  MPI_Status*  c_status = NULL;

  if (*incount > 0)
  {
    lrequest = alloc_request_array(*incount);

    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_statuses_ignore)
    #endif
    {
      c_status = alloc_status_array(*incount);
    }
    #if defined(HAS_MPI_STATUSES_IGNORE)
    else
    {
      c_status = MPI_STATUSES_IGNORE;
    }
    #endif

    for (i = 0; i < *incount; i++)
    {
      lrequest[i] = PMPI_Request_f2c(array_of_requests[i]);
    }
  }

  *ierr = MPI_Testsome(*incount, lrequest, outcount, indices, c_status);

  if ((*ierr == MPI_SUCCESS) && (*outcount != MPI_UNDEFINED))
  {
    for (i = 0; i < *incount; i++)
    {
      if (i < *outcount)
      {
        array_of_requests[indices[i]] =
          PMPI_Request_c2f(lrequest[indices[i]]);
      }
      else
      {
        found = j = 0;
        while ( (!found) && (j < *outcount) )
        {
          if (indices[j++] == i)
          {
            found = 1;
          }
        }
        if (!found)
        {
          array_of_requests[i] = PMPI_Request_c2f(lrequest[i]);
        }
      }
    }

    #if defined(HAS_MPI_STATUSES_IGNORE)
    if (array_of_statuses != epk_mpif_status_ignore)
    #endif
    {
      for (i = 0; i < *outcount; i++)
      {
        PMPI_Status_c2f(&c_status[i], &(array_of_statuses[i][0]));
        /* See the description of testsome in the standard;
           the Fortran index ranges are from 1, not zero */
        if (indices[i] >= 0)
        {
          indices[i]++;
        }
      }
    }
  }
} /* FSUB(MPI_Testsome) */

#endif

#if defined(HAS_MPI_STARTALL) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Startall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Startall)(MPI_Fint* count,
                        MPI_Fint  array_of_requests[],
                        MPI_Fint* ierr)
{
  int          i;
  MPI_Request* lrequest = NULL;

  if (*count > 0)
  {
    lrequest = alloc_request_array(*count);
    for (i = 0; i < *count; i++)
    {
      lrequest[i] = PMPI_Request_f2c(array_of_requests[i]);
    }
  }

  *ierr = MPI_Startall(*count, lrequest);

  if (*ierr == MPI_SUCCESS)
  {
    for (i = 0; i < *count; i++)
    {
      array_of_requests[i] = PMPI_Request_c2f(lrequest[i]);
    }
  }
}

#endif

#if defined(HAS_MPI_REQUEST_FREE) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Request_free)(MPI_Fint* request,
                            MPI_Fint* ierr)
{
  MPI_Request lrequest = PMPI_Request_f2c(*request);

  *ierr = MPI_Request_free(&lrequest);

  if (*ierr == MPI_SUCCESS)
  {
    *request = PMPI_Request_c2f(lrequest);
  }
}

#endif

#if defined(HAS_MPI_CANCEL) && !defined(NO_MPI_P2P)
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testall,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void FSUB(MPI_Cancel)(MPI_Fint* request,
                      MPI_Fint* ierr)
{
  MPI_Request lrequest;

  lrequest = PMPI_Request_f2c(*request);
  *ierr    = MPI_Cancel(&lrequest);
}

#endif

#if defined(HAS_MPI_BSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Bsend
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Bsend)(void*     buf,
                     MPI_Fint* count,
                     MPI_Fint* datatype,
                     MPI_Fint* dest,
                     MPI_Fint* tag,
                     MPI_Fint* comm,
                     MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Bsend(buf, *count, PMPI_Type_f2c(
                      *datatype), *dest, *tag, PMPI_Comm_f2c(*comm));
}

#endif
#if defined(HAS_MPI_BSEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Bsend_init
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Bsend_init)(void*     buf,
                          MPI_Fint* count,
                          MPI_Fint* datatype,
                          MPI_Fint* dest,
                          MPI_Fint* tag,
                          MPI_Fint* comm,
                          MPI_Fint* request,
                          MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Bsend_init(buf, *count, PMPI_Type_f2c(
                           *datatype), *dest, *tag, PMPI_Comm_f2c(
                           *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_BUFFER_ATTACH) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Buffer_attach
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Buffer_attach)(void*     buffer,
                             MPI_Fint* size,
                             MPI_Fint* ierr)
{
  *ierr = MPI_Buffer_attach(buffer, *size);
}

#endif
#if defined(HAS_MPI_BUFFER_DETACH) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Buffer_detach
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Buffer_detach)(void*     buffer,
                             MPI_Fint* size,
                             MPI_Fint* ierr)
{
  *ierr = MPI_Buffer_detach(buffer, size);
}

#endif
#if defined(HAS_MPI_IBSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Ibsend
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Ibsend)(void*     buf,
                      MPI_Fint* count,
                      MPI_Fint* datatype,
                      MPI_Fint* dest,
                      MPI_Fint* tag,
                      MPI_Fint* comm,
                      MPI_Fint* request,
                      MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Ibsend(buf, *count, PMPI_Type_f2c(
                       *datatype), *dest, *tag, PMPI_Comm_f2c(
                       *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_IPROBE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Iprobe
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Iprobe)(MPI_Fint* source,
                      MPI_Fint* tag,
                      MPI_Fint* comm,
                      MPI_Fint* flag,
                      MPI_Fint* status,
                      MPI_Fint* ierr)
{
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    /* hardcoded c_status_ptr needs to be reset */
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Iprobe(*source, *tag, PMPI_Comm_f2c(
                       *comm), flag, c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
}

#endif
#if defined(HAS_MPI_IRECV) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Irecv
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Irecv)(void*     buf,
                     MPI_Fint* count,
                     MPI_Fint* datatype,
                     MPI_Fint* source,
                     MPI_Fint* tag,
                     MPI_Fint* comm,
                     MPI_Fint* request,
                     MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Irecv(buf, *count, PMPI_Type_f2c(
                      *datatype), *source, *tag, PMPI_Comm_f2c(
                      *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_IRSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Irsend
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Irsend)(void*     buf,
                      MPI_Fint* count,
                      MPI_Fint* datatype,
                      MPI_Fint* dest,
                      MPI_Fint* tag,
                      MPI_Fint* comm,
                      MPI_Fint* request,
                      MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Irsend(buf, *count, PMPI_Type_f2c(
                       *datatype), *dest, *tag, PMPI_Comm_f2c(
                       *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_ISEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Isend
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Isend)(void*     buf,
                     MPI_Fint* count,
                     MPI_Fint* datatype,
                     MPI_Fint* dest,
                     MPI_Fint* tag,
                     MPI_Fint* comm,
                     MPI_Fint* request,
                     MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Isend(buf, *count, PMPI_Type_f2c(
                      *datatype), *dest, *tag, PMPI_Comm_f2c(
                      *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_ISSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Issend
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Issend)(void*     buf,
                      MPI_Fint* count,
                      MPI_Fint* datatype,
                      MPI_Fint* dest,
                      MPI_Fint* tag,
                      MPI_Fint* comm,
                      MPI_Fint* request,
                      MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Issend(buf, *count, PMPI_Type_f2c(
                       *datatype), *dest, *tag, PMPI_Comm_f2c(
                       *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_PROBE) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Probe
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Probe)(MPI_Fint* source,
                     MPI_Fint* tag,
                     MPI_Fint* comm,
                     MPI_Fint* status,
                     MPI_Fint* ierr)
{
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    /* hardcoded c_status_ptr needs to be reset */
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Probe(*source, *tag, PMPI_Comm_f2c(*comm), c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
}

#endif
#if defined(HAS_MPI_RECV) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Recv
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Recv)(void*     buf,
                    MPI_Fint* count,
                    MPI_Fint* datatype,
                    MPI_Fint* source,
                    MPI_Fint* tag,
                    MPI_Fint* comm,
                    MPI_Fint* status,
                    MPI_Fint* ierr)
{
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    /* hardcoded c_status_ptr needs to be reset */
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Recv(buf, *count, PMPI_Type_f2c(
                     *datatype), *source, *tag, PMPI_Comm_f2c(
                     *comm), c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_Recv) */

#endif
#if defined(HAS_MPI_RECV_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Recv_init
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Recv_init)(void*     buf,
                         MPI_Fint* count,
                         MPI_Fint* datatype,
                         MPI_Fint* source,
                         MPI_Fint* tag,
                         MPI_Fint* comm,
                         MPI_Fint* request,
                         MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Recv_init(buf, *count, PMPI_Type_f2c(
                          *datatype), *source, *tag, PMPI_Comm_f2c(
                          *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_RSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Rsend
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Rsend)(void*     buf,
                     MPI_Fint* count,
                     MPI_Fint* datatype,
                     MPI_Fint* dest,
                     MPI_Fint* tag,
                     MPI_Fint* comm,
                     MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Rsend(buf, *count, PMPI_Type_f2c(
                      *datatype), *dest, *tag, PMPI_Comm_f2c(*comm));
}

#endif
#if defined(HAS_MPI_RSEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Rsend_init
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Rsend_init)(void*     buf,
                          MPI_Fint* count,
                          MPI_Fint* datatype,
                          MPI_Fint* dest,
                          MPI_Fint* tag,
                          MPI_Fint* comm,
                          MPI_Fint* request,
                          MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Rsend_init(buf, *count, PMPI_Type_f2c(
                           *datatype), *dest, *tag, PMPI_Comm_f2c(
                           *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_SEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Send
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Send)(void*     buf,
                    MPI_Fint* count,
                    MPI_Fint* datatype,
                    MPI_Fint* dest,
                    MPI_Fint* tag,
                    MPI_Fint* comm,
                    MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Send(buf, *count, PMPI_Type_f2c(
                     *datatype), *dest, *tag, PMPI_Comm_f2c(*comm));
}

#endif
#if defined(HAS_MPI_SEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Send_init
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Send_init)(void*     buf,
                         MPI_Fint* count,
                         MPI_Fint* datatype,
                         MPI_Fint* dest,
                         MPI_Fint* tag,
                         MPI_Fint* comm,
                         MPI_Fint* request,
                         MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Send_init(buf, *count, PMPI_Type_f2c(
                          *datatype), *dest, *tag, PMPI_Comm_f2c(
                          *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_SENDRECV) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Sendrecv
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Sendrecv)(void*     sendbuf,
                        MPI_Fint* sendcount,
                        MPI_Fint* sendtype,
                        MPI_Fint* dest,
                        MPI_Fint* sendtag,
                        void*     recvbuf,
                        MPI_Fint* recvcount,
                        MPI_Fint* recvtype,
                        MPI_Fint* source,
                        MPI_Fint* recvtag,
                        MPI_Fint* comm,
                        MPI_Fint* status,
                        MPI_Fint* ierr)
{
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;

  #if defined(HAS_MPI_BOTTOM)
  if (sendbuf == epk_mpif_bottom)
  {
    sendbuf = MPI_BOTTOM;
  }
  #endif
  #if defined(HAS_MPI_BOTTOM)
  if (recvbuf == epk_mpif_bottom)
  {
    recvbuf = MPI_BOTTOM;
  }
  #endif
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    /* hardcoded c_status_ptr needs to be reset */
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Sendrecv(sendbuf, *sendcount, PMPI_Type_f2c(
                         *sendtype), *dest, *sendtag, recvbuf, *recvcount,
                       PMPI_Type_f2c(
                         *recvtype), *source, *recvtag,
                       PMPI_Comm_f2c(*comm), c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_Sendrecv) */

#endif
#if defined(HAS_MPI_SENDRECV_REPLACE) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Sendrecv_replace
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Sendrecv_replace)(void*     buf,
                                MPI_Fint* count,
                                MPI_Fint* datatype,
                                MPI_Fint* dest,
                                MPI_Fint* sendtag,
                                MPI_Fint* source,
                                MPI_Fint* recvtag,
                                MPI_Fint* comm,
                                MPI_Fint* status,
                                MPI_Fint* ierr)
{
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (status == epk_mpif_status_ignore)
  {
    /* hardcoded c_status_ptr needs to be reset */
    c_status_ptr = MPI_STATUS_IGNORE;
  }
  #endif

  *ierr = MPI_Sendrecv_replace(buf, *count, PMPI_Type_f2c(
                                 *datatype), *dest, *sendtag, *source,
                               *recvtag, PMPI_Comm_f2c(
                                 *comm), c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_Sendrecv_replace) */

#endif
#if defined(HAS_MPI_SSEND) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Ssend
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Ssend)(void*     buf,
                     MPI_Fint* count,
                     MPI_Fint* datatype,
                     MPI_Fint* dest,
                     MPI_Fint* tag,
                     MPI_Fint* comm,
                     MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Ssend(buf, *count, PMPI_Type_f2c(
                      *datatype), *dest, *tag, PMPI_Comm_f2c(*comm));
}

#endif
#if defined(HAS_MPI_SSEND_INIT) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Ssend_init
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Ssend_init)(void*     buf,
                          MPI_Fint* count,
                          MPI_Fint* datatype,
                          MPI_Fint* dest,
                          MPI_Fint* tag,
                          MPI_Fint* comm,
                          MPI_Fint* request,
                          MPI_Fint* ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Ssend_init(buf, *count, PMPI_Type_f2c(
                           *datatype), *dest, *tag, PMPI_Comm_f2c(
                           *comm), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_START) && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Start)(MPI_Fint* request,
                     MPI_Fint* ierr)
{
  MPI_Request c_request = PMPI_Request_f2c(*request);

  *ierr = MPI_Start(&c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif

#if defined(HAS_MPI_TEST_CANCELLED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_P2P)
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup p2p
 */
void FSUB(MPI_Test_cancelled)(MPI_Fint* status,
                              MPI_Fint* flag,
                              MPI_Fint* ierr)
{
  MPI_Status  c_status;
  MPI_Status* c_status_ptr = &c_status;

  PMPI_Status_f2c(status, c_status_ptr);

  *ierr = MPI_Test_cancelled(c_status_ptr, flag);
}

#endif

#endif
