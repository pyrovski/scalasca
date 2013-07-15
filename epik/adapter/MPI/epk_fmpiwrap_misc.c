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
 * @file  epk_fmpiwrap_misc.c
 *
 * @brief Fortran interface wrappers for miscelaneous and handler conversion
 *        functions
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
/** All uppercase name of MPI_Address */
#define MPI_Address_U MPI_ADDRESS
/** All uppercase name of MPI_Alloc_mem */
#define MPI_Alloc_mem_U MPI_ALLOC_MEM
/** All uppercase name of MPI_Free_mem */
#define MPI_Free_mem_U MPI_FREE_MEM
/** All uppercase name of MPI_Get_address */
#define MPI_Get_address_U MPI_GET_ADDRESS
/** All uppercase name of MPI_Get_version */
#define MPI_Get_version_U MPI_GET_VERSION
/** All uppercase name of MPI_Info_create */
#define MPI_Info_create_U MPI_INFO_CREATE
/** All uppercase name of MPI_Info_delete */
#define MPI_Info_delete_U MPI_INFO_DELETE
/** All uppercase name of MPI_Info_dup */
#define MPI_Info_dup_U MPI_INFO_DUP
/** All uppercase name of MPI_Info_free */
#define MPI_Info_free_U MPI_INFO_FREE
/** All uppercase name of MPI_Info_get */
#define MPI_Info_get_U MPI_INFO_GET
/** All uppercase name of MPI_Info_get_nkeys */
#define MPI_Info_get_nkeys_U MPI_INFO_GET_NKEYS
/** All uppercase name of MPI_Info_get_nthkey */
#define MPI_Info_get_nthkey_U MPI_INFO_GET_NTHKEY
/** All uppercase name of MPI_Info_get_valuelen */
#define MPI_Info_get_valuelen_U MPI_INFO_GET_VALUELEN
/** All uppercase name of MPI_Info_set */
#define MPI_Info_set_U MPI_INFO_SET
/** All uppercase name of MPI_Op_commutative */
#define MPI_Op_commutative_U MPI_OP_COMMUTATIVE
/** All uppercase name of MPI_Op_create */
#define MPI_Op_create_U MPI_OP_CREATE
/** All uppercase name of MPI_Op_free */
#define MPI_Op_free_U MPI_OP_FREE
/** All uppercase name of MPI_Request_get_status */
#define MPI_Request_get_status_U MPI_REQUEST_GET_STATUS

/* lowercase defines */
/** All lowercase name of MPI_Address */
#define MPI_Address_L mpi_address
/** All lowercase name of MPI_Alloc_mem */
#define MPI_Alloc_mem_L mpi_alloc_mem
/** All lowercase name of MPI_Free_mem */
#define MPI_Free_mem_L mpi_free_mem
/** All lowercase name of MPI_Get_address */
#define MPI_Get_address_L mpi_get_address
/** All lowercase name of MPI_Get_version */
#define MPI_Get_version_L mpi_get_version
/** All lowercase name of MPI_Info_create */
#define MPI_Info_create_L mpi_info_create
/** All lowercase name of MPI_Info_delete */
#define MPI_Info_delete_L mpi_info_delete
/** All lowercase name of MPI_Info_dup */
#define MPI_Info_dup_L mpi_info_dup
/** All lowercase name of MPI_Info_free */
#define MPI_Info_free_L mpi_info_free
/** All lowercase name of MPI_Info_get */
#define MPI_Info_get_L mpi_info_get
/** All lowercase name of MPI_Info_get_nkeys */
#define MPI_Info_get_nkeys_L mpi_info_get_nkeys
/** All lowercase name of MPI_Info_get_nthkey */
#define MPI_Info_get_nthkey_L mpi_info_get_nthkey
/** All lowercase name of MPI_Info_get_valuelen */
#define MPI_Info_get_valuelen_L mpi_info_get_valuelen
/** All lowercase name of MPI_Info_set */
#define MPI_Info_set_L mpi_info_set
/** All lowercase name of MPI_Op_commutative */
#define MPI_Op_commutative_L mpi_op_commutative
/** All lowercase name of MPI_Op_create */
#define MPI_Op_create_L mpi_op_create
/** All lowercase name of MPI_Op_free */
#define MPI_Op_free_L mpi_op_free
/** All lowercase name of MPI_Request_get_status */
#define MPI_Request_get_status_L mpi_request_get_status

