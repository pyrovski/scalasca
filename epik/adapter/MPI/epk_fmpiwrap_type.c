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
 * @file  epk_fmpiwrap_type.c
 *
 * @brief Fortran interface wrappers for datatype functions
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
/** All uppercase name of MPI_Pack */
#define MPI_Pack_U MPI_PACK
/** All uppercase name of MPI_Pack_external */
#define MPI_Pack_external_U MPI_PACK_EXTERNAL
/** All uppercase name of MPI_Pack_external_size */
#define MPI_Pack_external_size_U MPI_PACK_EXTERNAL_SIZE
/** All uppercase name of MPI_Pack_size */
#define MPI_Pack_size_U MPI_PACK_SIZE
/** All uppercase name of MPI_Type_commit */
#define MPI_Type_commit_U MPI_TYPE_COMMIT
/** All uppercase name of MPI_Type_contiguous */
#define MPI_Type_contiguous_U MPI_TYPE_CONTIGUOUS
/** All uppercase name of MPI_Type_create_darray */
#define MPI_Type_create_darray_U MPI_TYPE_CREATE_DARRAY
/** All uppercase name of MPI_Type_create_f90_complex */
#define MPI_Type_create_f90_complex_U MPI_TYPE_CREATE_F90_COMPLEX
/** All uppercase name of MPI_Type_create_f90_integer */
#define MPI_Type_create_f90_integer_U MPI_TYPE_CREATE_F90_INTEGER
/** All uppercase name of MPI_Type_create_f90_real */
#define MPI_Type_create_f90_real_U MPI_TYPE_CREATE_F90_REAL
/** All uppercase name of MPI_Type_create_hindexed */
#define MPI_Type_create_hindexed_U MPI_TYPE_CREATE_HINDEXED
/** All uppercase name of MPI_Type_create_hvector */
#define MPI_Type_create_hvector_U MPI_TYPE_CREATE_HVECTOR
/** All uppercase name of MPI_Type_create_indexed_block */
#define MPI_Type_create_indexed_block_U MPI_TYPE_CREATE_INDEXED_BLOCK
/** All uppercase name of MPI_Type_create_resized */
#define MPI_Type_create_resized_U MPI_TYPE_CREATE_RESIZED
/** All uppercase name of MPI_Type_create_struct */
#define MPI_Type_create_struct_U MPI_TYPE_CREATE_STRUCT
/** All uppercase name of MPI_Type_create_subarray */
#define MPI_Type_create_subarray_U MPI_TYPE_CREATE_SUBARRAY
/** All uppercase name of MPI_Type_dup */
#define MPI_Type_dup_U MPI_TYPE_DUP
/** All uppercase name of MPI_Type_extent */
#define MPI_Type_extent_U MPI_TYPE_EXTENT
/** All uppercase name of MPI_Type_free */
#define MPI_Type_free_U MPI_TYPE_FREE
/** All uppercase name of MPI_Type_get_contents */
#define MPI_Type_get_contents_U MPI_TYPE_GET_CONTENTS
/** All uppercase name of MPI_Type_get_envelope */
#define MPI_Type_get_envelope_U MPI_TYPE_GET_ENVELOPE
/** All uppercase name of MPI_Type_get_extent */
#define MPI_Type_get_extent_U MPI_TYPE_GET_EXTENT
/** All uppercase name of MPI_Type_get_true_extent */
#define MPI_Type_get_true_extent_U MPI_TYPE_GET_TRUE_EXTENT
/** All uppercase name of MPI_Type_hindexed */
#define MPI_Type_hindexed_U MPI_TYPE_HINDEXED
/** All uppercase name of MPI_Type_hvector */
#define MPI_Type_hvector_U MPI_TYPE_HVECTOR
/** All uppercase name of MPI_Type_indexed */
#define MPI_Type_indexed_U MPI_TYPE_INDEXED
/** All uppercase name of MPI_Type_lb */
#define MPI_Type_lb_U MPI_TYPE_LB
/** All uppercase name of MPI_Type_match_size */
#define MPI_Type_match_size_U MPI_TYPE_MATCH_SIZE
/** All uppercase name of MPI_Type_size */
#define MPI_Type_size_U MPI_TYPE_SIZE
/** All uppercase name of MPI_Type_struct */
#define MPI_Type_struct_U MPI_TYPE_STRUCT
/** All uppercase name of MPI_Type_ub */
#define MPI_Type_ub_U MPI_TYPE_UB
/** All uppercase name of MPI_Type_vector */
#define MPI_Type_vector_U MPI_TYPE_VECTOR
/** All uppercase name of MPI_Unpack */
#define MPI_Unpack_U MPI_UNPACK
/** All uppercase name of MPI_Unpack_external */
#define MPI_Unpack_external_U MPI_UNPACK_EXTERNAL

/** All uppercase name of MPI_Type_create_keyval */
#define MPI_Type_create_keyval_U MPI_TYPE_CREATE_KEYVAL
/** All uppercase name of MPI_Type_delete_attr */
#define MPI_Type_delete_attr_U MPI_TYPE_DELETE_ATTR
/** All uppercase name of MPI_Type_free_keyval */
#define MPI_Type_free_keyval_U MPI_TYPE_FREE_KEYVAL
/** All uppercase name of MPI_Type_get_attr */
#define MPI_Type_get_attr_U MPI_TYPE_GET_ATTR
/** All uppercase name of MPI_Type_get_name */
#define MPI_Type_get_name_U MPI_TYPE_GET_NAME
/** All uppercase name of MPI_Type_set_attr */
#define MPI_Type_set_attr_U MPI_TYPE_SET_ATTR
/** All uppercase name of MPI_Type_set_name */
#define MPI_Type_set_name_U MPI_TYPE_SET_NAME

