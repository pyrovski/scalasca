/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef _EPK_OMPLOCK_H
#define _EPK_OMPLOCK_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

/*
 *-----------------------------------------------------------------------------
 *
 *  EPIK Library (Event Processing Interface Kit)
 *
 *  - OpenMP lock management
 *
 *-----------------------------------------------------------------------------
 */

#include "elg_defs.h"

EXTERN elg_ui4 epk_lock_init(const void* lock);
EXTERN elg_ui4 epk_lock_id(const void* lock);
EXTERN void    epk_lock_destroy(const void* lock);
EXTERN void    epk_lock_close();

#endif
