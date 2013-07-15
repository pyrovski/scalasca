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

#include "elg_sync.h"
#include "elg_pform.h"
#include "epk_conf.h"

#include <stdlib.h>
#include <stdio.h>

#define ELG_LOOP_COUNT 10
#define ELG_META_LOOP_COUNT 15
#define ELG_MAX_LOOP_COUNT 15

static double elg_offset_master(double* ltime, int slave, MPI_Comm comm, int lcount)
{
  int i, min;
  double tsend[ELG_MAX_LOOP_COUNT], trecv[ELG_MAX_LOOP_COUNT];
  double pingpong_time, sync_time;
  MPI_Status stat;
  
  if (lcount > ELG_MAX_LOOP_COUNT) lcount = ELG_MAX_LOOP_COUNT;
  /* exchange lcount ping pong messages with slave */
  
  for (i = 0; i < lcount; i++)
    {
      tsend[i] = elg_pform_wtime();
      PMPI_Send(NULL, 0, MPI_INT, slave, 1, comm);
      PMPI_Recv(NULL, 0, MPI_INT, slave, 2, comm, &stat);
      trecv[i] = elg_pform_wtime();
    }
  
  /* select ping pong with shortest transfer time */
  
  pingpong_time = trecv[0] - tsend[0];
  min = 0;
  
  for (i = 1; i < lcount; i++)
    if ((trecv[i] - tsend[i]) < pingpong_time)
      {
	pingpong_time = (trecv[i] - tsend[i]);
	min = i;
      }
  
  sync_time = tsend[min] + (pingpong_time / 2);
  
  /* send sync_time together with corresponding measurement index to slave */
  
  PMPI_Send(&min, 1, MPI_INT, slave, 3, comm);
  PMPI_Send(&sync_time, 1, MPI_DOUBLE, slave, 4, comm);

  /* the process considered as the global clock returns 0.0 as offset */
  
  *ltime = elg_pform_wtime();
  return 0.0;
}


static double elg_offset_slave(double* ltime, int master, MPI_Comm comm, int lcount)
{
  int i, min;
  double tsendrecv[ELG_MAX_LOOP_COUNT];
  double sync_time;
  MPI_Status stat;
  
  if (lcount > ELG_MAX_LOOP_COUNT) lcount = ELG_MAX_LOOP_COUNT;
  
  for (i = 0; i < lcount; i++)
    {
      PMPI_Recv(NULL, 0, MPI_INT, master, 1, comm, &stat);
      tsendrecv[i] = elg_pform_wtime();
      PMPI_Send(NULL, 0, MPI_INT, master, 2, comm);
    }
  
  /* receive corresponding time together with its index from master */
  
  PMPI_Recv(&min, 1, MPI_INT, master, 3, comm,  &stat);
  PMPI_Recv(&sync_time, 1, MPI_DOUBLE, master, 4, comm, &stat);
  
  *ltime = tsendrecv[min];
  return sync_time - *ltime; 
}

double elg_offset(double* ltime, MPI_Comm comm,
                                 MPI_Comm mach_comm, MPI_Comm host_comm)
{
  int i;
  int myrank, myrank_mach, myrank_host, myrank_sync, myrank_metasync;
  int numprocs, numnodes, nummachs;
  double offset;
  
  MPI_Comm sync_comm;
  MPI_Comm metasync_comm;
  
  offset = 0.0;
  *ltime = elg_pform_wtime();
  
  /* barrier at entry */
  PMPI_Barrier(comm);
  
  PMPI_Comm_rank(comm, &myrank);
  PMPI_Comm_size(comm, &numprocs);
  
  /* get rank in communicator containing all processes on the same machine */
  PMPI_Comm_rank(mach_comm, &myrank_mach);
  
  /* get rank in communicator containing all processes on the same node */
  PMPI_Comm_rank(host_comm, &myrank_host);
  
  /* create communicator containing all processes with rank zero in mach_comm */
  PMPI_Comm_split(comm, myrank_mach, 0, &metasync_comm);
  PMPI_Comm_rank(metasync_comm, &myrank_metasync);
  PMPI_Comm_size(metasync_comm, &nummachs);
  
  /* create communicator containing all processes with rank zero in host_comm */
  PMPI_Comm_split(mach_comm, myrank_host, 0, &sync_comm);
  PMPI_Comm_rank(sync_comm, &myrank_sync);
  PMPI_Comm_size(sync_comm, &numnodes);
  
  /* measure offsets between all machines and the root machine (rank 0 in metasync_comm) */
  
  if (myrank_mach == 0)
    {
      for (i = 1; i < nummachs; i++)
	{
	  PMPI_Barrier(metasync_comm);
	  if (myrank_metasync == i){
	    offset = elg_offset_slave(ltime, 0, metasync_comm, ELG_META_LOOP_COUNT);
	  }
	  else if (myrank_metasync == 0) {
	    offset = elg_offset_master(ltime, i, metasync_comm, ELG_META_LOOP_COUNT);
	  }
	}
    }
  
  /* distribute offset and ltime across all processes on the same node */
  
  PMPI_Bcast(&offset, 1, MPI_DOUBLE, 0, sync_comm);
  
  /* measure offsets between all nodes and the root node (rank 0 in sync_comm) */
  
  if (myrank_host == 0)
    {
      if (!elg_pform_is_gclock()) 
	{
	  for (i = 1; i < numnodes; i++)
	    {
	      PMPI_Barrier(sync_comm);
	      if (myrank_sync == i)
		offset = offset + elg_offset_slave(ltime, 0, sync_comm, ELG_LOOP_COUNT);
	      else if (myrank_sync == 0)
		offset = offset + elg_offset_master(ltime, i, sync_comm, ELG_LOOP_COUNT);
	    }
	}
    }
  
  /* distribute offset and ltime across all processes on the same node */
  
  PMPI_Bcast(&offset, 1, MPI_DOUBLE, 0, host_comm);
  PMPI_Bcast(ltime, 1, MPI_DOUBLE, 0, host_comm);
  
  /* barrier at exit */
  PMPI_Barrier(comm);
  
  return offset;
}

