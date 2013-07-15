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
 * @file  epk_fmpiwrap_io.c
 *
 * @brief Fortran interface wrappers for parallel I/O
 */

#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "elg_error.h"
#include "elg_fmpi_defs.h"
#include "epk_fmpiwrap.h"
#include "epk_defs_mpi.h"
#include "epk_fwrapper_def.h"

/* lowercase defines */
/** All lowercase name of MPI_File_close */
#define MPI_File_close_L mpi_file_close
/** All lowercase name of MPI_File_delete */
#define MPI_File_delete_L mpi_file_delete
/** All lowercase name of MPI_File_get_amode */
#define MPI_File_get_amode_L mpi_file_get_amode
/** All lowercase name of MPI_File_get_atomicity */
#define MPI_File_get_atomicity_L mpi_file_get_atomicity
/** All lowercase name of MPI_File_get_byte_offset */
#define MPI_File_get_byte_offset_L mpi_file_get_byte_offset
/** All lowercase name of MPI_File_get_group */
#define MPI_File_get_group_L mpi_file_get_group
/** All lowercase name of MPI_File_get_info */
#define MPI_File_get_info_L mpi_file_get_info
/** All lowercase name of MPI_File_get_position */
#define MPI_File_get_position_L mpi_file_get_position
/** All lowercase name of MPI_File_get_position_shared */
#define MPI_File_get_position_shared_L mpi_file_get_position_shared
/** All lowercase name of MPI_File_get_size */
#define MPI_File_get_size_L mpi_file_get_size
/** All lowercase name of MPI_File_get_type_extent */
#define MPI_File_get_type_extent_L mpi_file_get_type_extent
/** All lowercase name of MPI_File_get_view */
#define MPI_File_get_view_L mpi_file_get_view
/** All lowercase name of MPI_File_iread */
#define MPI_File_iread_L mpi_file_iread
/** All lowercase name of MPI_File_iread_at */
#define MPI_File_iread_at_L mpi_file_iread_at
/** All lowercase name of MPI_File_iread_shared */
#define MPI_File_iread_shared_L mpi_file_iread_shared
/** All lowercase name of MPI_File_iwrite */
#define MPI_File_iwrite_L mpi_file_iwrite
/** All lowercase name of MPI_File_iwrite_at */
#define MPI_File_iwrite_at_L mpi_file_iwrite_at
/** All lowercase name of MPI_File_iwrite_shared */
#define MPI_File_iwrite_shared_L mpi_file_iwrite_shared
/** All lowercase name of MPI_File_open */
#define MPI_File_open_L mpi_file_open
/** All lowercase name of MPI_File_preallocate */
#define MPI_File_preallocate_L mpi_file_preallocate
/** All lowercase name of MPI_File_read */
#define MPI_File_read_L mpi_file_read
/** All lowercase name of MPI_File_read_all */
#define MPI_File_read_all_L mpi_file_read_all
/** All lowercase name of MPI_File_read_all_begin */
#define MPI_File_read_all_begin_L mpi_file_read_all_begin
/** All lowercase name of MPI_File_read_all_end */
#define MPI_File_read_all_end_L mpi_file_read_all_end
/** All lowercase name of MPI_File_read_at */
#define MPI_File_read_at_L mpi_file_read_at
/** All lowercase name of MPI_File_read_at_all */
#define MPI_File_read_at_all_L mpi_file_read_at_all
/** All lowercase name of MPI_File_read_at_all_begin */
#define MPI_File_read_at_all_begin_L mpi_file_read_at_all_begin
/** All lowercase name of MPI_File_read_at_all_end */
#define MPI_File_read_at_all_end_L mpi_file_read_at_all_end
/** All lowercase name of MPI_File_read_ordered */
#define MPI_File_read_ordered_L mpi_file_read_ordered
/** All lowercase name of MPI_File_read_ordered_begin */
#define MPI_File_read_ordered_begin_L mpi_file_read_ordered_begin
/** All lowercase name of MPI_File_read_ordered_end */
#define MPI_File_read_ordered_end_L mpi_file_read_ordered_end
/** All lowercase name of MPI_File_read_shared */
#define MPI_File_read_shared_L mpi_file_read_shared
/** All lowercase name of MPI_File_seek */
#define MPI_File_seek_L mpi_file_seek
/** All lowercase name of MPI_File_seek_shared */
#define MPI_File_seek_shared_L mpi_file_seek_shared
/** All lowercase name of MPI_File_set_atomicity */
#define MPI_File_set_atomicity_L mpi_file_set_atomicity
/** All lowercase name of MPI_File_set_info */
#define MPI_File_set_info_L mpi_file_set_info
/** All lowercase name of MPI_File_set_size */
#define MPI_File_set_size_L mpi_file_set_size
/** All lowercase name of MPI_File_set_view */
#define MPI_File_set_view_L mpi_file_set_view
/** All lowercase name of MPI_File_sync */
#define MPI_File_sync_L mpi_file_sync
/** All lowercase name of MPI_File_write */
#define MPI_File_write_L mpi_file_write
/** All lowercase name of MPI_File_write_all */
#define MPI_File_write_all_L mpi_file_write_all
/** All lowercase name of MPI_File_write_all_begin */
#define MPI_File_write_all_begin_L mpi_file_write_all_begin
/** All lowercase name of MPI_File_write_all_end */
#define MPI_File_write_all_end_L mpi_file_write_all_end
/** All lowercase name of MPI_File_write_at */
#define MPI_File_write_at_L mpi_file_write_at
/** All lowercase name of MPI_File_write_at_all */
#define MPI_File_write_at_all_L mpi_file_write_at_all
/** All lowercase name of MPI_File_write_at_all_begin */
#define MPI_File_write_at_all_begin_L mpi_file_write_at_all_begin
/** All lowercase name of MPI_File_write_at_all_end */
#define MPI_File_write_at_all_end_L mpi_file_write_at_all_end
/** All lowercase name of MPI_File_write_ordered */
#define MPI_File_write_ordered_L mpi_file_write_ordered
/** All lowercase name of MPI_File_write_ordered_begin */
#define MPI_File_write_ordered_begin_L mpi_file_write_ordered_begin
/** All lowercase name of MPI_File_write_ordered_end */
#define MPI_File_write_ordered_end_L mpi_file_write_ordered_end
/** All lowercase name of MPI_File_write_shared */
#define MPI_File_write_shared_L mpi_file_write_shared
/** All lowercase name of MPI_Register_datarep */
#define MPI_Register_datarep_L mpi_register_datarep