/* lowercase defines */
/** All lowercase name of MPI_Pack */
#define MPI_Pack_L mpi_pack
/** All lowercase name of MPI_Pack_external */
#define MPI_Pack_external_L mpi_pack_external
/** All lowercase name of MPI_Pack_external_size */
#define MPI_Pack_external_size_L mpi_pack_external_size
/** All lowercase name of MPI_Pack_size */
#define MPI_Pack_size_L mpi_pack_size
/** All lowercase name of MPI_Type_commit */
#define MPI_Type_commit_L mpi_type_commit
/** All lowercase name of MPI_Type_contiguous */
#define MPI_Type_contiguous_L mpi_type_contiguous
/** All lowercase name of MPI_Type_create_darray */
#define MPI_Type_create_darray_L mpi_type_create_darray
/** All lowercase name of MPI_Type_create_f90_complex */
#define MPI_Type_create_f90_complex_L mpi_type_create_f90_complex
/** All lowercase name of MPI_Type_create_f90_integer */
#define MPI_Type_create_f90_integer_L mpi_type_create_f90_integer
/** All lowercase name of MPI_Type_create_f90_real */
#define MPI_Type_create_f90_real_L mpi_type_create_f90_real
/** All lowercase name of MPI_Type_create_hindexed */
#define MPI_Type_create_hindexed_L mpi_type_create_hindexed
/** All lowercase name of MPI_Type_create_hvector */
#define MPI_Type_create_hvector_L mpi_type_create_hvector
/** All lowercase name of MPI_Type_create_indexed_block */
#define MPI_Type_create_indexed_block_L mpi_type_create_indexed_block
/** All lowercase name of MPI_Type_create_resized */
#define MPI_Type_create_resized_L mpi_type_create_resized
/** All lowercase name of MPI_Type_create_struct */
#define MPI_Type_create_struct_L mpi_type_create_struct
/** All lowercase name of MPI_Type_create_subarray */
#define MPI_Type_create_subarray_L mpi_type_create_subarray
/** All lowercase name of MPI_Type_dup */
#define MPI_Type_dup_L mpi_type_dup
/** All lowercase name of MPI_Type_extent */
#define MPI_Type_extent_L mpi_type_extent
/** All lowercase name of MPI_Type_free */
#define MPI_Type_free_L mpi_type_free
/** All lowercase name of MPI_Type_get_contents */
#define MPI_Type_get_contents_L mpi_type_get_contents
/** All lowercase name of MPI_Type_get_envelope */
#define MPI_Type_get_envelope_L mpi_type_get_envelope
/** All lowercase name of MPI_Type_get_extent */
#define MPI_Type_get_extent_L mpi_type_get_extent
/** All lowercase name of MPI_Type_get_true_extent */
#define MPI_Type_get_true_extent_L mpi_type_get_true_extent
/** All lowercase name of MPI_Type_hindexed */
#define MPI_Type_hindexed_L mpi_type_hindexed
/** All lowercase name of MPI_Type_hvector */
#define MPI_Type_hvector_L mpi_type_hvector
/** All lowercase name of MPI_Type_indexed */
#define MPI_Type_indexed_L mpi_type_indexed
/** All lowercase name of MPI_Type_lb */
#define MPI_Type_lb_L mpi_type_lb
/** All lowercase name of MPI_Type_match_size */
#define MPI_Type_match_size_L mpi_type_match_size
/** All lowercase name of MPI_Type_size */
#define MPI_Type_size_L mpi_type_size
/** All lowercase name of MPI_Type_struct */
#define MPI_Type_struct_L mpi_type_struct
/** All lowercase name of MPI_Type_ub */
#define MPI_Type_ub_L mpi_type_ub
/** All lowercase name of MPI_Type_vector */
#define MPI_Type_vector_L mpi_type_vector
/** All lowercase name of MPI_Unpack */
#define MPI_Unpack_L mpi_unpack
/** All lowercase name of MPI_Unpack_external */
#define MPI_Unpack_external_L mpi_unpack_external

/** All lowercase name of MPI_Type_create_keyval */
#define MPI_Type_create_keyval_L mpi_type_create_keyval
/** All lowercase name of MPI_Type_delete_attr */
#define MPI_Type_delete_attr_L mpi_type_delete_attr
/** All lowercase name of MPI_Type_free_keyval */
#define MPI_Type_free_keyval_L mpi_type_free_keyval
/** All lowercase name of MPI_Type_get_attr */
#define MPI_Type_get_attr_L mpi_type_get_attr
/** All lowercase name of MPI_Type_get_name */
#define MPI_Type_get_name_L mpi_type_get_name
/** All lowercase name of MPI_Type_set_attr */
#define MPI_Type_set_attr_L mpi_type_set_attr
/** All lowercase name of MPI_Type_set_name */
#define MPI_Type_set_name_L mpi_type_set_name

/**
 * @name Fortran wrappers for general functions
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if defined(HAS_MPI_PACK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Pack)(void*         inbuf,
                    int*          incount,
                    MPI_Datatype* datatype,
                    void*         outbuf,
                    int*          outsize,
                    int*          position,
                    MPI_Comm*     comm,
                    int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (inbuf == epk_mpif_bottom)
  {
    inbuf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Pack(inbuf,
                   *incount,
                   *datatype,
                   outbuf,
                   *outsize,
                   position,
                   *comm);
}

#endif
#if defined(HAS_MPI_PACK_EXTERNAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack_external
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Pack_external)(char*         datarep,
                             void*         inbuf,
                             int*          incount,
                             MPI_Datatype* datatype,
                             void*         outbuf,
                             MPI_Aint*     outsize,
                             MPI_Aint*     position,
                             int*          ierr,
                             int           datarep_len)
{
  char* c_datarep     = NULL;
  int   c_datarep_len = datarep_len;

  while ((c_datarep_len > 0) && (datarep[c_datarep_len - 1] == ' '))
  {
    c_datarep_len--;
  }
  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_datarep, datarep, datarep_len);
  c_datarep[c_datarep_len] = '\0';

  #if defined(HAS_MPI_BOTTOM)
  if (inbuf == epk_mpif_bottom)
  {
    inbuf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Pack_external(c_datarep,
                            inbuf,
                            *incount,
                            *datatype,
                            outbuf,
                            *outsize,
                            position);

  free(c_datarep);
} /* FSUB(MPI_Pack_external) */

#endif
#if defined(HAS_MPI_PACK_EXTERNAL_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack_external_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Pack_external_size)(char*         datarep,
                                  int*          incount,
                                  MPI_Datatype* datatype,
                                  MPI_Aint*     size,
                                  int*          ierr,
                                  int           datarep_len)
{
  char* c_datarep     = NULL;
  int   c_datarep_len = datarep_len;

  while ((c_datarep_len > 0) && (datarep[c_datarep_len - 1] == ' '))
  {
    c_datarep_len--;
  }
  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_datarep, datarep, datarep_len);
  c_datarep[c_datarep_len] = '\0';

  *ierr = MPI_Pack_external_size(c_datarep, *incount, *datatype, size);

  free(c_datarep);
}

#endif
#if defined(HAS_MPI_PACK_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Pack_size)(int*          incount,
                         MPI_Datatype* datatype,
                         MPI_Comm*     comm,
                         int*          size,
                         int*          ierr)
{
  *ierr = MPI_Pack_size(*incount, *datatype, *comm, size);
}

