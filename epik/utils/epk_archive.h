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

#ifndef _EPK_ARCHIVE_H
#define _EPK_ARCHIVE_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

typedef enum {
    EPK_TYPE_DIR=0,  /* directory */
    EPK_TYPE_ESD=1,  /* definitions */
    EPK_TYPE_ETM=2,  /* sum profile */
    EPK_TYPE_ELG=3,  /* event trace */
    EPK_TYPE_EMP=4,  /* id mappings */
    EPK_TYPE_OTF=5,  /* OTF trace */
    EPK_TYPE_CUBE=6, /* CUBE summary */
    EPK_TYPE_SION=7, /* SION traces */
    EPK_TYPE_MAX=8
} epk_type_t;

/*
 *-----------------------------------------------------------------------------
 * management of the experiment measurement archive directory (and format)
 *-----------------------------------------------------------------------------
 */

/* convert EPIK archive name to ELG filename if necessary */
EXTERN char* epk_get_elgfilename ( const char* path );

/* get the pretty name of the type of archive file */
EXTERN char*    epk_archive_filetype ( const char* filename );

/* determine whether given filename is an epik archive name */
EXTERN char*    epk_archive_is_epik ( const char* fprefix );

/* get the experiment measurement archive base directory name */
EXTERN char*    epk_archive_get_name ( void );

/* set the experiment measurement archive base directory name */
EXTERN char*    epk_archive_set_name ( const char* name );

/* get an archive (sub)directory */
EXTERN char*    epk_archive_directory ( const epk_type_t type );

/* get an archive filename of given type from given name */
EXTERN char*    epk_archive_filename ( const epk_type_t type, const char* name );

/* get an archive rank filename of given type from given name and rank */
EXTERN char*    epk_archive_rankname ( const epk_type_t type, const char* name,
                                       const unsigned rank );

/* prepare to update the experiment measurement archive */
EXTERN void     epk_archive_update ( char* filename );

/* commit the file in the experiment measurement archive */
EXTERN void     epk_archive_commit ( char* filename );

/* check whether the experiment archive exists */
EXTERN int      epk_archive_exists ( const char* dirname );

/* verify the non-existance of the desired experiment archive directory */
EXTERN int      epk_archive_verify ( const char* dirname );

/* check whether the experiment archive is locked */
EXTERN int      epk_archive_locked (void); 

/* remove the experiment archive lock file */
EXTERN void     epk_archive_unlock (void);

/* save starting directory */
EXTERN void     epk_archive_getcwd (void);

/* return to starting directory */
EXTERN void     epk_archive_return (void);

/* create the experiment archive and optional subdirectory (if necessary) */
EXTERN int      epk_archive_create ( const epk_type_t type );

/* remove the experiment archive subdirectory of the specified type */
EXTERN void     epk_archive_remove ( const epk_type_t type );

/* remove the named file (from the experiment archive) */
EXTERN void     epk_archive_remove_file ( char* filename );

/* remove the specified archive subdirectory and all of its contents */
EXTERN void     epk_archive_remove_all ( const epk_type_t type,
                                    const char* fprefix, const unsigned ranks);

#endif