/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if defined(HAS_MPI_ADDRESS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Address
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup misc
 */
void FSUB(MPI_Address)(void* location,
                       int*  address,
                       int*  ierr)
{
  MPI_Aint c_address;

  *ierr = MPI_Address(location, &c_address);

  *address = (int)c_address;
  if (*address != c_address)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Address\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_ALLOC_MEM) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Alloc_mem
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Alloc_mem)(MPI_Aint* size,
                         MPI_Info* info,
                         void*     baseptr,
                         int*      ierr)
{
  *ierr = MPI_Alloc_mem(*size, *info, baseptr);
}

#endif
#if defined(HAS_MPI_FREE_MEM) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Free_mem
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Free_mem)(void* base,
                        int*  ierr)
{
  *ierr = MPI_Free_mem(base);
}

#endif
#if defined(HAS_MPI_GET_ADDRESS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Get_address
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Get_address)(void*     location,
                           MPI_Aint* address,
                           int*      ierr)
{
  *ierr = MPI_Get_address(location, address);
}

#endif
#if defined(HAS_MPI_GET_VERSION) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Get_version
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.2
 * @ingroup misc
 */
void FSUB(MPI_Get_version)(int* version,
                           int* subversion,
                           int* ierr)
{
  *ierr = MPI_Get_version(version, subversion);
}

#endif
#if defined(HAS_MPI_INFO_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_create)(MPI_Info* info,
                           int*      ierr)
{
  *ierr = MPI_Info_create(info);
}

#endif
#if defined(HAS_MPI_INFO_DELETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_delete
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_delete)(MPI_Info* info,
                           char*     key,
                           int*      ierr,
                           int       key_len)
{
  char* c_key     = NULL;
  int   c_key_len = key_len;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  *ierr = MPI_Info_delete(*info, c_key);

  free(c_key);
}

#endif
#if defined(HAS_MPI_INFO_DUP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_dup
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_dup)(MPI_Info* info,
                        MPI_Info* newinfo,
                        int*      ierr)
{
  *ierr = MPI_Info_dup(*info, newinfo);
}

#endif
#if defined(HAS_MPI_INFO_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_free)(MPI_Info* info,
                         int*      ierr)
{
  *ierr = MPI_Info_free(info);
}

#endif
#if defined(HAS_MPI_INFO_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get)(MPI_Info* info,
                        char*     key,
                        int*      valuelen,
                        char*     value,
                        int*      flag,
                        int*      ierr,
                        int       key_len,
                        int       value_len)
{
  char* c_key       = NULL;
  int   c_key_len   = key_len;
  char* c_value     = NULL;
  int   c_value_len = 0;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  c_value = (char*)malloc((value_len + 1) * sizeof (char));
  if (!c_value)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Info_get(*info, c_key, *valuelen, c_value, flag);

  free(c_key);

  c_value_len = strlen(c_value);
  strncpy(value, c_value, c_value_len);
  memset(value + c_value_len, ' ', value_len - c_value_len);
  free(c_value);
} /* FSUB(MPI_Info_get) */

#endif
#if defined(HAS_MPI_INFO_GET_NKEYS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get_nkeys
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get_nkeys)(MPI_Info* info,
                              int*      nkeys,
                              int*      ierr)
{
  *ierr = MPI_Info_get_nkeys(*info, nkeys);
}

#endif
#if defined(HAS_MPI_INFO_GET_NTHKEY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get_nthkey
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get_nthkey)(MPI_Info* info,
                               int*      n,
                               char*     key,
                               int*      ierr,
                               int       key_len)
{
  char* c_key     = NULL;
  int   c_key_len = 0;

  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Info_get_nthkey(*info, *n, c_key);

  c_key_len = strlen(c_key);
  strncpy(key, c_key, c_key_len);
  memset(key + c_key_len, ' ', key_len - c_key_len);
  free(c_key);
}

#endif
#if defined(HAS_MPI_INFO_GET_VALUELEN) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get_valuelen
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get_valuelen)(MPI_Info* info,
                                 char*     key,
                                 int*      valuelen,
                                 int*      flag,
                                 int*      ierr,
                                 int       key_len)
{
  char* c_key     = NULL;
  int   c_key_len = key_len;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  *ierr = MPI_Info_get_valuelen(*info, c_key, valuelen, flag);

  free(c_key);
}

#endif
#if defined(HAS_MPI_INFO_SET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_set
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_set)(MPI_Info* info,
                        char*     key,
                        char*     value,
                        int*      ierr,
                        int       key_len,
                        int       value_len)
{
  char* c_key       = NULL;
  int   c_key_len   = key_len;
  char* c_value     = NULL;
  int   c_value_len = value_len;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  while ((c_value_len > 0) && (value[c_value_len - 1] == ' '))
  {
    c_value_len--;
  }
  c_value = (char*)malloc((value_len + 1) * sizeof (char));
  if (!c_value)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_value, value, value_len);
  c_value[c_value_len] = '\0';

  *ierr = MPI_Info_set(*info, c_key, c_value);

  free(c_key);
  free(c_value);
} /* FSUB(MPI_Info_set) */