#endif
#if defined(HAS_MPI_TYPE_COMMIT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_commit
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_commit)(MPI_Datatype* datatype,
                           int*          ierr)
{
  *ierr = MPI_Type_commit(datatype);
}

#endif
#if defined(HAS_MPI_TYPE_CONTIGUOUS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_contiguous
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_contiguous)(int*          count,
                               MPI_Datatype* oldtype,
                               MPI_Datatype* newtype,
                               int*          ierr)
{
  *ierr = MPI_Type_contiguous(*count, *oldtype, newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_DARRAY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_darray
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_darray)(int*          size,
                                  int*          rank,
                                  int*          ndims,
                                  int           array_of_gsizes[],
                                  int           array_of_distribs[],
                                  int           array_of_dargs[],
                                  int           array_of_psizes[],
                                  int*          order,
                                  MPI_Datatype* oldtype,
                                  MPI_Datatype* newtype,
                                  int*          ierr)
{
  *ierr = MPI_Type_create_darray(*size,
                                 *rank,
                                 *ndims,
                                 array_of_gsizes,
                                 array_of_distribs,
                                 array_of_dargs,
                                 array_of_psizes,
                                 *order,
                                 *oldtype,
                                 newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_F90_COMPLEX) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_f90_complex
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_f90_complex)(int*          p,
                                       int*          r,
                                       MPI_Datatype* newtype,
                                       int*          ierr)
{
  *ierr = MPI_Type_create_f90_complex(*p, *r, newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_F90_INTEGER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_f90_integer
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_f90_integer)(int*          r,
                                       MPI_Datatype* newtype,
                                       int*          ierr)
{
  *ierr = MPI_Type_create_f90_integer(*r, newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_F90_REAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_f90_real
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_f90_real)(int*          p,
                                    int*          r,
                                    MPI_Datatype* newtype,
                                    int*          ierr)
{
  *ierr = MPI_Type_create_f90_real(*p, *r, newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_HINDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_hindexed
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_hindexed)(int*          count,
                                    int           array_of_blocklengths[],
                                    MPI_Aint      array_of_displacements[],
                                    MPI_Datatype* oldtype,
                                    MPI_Datatype* newtype,
                                    int*          ierr)
{
  *ierr = MPI_Type_create_hindexed(*count,
                                   array_of_blocklengths,
                                   array_of_displacements,
                                   *oldtype,
                                   newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_HVECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_hvector
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_hvector)(int*          count,
                                   int*          blocklength,
                                   MPI_Aint*     stride,
                                   MPI_Datatype* oldtype,
                                   MPI_Datatype* newtype,
                                   int*          ierr)
{
  *ierr = MPI_Type_create_hvector(*count,
                                  *blocklength,
                                  *stride,
                                  *oldtype,
                                  newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_INDEXED_BLOCK) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_indexed_block
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_indexed_block)(
  int*          count,
  int*          blocklength,
  int
  array_of_displacements[],
  MPI_Datatype* oldtype,
  MPI_Datatype* newtype,
  int*          ierr)
{
  *ierr = MPI_Type_create_indexed_block(*count,
                                        *blocklength,
                                        array_of_displacements,
                                        *oldtype,
                                        newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_RESIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_resized
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_resized)(MPI_Datatype* oldtype,
                                   MPI_Aint*     lb,
                                   MPI_Aint*     extent,
                                   MPI_Datatype* newtype,
                                   int*          ierr)
{
  *ierr = MPI_Type_create_resized(*oldtype, *lb, *extent, newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_STRUCT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_struct
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_struct)(int*          count,
                                  int           array_of_blocklengths[],
                                  MPI_Aint      array_of_displacements[],
                                  MPI_Datatype  array_of_types[],
                                  MPI_Datatype* newtype,
                                  int*          ierr)
{
  *ierr = MPI_Type_create_struct(*count,
                                 array_of_blocklengths,
                                 array_of_displacements,
                                 array_of_types,
                                 newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_SUBARRAY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_subarray
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_subarray)(int*          ndims,
                                    int           array_of_sizes[],
                                    int           array_of_subsizes[],
                                    int           array_of_starts[],
                                    int*          order,
                                    MPI_Datatype* oldtype,
                                    MPI_Datatype* newtype,
                                    int*          ierr)
{
  *ierr = MPI_Type_create_subarray(*ndims,
                                   array_of_sizes,
                                   array_of_subsizes,
                                   array_of_starts,
                                   *order,
                                   *oldtype,
                                   newtype);
}

#endif
#if defined(HAS_MPI_TYPE_DUP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_dup
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_dup)(MPI_Datatype* type,
                        MPI_Datatype* newtype,
                        int*          ierr)
{
  *ierr = MPI_Type_dup(*type, newtype);
}

#endif
#if defined(HAS_MPI_TYPE_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_extent
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_extent)(MPI_Datatype* datatype,
                           int*          extent,
                           int*          ierr)
{
  MPI_Aint c_extent;

  *ierr = MPI_Type_extent(*datatype, &c_extent);

  *extent = (int)c_extent;
  if (*extent != c_extent)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Type_extent\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_TYPE_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_free
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_free)(MPI_Datatype* datatype,
                         int*          ierr)
{
  *ierr = MPI_Type_free(datatype);
}

#endif
#if defined(HAS_MPI_TYPE_GET_CONTENTS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_get_contents
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_get_contents)(MPI_Datatype* datatype,
                                 int*          max_integers,
                                 int*          max_addresses,
                                 int*          max_datatypes,
                                 int           array_of_integers[],
                                 MPI_Aint      array_of_addresses[],
                                 MPI_Datatype  array_of_datatypes[],
                                 int*          ierr)
{
  *ierr = MPI_Type_get_contents(*datatype,
                                *max_integers,
                                *max_addresses,
                                *max_datatypes,
                                array_of_integers,
                                array_of_addresses,
                                array_of_datatypes);
}

#endif
#if defined(HAS_MPI_TYPE_GET_ENVELOPE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_get_envelope
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_get_envelope)(MPI_Datatype* datatype,
                                 int*          num_integers,
                                 int*          num_addresses,
                                 int*          num_datatypes,
                                 int*          combiner,
                                 int*          ierr)
{
  *ierr = MPI_Type_get_envelope(*datatype,
                                num_integers,
                                num_addresses,
                                num_datatypes,
                                combiner);
}

#endif
#if defined(HAS_MPI_TYPE_GET_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_get_extent
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_get_extent)(MPI_Datatype* datatype,
                               MPI_Aint*     lb,
                               MPI_Aint*     extent,
                               int*          ierr)
{
  *ierr = MPI_Type_get_extent(*datatype, lb, extent);
}

#endif
#if defined(HAS_MPI_TYPE_GET_TRUE_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_get_true_extent
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_get_true_extent)(MPI_Datatype* datatype,
                                    MPI_Aint*     true_lb,
                                    MPI_Aint*     true_extent,
                                    int*          ierr)
{
  *ierr = MPI_Type_get_true_extent(*datatype, true_lb, true_extent);
}

#endif
#if defined(HAS_MPI_TYPE_HINDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_hindexed
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_hindexed)(int*          count,
                             int*          array_of_blocklengths,
                             int*          array_of_displacements,
                             MPI_Datatype* oldtype,
                             MPI_Datatype* newtype,
                             int*          ierr)
{
  int       i;
  MPI_Aint* c_array_of_displacements;

  c_array_of_displacements = (MPI_Aint*)malloc(*count * sizeof (MPI_Aint));
  for (i = 0; i < *count; ++i)
  {
    c_array_of_displacements[i] = array_of_displacements[i];
  }

  *ierr = MPI_Type_hindexed(*count,
                            array_of_blocklengths,
                            c_array_of_displacements,
                            *oldtype,
                            newtype);

  free(c_array_of_displacements);
}

#endif
#if defined(HAS_MPI_TYPE_HVECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_hvector
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_hvector)(int*          count,
                            int*          blocklength,
                            int*          stride,
                            MPI_Datatype* oldtype,
                            MPI_Datatype* newtype,
                            int*          ierr)
{
  MPI_Aint c_stride = *stride;

  *ierr = MPI_Type_hvector(*count,
                           *blocklength,
                           *&c_stride,
                           *oldtype,
                           newtype);
}

