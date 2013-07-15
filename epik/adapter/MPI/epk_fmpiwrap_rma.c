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
 * @file  epk_fmpiwrap_rma.c
 *
 * @brief Fortran interface wrappers for one-sided communication
 */

#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "elg_error.h"
#include "elg_fmpi_defs.h"
#include "epk_fmpiwrap.h"
#include "epk_defs_mpi.h"
#include "epk_fwrapper_def.h"

/* uppercase defines */
/** All uppercase name of MPI_Accumulate */
#define MPI_Accumulate_U MPI_ACCUMULATE
/** All uppercase name of MPI_Get */
#define MPI_Get_U MPI_GET
/** All uppercase name of MPI_Put */
#define MPI_Put_U MPI_PUT
/** All uppercase name of MPI_Win_complete */
#define MPI_Win_complete_U MPI_WIN_COMPLETE
/** All uppercase name of MPI_Win_create */
#define MPI_Win_create_U MPI_WIN_CREATE
/** All uppercase name of MPI_Win_fence */
#define MPI_Win_fence_U MPI_WIN_FENCE
/** All uppercase name of MPI_Win_free */
#define MPI_Win_free_U MPI_WIN_FREE
/** All uppercase name of MPI_Win_get_group */
#define MPI_Win_get_group_U MPI_WIN_GET_GROUP
/** All uppercase name of MPI_Win_lock */
#define MPI_Win_lock_U MPI_WIN_LOCK
/** All uppercase name of MPI_Win_post */
#define MPI_Win_post_U MPI_WIN_POST
/** All uppercase name of MPI_Win_start */
#define MPI_Win_start_U MPI_WIN_START
/** All uppercase name of MPI_Win_test */
#define MPI_Win_test_U MPI_WIN_TEST
/** All uppercase name of MPI_Win_unlock */
#define MPI_Win_unlock_U MPI_WIN_UNLOCK
/** All uppercase name of MPI_Win_wait */
#define MPI_Win_wait_U MPI_WIN_WAIT

/** All uppercase name of MPI_Win_call_errhandler */
#define MPI_Win_call_errhandler_U MPI_WIN_CALL_ERRHANDLER
/** All uppercase name of MPI_Win_create_errhandler */
#define MPI_Win_create_errhandler_U MPI_WIN_CREATE_ERRHANDLER
/** All uppercase name of MPI_Win_get_errhandler */
#define MPI_Win_get_errhandler_U MPI_WIN_GET_ERRHANDLER
/** All uppercase name of MPI_Win_set_errhandler */
#define MPI_Win_set_errhandler_U MPI_WIN_SET_ERRHANDLER

/** All uppercase name of MPI_Win_create_keyval */
#define MPI_Win_create_keyval_U MPI_WIN_CREATE_KEYVAL
/** All uppercase name of MPI_Win_delete_attr */
#define MPI_Win_delete_attr_U MPI_WIN_DELETE_ATTR
/** All uppercase name of MPI_Win_free_keyval */
#define MPI_Win_free_keyval_U MPI_WIN_FREE_KEYVAL
/** All uppercase name of MPI_Win_get_attr */
#define MPI_Win_get_attr_U MPI_WIN_GET_ATTR
/** All uppercase name of MPI_Win_get_name */
#define MPI_Win_get_name_U MPI_WIN_GET_NAME
/** All uppercase name of MPI_Win_set_attr */
#define MPI_Win_set_attr_U MPI_WIN_SET_ATTR
/** All uppercase name of MPI_Win_set_name */
#define MPI_Win_set_name_U MPI_WIN_SET_NAME

/* lowercase defines */
/** All lowercase name of MPI_Accumulate */
#define MPI_Accumulate_L mpi_accumulate
/** All lowercase name of MPI_Get */
#define MPI_Get_L mpi_get
/** All lowercase name of MPI_Put */
#define MPI_Put_L mpi_put
/** All lowercase name of MPI_Win_complete */
#define MPI_Win_complete_L mpi_win_complete
/** All lowercase name of MPI_Win_create */
#define MPI_Win_create_L mpi_win_create
/** All lowercase name of MPI_Win_fence */
#define MPI_Win_fence_L mpi_win_fence
/** All lowercase name of MPI_Win_free */
#define MPI_Win_free_L mpi_win_free
/** All lowercase name of MPI_Win_get_group */
#define MPI_Win_get_group_L mpi_win_get_group
/** All lowercase name of MPI_Win_lock */
#define MPI_Win_lock_L mpi_win_lock
/** All lowercase name of MPI_Win_post */
#define MPI_Win_post_L mpi_win_post
/** All lowercase name of MPI_Win_start */
#define MPI_Win_start_L mpi_win_start
/** All lowercase name of MPI_Win_test */
#define MPI_Win_test_L mpi_win_test
/** All lowercase name of MPI_Win_unlock */
#define MPI_Win_unlock_L mpi_win_unlock
/** All lowercase name of MPI_Win_wait */
#define MPI_Win_wait_L mpi_win_wait