/* uppercase defines */
/** All uppercase name of MPI_File_close */
#define MPI_File_close_U MPI_FILE_CLOSE
/** All uppercase name of MPI_File_delete */
#define MPI_File_delete_U MPI_FILE_DELETE
/** All uppercase name of MPI_File_get_amode */
#define MPI_File_get_amode_U MPI_FILE_GET_AMODE
/** All uppercase name of MPI_File_get_atomicity */
#define MPI_File_get_atomicity_U MPI_FILE_GET_ATOMICITY
/** All uppercase name of MPI_File_get_byte_offset */
#define MPI_File_get_byte_offset_U MPI_FILE_GET_BYTE_OFFSET
/** All uppercase name of MPI_File_get_group */
#define MPI_File_get_group_U MPI_FILE_GET_GROUP
/** All uppercase name of MPI_File_get_info */
#define MPI_File_get_info_U MPI_FILE_GET_INFO
/** All uppercase name of MPI_File_get_position */
#define MPI_File_get_position_U MPI_FILE_GET_POSITION
/** All uppercase name of MPI_File_get_position_shared */
#define MPI_File_get_position_shared_U MPI_FILE_GET_POSITION_SHARED
/** All uppercase name of MPI_File_get_size */
#define MPI_File_get_size_U MPI_FILE_GET_SIZE
/** All uppercase name of MPI_File_get_type_extent */
#define MPI_File_get_type_extent_U MPI_FILE_GET_TYPE_EXTENT
/** All uppercase name of MPI_File_get_view */
#define MPI_File_get_view_U MPI_FILE_GET_VIEW
/** All uppercase name of MPI_File_iread */
#define MPI_File_iread_U MPI_FILE_IREAD
/** All uppercase name of MPI_File_iread_at */
#define MPI_File_iread_at_U MPI_FILE_IREAD_AT
/** All uppercase name of MPI_File_iread_shared */
#define MPI_File_iread_shared_U MPI_FILE_IREAD_SHARED
/** All uppercase name of MPI_File_iwrite */
#define MPI_File_iwrite_U MPI_FILE_IWRITE
/** All uppercase name of MPI_File_iwrite_at */
#define MPI_File_iwrite_at_U MPI_FILE_IWRITE_AT
/** All uppercase name of MPI_File_iwrite_shared */
#define MPI_File_iwrite_shared_U MPI_FILE_IWRITE_SHARED
/** All uppercase name of MPI_File_open */
#define MPI_File_open_U MPI_FILE_OPEN
/** All uppercase name of MPI_File_preallocate */
#define MPI_File_preallocate_U MPI_FILE_PREALLOCATE
/** All uppercase name of MPI_File_read */
#define MPI_File_read_U MPI_FILE_READ
/** All uppercase name of MPI_File_read_all */
#define MPI_File_read_all_U MPI_FILE_READ_ALL
/** All uppercase name of MPI_File_read_all_begin */
#define MPI_File_read_all_begin_U MPI_FILE_READ_ALL_BEGIN
/** All uppercase name of MPI_File_read_all_end */
#define MPI_File_read_all_end_U MPI_FILE_READ_ALL_END
/** All uppercase name of MPI_File_read_at */
#define MPI_File_read_at_U MPI_FILE_READ_AT
/** All uppercase name of MPI_File_read_at_all */
#define MPI_File_read_at_all_U MPI_FILE_READ_AT_ALL
/** All uppercase name of MPI_File_read_at_all_begin */
#define MPI_File_read_at_all_begin_U MPI_FILE_READ_AT_ALL_BEGIN
/** All uppercase name of MPI_File_read_at_all_end */
#define MPI_File_read_at_all_end_U MPI_FILE_READ_AT_ALL_END
/** All uppercase name of MPI_File_read_ordered */
#define MPI_File_read_ordered_U MPI_FILE_READ_ORDERED
/** All uppercase name of MPI_File_read_ordered_begin */
#define MPI_File_read_ordered_begin_U MPI_FILE_READ_ORDERED_BEGIN
/** All uppercase name of MPI_File_read_ordered_end */
#define MPI_File_read_ordered_end_U MPI_FILE_READ_ORDERED_END
/** All uppercase name of MPI_File_read_shared */
#define MPI_File_read_shared_U MPI_FILE_READ_SHARED
/** All uppercase name of MPI_File_seek */
#define MPI_File_seek_U MPI_FILE_SEEK
/** All uppercase name of MPI_File_seek_shared */
#define MPI_File_seek_shared_U MPI_FILE_SEEK_SHARED
/** All uppercase name of MPI_File_set_atomicity */
#define MPI_File_set_atomicity_U MPI_FILE_SET_ATOMICITY
/** All uppercase name of MPI_File_set_info */
#define MPI_File_set_info_U MPI_FILE_SET_INFO
/** All uppercase name of MPI_File_set_size */
#define MPI_File_set_size_U MPI_FILE_SET_SIZE
/** All uppercase name of MPI_File_set_view */
#define MPI_File_set_view_U MPI_FILE_SET_VIEW
/** All uppercase name of MPI_File_sync */
#define MPI_File_sync_U MPI_FILE_SYNC
/** All uppercase name of MPI_File_write */
#define MPI_File_write_U MPI_FILE_WRITE
/** All uppercase name of MPI_File_write_all */
#define MPI_File_write_all_U MPI_FILE_WRITE_ALL
/** All uppercase name of MPI_File_write_all_begin */
#define MPI_File_write_all_begin_U MPI_FILE_WRITE_ALL_BEGIN
/** All uppercase name of MPI_File_write_all_end */
#define MPI_File_write_all_end_U MPI_FILE_WRITE_ALL_END
/** All uppercase name of MPI_File_write_at */
#define MPI_File_write_at_U MPI_FILE_WRITE_AT
/** All uppercase name of MPI_File_write_at_all */
#define MPI_File_write_at_all_U MPI_FILE_WRITE_AT_ALL
/** All uppercase name of MPI_File_write_at_all_begin */
#define MPI_File_write_at_all_begin_U MPI_FILE_WRITE_AT_ALL_BEGIN
/** All uppercase name of MPI_File_write_at_all_end */
#define MPI_File_write_at_all_end_U MPI_FILE_WRITE_AT_ALL_END
/** All uppercase name of MPI_File_write_ordered */
#define MPI_File_write_ordered_U MPI_FILE_WRITE_ORDERED
/** All uppercase name of MPI_File_write_ordered_begin */
#define MPI_File_write_ordered_begin_U MPI_FILE_WRITE_ORDERED_BEGIN
/** All uppercase name of MPI_File_write_ordered_end */
#define MPI_File_write_ordered_end_U MPI_FILE_WRITE_ORDERED_END
/** All uppercase name of MPI_File_write_shared */
#define MPI_File_write_shared_U MPI_FILE_WRITE_SHARED
/** All uppercase name of MPI_Register_datarep */
#define MPI_Register_datarep_U MPI_REGISTER_DATAREP

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