#endif
#if defined(HAS_MPI_TYPE_INDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_indexed
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_indexed)(int*          count,
                            int*          array_of_blocklengths,
                            int*          array_of_displacements,
                            MPI_Datatype* oldtype,
                            MPI_Datatype* newtype,
                            int*          ierr)
{
  *ierr = MPI_Type_indexed(*count,
                           array_of_blocklengths,
                           array_of_displacements,
                           *oldtype,
                           newtype);
}

#endif
#if defined(HAS_MPI_TYPE_LB) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_lb
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_lb)(MPI_Datatype* datatype,
                       int*          displacement,
                       int*          ierr)
{
  MPI_Aint c_displacement;

  *ierr = MPI_Type_lb(*datatype, &c_displacement);

  *displacement = (int)c_displacement;
  if (*displacement != c_displacement)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Type_lb\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_TYPE_MATCH_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_match_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_match_size)(int*          typeclass,
                               int*          size,
                               MPI_Datatype* type,
                               int*          ierr)
{
  *ierr = MPI_Type_match_size(*typeclass, *size, type);
}

#endif
#if defined(HAS_MPI_TYPE_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_size)(MPI_Datatype* datatype,
                         int*          size,
                         int*          ierr)
{
  *ierr = MPI_Type_size(*datatype, size);
}

#endif
#if defined(HAS_MPI_TYPE_STRUCT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_struct
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_struct)(int*          count,
                           int*          array_of_blocklengths,
                           int*          array_of_displacements,
                           MPI_Datatype* array_of_types,
                           MPI_Datatype* newtype,
                           int*          ierr)
{
  int       i;
  MPI_Aint* c_array_of_displacements;

  c_array_of_displacements = (MPI_Aint*)malloc(*count * sizeof (MPI_Aint));
  for (i = 0; i < *count; ++i)
  {
    c_array_of_displacements[i] = array_of_displacements[i];
  }

  *ierr = MPI_Type_struct(*count,
                          array_of_blocklengths,
                          c_array_of_displacements,
                          array_of_types,
                          newtype);

  free(c_array_of_displacements);
}

#endif
#if defined(HAS_MPI_TYPE_UB) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_ub
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_ub)(MPI_Datatype* datatype,
                       int*          displacement,
                       int*          ierr)
{
  MPI_Aint c_displacement;

  *ierr = MPI_Type_ub(*datatype, &c_displacement);

  *displacement = (int)c_displacement;
  if (*displacement != c_displacement)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Type_ub\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_TYPE_VECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_vector
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_vector)(int*          count,
                           int*          blocklength,
                           int*          stride,
                           MPI_Datatype* oldtype,
                           MPI_Datatype* newtype,
                           int*          ierr)
{
  *ierr = MPI_Type_vector(*count, *blocklength, *stride, *oldtype, newtype);
}

#endif
#if defined(HAS_MPI_UNPACK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Unpack
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Unpack)(void*         inbuf,
                      int*          insize,
                      int*          position,
                      void*         outbuf,
                      int*          outcount,
                      MPI_Datatype* datatype,
                      MPI_Comm*     comm,
                      int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (outbuf == epk_mpif_bottom)
  {
    outbuf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Unpack(inbuf,
                     *insize,
                     position,
                     outbuf,
                     *outcount,
                     *datatype,
                     *comm);
}

#endif
#if defined(HAS_MPI_UNPACK_EXTERNAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Unpack_external
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Unpack_external)(char*         datarep,
                               void*         inbuf,
                               MPI_Aint*     insize,
                               MPI_Aint*     position,
                               void*         outbuf,
                               int*          outcount,
                               MPI_Datatype* datatype,
                               int*          ierr,
                               int           datarep_len)
{
  char* c_datarep     = NULL;
  int   c_datarep_len = datarep_len;

  while ((c_datarep_len > 0) && (datarep[c_datarep_len - 1] == ' '))
  {
    c_datarep_len--;
  }
  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_datarep, datarep, datarep_len);
  c_datarep[c_datarep_len] = '\0';

  #if defined(HAS_MPI_BOTTOM)
  if (outbuf == epk_mpif_bottom)
  {
    outbuf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Unpack_external(c_datarep,
                              inbuf,
                              *insize,
                              position,
                              outbuf,
                              *outcount,
                              *datatype);

  free(c_datarep);
} /* FSUB(MPI_Unpack_external) */

#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if defined(HAS_MPI_TYPE_CREATE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_create_keyval
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_create_keyval)(
  MPI_Type_copy_attr_function* type_copy_attr_fn,
  MPI_Type_delete_attr_function*
  type_delete_attr_fn,
  int*
  type_keyval,
  void*
  extra_state,
  int*                         ierr)
{
  *ierr = MPI_Type_create_keyval(type_copy_attr_fn,
                                 type_delete_attr_fn,
                                 type_keyval,
                                 extra_state);
}

