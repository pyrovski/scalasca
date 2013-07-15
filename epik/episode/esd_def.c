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

#include "esd_def.h"
#include "esd_gen.h"
#include "epk_conf.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 *-----------------------------------------------------------------------------
 * Defining source code entities
 *-----------------------------------------------------------------------------
 */

elg_ui4 esd_def_string(const char* str)
{
  static elg_ui4 esd_string_id = 0;

  int stringlen = strlen(str) + 1;

  if (stringlen <= 250)
    {
      EsdGen_write_STRING(ESDPROC_GEN(proc), esd_string_id, 0, str);
    }
  else
    {
      int i;
      int restlen = stringlen - 250;
      elg_ui1 cntc  = (restlen / 255) + (restlen % 255 ? 1 : 0);

      EsdGen_write_STRING(ESDPROC_GEN(proc), esd_string_id, cntc, str);
      str += 250; 
      
      for (i = 1; i < cntc; i++)
	{
	  EsdGen_write_STRING_CNT(ESDPROC_GEN(proc), str);
	  str += 255;
	}
      /* last continuation record */
      EsdGen_write_STRING_CNT(ESDPROC_GEN(proc), str);
    }

  return esd_string_id++;
}

elg_ui4 esd_def_file(const char* fname)
{
  static elg_ui4 esd_file_id = 0;

  elg_ui4 strid = esd_def_string(fname);

  EsdGen_write_FILE(ESDPROC_GEN(proc), esd_file_id, strid);
  return esd_file_id++;
}


elg_ui4 esd_def_region(const char* rname,
		       elg_ui4 fid,
		       elg_ui4 begln,
		       elg_ui4 endln,
		       const char* rdesc,
		       elg_ui1 rtype)
{
  static elg_ui4 esd_region_id=0;

  elg_ui4 rdid = esd_def_string(rdesc);
  elg_ui4 rnid = esd_def_string(rname);

#ifndef DEBUG
  if (strcmp(rdesc,"USR")==0 || (rtype >= ELG_OMP_PARALLEL))
#endif
    elg_cntl_msg("%s region %d[%02d]:\"%s\"", rdesc, esd_region_id, rtype, rname);

  if (rtype == ELG_OMP_PARALLEL) {
    extern void esd_def_fork_rid(elg_ui4);
    esd_def_fork_rid(esd_region_id);
  }

  EsdGen_write_REGION(ESDPROC_GEN(proc),
		      esd_region_id,
		      rnid,
		      fid,
		      begln,
		      endln,
		      rdid,
		      rtype);
  return esd_region_id++;
}

elg_ui4 esd_def_csite(elg_ui4 fid,
		      elg_ui4 lno,
		      elg_ui4 erid, 
		      elg_ui4 lrid)
{
    static elg_ui4 esd_csite_id = 0;

    if ( lrid != ELG_NO_ID ) fprintf(stderr, ">>>>>>>>>>>> lrid != ELG_NO_ID\n");

    EsdGen_write_CALL_SITE(ESDPROC_GEN(proc),
                           esd_csite_id, fid, lno, erid, lrid);
    return esd_csite_id++;
}

elg_ui4 esd_def_callpath (elg_ui4 rid, elg_ui4 ppid, elg_ui8 order)
{
    static elg_ui4 esd_callpath_id = 0;

    /* check validity of definition */
    if ((ppid > esd_callpath_id) && (ppid != ELG_NO_ID)) return ELG_NO_ID;

    EsdGen_write_CALL_PATH(ESDPROC_GEN(proc),
                           esd_callpath_id, rid, ppid, order);
    return esd_callpath_id++;
}

/**
 * @brief Create a distributed communicator definition entry
 * @note  Entries of this kind are only used in the temporary 
 *        definition buffer during measurement and will be
 *        surplaced by comm_ref/group records during unification.
 * @param[in]  cid    communicator id unique to the root process
 * @param[in]  root   global rank of root process in this communicator
 * @param[in]  lcid   local communicator id used in event records
 * @param[in]  lrank  rank of local process in new communicator
 * @param[in]  size   size of new communicator
 */
void esd_def_mpi_comm(elg_ui4 cid,
                      elg_ui4 root,
                      elg_ui4 lcid,
                      elg_ui4 lrank,
                      elg_ui4 size)
{
  EsdGen_write_MPI_COMM_DIST(ESDPROC_GEN(proc), cid, root, lcid, lrank, size);
}


void esd_def_mpi_group(elg_ui4 gid,
                       elg_ui1 mode,
                       elg_ui4 grpc,
                       elg_ui4 grpv[])
{
    EsdGen_write_MPI_GROUP(ESDPROC_GEN(proc), gid, mode, grpc, grpv);
}


/* formerly elg_mpi_win */
void esd_def_mpi_win( elg_ui4 wid, elg_ui4 cid )
{
    EsdGen_write_MPI_WIN( ESDPROC_GEN(proc), wid, cid );
}


/*
    machine and node description definition
    including hardware topology where available
*/