/**
 * @name Fortran wrappers for administrative functions
 * @{
 */

#if defined(HAS_MPI_FILE_CLOSE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_close
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_close)(MPI_File* fh,
                          int*      ierr)
{
  *ierr = MPI_File_close(fh);
}

#endif
#if defined(HAS_MPI_FILE_DELETE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_delete
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_delete)(char*     filename,
                           MPI_Info* info,
                           int*      ierr,
                           int       filename_len)
{
  char* c_filename     = NULL;
  int   c_filename_len = filename_len;

  while ((c_filename_len > 0) && (filename[c_filename_len - 1] == ' '))
  {
    c_filename_len--;
  }
  c_filename = (char*)malloc((filename_len + 1) * sizeof (char));
  if (!c_filename)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_filename, filename, filename_len);
  c_filename[c_filename_len] = '\0';

  *ierr = MPI_File_delete(c_filename, *info);

  free(c_filename);
}

#endif
#if defined(HAS_MPI_FILE_GET_AMODE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_amode
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_amode)(MPI_File* fh,
                              int*      amode,
                              int*      ierr)
{
  *ierr = MPI_File_get_amode(*fh, amode);
}

#endif
#if defined(HAS_MPI_FILE_GET_ATOMICITY) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_atomicity
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_atomicity)(MPI_File* fh,
                                  int*      flag,
                                  int*      ierr)
{
  *ierr = MPI_File_get_atomicity(*fh, flag);
}

#endif
#if defined(HAS_MPI_FILE_GET_BYTE_OFFSET) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_byte_offset
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_byte_offset)(MPI_File*   fh,
                                    MPI_Offset* offset,
                                    MPI_Offset* disp,
                                    int*        ierr)
{
  *ierr = MPI_File_get_byte_offset(*fh, *offset, disp);
}

#endif
#if defined(HAS_MPI_FILE_GET_GROUP) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_group
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_group)(MPI_File*  fh,
                              MPI_Group* group,
                              int*       ierr)
{
  *ierr = MPI_File_get_group(*fh, group);
}

#endif
#if defined(HAS_MPI_FILE_GET_INFO) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_info
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_info)(MPI_File* fh,
                             MPI_Info* info_used,
                             int*      ierr)
{
  *ierr = MPI_File_get_info(*fh, info_used);
}

#endif
#if defined(HAS_MPI_FILE_GET_POSITION) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_position
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_position)(MPI_File*   fh,
                                 MPI_Offset* offset,
                                 int*        ierr)
{
  *ierr = MPI_File_get_position(*fh, offset);
}

#endif
#if defined(HAS_MPI_FILE_GET_POSITION_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_position_shared
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_position_shared)(MPI_File*   fh,
                                        MPI_Offset* offset,
                                        int*        ierr)
{
  *ierr = MPI_File_get_position_shared(*fh, offset);
}

#endif
#if defined(HAS_MPI_FILE_GET_SIZE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_size)(MPI_File*   fh,
                             MPI_Offset* size,
                             int*        ierr)
{
  *ierr = MPI_File_get_size(*fh, size);
}

#endif
#if defined(HAS_MPI_FILE_GET_TYPE_EXTENT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_type_extent
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_type_extent)(MPI_File*     fh,
                                    MPI_Datatype* datatype,
                                    MPI_Aint*     extent,
                                    int*          ierr)
{
  *ierr = MPI_File_get_type_extent(*fh, *datatype, extent);
}

#endif
#if defined(HAS_MPI_FILE_GET_VIEW) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_view
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_view)(MPI_File*     fh,
                             MPI_Offset*   disp,
                             MPI_Datatype* etype,
                             MPI_Datatype* filetype,
                             char*         datarep,
                             int*          ierr,
                             int           datarep_len)
{
  char* c_datarep     = NULL;
  int   c_datarep_len = 0;

  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_File_get_view(*fh, disp, etype, filetype, c_datarep);

  c_datarep_len = strlen(c_datarep);
  strncpy(datarep, c_datarep, c_datarep_len);
  memset(datarep + c_datarep_len, ' ', datarep_len - c_datarep_len);
  free(c_datarep);
}

#endif
#if defined(HAS_MPI_FILE_OPEN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_open
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_open)(MPI_Comm* comm,
                         char*     filename,
                         int*      amode,
                         MPI_Info* info,
                         MPI_File* fh,
                         int*      ierr,
                         int       filename_len)
{
  char* c_filename     = NULL;
  int   c_filename_len = filename_len;

  while ((c_filename_len > 0) && (filename[c_filename_len - 1] == ' '))
  {
    c_filename_len--;
  }
  c_filename = (char*)malloc((filename_len + 1) * sizeof (char));
  if (!c_filename)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_filename, filename, filename_len);
  c_filename[c_filename_len] = '\0';

  *ierr = MPI_File_open(*comm, c_filename, *amode, *info, fh);

  free(c_filename);
}

#endif
#if defined(HAS_MPI_FILE_PREALLOCATE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_preallocate
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_preallocate)(MPI_File*   fh,
                                MPI_Offset* size,
                                int*        ierr)
{
  *ierr = MPI_File_preallocate(*fh, *size);
}

#endif
#if defined(HAS_MPI_FILE_SEEK) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_seek
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_seek)(MPI_File*   fh,
                         MPI_Offset* offset,
                         int*        whence,
                         int*        ierr)
{
  *ierr = MPI_File_seek(*fh, *offset, *whence);
}

#endif
#if defined(HAS_MPI_FILE_SEEK_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_seek_shared
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_seek_shared)(MPI_File*   fh,
                                MPI_Offset* offset,
                                int*        whence,
                                int*        ierr)
{
  *ierr = MPI_File_seek_shared(*fh, *offset, *whence);
}

#endif
#if defined(HAS_MPI_FILE_SET_ATOMICITY) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_atomicity
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_atomicity)(MPI_File* fh,
                                  int*      flag,
                                  int*      ierr)
{
  *ierr = MPI_File_set_atomicity(*fh, *flag);
}

#endif
#if defined(HAS_MPI_FILE_SET_INFO) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_info
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_info)(MPI_File* fh,
                             MPI_Info* info,
                             int*      ierr)
{
  *ierr = MPI_File_set_info(*fh, *info);
}

#endif
#if defined(HAS_MPI_FILE_SET_SIZE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_size
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_size)(MPI_File*   fh,
                             MPI_Offset* size,
                             int*        ierr)
{
  *ierr = MPI_File_set_size(*fh, *size);
}

#endif
#if defined(HAS_MPI_FILE_SET_VIEW) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_view
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_view)(MPI_File*     fh,
                             MPI_Offset*   disp,
                             MPI_Datatype* etype,
                             MPI_Datatype* filetype,
                             char*         datarep,
                             MPI_Info*     info,
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

  *ierr = MPI_File_set_view(*fh,
                            *disp,
                            *etype,
                            *filetype,
                            c_datarep,
                            *info);

  free(c_datarep);
}