#endif
#if defined(HAS_MPI_TYPE_DELETE_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_delete_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_delete_attr)(MPI_Datatype* type,
                                int*          type_keyval,
                                int*          ierr)
{
  *ierr = MPI_Type_delete_attr(*type, *type_keyval);
}

#endif
#if defined(HAS_MPI_TYPE_FREE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_free_keyval
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_free_keyval)(int* type_keyval,
                                int* ierr)
{
  *ierr = MPI_Type_free_keyval(type_keyval);
}

#endif
#if defined(HAS_MPI_TYPE_GET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_get_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_get_attr)(MPI_Datatype* type,
                             int*          type_keyval,
                             void*         attribute_val,
                             int*          flag,
                             int*          ierr)
{
  *ierr = MPI_Type_get_attr(*type, *type_keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_TYPE_GET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_get_name
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_get_name)(MPI_Datatype* type,
                             char*         type_name,
                             int*          resultlen,
                             int*          ierr,
                             int           type_name_len)
{
  char* c_type_name     = NULL;
  int   c_type_name_len = 0;

  c_type_name = (char*)malloc((type_name_len + 1) * sizeof (char));
  if (!c_type_name)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Type_get_name(*type, c_type_name, resultlen);

  c_type_name_len = strlen(c_type_name);
  strncpy(type_name, c_type_name, c_type_name_len);
  memset(type_name + c_type_name_len, ' ', type_name_len - c_type_name_len);
  free(c_type_name);
}

#endif
#if defined(HAS_MPI_TYPE_SET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_set_attr
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_set_attr)(MPI_Datatype* type,
                             int*          type_keyval,
                             void*         attribute_val,
                             int*          ierr)
{
  *ierr = MPI_Type_set_attr(*type, *type_keyval, attribute_val);
}

#endif
#if defined(HAS_MPI_TYPE_SET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_set_name
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_set_name)(MPI_Datatype* type,
                             char*         type_name,
                             int*          ierr,
                             int           type_name_len)
{
  char* c_type_name     = NULL;
  int   c_type_name_len = type_name_len;

  while ((c_type_name_len > 0) && (type_name[c_type_name_len - 1] == ' '))
  {
    c_type_name_len--;
  }
  c_type_name = (char*)malloc((type_name_len + 1) * sizeof (char));
  if (!c_type_name)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_type_name, type_name, type_name_len);
  c_type_name[c_type_name_len] = '\0';

  *ierr = MPI_Type_set_name(*type, c_type_name);

  free(c_type_name);
}

#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */

#else /* !NEED_F2C_CONV */

#if defined (HAS_MPI_TYPE_CREATE_STRUCT) && !defined(NO_MPI_TYPE)
/**
 * Manual measurement wrapper for MPI_Type_create_struct
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_type
 */
void FSUB(MPI_Type_create_struct)(MPI_Fint* count,
                                  MPI_Fint* array_of_blocklengths,
                                  MPI_Aint  array_of_displacements[],
                                  MPI_Fint* array_of_types,
                                  MPI_Fint* newtype,
                                  MPI_Fint* ierr)
{
  MPI_Fint      i;
  MPI_Datatype  c_newtype;
  MPI_Datatype* c_array_of_types =
    (MPI_Datatype*)malloc(*count * sizeof (MPI_Datatype));

  if (!c_array_of_types)
  {
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < *count; ++i)
  {
    c_array_of_types[i] = PMPI_Type_f2c(array_of_types[i]);
  }
  *ierr = MPI_Type_create_struct(*count, array_of_blocklengths,
                                 array_of_displacements,
                                 c_array_of_types, &c_newtype);
  *newtype = PMPI_Type_c2f(c_newtype);

  free(c_array_of_types);
}

#endif

#if defined (HAS_MPI_TYPE_GET_CONTENTS) && !defined(NO_MPI_TYPE)
/**
 * Manual measurement wrapper for MPI_Type_get_contents
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_type
 */
void FSUB(MPI_Type_get_contents)(MPI_Fint* datatype,
                                 MPI_Fint* max_integers,
                                 MPI_Fint* max_addresses,
                                 MPI_Fint* max_datatypes,
                                 MPI_Fint* array_of_integers,
                                 MPI_Aint* array_of_addresses,
                                 MPI_Fint* array_of_datatypes,
                                 MPI_Fint* ierr)
{
  MPI_Fint      i;
  MPI_Datatype* c_array_of_datatypes = NULL;

  /* provide storage for C Datatypes */
  c_array_of_datatypes =
    (MPI_Datatype*)malloc(*max_datatypes * sizeof (MPI_Datatype));
  if (!c_array_of_datatypes)
  {
    exit(EXIT_FAILURE);
  }
  *ierr = MPI_Type_get_contents(PMPI_Type_f2c(*datatype),
                                *max_integers,
                                *max_addresses,
                                *max_datatypes,
                                array_of_integers,
                                array_of_addresses,
                                c_array_of_datatypes);
  /* convert C handles to Fortran handles */
  for (i = 0; i < *max_datatypes; ++i)
  {
    array_of_datatypes[i] = PMPI_Type_c2f(c_array_of_datatypes[i]);
  }
  /* free local buffer */
  free(c_array_of_datatypes);
} /* FSUB(MPI_Type_get_contents) */

#endif

#if defined(HAS_MPI_PACK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Pack)(void*     inbuf,
                    MPI_Fint* incount,
                    MPI_Fint* datatype,
                    void*     outbuf,
                    MPI_Fint* outsize,
                    MPI_Fint* position,
                    MPI_Fint* comm,
                    MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (inbuf == epk_mpif_bottom)
  {
    inbuf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Pack(inbuf, *incount, PMPI_Type_f2c(
                     *datatype), outbuf, *outsize, position,
                   PMPI_Comm_f2c(*comm));
}

