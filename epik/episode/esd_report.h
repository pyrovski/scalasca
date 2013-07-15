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

#ifndef _ESD_REPORT_H
#define _ESD_REPORT_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "esd_unify.h"

EXTERN int esd_report_open (char* filename);

EXTERN void esd_report_defs (EsdUnifyData* udata, int unknowns);

EXTERN void esd_report_paths (EsdUnifyData* udata, int unknowns);

EXTERN void esd_report_sev_row (int metricid, int cpathid, double* sevbuf);

EXTERN void esd_report_sev_threads (int metricid, int cpathid, double* sevbuf);

EXTERN void esd_report_close (void);

#endif
