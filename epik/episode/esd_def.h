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

#ifndef _ESD_DEF_H
#define _ESD_DEF_H

#ifdef __cplusplus
#   define EXTERN extern "C" 
#else
#   define EXTERN extern 
#endif

#include "elg_defs.h"
#include "epik_topol.h"

/*
 *-----------------------------------------------------------------------------
 * Definition records
 *-----------------------------------------------------------------------------
 */

/* 
   This function can be used to register a source file. Generates an
   ELG_FILE record as well as appropriate ELG_STRING and ELG_STRING_CNT records.
   The file identifier is assigned automatically and returned.  
*/

EXTERN elg_ui4  esd_def_file       ( const char* fname );

/* 
   This function can be used to register a source region. Generates an
   ELG_REGION record as well as appropriate ELG_STRING and ELG_STRING_CNT records.
   The file identifier fid has to be defined in advance.
   The region identifier is assigned automatically and returned.
*/

EXTERN elg_ui4  esd_def_region     ( const char* rname,
				     elg_ui4 fid,
				     elg_ui4 begln,
				     elg_ui4 endln,
				     const char* rdesc,
				     elg_ui1 rtype );

/* 
   This function can be used to register a call site. Generates an
   ELG_CALL_SITE record. The file identifier fid as well as the identifiers
   of the regions to be left and entered have to be defined in advance. 
   The call-site identifier is assigned automatically and returned.
*/

EXTERN elg_ui4  esd_def_csite     ( elg_ui4 fid,
				    elg_ui4 lno,
				    elg_ui4 erid, 
				    elg_ui4 lrid );
/* 
   This function can be used to register a call path.  Generates an
   ELG_CALL_PATH record.  The parent call-path identifier ppid as well as
   the identifier of the extension region rid have to be defined in advance. 
   The order parameter is used to organise otherwise identical call-paths
   (e.g., from the same process but with different return addresses, or
   based on first instance timestamp for different processes.)
   The call-path identifier is assigned automatically and returned.
*/

EXTERN elg_ui4  esd_def_callpath  ( elg_ui4 rid,
				    elg_ui4 ppid,
				    elg_ui8 order );

/*
   Defines an MPI communicator. Generates a distributed ELG_MPI_COMM_DIST
   record.
*/   
EXTERN void esd_def_mpi_comm      ( elg_ui4 cid,
                                    elg_ui4 root,
                                    elg_ui4 lcid,
                                    elg_ui4 lrank,
                                    elg_ui4 size );

/*
   Defines an MPI group. Generates an ELG_MPI_GROUP record.
 */
EXTERN void esd_def_mpi_group     ( elg_ui4 gid,
                                    elg_ui1 mode,
                                    elg_ui4 grpc,
                                    elg_ui4 grpv[] );

/* 
   Defines an MPI window.  Generates an ELG_MPI_WIN record.
*/

EXTERN void esd_def_mpi_win        ( elg_ui4 wid, elg_ui4 cid);


/*
  The machine definition is determined internally.
*/

EXTERN void esd_def_machine         ( elg_ui4 node_id, elg_ui4 proc_id,
                                      elg_ui4 num_procs, elg_ui4 num_thrds );

/*
  Process/thread location within machine.
*/

EXTERN void esd_def_location        ( elg_ui4 mach_id, 
                                      elg_ui4 node_id,
                                      elg_ui4 proc_id,
                                      elg_ui4 thrd_id );

EXTERN void esd_def_process         ( elg_ui4 proc_id, 
                                      const char* proc_name );

EXTERN void esd_def_thread          ( elg_ui4 proc_id, 
                                      elg_ui4 thrd_id,
                                      const char* thrd_name );

/*
  Metric definitions are internally determined.
*/

EXTERN void esd_def_metrics         ( );

/*
  Record of event types.
*/

EXTERN void esd_def_event_types     ( elg_ui4 ntypes,
				      elg_ui1 typev[] );

/*
  Record(s) of event type counts.
*/

EXTERN void esd_def_event_counts    ( elg_ui4 ntypes,
				      elg_ui4 countv[] );


EXTERN  elg_ui4 esd_def_string(const char* str);

/*
  This function can be used to register a cartesian topology. Generates an
  ELG_CART_TOPOLOGY record. The number of cartesian dimensions as well as 
  the arrays storing information about the number of processes in each 
  dimension and periodicity in each dimension have to be defined in advance.
  The Cartesian topology identifier is assigned automatically and returned.
*/
elg_ui4 esd_def_cart         ( elg_ui4 cid,
                               EPIK_TOPOL * top);

/*
   Generates an ELG_CART_COORDS record
*/
void esd_def_coords          ( EPIK_TOPOL * top );

#endif