/** All lowercase name of MPI_Win_call_errhandler */
#define MPI_Win_call_errhandler_L mpi_win_call_errhandler
/** All lowercase name of MPI_Win_create_errhandler */
#define MPI_Win_create_errhandler_L mpi_win_create_errhandler
/** All lowercase name of MPI_Win_get_errhandler */
#define MPI_Win_get_errhandler_L mpi_win_get_errhandler
/** All lowercase name of MPI_Win_set_errhandler */
#define MPI_Win_set_errhandler_L mpi_win_set_errhandler

/** All lowercase name of MPI_Win_create_keyval */
#define MPI_Win_create_keyval_L mpi_win_create_keyval
/** All lowercase name of MPI_Win_delete_attr */
#define MPI_Win_delete_attr_L mpi_win_delete_attr
/** All lowercase name of MPI_Win_free_keyval */
#define MPI_Win_free_keyval_L mpi_win_free_keyval
/** All lowercase name of MPI_Win_get_attr */
#define MPI_Win_get_attr_L mpi_win_get_attr
/** All lowercase name of MPI_Win_get_name */
#define MPI_Win_get_name_L mpi_win_get_name
/** All lowercase name of MPI_Win_set_attr */
#define MPI_Win_set_attr_L mpi_win_set_attr
/** All lowercase name of MPI_Win_set_name */
#define MPI_Win_set_name_L mpi_win_set_name

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

/**
 * @name Fortran wrappers for access functions
 * @{
 */
#if defined(HAS_MPI_ACCUMULATE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Accumulate
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Accumulate)(void*         origin_addr,
                          int*          origin_count,
                          MPI_Datatype* origin_datatype,
                          int*          target_rank,
                          MPI_Aint*     target_disp,
                          int*          target_count,
                          MPI_Datatype* target_datatype,
                          MPI_Op*       op,
                          MPI_Win*      win,
                          int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (origin_addr == epk_mpif_bottom)
  {
    origin_addr = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Accumulate(origin_addr,
                         *origin_count,
                         *origin_datatype,
                         *target_rank,
                         *target_disp,
                         *target_count,
                         *target_datatype,
                         *op,
                         *win);
}

#endif
#if defined(HAS_MPI_GET) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Get
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Get)(void*         origin_addr,
                   int*          origin_count,
                   MPI_Datatype* origin_datatype,
                   int*          target_rank,
                   MPI_Aint*     target_disp,
                   int*          target_count,
                   MPI_Datatype* target_datatype,
                   MPI_Win*      win,
                   int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (origin_addr == epk_mpif_bottom)
  {
    origin_addr = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Get(origin_addr,
                  *origin_count,
                  *origin_datatype,
                  *target_rank,
                  *target_disp,
                  *target_count,
                  *target_datatype,
                  *win);
}

#endif
#if defined(HAS_MPI_PUT) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Put
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Put)(void*         origin_addr,
                   int*          origin_count,
                   MPI_Datatype* origin_datatype,
                   int*          target_rank,
                   MPI_Aint*     target_disp,
                   int*          target_count,
                   MPI_Datatype* target_datatype,
                   MPI_Win*      win,
                   int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (origin_addr == epk_mpif_bottom)
  {
    origin_addr = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Put(origin_addr,
                  *origin_count,
                  *origin_datatype,
                  *target_rank,
                  *target_disp,
                  *target_count,
                  *target_datatype,
                  *win);
}

#endif

/**
 * @}
 * @name Fortran wrappers for window management functions
 * @{
 */
#if defined(HAS_MPI_WIN_CREATE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_create)(void*     base,
                          MPI_Aint* size,
                          int*      disp_unit,
                          MPI_Info* info,
                          MPI_Comm* comm,
                          MPI_Win*  win,
                          int*      ierr)
{
  *ierr = MPI_Win_create(base, *size, *disp_unit, *info, *comm, win);
}