#endif
#if defined(HAS_MPI_FILE_SYNC) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_sync
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_sync)(MPI_File* fh,
                         int*      ierr)
{
  *ierr = MPI_File_sync(*fh);
}

#endif
#if defined(HAS_MPI_REGISTER_DATAREP) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA)
/**
 * Measurement wrapper for MPI_Register_datarep
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_Register_datarep)(
  char* datarep,
  MPI_Datarep_conversion_function*
  read_conversion_fn,
  MPI_Datarep_conversion_function*
  write_conversion_fn,
  MPI_Datarep_extent_function*
  dtype_file_extent_fn,
  void*
  extra_state,
  int*  ierr,
  int
  datarep_len)
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

  *ierr = MPI_Register_datarep(c_datarep,
                               read_conversion_fn,
                               write_conversion_fn,
                               dtype_file_extent_fn,
                               extra_state);

  free(c_datarep);
}

#endif

/**
 * @}
 * @name Fortran wrappers for access functions
 * @{
 */

#if defined(HAS_MPI_FILE_IREAD) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iread
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iread)(MPI_File*     fh,
                          void*         buf,
                          int*          count,
                          MPI_Datatype* datatype,
                          MPI_Request*  request,
                          int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iread(*fh, buf, *count, *datatype, request);
}

#endif
#if defined(HAS_MPI_FILE_IREAD_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iread_at
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iread_at)(MPI_File*     fh,
                             MPI_Offset*   offset,
                             void*         buf,
                             int*          count,
                             MPI_Datatype* datatype,
                             MPI_Request*  request,
                             int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iread_at(*fh, *offset, buf, *count, *datatype, request);
}

#endif
#if defined(HAS_MPI_FILE_IREAD_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iread_shared
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iread_shared)(MPI_File*     fh,
                                 void*         buf,
                                 int*          count,
                                 MPI_Datatype* datatype,
                                 MPI_Request*  request,
                                 int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iread_shared(*fh, buf, *count, *datatype, request);
}

#endif
#if defined(HAS_MPI_FILE_IWRITE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iwrite
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iwrite)(MPI_File*     fh,
                           void*         buf,
                           int*          count,
                           MPI_Datatype* datatype,
                           MPI_Request*  request,
                           int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iwrite(*fh, buf, *count, *datatype, request);
}

#endif
#if defined(HAS_MPI_FILE_IWRITE_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iwrite_at
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iwrite_at)(MPI_File*     fh,
                              MPI_Offset*   offset,
                              void*         buf,
                              int*          count,
                              MPI_Datatype* datatype,
                              MPI_Request*  request,
                              int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iwrite_at(*fh, *offset, buf, *count, *datatype, request);
}

#endif
#if defined(HAS_MPI_FILE_IWRITE_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iwrite_shared
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iwrite_shared)(MPI_File*     fh,
                                  void*         buf,
                                  int*          count,
                                  MPI_Datatype* datatype,
                                  MPI_Request*  request,
                                  int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iwrite_shared(*fh, buf, *count, *datatype, request);
}

#endif
#if defined(HAS_MPI_FILE_READ) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read)(MPI_File*     fh,
                         void*         buf,
                         int*          count,
                         MPI_Datatype* datatype,
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

  *ierr = MPI_File_read(*fh, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_READ_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_all
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_all)(MPI_File*     fh,
                             void*         buf,
                             int*          count,
                             MPI_Datatype* datatype,
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

  *ierr = MPI_File_read_all(*fh, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_READ_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_all_begin
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_all_begin)(MPI_File*     fh,
                                   void*         buf,
                                   int*          count,
                                   MPI_Datatype* datatype,
                                   int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_read_all_begin(*fh, buf, *count, *datatype);
}

#endif
#if defined(HAS_MPI_FILE_READ_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_all_end
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_all_end)(MPI_File*   fh,
                                 void*       buf,
                                 MPI_Status* status,
                                 int*        ierr)
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

  *ierr = MPI_File_read_all_end(*fh, buf, status);
}

#endif
#if defined(HAS_MPI_FILE_READ_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at)(MPI_File*     fh,
                            MPI_Offset*   offset,
                            void*         buf,
                            int*          count,
                            MPI_Datatype* datatype,
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

  *ierr = MPI_File_read_at(*fh, *offset, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_READ_AT_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at_all
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at_all)(MPI_File*     fh,
                                MPI_Offset*   offset,
                                void*         buf,
                                int*          count,
                                MPI_Datatype* datatype,
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

  *ierr = MPI_File_read_at_all(*fh,
                               *offset,
                               buf,
                               *count,
                               *datatype,
                               status);
}

#endif
#if defined(HAS_MPI_FILE_READ_AT_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at_all_begin
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at_all_begin)(MPI_File*     fh,
                                      MPI_Offset*   offset,
                                      void*         buf,
                                      int*          count,
                                      MPI_Datatype* datatype,
                                      int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_read_at_all_begin(*fh, *offset, buf, *count, *datatype);
}

#endif
#if defined(HAS_MPI_FILE_READ_AT_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at_all_end
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at_all_end)(MPI_File*   fh,
                                    void*       buf,
                                    MPI_Status* status,
                                    int*        ierr)
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

  *ierr = MPI_File_read_at_all_end(*fh, buf, status);
}

#endif
#if defined(HAS_MPI_FILE_READ_ORDERED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_ordered
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_ordered)(MPI_File*     fh,
                                 void*         buf,
                                 int*          count,
                                 MPI_Datatype* datatype,
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

  *ierr = MPI_File_read_ordered(*fh, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_READ_ORDERED_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_ordered_begin
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_ordered_begin)(MPI_File*     fh,
                                       void*         buf,
                                       int*          count,
                                       MPI_Datatype* datatype,
                                       int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_read_ordered_begin(*fh, buf, *count, *datatype);
}

#endif
#if defined(HAS_MPI_FILE_READ_ORDERED_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_ordered_end
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_ordered_end)(MPI_File*   fh,
                                     void*       buf,
                                     MPI_Status* status,
                                     int*        ierr)
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

  *ierr = MPI_File_read_ordered_end(*fh, buf, status);
}