#endif
#if defined(HAS_MPI_PACK_EXTERNAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack_external
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Pack_external)(char*     datarep,
                             void*     inbuf,
                             MPI_Fint* incount,
                             MPI_Fint* datatype,
                             void*     outbuf,
                             MPI_Aint* outsize,
                             MPI_Aint* position,
                             MPI_Fint* ierr,
                             int       datarep_len)
{
  char* c_datarep     = NULL;
  int   c_datarep_len = datarep_len;

  while ((c_datarep_len > 0) && (datarep[c_datarep_len - 1] == ' '))
  {
    c_datarep_len--;
  }
  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_datarep, datarep, datarep_len);
  c_datarep[c_datarep_len] = '\0';

  #if defined(HAS_MPI_BOTTOM)
  if (inbuf == epk_mpif_bottom)
  {
    inbuf = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_Pack_external(c_datarep, inbuf, *incount, PMPI_Type_f2c(
                        *datatype), outbuf, *outsize, position);

  free(c_datarep);
} /* FSUB(MPI_Pack_external) */

#endif
#if defined(HAS_MPI_PACK_EXTERNAL_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack_external_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Pack_external_size)(char*     datarep,
                                  MPI_Fint* incount,
                                  MPI_Fint* datatype,
                                  MPI_Aint* size,
                                  MPI_Fint* ierr,
                                  int       datarep_len)
{
  char* c_datarep     = NULL;
  int   c_datarep_len = datarep_len;

  while ((c_datarep_len > 0) && (datarep[c_datarep_len - 1] == ' '))
  {
    c_datarep_len--;
  }
  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_datarep, datarep, datarep_len);
  c_datarep[c_datarep_len] = '\0';

  *ierr =
    MPI_Pack_external_size(c_datarep, *incount, PMPI_Type_f2c(
                             *datatype), size);

  free(c_datarep);
}

#endif
#if defined(HAS_MPI_PACK_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Pack_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Pack_size)(MPI_Fint* incount,
                         MPI_Fint* datatype,
                         MPI_Fint* comm,
                         MPI_Fint* size,
                         MPI_Fint* ierr)
{
  *ierr =
    MPI_Pack_size(*incount, PMPI_Type_f2c(*datatype), PMPI_Comm_f2c(
                    *comm), size);
}

#endif
#if defined(HAS_MPI_TYPE_COMMIT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_commit
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_commit)(MPI_Fint* datatype,
                           MPI_Fint* ierr)
{
  MPI_Datatype c_datatype = PMPI_Type_f2c(*datatype);

  *ierr = MPI_Type_commit(&c_datatype);

  *datatype = PMPI_Type_c2f(c_datatype);
}