#endif
#if defined(HAS_MPI_WIN_FREE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_free)(MPI_Win* win,
                        int*     ierr)
{
  *ierr = MPI_Win_free(win);
}

#endif

#if defined(HAS_MPI_WIN_GET_GROUP) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA)
/**
 * Measurement wrapper for MPI_Win_get_group
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_get_group)(MPI_Win*   win,
                             MPI_Group* group,
                             int*       ierr)
{
  *ierr = MPI_Win_get_group(*win, group);
}

#endif

/**
 * @}
 * @name Fortran wrappers for synchonization functions
 * @{
 */
#if defined(HAS_MPI_WIN_COMPLETE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_complete
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_complete)(MPI_Win* win,
                            int*     ierr)
{
  *ierr = MPI_Win_complete(*win);
}

#endif
#if defined(HAS_MPI_WIN_FENCE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_fence
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_fence)(int*     assert,
                         MPI_Win* win,
                         int*     ierr)
{
  *ierr = MPI_Win_fence(*assert, *win);
}

#endif
#if defined(HAS_MPI_WIN_LOCK) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_lock
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_lock)(int*     lock_type,
                        int*     rank,
                        int*     assert,
                        MPI_Win* win,
                        int*     ierr)
{
  *ierr = MPI_Win_lock(*lock_type, *rank, *assert, *win);
}

#endif
#if defined(HAS_MPI_WIN_POST) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_post
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_post)(MPI_Group* group,
                        int*       assert,
                        MPI_Win*   win,
                        int*       ierr)
{
  *ierr = MPI_Win_post(*group, *assert, *win);
}

#endif
#if defined(HAS_MPI_WIN_START) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_start
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_start)(MPI_Group* group,
                         int*       assert,
                         MPI_Win*   win,
                         int*       ierr)
{
  *ierr = MPI_Win_start(*group, *assert, *win);
}

#endif
#if defined(HAS_MPI_WIN_TEST) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_test
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_test)(MPI_Win* win,
                        int*     flag,
                        int*     ierr)
{
  *ierr = MPI_Win_test(*win, flag);
}

#endif
#if defined(HAS_MPI_WIN_UNLOCK) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_unlock
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_unlock)(int*     rank,
                          MPI_Win* win,
                          int*     ierr)
{
  *ierr = MPI_Win_unlock(*rank, *win);
}

#endif
#if defined(HAS_MPI_WIN_WAIT) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_wait
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_wait)(MPI_Win* win,
                        int*     ierr)
{
  *ierr = MPI_Win_wait(*win);
}

#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */
#if defined(HAS_MPI_WIN_CALL_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_call_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_call_errhandler)(MPI_Win* win,
                                   int*     errorcode,
                                   int*     ierr)
{
  *ierr = MPI_Win_call_errhandler(*win, *errorcode);
}

#endif
#if defined(HAS_MPI_WIN_CREATE_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_create_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_create_errhandler)(MPI_Win_errhandler_fn* function,
                                     MPI_Errhandler*        errhandler,
                                     int*                   ierr)
{
  *ierr = MPI_Win_create_errhandler(function, errhandler);
}

#endif
#if defined(HAS_MPI_WIN_GET_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_get_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_get_errhandler)(MPI_Win*        win,
                                  MPI_Errhandler* errhandler,
                                  int*            ierr)
{
  *ierr = MPI_Win_get_errhandler(*win, errhandler);
}

#endif
#if defined(HAS_MPI_WIN_SET_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_set_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_set_errhandler)(MPI_Win*        win,
                                  MPI_Errhandler* errhandler,
                                  int*            ierr)
{
  *ierr = MPI_Win_set_errhandler(*win, *errhandler);
}

#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */
#if defined(HAS_MPI_WIN_CREATE_KEYVAL) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_create_keyval
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_create_keyval)(
  MPI_Win_copy_attr_function* win_copy_attr_fn,
  MPI_Win_delete_attr_function*
  win_delete_attr_fn,
  int*                        win_keyval,
  void*                       extra_state,
  int*                        ierr)
{
  *ierr = MPI_Win_create_keyval(win_copy_attr_fn,
                                win_delete_attr_fn,
                                win_keyval,
                                extra_state);
}

#endif
#if defined(HAS_MPI_WIN_DELETE_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_delete_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_delete_attr)(MPI_Win* win,
                               int*     win_keyval,
                               int*     ierr)
{
  *ierr = MPI_Win_delete_attr(*win, *win_keyval);
}

