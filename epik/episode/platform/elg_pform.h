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

#ifndef _ELG_PFORM_H
#define _ELG_PFORM_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_pform_defs.h"
#include "../epik_topol.h"
/*
 *-----------------------------------------------------------------------------
 *
 *  EPILOG Library (Event Processing, Investigating, and Logging)
 *
 *  - Platform specific functions 
 *   (every platform must provide its own implementation)
 * 
 *----------------------------------------------------------------------------- 
 */

/*
  
  (0) Platform specific initialization:

  Called once on every node to allow platform specific initializations

  - elg_pform_init()

  (1) Event notification:

  The module is notified upon the occurrence of the following events
  by calling one of the event-notification functions:

  - elg_pform_mpi_init():     MPI has just been initialized 
  - elg_pform_mpi_finalize(): MPI will be finalized now 

  (2) Clock

  If the platform supports a global clock then elg_pform_is_gclock()
  returns a value != 0, otherwise it returns 0. Depending on the
  result elg_pform_wtime() returns either the local or the global
  wall-clock time in seconds.

  (3) File system
  
  It is assumed that the platform provides a global file system and
  that the current working directory is part of it.  elg_pform_ldir()
  returns the value of EPK_LDIR which may be used to specify the name
  of a local directory that can be used to store temporary files.

  (3) Platform and SMP node information

  Configuration information of the total system and the SMP nodes
  where the function is called

  elg_pform_name():          Unique string identifying the platform
  elg_pform_num_nodes():     Total number of SM nodes in the system
  elg_pform_node_id():       Unique number identifying the SMP node
  elg_pform_node_name():     Unique string identifying the SMP node
  elg_pform_node_num_cpus(): Number of CPUs in SMP node
  EPIK_Pform_hw_topol():      HW topology
 
*/

#define ELG_PFORM_LDIR EPK_LDIR
#define ELG_PFORM_GDIR EPK_GDIR

/* platform specific initialization */
EXTERN void   elg_pform_init();

/* MPI event notification */
EXTERN void   elg_pform_mpi_init(); 
EXTERN void   elg_pform_mpi_finalize(); 

/* directory of global file system  */
EXTERN const char*  elg_pform_gdir();

/* directory of local file system  */
EXTERN const char*  elg_pform_ldir();

/* is a global clock provided ? */
EXTERN int    elg_pform_is_gclock();

/* local or global wall-clock time in seconds */
#ifndef elg_pform_wtime
EXTERN double elg_pform_wtime();
#endif

/* platform name */
EXTERN const char*  elg_pform_name();

/* total number of nodes (if available, otherwise 1) */
EXTERN int    elg_pform_num_nodes();

/* unique numeric SMP-node identifier */
EXTERN long   elg_pform_node_id();

/* unique string SMP-node identifier */
EXTERN const char*  elg_pform_node_name();

/* number of CPUs */
EXTERN int    elg_pform_num_cpus();

/* Creates hardware topology oject */
EXTERN EPIK_TOPOL * EPIK_Pform_hw_topol();

/* Gives hardware topology coordinates, thread-wise */
EXTERN void EPIK_Pform_hw_topol_getcoords(EPIK_TOPOL * t);

/* return core id on which thread tid is running */
EXTERN int elg_pform_hw_core_id(int tid);

#endif