#endif
#if defined(HAS_MPI_OP_COMMUTATIVE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Op_commutative
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup misc
 */
void FSUB(MPI_Op_commutative)(MPI_Op* op,
                              int*    commute,
                              int*    ierr)
{
  *ierr = MPI_Op_commutative(*op, commute);
}

#endif
#if defined(HAS_MPI_OP_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Op_create
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup misc
 */
void FSUB(MPI_Op_create)(MPI_User_function* function,
                         int*               commute,
                         MPI_Op*            op,
                         int*               ierr)
{
  *ierr = MPI_Op_create(function, *commute, op);
}

#endif
#if defined(HAS_MPI_OP_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Op_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup misc
 */
void FSUB(MPI_Op_free)(MPI_Op* op,
                       int*    ierr)
{
  *ierr = MPI_Op_free(op);
}

#endif
#if defined(HAS_MPI_REQUEST_GET_STATUS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Request_get_status)(MPI_Request* request,
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

  *ierr = MPI_Request_get_status(*request, flag, status);
}

#endif

#else /* !NEED_F2C_CONV */

#if defined(HAS_MPI_ADDRESS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Address
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup misc
 */
void FSUB(MPI_Address)(void*     location,
                       MPI_Fint* address,
                       MPI_Fint* ierr)
{
  MPI_Aint c_address;

  *ierr = MPI_Address(location, &c_address);

  *address = (MPI_Fint)c_address;
  if (*address != c_address)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Address\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_ALLOC_MEM) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Alloc_mem
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Alloc_mem)(MPI_Aint* size,
                         MPI_Fint* info,
                         void*     baseptr,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Alloc_mem(*size, PMPI_Info_f2c(*info), baseptr);
}

#endif
#if defined(HAS_MPI_FREE_MEM) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Free_mem
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Free_mem)(void*     base,
                        MPI_Fint* ierr)
{
  *ierr = MPI_Free_mem(base);
}

#endif
#if defined(HAS_MPI_GET_ADDRESS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Get_address
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Get_address)(void*     location,
                           MPI_Aint* address,
                           MPI_Fint* ierr)
{
  *ierr = MPI_Get_address(location, address);
}

#endif
#if defined(HAS_MPI_GET_VERSION) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Get_version
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.2
 * @ingroup misc
 */
void FSUB(MPI_Get_version)(MPI_Fint* version,
                           MPI_Fint* subversion,
                           MPI_Fint* ierr)
{
  *ierr = MPI_Get_version(version, subversion);
}

#endif
#if defined(HAS_MPI_INFO_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_create)(MPI_Fint* info,
                           MPI_Fint* ierr)
{
  MPI_Info c_info;

  *ierr = MPI_Info_create(&c_info);

  *info = PMPI_Info_c2f(c_info);
}

#endif
#if defined(HAS_MPI_INFO_DELETE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_delete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_delete)(MPI_Fint* info,
                           char*     key,
                           MPI_Fint* ierr,
                           int       key_len)
{
  MPI_Info c_info    = PMPI_Info_f2c(*info);
  char*    c_key     = NULL;
  int      c_key_len = key_len;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  *ierr = MPI_Info_delete(c_info, c_key);

  *info = PMPI_Info_c2f(c_info);
  free(c_key);
}

#endif
#if defined(HAS_MPI_INFO_DUP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_dup
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_dup)(MPI_Fint* info,
                        MPI_Fint* newinfo,
                        MPI_Fint* ierr)
{
  MPI_Info c_newinfo;

  *ierr = MPI_Info_dup(PMPI_Info_f2c(*info), &c_newinfo);

  *newinfo = PMPI_Info_c2f(c_newinfo);
}

#endif
#if defined(HAS_MPI_INFO_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_free)(MPI_Fint* info,
                         MPI_Fint* ierr)
{
  MPI_Info c_info = PMPI_Info_f2c(*info);

  *ierr = MPI_Info_free(&c_info);

  *info = PMPI_Info_c2f(c_info);
}

#endif
#if defined(HAS_MPI_INFO_GET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get)(MPI_Fint* info,
                        char*     key,
                        MPI_Fint* valuelen,
                        char*     value,
                        MPI_Fint* flag,
                        MPI_Fint* ierr,
                        int       key_len,
                        int       value_len)
{
  char* c_key       = NULL;
  int   c_key_len   = key_len;
  char* c_value     = NULL;
  int   c_value_len = 0;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  c_value = (char*)malloc((value_len + 1) * sizeof (char));
  if (!c_value)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Info_get(PMPI_Info_f2c(
                         *info), c_key, *valuelen, c_value, flag);

  free(c_key);

  c_value_len = strlen(c_value);
  strncpy(value, c_value, c_value_len);
  memset(value + c_value_len, ' ', value_len - c_value_len);
  free(c_value);
} /* FSUB(MPI_Info_get) */