#endif
#if defined(HAS_MPI_WIN_FREE_KEYVAL) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_free_keyval
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_free_keyval)(int* win_keyval,
                               int* ierr)
{
  *ierr = MPI_Win_free_keyval(win_keyval);
}

#endif
#if defined(HAS_MPI_WIN_GET_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_get_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_get_attr)(MPI_Win* win,
                            int*     win_keyval,
                            void*    attribute_val,
                            int*     flag,
                            int*     ierr)
{
  *ierr = MPI_Win_get_attr(*win, *win_keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_WIN_GET_NAME) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_get_name
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_get_name)(MPI_Win* win,
                            char*    win_name,
                            int*     resultlen,
                            int*     ierr,
                            int      win_name_len)
{
  char* c_win_name     = NULL;
  int   c_win_name_len = 0;

  c_win_name = (char*)malloc((win_name_len + 1) * sizeof (char));
  if (!c_win_name)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Win_get_name(*win, c_win_name, resultlen);

  c_win_name_len = strlen(c_win_name);
  strncpy(win_name, c_win_name, c_win_name_len);
  memset(win_name + c_win_name_len, ' ', win_name_len - c_win_name_len);
  free(c_win_name);
}

#endif
#if defined(HAS_MPI_WIN_SET_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_set_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_set_attr)(MPI_Win* win,
                            int*     win_keyval,
                            void*    attribute_val,
                            int*     ierr)
{
  *ierr = MPI_Win_set_attr(*win, *win_keyval, attribute_val);
}

#endif
#if defined(HAS_MPI_WIN_SET_NAME) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_set_name
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_set_name)(MPI_Win* win,
                            char*    win_name,
                            int*     ierr,
                            int      win_name_len)
{
  char* c_win_name     = NULL;
  int   c_win_name_len = win_name_len;

  while ((c_win_name_len > 0) && (win_name[c_win_name_len - 1] == ' '))
  {
    c_win_name_len--;
  }
  c_win_name = (char*)malloc((win_name_len + 1) * sizeof (char));
  if (!c_win_name)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_win_name, win_name, win_name_len);
  c_win_name[c_win_name_len] = '\0';

  *ierr = MPI_Win_set_name(*win, c_win_name);

  free(c_win_name);
}

#endif

#else /* !NEED_F2C_CONV */

/**
 * @name Fortran wrappers for access functions
 * @{
 */