#endif
#if defined(HAS_MPI_FILE_READ_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_shared
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_shared)(MPI_File*     fh,
                                void*         buf,
                                int*          count,
                                MPI_Datatype* datatype,
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

  *ierr = MPI_File_read_shared(*fh, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write)(MPI_File*     fh,
                          void*         buf,
                          int*          count,
                          MPI_Datatype* datatype,
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

  *ierr = MPI_File_write(*fh, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_all
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_all)(MPI_File*     fh,
                              void*         buf,
                              int*          count,
                              MPI_Datatype* datatype,
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

  *ierr = MPI_File_write_all(*fh, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_all_begin
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_all_begin)(MPI_File*     fh,
                                    void*         buf,
                                    int*          count,
                                    MPI_Datatype* datatype,
                                    int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_write_all_begin(*fh, buf, *count, *datatype);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_all_end
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_all_end)(MPI_File*   fh,
                                  void*       buf,
                                  MPI_Status* status,
                                  int*        ierr)
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

  *ierr = MPI_File_write_all_end(*fh, buf, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at)(MPI_File*     fh,
                             MPI_Offset*   offset,
                             void*         buf,
                             int*          count,
                             MPI_Datatype* datatype,
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

  *ierr = MPI_File_write_at(*fh, *offset, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_AT_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at_all
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at_all)(MPI_File*     fh,
                                 MPI_Offset*   offset,
                                 void*         buf,
                                 int*          count,
                                 MPI_Datatype* datatype,
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

  *ierr = MPI_File_write_at_all(*fh,
                                *offset,
                                buf,
                                *count,
                                *datatype,
                                status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_AT_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at_all_begin
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at_all_begin)(MPI_File*     fh,
                                       MPI_Offset*   offset,
                                       void*         buf,
                                       int*          count,
                                       MPI_Datatype* datatype,
                                       int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_write_at_all_begin(*fh, *offset, buf, *count, *datatype);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_AT_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at_all_end
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at_all_end)(MPI_File*   fh,
                                     void*       buf,
                                     MPI_Status* status,
                                     int*        ierr)
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

  *ierr = MPI_File_write_at_all_end(*fh, buf, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ORDERED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_ordered
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_ordered)(MPI_File*     fh,
                                  void*         buf,
                                  int*          count,
                                  MPI_Datatype* datatype,
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

  *ierr = MPI_File_write_ordered(*fh, buf, *count, *datatype, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ORDERED_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_ordered_begin
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_ordered_begin)(MPI_File*     fh,
                                        void*         buf,
                                        int*          count,
                                        MPI_Datatype* datatype,
                                        int*          ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_write_ordered_begin(*fh, buf, *count, *datatype);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ORDERED_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_ordered_end
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_ordered_end)(MPI_File*   fh,
                                      void*       buf,
                                      MPI_Status* status,
                                      int*        ierr)
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

  *ierr = MPI_File_write_ordered_end(*fh, buf, status);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_shared
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_shared)(MPI_File*     fh,
                                 void*         buf,
                                 int*          count,
                                 MPI_Datatype* datatype,
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

  *ierr = MPI_File_write_shared(*fh, buf, *count, *datatype, status);
}

#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if defined(HAS_MPI_FILE_CALL_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_call_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_call_errhandler)(MPI_File* fh,
                                    int*      errorcode,
                                    int*      ierr)
{
  *ierr = MPI_File_call_errhandler(*fh, *errorcode);
}

#endif
#if defined(HAS_MPI_FILE_CREATE_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_create_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_create_errhandler)(MPI_File_errhandler_fn* function,
                                      MPI_Errhandler*         errhandler,
                                      int*                    ierr)
{
  *ierr = MPI_File_create_errhandler(function, errhandler);
}

#endif
#if defined(HAS_MPI_FILE_GET_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_get_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_get_errhandler)(MPI_File*       file,
                                   MPI_Errhandler* errhandler,
                                   int*            ierr)
{
  *ierr = MPI_File_get_errhandler(*file, errhandler);
}

#endif
#if defined(HAS_MPI_FILE_SET_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_set_errhandler
 * @note Auto-generated by wrapgen from template: fstd.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_set_errhandler)(MPI_File*       file,
                                   MPI_Errhandler* errhandler,
                                   int*            ierr)
{
  *ierr = MPI_File_set_errhandler(*file, *errhandler);
}

#endif

#else /* !NEED_F2C_CONV */

/**
 * @}
 * @name Fortran wrappers for administrative functions
 * @{
 */

#if defined(HAS_MPI_FILE_CLOSE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_close
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_close)(MPI_Fint* fh,
                          MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_close(&c_fh);

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_DELETE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_delete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_delete)(char*     filename,
                           MPI_Fint* info,
                           MPI_Fint* ierr,
                           int       filename_len)
{
  char* c_filename     = NULL;
  int   c_filename_len = filename_len;

  while ((c_filename_len > 0) && (filename[c_filename_len - 1] == ' '))
  {
    c_filename_len--;
  }
  c_filename = (char*)malloc((filename_len + 1) * sizeof (char));
  if (!c_filename)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_filename, filename, filename_len);
  c_filename[c_filename_len] = '\0';

  *ierr = MPI_File_delete(c_filename, PMPI_Info_f2c(*info));

  free(c_filename);
}

#endif
#if defined(HAS_MPI_FILE_GET_AMODE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_amode
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_amode)(MPI_Fint* fh,
                              MPI_Fint* amode,
                              MPI_Fint* ierr)
{
  *ierr = MPI_File_get_amode(PMPI_File_f2c(*fh), amode);
}

#endif
#if defined(HAS_MPI_FILE_GET_ATOMICITY) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_atomicity
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_atomicity)(MPI_Fint* fh,
                                  MPI_Fint* flag,
                                  MPI_Fint* ierr)
{
  *ierr = MPI_File_get_atomicity(PMPI_File_f2c(*fh), flag);
}

#endif
#if defined(HAS_MPI_FILE_GET_BYTE_OFFSET) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_byte_offset
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_byte_offset)(MPI_Fint*   fh,
                                    MPI_Offset* offset,
                                    MPI_Offset* disp,
                                    MPI_Fint*   ierr)
{
  *ierr = MPI_File_get_byte_offset(PMPI_File_f2c(*fh), *offset, disp);
}

#endif
#if defined(HAS_MPI_FILE_GET_GROUP) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_group
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_group)(MPI_Fint* fh,
                              MPI_Fint* group,
                              MPI_Fint* ierr)
{
  MPI_Group c_group;

  *ierr = MPI_File_get_group(PMPI_File_f2c(*fh), &c_group);

  *group = PMPI_Group_c2f(c_group);
}

#endif
#if defined(HAS_MPI_FILE_GET_INFO) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_info
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_info)(MPI_Fint* fh,
                             MPI_Fint* info_used,
                             MPI_Fint* ierr)
{
  MPI_Info c_info_used;

  *ierr = MPI_File_get_info(PMPI_File_f2c(*fh), &c_info_used);

  *info_used = PMPI_Info_c2f(c_info_used);
}