#endif
#if defined(HAS_MPI_TYPE_CONTIGUOUS) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_contiguous
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_contiguous)(MPI_Fint* count,
                               MPI_Fint* oldtype,
                               MPI_Fint* newtype,
                               MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_contiguous(*count, PMPI_Type_f2c(*oldtype), &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_DARRAY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_darray
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_darray)(MPI_Fint* size,
                                  MPI_Fint* rank,
                                  MPI_Fint* ndims,
                                  MPI_Fint* array_of_gsizes,
                                  MPI_Fint* array_of_distribs,
                                  MPI_Fint* array_of_dargs,
                                  MPI_Fint* array_of_psizes,
                                  MPI_Fint* order,
                                  MPI_Fint* oldtype,
                                  MPI_Fint* newtype,
                                  MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_darray(*size,
                                 *rank,
                                 *ndims,
                                 array_of_gsizes,
                                 array_of_distribs,
                                 array_of_dargs,
                                 array_of_psizes,
                                 *order,
                                 PMPI_Type_f2c(*oldtype),
                                 &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_F90_COMPLEX) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_f90_complex
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_f90_complex)(MPI_Fint* p,
                                       MPI_Fint* r,
                                       MPI_Fint* newtype,
                                       MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_f90_complex(*p, *r, &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_F90_INTEGER) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_f90_integer
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_f90_integer)(MPI_Fint* r,
                                       MPI_Fint* newtype,
                                       MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_f90_integer(*r, &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_F90_REAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_f90_real
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_f90_real)(MPI_Fint* p,
                                    MPI_Fint* r,
                                    MPI_Fint* newtype,
                                    MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_f90_real(*p, *r, &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_HINDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_hindexed
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_hindexed)(MPI_Fint* count,
                                    MPI_Fint* array_of_blocklengths,
                                    MPI_Aint  array_of_displacements[],
                                    MPI_Fint* oldtype,
                                    MPI_Fint* newtype,
                                    MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_hindexed(*count,
                                   array_of_blocklengths,
                                   array_of_displacements,
                                   PMPI_Type_f2c(*oldtype),
                                   &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_HVECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_hvector
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_hvector)(MPI_Fint* count,
                                   MPI_Fint* blocklength,
                                   MPI_Aint* stride,
                                   MPI_Fint* oldtype,
                                   MPI_Fint* newtype,
                                   MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_hvector(*count,
                                  *blocklength,
                                  *stride,
                                  PMPI_Type_f2c(*oldtype),
                                  &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_INDEXED_BLOCK) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_indexed_block
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_indexed_block)(MPI_Fint* count,
                                         MPI_Fint* blocklength,
                                         MPI_Fint* array_of_displacements,
                                         MPI_Fint* oldtype,
                                         MPI_Fint* newtype,
                                         MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_indexed_block(*count,
                                        *blocklength,
                                        array_of_displacements,
                                        PMPI_Type_f2c(*oldtype),
                                        &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_RESIZED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_resized
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_resized)(MPI_Fint* oldtype,
                                   MPI_Aint* lb,
                                   MPI_Aint* extent,
                                   MPI_Fint* newtype,
                                   MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_resized(PMPI_Type_f2c(
                                    *oldtype), *lb, *extent, &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_CREATE_SUBARRAY) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_create_subarray
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_create_subarray)(MPI_Fint* ndims,
                                    MPI_Fint* array_of_sizes,
                                    MPI_Fint* array_of_subsizes,
                                    MPI_Fint* array_of_starts,
                                    MPI_Fint* order,
                                    MPI_Fint* oldtype,
                                    MPI_Fint* newtype,
                                    MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_create_subarray(*ndims,
                                   array_of_sizes,
                                   array_of_subsizes,
                                   array_of_starts,
                                   *order,
                                   PMPI_Type_f2c(*oldtype),
                                   &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_DUP) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_dup
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_dup)(MPI_Fint* type,
                        MPI_Fint* newtype,
                        MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_dup(PMPI_Type_f2c(*type), &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_extent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_extent)(MPI_Fint* datatype,
                           MPI_Fint* extent,
                           MPI_Fint* ierr)
{
  MPI_Aint c_extent;

  *ierr = MPI_Type_extent(PMPI_Type_f2c(*datatype), &c_extent);

  *extent = (MPI_Fint)c_extent;
  if (*extent != c_extent)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Type_extent\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_TYPE_FREE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_free)(MPI_Fint* datatype,
                         MPI_Fint* ierr)
{
  MPI_Datatype c_datatype = PMPI_Type_f2c(*datatype);

  *ierr = MPI_Type_free(&c_datatype);

  *datatype = PMPI_Type_c2f(c_datatype);
}

#endif
#if defined(HAS_MPI_TYPE_GET_ENVELOPE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_get_envelope
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_get_envelope)(MPI_Fint* datatype,
                                 MPI_Fint* num_integers,
                                 MPI_Fint* num_addresses,
                                 MPI_Fint* num_datatypes,
                                 MPI_Fint* combiner,
                                 MPI_Fint* ierr)
{
  *ierr = MPI_Type_get_envelope(PMPI_Type_f2c(
                                  *datatype), num_integers, num_addresses,
                                num_datatypes, combiner);
}

#endif
#if defined(HAS_MPI_TYPE_GET_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_get_extent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_get_extent)(MPI_Fint* datatype,
                               MPI_Aint* lb,
                               MPI_Aint* extent,
                               MPI_Fint* ierr)
{
  *ierr = MPI_Type_get_extent(PMPI_Type_f2c(*datatype), lb, extent);
}

#endif
#if defined(HAS_MPI_TYPE_GET_TRUE_EXTENT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_get_true_extent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_get_true_extent)(MPI_Fint* datatype,
                                    MPI_Aint* true_lb,
                                    MPI_Aint* true_extent,
                                    MPI_Fint* ierr)
{
  *ierr = MPI_Type_get_true_extent(PMPI_Type_f2c(
                                     *datatype), true_lb, true_extent);
}

#endif
#if defined(HAS_MPI_TYPE_HINDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_hindexed
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_hindexed)(MPI_Fint* count,
                             MPI_Fint* array_of_blocklengths,
                             MPI_Fint* array_of_displacements,
                             MPI_Fint* oldtype,
                             MPI_Fint* newtype,
                             MPI_Fint* ierr)
{
  int          i;
  MPI_Aint*    c_array_of_displacements;
  MPI_Datatype c_newtype;

  c_array_of_displacements = (MPI_Aint*)malloc(*count * sizeof (MPI_Aint));
  for (i = 0; i < *count; ++i)
  {
    c_array_of_displacements[i] = array_of_displacements[i];
  }

  *ierr = MPI_Type_hindexed(*count,
                            array_of_blocklengths,
                            c_array_of_displacements,
                            PMPI_Type_f2c(*oldtype),
                            &c_newtype);

  free(c_array_of_displacements);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_HVECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_hvector
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_hvector)(MPI_Fint* count,
                            MPI_Fint* blocklength,
                            MPI_Fint* stride,
                            MPI_Fint* oldtype,
                            MPI_Fint* newtype,
                            MPI_Fint* ierr)
{
  MPI_Aint     c_stride = *stride;
  MPI_Datatype c_newtype;

  *ierr =
    MPI_Type_hvector(*count, *blocklength, c_stride, PMPI_Type_f2c(
                       *oldtype), &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_INDEXED) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_indexed
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_indexed)(MPI_Fint* count,
                            MPI_Fint* array_of_blocklengths,
                            MPI_Fint* array_of_displacements,
                            MPI_Fint* oldtype,
                            MPI_Fint* newtype,
                            MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr = MPI_Type_indexed(*count,
                           array_of_blocklengths,
                           array_of_displacements,
                           PMPI_Type_f2c(*oldtype),
                           &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_TYPE_LB) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_lb
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_lb)(MPI_Fint* datatype,
                       MPI_Fint* displacement,
                       MPI_Fint* ierr)
{
  MPI_Aint c_displacement;

  *ierr = MPI_Type_lb(PMPI_Type_f2c(*datatype), &c_displacement);

  *displacement = (MPI_Fint)c_displacement;
  if (*displacement != c_displacement)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Type_lb\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_TYPE_MATCH_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_match_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Type_match_size)(MPI_Fint* typeclass,
                               MPI_Fint* size,
                               MPI_Fint* type,
                               MPI_Fint* ierr)
{
  MPI_Datatype c_type;

  *ierr = MPI_Type_match_size(*typeclass, *size, &c_type);

  *type = PMPI_Type_c2f(c_type);
}

#endif
#if defined(HAS_MPI_TYPE_SIZE) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_size)(MPI_Fint* datatype,
                         MPI_Fint* size,
                         MPI_Fint* ierr)
{
  *ierr = MPI_Type_size(PMPI_Type_f2c(*datatype), size);
}

#endif
#if defined(HAS_MPI_TYPE_STRUCT) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_struct
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_struct)(MPI_Fint* count,
                           MPI_Fint* array_of_blocklengths,
                           MPI_Fint* array_of_displacements,
                           MPI_Fint* array_of_types,
                           MPI_Fint* newtype,
                           MPI_Fint* ierr)
{
  int           i;
  MPI_Aint*     c_array_of_displacements;
  MPI_Datatype  c_newtype;
  MPI_Datatype* c_array_of_types;

  c_array_of_displacements = (MPI_Aint*)malloc(*count * sizeof (MPI_Aint));
  for (i = 0; i < *count; ++i)
  {
    c_array_of_displacements[i] = array_of_displacements[i];
  }

  c_array_of_types = (MPI_Datatype*)malloc(*count * sizeof (MPI_Datatype));
  if (!c_array_of_types)
  {
    exit(EXIT_FAILURE);
  }
  /* index variable i, is already defined by the implicit conversion done
   * in the special 'noaint' handler */
  for (i = 0; i < *count; ++i)
  {
    c_array_of_types[i] = PMPI_Type_f2c(array_of_types[i]);
  }

  *ierr = MPI_Type_struct(*count,
                          array_of_blocklengths,
                          c_array_of_displacements,
                          c_array_of_types,
                          &c_newtype);

  free(c_array_of_types);
  free(c_array_of_displacements);

  *newtype = PMPI_Type_c2f(c_newtype);
} /* FSUB(MPI_Type_struct) */

#endif
#if defined(HAS_MPI_TYPE_UB) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_ub
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_ub)(MPI_Fint* datatype,
                       MPI_Fint* displacement,
                       MPI_Fint* ierr)
{
  MPI_Aint c_displacement;

  *ierr = MPI_Type_ub(PMPI_Type_f2c(*datatype), &c_displacement);

  *displacement = (MPI_Fint)c_displacement;
  if (*displacement != c_displacement)
  {
    elg_error_msg(
      "Value truncated in \"MPI_Type_ub\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details.");
  }
}

