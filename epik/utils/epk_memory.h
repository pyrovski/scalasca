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

#ifndef _EPK_MEMORY_H
#define _EPK_MEMORY_H


#ifdef __cplusplus
#  define EXTERN extern "C"
#else
#  define EXTERN extern
#endif


/* current memory usage in kilobytes */
EXTERN int epk_memusage();


#endif   /* !_EPK_MEMORY_H */