#endif
#if defined(HAS_MPI_FILE_GET_POSITION) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_position
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_position)(MPI_Fint*   fh,
                                 MPI_Offset* offset,
                                 MPI_Fint*   ierr)
{
  *ierr = MPI_File_get_position(PMPI_File_f2c(*fh), offset);
}

#endif
#if defined(HAS_MPI_FILE_GET_POSITION_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_position_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_position_shared)(MPI_Fint*   fh,
                                        MPI_Offset* offset,
                                        MPI_Fint*   ierr)
{
  *ierr = MPI_File_get_position_shared(PMPI_File_f2c(*fh), offset);
}

#endif
#if defined(HAS_MPI_FILE_GET_SIZE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_size)(MPI_Fint*   fh,
                             MPI_Offset* size,
                             MPI_Fint*   ierr)
{
  *ierr = MPI_File_get_size(PMPI_File_f2c(*fh), size);
}

#endif
#if defined(HAS_MPI_FILE_GET_TYPE_EXTENT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_type_extent
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_type_extent)(MPI_Fint* fh,
                                    MPI_Fint* datatype,
                                    MPI_Aint* extent,
                                    MPI_Fint* ierr)
{
  *ierr =
    MPI_File_get_type_extent(PMPI_File_f2c(*fh), PMPI_Type_f2c(
                               *datatype), extent);
}

#endif
#if defined(HAS_MPI_FILE_GET_VIEW) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_get_view
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_get_view)(MPI_Fint*   fh,
                             MPI_Offset* disp,
                             MPI_Fint*   etype,
                             MPI_Fint*   filetype,
                             char*       datarep,
                             MPI_Fint*   ierr,
                             int         datarep_len)
{
  MPI_Datatype c_etype;
  MPI_Datatype c_filetype;
  char*        c_datarep     = NULL;
  int          c_datarep_len = 0;

  c_datarep = (char*)malloc((datarep_len + 1) * sizeof (char));
  if (!c_datarep)
  {
    exit(EXIT_FAILURE);
  }

  *ierr = MPI_File_get_view(PMPI_File_f2c(
                              *fh), disp, &c_etype, &c_filetype, c_datarep);

  *etype    = PMPI_Type_c2f(c_etype);
  *filetype = PMPI_Type_c2f(c_filetype);

  c_datarep_len = strlen(c_datarep);
  strncpy(datarep, c_datarep, c_datarep_len);
  memset(datarep + c_datarep_len, ' ', datarep_len - c_datarep_len);
  free(c_datarep);
}

#endif
#if defined(HAS_MPI_FILE_OPEN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_open
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_open)(MPI_Fint* comm,
                         char*     filename,
                         MPI_Fint* amode,
                         MPI_Fint* info,
                         MPI_Fint* fh,
                         MPI_Fint* ierr,
                         int       filename_len)
{
  char*    c_filename     = NULL;
  int      c_filename_len = filename_len;
  MPI_File c_fh;

  while ((c_filename_len > 0) && (filename[c_filename_len - 1] == ' '))
  {
    c_filename_len--;
  }
  c_filename = (char*)malloc((filename_len + 1) * sizeof (char));
  if (!c_filename)
  {
    exit(EXIT_FAILURE);
  }
  strncpy(c_filename, filename, filename_len);
  c_filename[c_filename_len] = '\0';

  *ierr = MPI_File_open(PMPI_Comm_f2c(
                          *comm), c_filename, *amode, PMPI_Info_f2c(
                          *info), &c_fh);

  free(c_filename);
  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_PREALLOCATE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_preallocate
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_preallocate)(MPI_Fint*   fh,
                                MPI_Offset* size,
                                MPI_Fint*   ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_preallocate(c_fh, *size);

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_SEEK) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_seek
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_seek)(MPI_Fint*   fh,
                         MPI_Offset* offset,
                         MPI_Fint*   whence,
                         MPI_Fint*   ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_seek(c_fh, *offset, *whence);

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_SEEK_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_seek_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_seek_shared)(MPI_Fint*   fh,
                                MPI_Offset* offset,
                                MPI_Fint*   whence,
                                MPI_Fint*   ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_seek_shared(c_fh, *offset, *whence);

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_SET_ATOMICITY) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_atomicity
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_atomicity)(MPI_Fint* fh,
                                  MPI_Fint* flag,
                                  MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_set_atomicity(c_fh, *flag);

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_SET_INFO) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_info
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_info)(MPI_Fint* fh,
                             MPI_Fint* info,
                             MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_set_info(c_fh, PMPI_Info_f2c(*info));

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_SET_SIZE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_size)(MPI_Fint*   fh,
                             MPI_Offset* size,
                             MPI_Fint*   ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_set_size(c_fh, *size);

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_SET_VIEW) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_set_view
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_set_view)(MPI_Fint*   fh,
                             MPI_Offset* disp,
                             MPI_Fint*   etype,
                             MPI_Fint*   filetype,
                             char*       datarep,
                             MPI_Fint*   info,
                             MPI_Fint*   ierr,
                             int         datarep_len)
{
  MPI_File c_fh          = PMPI_File_f2c(*fh);
  char*    c_datarep     = NULL;
  int      c_datarep_len = datarep_len;

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

  *ierr = MPI_File_set_view(c_fh, *disp, PMPI_Type_f2c(
                              *etype), PMPI_Type_f2c(
                              *filetype), c_datarep, PMPI_Info_f2c(*info));

  *fh = PMPI_File_c2f(c_fh);
  free(c_datarep);
}

#endif
#if defined(HAS_MPI_FILE_SYNC) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_sync
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_sync)(MPI_Fint* fh,
                         MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  *ierr = MPI_File_sync(c_fh);

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_REGISTER_DATAREP) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA)
/**
 * Measurement wrapper for MPI_Register_datarep
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_Register_datarep)(char*     datarep,
                                void*     read_conversion_fn,
                                void*     write_conversion_fn,
                                void*     dtype_file_extent_fn,
                                void*     extra_state,
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
    MPI_Register_datarep(
      c_datarep,
      (MPI_Datarep_conversion_function*)
      read_conversion_fn,
      (MPI_Datarep_conversion_function*)
      write_conversion_fn,
      (MPI_Datarep_extent_function*)
      dtype_file_extent_fn,
      extra_state);

  free(c_datarep);
}

#endif

/**
 * @}
 * @name Fortran wrappers for access functions
 * @{
 */

