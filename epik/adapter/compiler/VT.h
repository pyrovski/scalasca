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

#ifndef _VT_H
#define _VT_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Simple Vampirtrace Compiler Interface
 * 
 *-----------------------------------------------------------------------------
 */

EXTERN int VT_initialize(int* argc, char*** argv);
EXTERN int VT_finalize();
EXTERN int VT_symdef(int handle, char* name, char* group);
EXTERN int VT_begin(int handle);
EXTERN int VT_end(int handle);
EXTERN int VT_traceon();
EXTERN int VT_traceoff();

#endif