#if defined(HAS_MPI_ACCUMULATE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Accumulate
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Accumulate)(void*     origin_addr,
                          MPI_Fint* origin_count,
                          MPI_Fint* origin_datatype,
                          MPI_Fint* target_rank,
                          MPI_Aint* target_disp,
                          MPI_Fint* target_count,
                          MPI_Fint* target_datatype,
                          MPI_Fint* op,
                          MPI_Fint* win,
                          MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (origin_addr == epk_mpif_bottom)
  {
    origin_addr = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_Accumulate(origin_addr, *origin_count, PMPI_Type_f2c(
                     *origin_datatype), *target_rank, *target_disp,
                   *target_count,
                   PMPI_Type_f2c(*target_datatype), PMPI_Op_f2c(
                     *op), PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_GET) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Get)(void*     origin_addr,
                   MPI_Fint* origin_count,
                   MPI_Fint* origin_datatype,
                   MPI_Fint* target_rank,
                   MPI_Aint* target_disp,
                   MPI_Fint* target_count,
                   MPI_Fint* target_datatype,
                   MPI_Fint* win,
                   MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (origin_addr == epk_mpif_bottom)
  {
    origin_addr = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_Get(origin_addr, *origin_count, PMPI_Type_f2c(
              *origin_datatype), *target_rank, *target_disp, *target_count,
            PMPI_Type_f2c(*target_datatype), PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_PUT) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Put
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Put)(void*     origin_addr,
                   MPI_Fint* origin_count,
                   MPI_Fint* origin_datatype,
                   MPI_Fint* target_rank,
                   MPI_Aint* target_disp,
                   MPI_Fint* target_count,
                   MPI_Fint* target_datatype,
                   MPI_Fint* win,
                   MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (origin_addr == epk_mpif_bottom)
  {
    origin_addr = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_Put(origin_addr, *origin_count, PMPI_Type_f2c(
              *origin_datatype), *target_rank, *target_disp, *target_count,
            PMPI_Type_f2c(*target_datatype), PMPI_Win_f2c(*win));
}

#endif

/**
 * @}
 * @name Fortran wrappers for window management functions
 * @{
 */
#if defined(HAS_MPI_WIN_CREATE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_create)(void*     base,
                          MPI_Aint* size,
                          MPI_Fint* disp_unit,
                          MPI_Fint* info,
                          MPI_Fint* comm,
                          MPI_Fint* win,
                          MPI_Fint* ierr)
{
  MPI_Win c_win;

  *ierr = MPI_Win_create(base, *size, *disp_unit, PMPI_Info_f2c(
                           *info), PMPI_Comm_f2c(*comm), &c_win);

  *win = PMPI_Win_c2f(c_win);
}

#endif
#if defined(HAS_MPI_WIN_FREE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_free)(MPI_Fint* win,
                        MPI_Fint* ierr)
{
  MPI_Win c_win = PMPI_Win_f2c(*win);

  *ierr = MPI_Win_free(&c_win);

  *win = PMPI_Win_c2f(c_win);
}

#endif

#if defined(HAS_MPI_WIN_GET_GROUP) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA)
/**
 * Measurement wrapper for MPI_Win_get_group
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_get_group)(MPI_Fint* win,
                             MPI_Fint* group,
                             MPI_Fint* ierr)
{
  MPI_Group c_group;

  *ierr = MPI_Win_get_group(PMPI_Win_f2c(*win), &c_group);

  *group = PMPI_Group_c2f(c_group);
}

#endif

/**
 * @}
 * @name Fortran wrappers for synchonization functions
 * @{
 */
#if defined(HAS_MPI_WIN_COMPLETE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_complete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_complete)(MPI_Fint* win,
                            MPI_Fint* ierr)
{
  *ierr = MPI_Win_complete(PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_WIN_FENCE) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_fence
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_fence)(MPI_Fint* assert,
                         MPI_Fint* win,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Win_fence(*assert, PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_WIN_LOCK) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_lock
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_lock)(MPI_Fint* lock_type,
                        MPI_Fint* rank,
                        MPI_Fint* assert,
                        MPI_Fint* win,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Win_lock(*lock_type, *rank, *assert, PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_WIN_POST) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_post
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_post)(MPI_Fint* group,
                        MPI_Fint* assert,
                        MPI_Fint* win,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Win_post(PMPI_Group_f2c(*group), *assert, PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_WIN_START) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_start
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_start)(MPI_Fint* group,
                         MPI_Fint* assert,
                         MPI_Fint* win,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Win_start(PMPI_Group_f2c(*group), *assert, PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_WIN_TEST) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_test
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_test)(MPI_Fint* win,
                        MPI_Fint* flag,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Win_test(PMPI_Win_f2c(*win), flag);
}

#endif
#if defined(HAS_MPI_WIN_UNLOCK) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_unlock
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_unlock)(MPI_Fint* rank,
                          MPI_Fint* win,
                          MPI_Fint* ierr)
{
  *ierr = MPI_Win_unlock(*rank, PMPI_Win_f2c(*win));
}

#endif
#if defined(HAS_MPI_WIN_WAIT) && !defined(NO_MPI_RMA)
/**
 * Measurement wrapper for MPI_Win_wait
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma
 */
void FSUB(MPI_Win_wait)(MPI_Fint* win,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Win_wait(PMPI_Win_f2c(*win));
}

#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */
#if defined(HAS_MPI_WIN_CALL_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_call_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_call_errhandler)(MPI_Fint* win,
                                   MPI_Fint* errorcode,
                                   MPI_Fint* ierr)
{
  *ierr = MPI_Win_call_errhandler(PMPI_Win_f2c(*win), *errorcode);
}

#endif
#if defined(HAS_MPI_WIN_CREATE_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_create_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_create_errhandler)(void*     function,
                                     void*     errhandler,
                                     MPI_Fint* ierr)
{
  *ierr =
    MPI_Win_create_errhandler((MPI_Win_errhandler_fn*)function,
                              (MPI_Errhandler*)errhandler);
}

#endif
#if defined(HAS_MPI_WIN_GET_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_get_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_get_errhandler)(MPI_Fint* win,
                                  void*     errhandler,
                                  MPI_Fint* ierr)
{
  *ierr = MPI_Win_get_errhandler(PMPI_Win_f2c(
                                   *win), (MPI_Errhandler*)errhandler);
}