#if defined(HAS_MPI_FILE_IREAD) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iread
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iread)(MPI_Fint* fh,
                          void*     buf,
                          MPI_Fint* count,
                          MPI_Fint* datatype,
                          MPI_Fint* request,
                          MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iread(c_fh, buf, *count, PMPI_Type_f2c(
                           *datatype), &c_request);

  *fh      = PMPI_File_c2f(c_fh);
  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_FILE_IREAD_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iread_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iread_at)(MPI_Fint*   fh,
                             MPI_Offset* offset,
                             void*       buf,
                             MPI_Fint*   count,
                             MPI_Fint*   datatype,
                             MPI_Fint*   request,
                             MPI_Fint*   ierr)
{
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iread_at(PMPI_File_f2c(
                              *fh), *offset, buf, *count,
                            PMPI_Type_f2c(*datatype), &c_request);

  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_FILE_IREAD_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iread_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iread_shared)(MPI_Fint* fh,
                                 void*     buf,
                                 MPI_Fint* count,
                                 MPI_Fint* datatype,
                                 MPI_Fint* request,
                                 MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iread_shared(c_fh, buf, *count, PMPI_Type_f2c(
                                  *datatype), &c_request);

  *fh      = PMPI_File_c2f(c_fh);
  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_FILE_IWRITE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iwrite
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iwrite)(MPI_Fint* fh,
                           void*     buf,
                           MPI_Fint* count,
                           MPI_Fint* datatype,
                           MPI_Fint* request,
                           MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_iwrite(c_fh, buf, *count, PMPI_Type_f2c(
                            *datatype), &c_request);

  *fh      = PMPI_File_c2f(c_fh);
  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_FILE_IWRITE_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iwrite_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iwrite_at)(MPI_Fint*   fh,
                              MPI_Offset* offset,
                              void*       buf,
                              MPI_Fint*   count,
                              MPI_Fint*   datatype,
                              MPI_Fint*   request,
                              MPI_Fint*   ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_File_iwrite_at(c_fh, *offset, buf, *count, PMPI_Type_f2c(
                         *datatype), &c_request);

  *fh      = PMPI_File_c2f(c_fh);
  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_FILE_IWRITE_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_iwrite_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_iwrite_shared)(MPI_Fint* fh,
                                  void*     buf,
                                  MPI_Fint* count,
                                  MPI_Fint* datatype,
                                  MPI_Fint* request,
                                  MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
  MPI_Request c_request;

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_File_iwrite_shared(c_fh, buf, *count, PMPI_Type_f2c(
                             *datatype), &c_request);

  *fh      = PMPI_File_c2f(c_fh);
  *request = PMPI_Request_c2f(c_request);
}

#endif
#if defined(HAS_MPI_FILE_READ) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read)(MPI_Fint* fh,
                         void*     buf,
                         MPI_Fint* count,
                         MPI_Fint* datatype,
                         MPI_Fint* status,
                         MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_read(c_fh, buf, *count, PMPI_Type_f2c(
                          *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read) */

#endif
#if defined(HAS_MPI_FILE_READ_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_all)(MPI_Fint* fh,
                             void*     buf,
                             MPI_Fint* count,
                             MPI_Fint* datatype,
                             MPI_Fint* status,
                             MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_read_all(c_fh, buf, *count, PMPI_Type_f2c(
                              *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_all) */

#endif
#if defined(HAS_MPI_FILE_READ_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_all_begin)(MPI_Fint* fh,
                                   void*     buf,
                                   MPI_Fint* count,
                                   MPI_Fint* datatype,
                                   MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_File_read_all_begin(c_fh, buf, *count, PMPI_Type_f2c(*datatype));

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_READ_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_all_end)(MPI_Fint* fh,
                                 void*     buf,
                                 MPI_Fint* status,
                                 MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_read_all_end(c_fh, buf, c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_all_end) */

#endif
#if defined(HAS_MPI_FILE_READ_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at)(MPI_Fint*   fh,
                            MPI_Offset* offset,
                            void*       buf,
                            MPI_Fint*   count,
                            MPI_Fint*   datatype,
                            MPI_Fint*   status,
                            MPI_Fint*   ierr)
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

  *ierr = MPI_File_read_at(PMPI_File_f2c(
                             *fh), *offset, buf, *count,
                           PMPI_Type_f2c(*datatype), c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_at) */

#endif
#if defined(HAS_MPI_FILE_READ_AT_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at_all)(MPI_Fint*   fh,
                                MPI_Offset* offset,
                                void*       buf,
                                MPI_Fint*   count,
                                MPI_Fint*   datatype,
                                MPI_Fint*   status,
                                MPI_Fint*   ierr)
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

  *ierr = MPI_File_read_at_all(PMPI_File_f2c(
                                 *fh), *offset, buf, *count,
                               PMPI_Type_f2c(*datatype), c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_at_all) */

#endif
#if defined(HAS_MPI_FILE_READ_AT_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at_all_begin)(MPI_Fint*   fh,
                                      MPI_Offset* offset,
                                      void*       buf,
                                      MPI_Fint*   count,
                                      MPI_Fint*   datatype,
                                      MPI_Fint*   ierr)
{
  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_read_at_all_begin(PMPI_File_f2c(
                                       *fh), *offset, buf, *count,
                                     PMPI_Type_f2c(*datatype));
}

#endif
#if defined(HAS_MPI_FILE_READ_AT_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_at_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_at_all_end)(MPI_Fint* fh,
                                    void*     buf,
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

  *ierr = MPI_File_read_at_all_end(PMPI_File_f2c(*fh), buf, c_status_ptr);

  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_at_all_end) */