#endif
#if defined(HAS_MPI_INFO_GET_NKEYS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get_nkeys
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get_nkeys)(MPI_Fint* info,
                              MPI_Fint* nkeys,
                              MPI_Fint* ierr)
{
  *ierr = MPI_Info_get_nkeys(PMPI_Info_f2c(*info), nkeys);
}

#endif
#if defined(HAS_MPI_INFO_GET_NTHKEY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get_nthkey
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get_nthkey)(MPI_Fint* info,
                               MPI_Fint* n,
                               char*     key,
                               MPI_Fint* ierr,
                               int       key_len)
{
  char* c_key     = NULL;
  int   c_key_len = 0;

  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Info_get_nthkey(PMPI_Info_f2c(*info), *n, c_key);

  c_key_len = strlen(c_key);
  strncpy(key, c_key, c_key_len);
  memset(key + c_key_len, ' ', key_len - c_key_len);
  free(c_key);
}

#endif
#if defined(HAS_MPI_INFO_GET_VALUELEN) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_get_valuelen
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_get_valuelen)(MPI_Fint* info,
                                 char*     key,
                                 MPI_Fint* valuelen,
                                 MPI_Fint* flag,
                                 MPI_Fint* ierr,
                                 int       key_len)
{
  char* c_key     = NULL;
  int   c_key_len = key_len;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  *ierr = MPI_Info_get_valuelen(PMPI_Info_f2c(
                                  *info), c_key, valuelen, flag);

  free(c_key);
}

#endif
#if defined(HAS_MPI_INFO_SET) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Info_set
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Info_set)(MPI_Fint* info,
                        char*     key,
                        char*     value,
                        MPI_Fint* ierr,
                        int       key_len,
                        int       value_len)
{
  MPI_Info c_info      = PMPI_Info_f2c(*info);
  char*    c_key       = NULL;
  int      c_key_len   = key_len;
  char*    c_value     = NULL;
  int      c_value_len = value_len;

  while ((c_key_len > 0) && (key[c_key_len - 1] == ' '))
  {
    c_key_len--;
  }
  c_key = (char*)malloc((key_len + 1) * sizeof (char));
  if (!c_key)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_key, key, key_len);
  c_key[c_key_len] = '\0';

  while ((c_value_len > 0) && (value[c_value_len - 1] == ' '))
  {
    c_value_len--;
  }
  c_value = (char*)malloc((value_len + 1) * sizeof (char));
  if (!c_value)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_value, value, value_len);
  c_value[c_value_len] = '\0';

  *ierr = MPI_Info_set(c_info, c_key, c_value);

  *info = PMPI_Info_c2f(c_info);
  free(c_key);
  free(c_value);
} /* FSUB(MPI_Info_set) */

#endif
#if defined(HAS_MPI_OP_COMMUTATIVE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Op_commutative
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.2
 * @ingroup misc
 */
void FSUB(MPI_Op_commutative)(MPI_Fint* op,
                              MPI_Fint* commute,
                              MPI_Fint* ierr)
{
  *ierr = MPI_Op_commutative(PMPI_Op_f2c(*op), commute);
}

#endif
#if defined(HAS_MPI_OP_CREATE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Op_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup misc
 */
void FSUB(MPI_Op_create)(void*     function,
                         MPI_Fint* commute,
                         MPI_Fint* op,
                         MPI_Fint* ierr)
{
  MPI_Op c_op;

  *ierr = MPI_Op_create((MPI_User_function*)function, *commute, &c_op);

  *op = PMPI_Op_c2f(c_op);
}

#endif
#if defined(HAS_MPI_OP_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Op_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup misc
 */
void FSUB(MPI_Op_free)(MPI_Fint* op,
                       MPI_Fint* ierr)
{
  MPI_Op c_op = PMPI_Op_f2c(*op);

  *ierr = MPI_Op_free(&c_op);

  *op = PMPI_Op_c2f(c_op);
}

#endif
#if defined(HAS_MPI_REQUEST_GET_STATUS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_MISC)
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup misc
 */
void FSUB(MPI_Request_get_status)(MPI_Fint* request,
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

  *ierr = MPI_Request_get_status(PMPI_Request_f2c(
                                   *request), flag, c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
}

#endif

#endif

/**
 * @}
 */