#endif
#if defined(HAS_MPI_WIN_SET_ERRHANDLER) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_Win_set_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_err
 */
void FSUB(MPI_Win_set_errhandler)(MPI_Fint* win,
                                  void*     errhandler,
                                  MPI_Fint* ierr)
{
  MPI_Win c_win = PMPI_Win_f2c(*win);

  *ierr = MPI_Win_set_errhandler(c_win, *((MPI_Errhandler*)errhandler));

  *win = PMPI_Win_c2f(c_win);
}

#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */
#if defined(HAS_MPI_WIN_CREATE_KEYVAL) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_create_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_create_keyval)(void*     win_copy_attr_fn,
                                 void*     win_delete_attr_fn,
                                 MPI_Fint* win_keyval,
                                 void*     extra_state,
                                 MPI_Fint* ierr)
{
  *ierr = MPI_Win_create_keyval(
    (MPI_Win_copy_attr_function*)win_copy_attr_fn,
    (MPI_Win_delete_attr_function*)win_delete_attr_fn,
    win_keyval,
    extra_state);
}

#endif
#if defined(HAS_MPI_WIN_DELETE_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_delete_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_delete_attr)(MPI_Fint* win,
                               MPI_Fint* win_keyval,
                               MPI_Fint* ierr)
{
  MPI_Win c_win = PMPI_Win_f2c(*win);

  *ierr = MPI_Win_delete_attr(c_win, *win_keyval);

  *win = PMPI_Win_c2f(c_win);
}

#endif
#if defined(HAS_MPI_WIN_FREE_KEYVAL) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_free_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_free_keyval)(MPI_Fint* win_keyval,
                               MPI_Fint* ierr)
{
  *ierr = MPI_Win_free_keyval(win_keyval);
}

#endif
#if defined(HAS_MPI_WIN_GET_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_get_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_get_attr)(MPI_Fint* win,
                            MPI_Fint* win_keyval,
                            void*     attribute_val,
                            MPI_Fint* flag,
                            MPI_Fint* ierr)
{
  *ierr = MPI_Win_get_attr(PMPI_Win_f2c(
                             *win), *win_keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_WIN_GET_NAME) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_get_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_get_name)(MPI_Fint* win,
                            char*     win_name,
                            MPI_Fint* resultlen,
                            MPI_Fint* ierr,
                            int       win_name_len)
{
  char* c_win_name     = NULL;
  int   c_win_name_len = 0;

  c_win_name = (char*)malloc((win_name_len + 1) * sizeof (char));
  if (!c_win_name)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Win_get_name(PMPI_Win_f2c(*win), c_win_name, resultlen);

  c_win_name_len = strlen(c_win_name);
  strncpy(win_name, c_win_name, c_win_name_len);
  memset(win_name + c_win_name_len, ' ', win_name_len - c_win_name_len);
  free(c_win_name);
}

#endif
#if defined(HAS_MPI_WIN_SET_ATTR) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_set_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_set_attr)(MPI_Fint* win,
                            MPI_Fint* win_keyval,
                            void*     attribute_val,
                            MPI_Fint* ierr)
{
  MPI_Win c_win = PMPI_Win_f2c(*win);

  *ierr = MPI_Win_set_attr(c_win, *win_keyval, attribute_val);

  *win = PMPI_Win_c2f(c_win);
}

#endif
#if defined(HAS_MPI_WIN_SET_NAME) && !defined(NO_MPI_RMA) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Win_set_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup rma_ext
 */
void FSUB(MPI_Win_set_name)(MPI_Fint* win,
                            char*     win_name,
                            MPI_Fint* ierr,
                            int       win_name_len)
{
  MPI_Win c_win          = PMPI_Win_f2c(*win);
  char*   c_win_name     = NULL;
  int     c_win_name_len = win_name_len;

  while ((c_win_name_len > 0) && (win_name[c_win_name_len - 1] == ' '))
  {
    c_win_name_len--;
  }
  c_win_name = (char*)malloc((win_name_len + 1) * sizeof (char));
  if (!c_win_name)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_win_name, win_name, win_name_len);
  c_win_name[c_win_name_len] = '\0';

  *ierr = MPI_Win_set_name(c_win, c_win_name);

  *win = PMPI_Win_c2f(c_win);
  free(c_win_name);
}

#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */

#endif /* !NEED_F2C_CONV */

/**
 * @}
 */