#endif
#if defined(HAS_MPI_FILE_READ_ORDERED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_ordered
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_ordered)(MPI_Fint* fh,
                                 void*     buf,
                                 MPI_Fint* count,
                                 MPI_Fint* datatype,
                                 MPI_Fint* status,
                                 MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_read_ordered(c_fh, buf, *count, PMPI_Type_f2c(
                                  *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_ordered) */

#endif
#if defined(HAS_MPI_FILE_READ_ORDERED_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_ordered_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_ordered_begin)(MPI_Fint* fh,
                                       void*     buf,
                                       MPI_Fint* count,
                                       MPI_Fint* datatype,
                                       MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_File_read_ordered_begin(c_fh, buf, *count, PMPI_Type_f2c(*datatype));

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_READ_ORDERED_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_ordered_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_ordered_end)(MPI_Fint* fh,
                                     void*     buf,
                                     MPI_Fint* status,
                                     MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_read_ordered_end(c_fh, buf, c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_ordered_end) */

#endif
#if defined(HAS_MPI_FILE_READ_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_read_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_read_shared)(MPI_Fint* fh,
                                void*     buf,
                                MPI_Fint* count,
                                MPI_Fint* datatype,
                                MPI_Fint* status,
                                MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_read_shared(c_fh, buf, *count, PMPI_Type_f2c(
                                 *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_read_shared) */

#endif
#if defined(HAS_MPI_FILE_WRITE) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write)(MPI_Fint* fh,
                          void*     buf,
                          MPI_Fint* count,
                          MPI_Fint* datatype,
                          MPI_Fint* status,
                          MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_write(c_fh, buf, *count, PMPI_Type_f2c(
                           *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write) */

#endif
#if defined(HAS_MPI_FILE_WRITE_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_all)(MPI_Fint* fh,
                              void*     buf,
                              MPI_Fint* count,
                              MPI_Fint* datatype,
                              MPI_Fint* status,
                              MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_write_all(c_fh, buf, *count, PMPI_Type_f2c(
                               *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_all) */

#endif
#if defined(HAS_MPI_FILE_WRITE_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_all_begin)(MPI_Fint* fh,
                                    void*     buf,
                                    MPI_Fint* count,
                                    MPI_Fint* datatype,
                                    MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_File_write_all_begin(c_fh, buf, *count, PMPI_Type_f2c(*datatype));

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_all_end)(MPI_Fint* fh,
                                  void*     buf,
                                  MPI_Fint* status,
                                  MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_write_all_end(c_fh, buf, c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_all_end) */

#endif
#if defined(HAS_MPI_FILE_WRITE_AT) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at)(MPI_Fint*   fh,
                             MPI_Offset* offset,
                             void*       buf,
                             MPI_Fint*   count,
                             MPI_Fint*   datatype,
                             MPI_Fint*   status,
                             MPI_Fint*   ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr =
    MPI_File_write_at(c_fh, *offset, buf, *count, PMPI_Type_f2c(
                        *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_at) */

#endif
#if defined(HAS_MPI_FILE_WRITE_AT_ALL) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at_all
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at_all)(MPI_Fint*   fh,
                                 MPI_Offset* offset,
                                 void*       buf,
                                 MPI_Fint*   count,
                                 MPI_Fint*   datatype,
                                 MPI_Fint*   status,
                                 MPI_Fint*   ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr =
    MPI_File_write_at_all(c_fh, *offset, buf, *count, PMPI_Type_f2c(
                            *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_at_all) */

#endif
#if defined(HAS_MPI_FILE_WRITE_AT_ALL_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at_all_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at_all_begin)(MPI_Fint*   fh,
                                       MPI_Offset* offset,
                                       void*       buf,
                                       MPI_Fint*   count,
                                       MPI_Fint*   datatype,
                                       MPI_Fint*   ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr = MPI_File_write_at_all_begin(c_fh,
                                      *offset,
                                      buf,
                                      *count,
                                      PMPI_Type_f2c(*datatype));

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_AT_ALL_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_at_all_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_at_all_end)(MPI_Fint* fh,
                                     void*     buf,
                                     MPI_Fint* status,
                                     MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_write_at_all_end(c_fh, buf, c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_at_all_end) */

#endif
#if defined(HAS_MPI_FILE_WRITE_ORDERED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_ordered
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_ordered)(MPI_Fint* fh,
                                  void*     buf,
                                  MPI_Fint* count,
                                  MPI_Fint* datatype,
                                  MPI_Fint* status,
                                  MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr =
    MPI_File_write_ordered(c_fh, buf, *count, PMPI_Type_f2c(
                             *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_ordered) */

#endif
#if defined(HAS_MPI_FILE_WRITE_ORDERED_BEGIN) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_ordered_begin
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_ordered_begin)(MPI_Fint* fh,
                                        void*     buf,
                                        MPI_Fint* count,
                                        MPI_Fint* datatype,
                                        MPI_Fint* ierr)
{
  MPI_File c_fh = PMPI_File_f2c(*fh);

  #if defined(HAS_MPI_BOTTOM)
  if (buf == epk_mpif_bottom)
  {
    buf = MPI_BOTTOM;
  }
  #endif

  *ierr =
    MPI_File_write_ordered_begin(c_fh, buf, *count,
                                 PMPI_Type_f2c(*datatype));

  *fh = PMPI_File_c2f(c_fh);
}

#endif
#if defined(HAS_MPI_FILE_WRITE_ORDERED_END) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_ordered_end
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_ordered_end)(MPI_Fint* fh,
                                      void*     buf,
                                      MPI_Fint* status,
                                      MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_write_ordered_end(c_fh, buf, c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_ordered_end) */

#endif
#if defined(HAS_MPI_FILE_WRITE_SHARED) && !defined(NO_MPI_IO)
/**
 * Measurement wrapper for MPI_File_write_shared
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io
 */
void FSUB(MPI_File_write_shared)(MPI_Fint* fh,
                                 void*     buf,
                                 MPI_Fint* count,
                                 MPI_Fint* datatype,
                                 MPI_Fint* status,
                                 MPI_Fint* ierr)
{
  MPI_File    c_fh = PMPI_File_f2c(*fh);
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

  *ierr = MPI_File_write_shared(c_fh, buf, *count, PMPI_Type_f2c(
                                  *datatype), c_status_ptr);

  *fh = PMPI_File_c2f(c_fh);
  #if defined(HAS_MPI_STATUS_IGNORE)
  if (c_status_ptr != MPI_STATUS_IGNORE)
  #endif
  {
    PMPI_Status_c2f(c_status_ptr, status);
  }
} /* FSUB(MPI_File_write_shared) */

#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if defined(HAS_MPI_FILE_CALL_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_call_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_call_errhandler)(MPI_Fint* fh,
                                    MPI_Fint* errorcode,
                                    MPI_Fint* ierr)
{
  *ierr = MPI_File_call_errhandler(PMPI_File_f2c(*fh), *errorcode);
}

#endif
#if defined(HAS_MPI_FILE_CREATE_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_create_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_create_errhandler)(void*     function,
                                      void*     errhandler,
                                      MPI_Fint* ierr)
{
  *ierr =
    MPI_File_create_errhandler((MPI_File_errhandler_fn*)function,
                               (MPI_Errhandler*)errhandler);
}

#endif
#if defined(HAS_MPI_FILE_GET_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_get_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_get_errhandler)(MPI_Fint* file,
                                   void*     errhandler,
                                   MPI_Fint* ierr)
{
  *ierr = MPI_File_get_errhandler(PMPI_File_f2c(
                                    *file), (MPI_Errhandler*)errhandler);
}

#endif
#if defined(HAS_MPI_FILE_SET_ERRHANDLER) && !defined(NO_MPI_IO) \
  && !defined(NO_MPI_EXTRA) && !defined(NO_MPI_ERR)
/**
 * Measurement wrapper for MPI_File_set_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI 2.0
 * @ingroup io_err
 */
void FSUB(MPI_File_set_errhandler)(MPI_Fint* file,
                                   void*     errhandler,
                                   MPI_Fint* ierr)
{
  MPI_File c_file = PMPI_File_f2c(*file);

  *ierr = MPI_File_set_errhandler(c_file, *((MPI_Errhandler*)errhandler));

  *file = PMPI_File_c2f(c_file);
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