void esd_def_machine( elg_ui4 node_id, elg_ui4 proc_id, elg_ui4 num_procs, elg_ui4 num_thrds )
{
  EPIK_TOPOL* topology, *topoProcessesVSthreads;

  double clckrt = 0.0;
  int tid;
  elg_ui4 my_machine;

#if (defined (EPK_METR))
  clckrt      = esd_metric_clckrt();
#endif

  my_machine = epk_str2int(epk_get(EPK_MACHINE_ID));
  EsdGen_write_MACHINE(ESDPROC_GEN(proc), 
		       my_machine, elg_pform_num_nodes(), esd_def_string(epk_get(EPK_MACHINE_NAME)));

  EsdGen_write_NODE(ESDPROC_GEN(proc),
                    node_id, my_machine, elg_pform_num_cpus(), 
		    esd_def_string(elg_pform_node_name()), clckrt);

  /* write process/thread location records */
  for (tid = 0; tid < (int)num_thrds; tid++)
    {
      esd_def_location(my_machine, node_id, proc_id, tid);
    }
  
  /* Record topology if available */
  if ( (topology = EPIK_Pform_hw_topol()) )
    {
      EPIK_Cart_commit(topology);
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
      {      
#pragma omp parallel for ordered num_threads(num_thrds)
	for (tid = 0; tid < (int)num_thrds; tid++) {
#pragma omp ordered
	  {
	    /* Create coords for each thread
	     * (includes MPI rank ==  masterthread */
	    EPIK_Pform_hw_topol_getcoords(topology);
	    EPIK_Cart_coords_commit(topology);
	  }
	}
      }
#else
      {
	/* Create coords for MPI rank */
	EPIK_Pform_hw_topol_getcoords(topology);
	EPIK_Cart_coords_commit(topology);
      }
#endif
    } 

    /* process x thread topology */

  topoProcessesVSthreads=EPIK_Cart_create("Process/Thread",2);
  EPIK_Cart_add_dim(topoProcessesVSthreads,num_thrds,1,"Thread");
  EPIK_Cart_add_dim(topoProcessesVSthreads,num_procs,0,"Process");
  EPIK_Cart_commit(topoProcessesVSthreads);
#if (defined (ELG_OMPI) || defined (ELG_OMP)) 
#pragma omp parallel for ordered num_threads(num_thrds)
  for (tid = 0; tid < (int)num_thrds; tid++) {
#pragma omp ordered
      {
	EPIK_Cart_set_coords(topoProcessesVSthreads,omp_get_thread_num(),proc_id);
        EPIK_Cart_coords_commit(topoProcessesVSthreads);
      }
    }
#else
    {
      /* In the serial and MPI case there are no threads, so thread
       * coordinate is always 0 */
      EPIK_Cart_set_coords(topoProcessesVSthreads,0,proc_id);
      EPIK_Cart_coords_commit(topoProcessesVSthreads);
    }
#endif
}

/*
    Location of process/thread within machine
*/

void esd_def_location(elg_ui4 mach_id, elg_ui4 node_id, elg_ui4 proc_id, elg_ui4 thrd_id)
{
    /* ensure local location identifier is set */
    elg_ui4 location_id = thrd_id;
    EsdGen_write_LOCATION(ESDPROC_GEN(proc),
                          location_id, mach_id, node_id, proc_id, thrd_id);
}

/*
    Process (name) definition
*/

void esd_def_process(elg_ui4 proc_id, const char* proc_name)
{
    EsdGen_write_PROCESS(ESDPROC_GEN(proc),
                        proc_id, esd_def_string(proc_name));
}

/*
    Thread (name) definition
*/

void esd_def_thread(elg_ui4 proc_id, elg_ui4 thrd_id, const char* thrd_name)
{
    EsdGen_write_THREAD(ESDPROC_GEN(proc),
                        thrd_id, proc_id, esd_def_string(thrd_name));
}

/*
    Metric descriptions
*/

void esd_def_metrics( )
{
#if (defined (EPK_METR))
  int i;

  /* write metric-description records */
  for ( i = 0; i < esd_metric_num(); i++ )
    EsdGen_write_METRIC(ESDPROC_GEN(proc),
			i,
			esd_def_string(esd_metric_name(i)),
			esd_def_string(esd_metric_descr(i)),
			esd_metric_dtype(i),
			esd_metric_mode(i),
			esd_metric_iv(i));
  
#endif
}

/*
    Record of event types.
*/

void esd_def_event_types (elg_ui4 ntypes, elg_ui1 typev[])
{
    EsdGen_write_EVENT_TYPES(ESDPROC_GEN(proc),
                          ntypes, typev);
}

/*
    Record(s) of event type counts.
*/

void esd_def_event_counts (elg_ui4 ntypes, elg_ui4 countv[])
{
    EsdGen_write_EVENT_COUNTS(ESDPROC_GEN(proc),
                          ntypes, countv);
}

/* 
   defines a record of type ELG_CART_TOPOLOGY 
*/
elg_ui4 esd_def_cart (elg_ui4 cid,
		      EPIK_TOPOL * topology)
{
  int i;
  static elg_ui4 esd_cart_id       = 0;
  elg_ui4 topo_name_id=ELG_NO_ID;
  elg_ui4 dim_names_ids[topology->num_dims];

  for ( i = 0 ; i < topology->num_dims ; i++ ) {
    dim_names_ids[i]=ELG_NO_ID;
  }
  topology->topid=esd_cart_id++;
    if(topology->dim_names!=NULL) {
    for ( i = 0 ; i < topology->num_dims ; i++ ){
        if(topology->dim_names[i]!=NULL) {
	    /* Creates one string id to each named dimension */
            dim_names_ids[i]=esd_def_string(topology->dim_names[i]);
        }
    }
  } 
    if(topology->topo_name!=NULL) {
            topo_name_id=esd_def_string(topology->topo_name);
    }
    
    EsdGen_write_CART_TOPOLOGY(ESDPROC_GEN(proc),
			       cid, topo_name_id, dim_names_ids, topology);

    return topology->topid;
}

/* 
    each process calls this function to write its coordinates
    as defined by the topology
*/
void esd_def_coords(EPIK_TOPOL * topology)
{
    if (topology->topid == ELG_NO_ID)
        return;
    /* ensure local location identifier is set */
    elg_ui4 location_id = ESD_MY_THREAD;
    EsdGen_write_CART_COORDS(ESDPROC_GEN(proc),
			    location_id, topology);
}