#endif
#if defined(HAS_MPI_TYPE_VECTOR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Type_vector
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Type_vector)(MPI_Fint* count,
                           MPI_Fint* blocklength,
                           MPI_Fint* stride,
                           MPI_Fint* oldtype,
                           MPI_Fint* newtype,
                           MPI_Fint* ierr)
{
  MPI_Datatype c_newtype;

  *ierr =
    MPI_Type_vector(*count, *blocklength, *stride, PMPI_Type_f2c(
                      *oldtype), &c_newtype);

  *newtype = PMPI_Type_c2f(c_newtype);
}

#endif
#if defined(HAS_MPI_UNPACK) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Unpack
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 1.0
 * @ingroup type
 */
void FSUB(MPI_Unpack)(void*     inbuf,
                      MPI_Fint* insize,
                      MPI_Fint* position,
                      void*     outbuf,
                      MPI_Fint* outcount,
                      MPI_Fint* datatype,
                      MPI_Fint* comm,
                      MPI_Fint* ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (outbuf == epk_mpif_bottom)
  {
    outbuf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Unpack(inbuf,
                     *insize,
                     position,
                     outbuf,
                     *outcount,
                     PMPI_Type_f2c(*datatype),
                     PMPI_Comm_f2c(*comm));
}

#endif
#if defined(HAS_MPI_UNPACK_EXTERNAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE)
/**
 * Measurement wrapper for MPI_Unpack_external
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type
 */
void FSUB(MPI_Unpack_external)(char*     datarep,
                               void*     inbuf,
                               MPI_Aint* insize,
                               MPI_Aint* position,
                               void*     outbuf,
                               MPI_Fint* outcount,
                               MPI_Fint* datatype,
                               MPI_Fint* ierr,
                               int       datarep_len)
{
  char* c_datarep     = NULL;
  int   c_datarep_len = datarep_len;

  while ((c_datarep_len > 0) && (datarep[c_datarep_len - 1] == ' '))
  {
    c_datarep_len--;
  }
  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_datarep, datarep, datarep_len);
  c_datarep[c_datarep_len] = '\0';

  #if defined(HAS_MPI_BOTTOM)
  if (outbuf == epk_mpif_bottom)
  {
    outbuf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_Unpack_external(c_datarep,
                              inbuf,
                              *insize,
                              position,
                              outbuf,
                              *outcount,
                              PMPI_Type_f2c(*datatype));

  free(c_datarep);
} /* FSUB(MPI_Unpack_external) */

#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if defined(HAS_MPI_TYPE_CREATE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_create_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_create_keyval)(void*     type_copy_attr_fn,
                                  void*     type_delete_attr_fn,
                                  MPI_Fint* type_keyval,
                                  void*     extra_state,
                                  MPI_Fint* ierr)
{
  *ierr = MPI_Type_create_keyval(
    (MPI_Type_copy_attr_function*)type_copy_attr_fn,
    (MPI_Type_delete_attr_function*)type_delete_attr_fn,
    type_keyval,
    extra_state);
}

#endif
#if defined(HAS_MPI_TYPE_DELETE_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_delete_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_delete_attr)(MPI_Fint* type,
                                MPI_Fint* type_keyval,
                                MPI_Fint* ierr)
{
  MPI_Datatype c_type = PMPI_Type_f2c(*type);

  *ierr = MPI_Type_delete_attr(c_type, *type_keyval);

  *type = PMPI_Type_c2f(c_type);
}

#endif
#if defined(HAS_MPI_TYPE_FREE_KEYVAL) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_free_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_free_keyval)(MPI_Fint* type_keyval,
                                MPI_Fint* ierr)
{
  *ierr = MPI_Type_free_keyval(type_keyval);
}

#endif
#if defined(HAS_MPI_TYPE_GET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_get_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_get_attr)(MPI_Fint* type,
                             MPI_Fint* type_keyval,
                             void*     attribute_val,
                             MPI_Fint* flag,
                             MPI_Fint* ierr)
{
  *ierr = MPI_Type_get_attr(PMPI_Type_f2c(
                              *type), *type_keyval, attribute_val, flag);
}

#endif
#if defined(HAS_MPI_TYPE_GET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_get_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_get_name)(MPI_Fint* type,
                             char*     type_name,
                             MPI_Fint* resultlen,
                             MPI_Fint* ierr,
                             int       type_name_len)
{
  char* c_type_name     = NULL;
  int   c_type_name_len = 0;

  c_type_name = (char*)malloc((type_name_len + 1) * sizeof (char));
  if (!c_type_name)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_Type_get_name(PMPI_Type_f2c(*type), c_type_name, resultlen);

  c_type_name_len = strlen(c_type_name);
  strncpy(type_name, c_type_name, c_type_name_len);
  memset(type_name + c_type_name_len, ' ', type_name_len - c_type_name_len);
  free(c_type_name);
}

#endif
#if defined(HAS_MPI_TYPE_SET_ATTR) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_set_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_set_attr)(MPI_Fint* type,
                             MPI_Fint* type_keyval,
                             void*     attribute_val,
                             MPI_Fint* ierr)
{
  MPI_Datatype c_type = PMPI_Type_f2c(*type);

  *ierr = MPI_Type_set_attr(c_type, *type_keyval, attribute_val);

  *type = PMPI_Type_c2f(c_type);
}

#endif
#if defined(HAS_MPI_TYPE_SET_NAME) && !defined(NO_MPI_EXTRA) \
  && !defined(NO_MPI_TYPE) && !defined(NO_MPI_EXT)
/**
 * Measurement wrapper for MPI_Type_set_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup type_ext
 */
void FSUB(MPI_Type_set_name)(MPI_Fint* type,
                             char*     type_name,
                             MPI_Fint* ierr,
                             int       type_name_len)
{
  MPI_Datatype c_type          = PMPI_Type_f2c(*type);
  char*        c_type_name     = NULL;
  int          c_type_name_len = type_name_len;

  while ((c_type_name_len > 0) && (type_name[c_type_name_len - 1] == ' '))
  {
    c_type_name_len--;
  }
  c_type_name = (char*)malloc((type_name_len + 1) * sizeof (char));
  if (!c_type_name)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_type_name, type_name, type_name_len);
  c_type_name[c_type_name_len] = '\0';

  *ierr = MPI_Type_set_name(c_type, c_type_name);

  *type = PMPI_Type_c2f(c_type);
  free(c_type_name);
}

#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */

#endif

/**
 * @}
 */
