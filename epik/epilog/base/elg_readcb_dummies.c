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

#include <stdlib.h>
#include "elg_error.h"
#include "elg_readcb.h"

void elg_readcb_ALL( elg_ui1 type, 
		   elg_ui1 length,
                   void* userdata )
{}

void elg_readcb_STRING( elg_ui4 strid,      /* string identifier */
		      elg_ui1 cntc,         /* number of continuation records */
		      const char* str,      /* string */
                      void* userdata ) 
{}

void elg_readcb_STRING_CNT( const char* str, /* continued string */
                          void* userdata ) 
{}

void elg_readcb_MACHINE( elg_ui4 mid,       /* machine identifier */
		       elg_ui4 nodec,       /* number of nodes */
		       elg_ui4 mnid,        /* machine name identifier */
                       void* userdata ) 
{}

void elg_readcb_NODE( elg_ui4 nid,          /* node identifier */
		    elg_ui4 mid,            /* machine identifier */
		    elg_ui4 cpuc,           /* number of CPUs */
		    elg_ui4 nnid,           /* node name identifer */
		    elg_d8  cr,             /* number of clock cycles/sec */
                    void* userdata ) 
{}

void elg_readcb_PROCESS( elg_ui4 pid,       /* process identifier */
		       elg_ui4 pnid,        /* process name identifier */
                       void* userdata ) 
{}

void elg_readcb_THREAD( elg_ui4 tid,        /* thread identifier */
		      elg_ui4 pid,          /* process identifier */
		      elg_ui4 tnid,         /* thread name identifier */
                      void* userdata ) 
{}

void elg_readcb_LOCATION( elg_ui4 lid,      /* location identifier */
			elg_ui4 mid,        /* machine identifier */
			elg_ui4 nid,        /* node identifier */
			elg_ui4 pid,        /* process identifier */
			elg_ui4 tid,        /* thread identifier */
                        void* userdata ) 
{}

void elg_readcb_FILE( elg_ui4 fid,          /* file identifier */
		    elg_ui4 fnid,           /* file name identifier */
                    void* userdata ) 
{}

void elg_readcb_REGION( elg_ui4 rid,        /* region identifier */
		      elg_ui4 rnid,         /* region name identifier */
		      elg_ui4 fid,          /* source file identifier */
		      elg_ui4 begln,        /* begin line number */
		      elg_ui4 endln,        /* end line number */
		      elg_ui4 rdid,         /* region description identifier */
		      elg_ui1 rtype,        /* region type */
                      void* userdata ) 
{}

void elg_readcb_CALL_SITE( elg_ui4 csid,    /* call site identifier */
			 elg_ui4 fid,       /* source file identifier */
			 elg_ui4 lno,       /* line number */
			 elg_ui4 erid,      /* region identifer to be entered */
			 elg_ui4 lrid,      /* region identifer to be left */
                         void* userdata ) 
{}

void elg_readcb_CALL_PATH( elg_ui4 cpid,    /* call-path identifier */
			 elg_ui4 rid,       /* node region identifier */
			 elg_ui4 ppid,      /* parent call-path identifier */
			 elg_ui8 order,     /* node order specifier */
                         void* userdata ) 
{}

void elg_readcb_METRIC( elg_ui4 metid,      /* metric identifier */
		      elg_ui4 metnid,       /* metric name identifier */
		      elg_ui4 metdid,       /* metric description identifier */
		      elg_ui1 metdtype,     /* metric data type */
		      elg_ui1 metmode,      /* metric mode */
		      elg_ui1 metiv,        /* time interval referenced */
                      void* userdata ) 
{}

void elg_readcb_MPI_GROUP( elg_ui4 gid,     /* group identifier */
                           elg_ui1 mode,    /* mode flags */
                           elg_ui4 grpc,    /* number of ranks in group */
                           elg_ui4 grpv[],  /* local |-> global rank mapping */
                           void*   userdata)
{}

void elg_readcb_MPI_COMM_DIST ( elg_ui4 cid,    /* communicator id unique to root */
                                elg_ui4 root,   /* global rank of root process */
                                elg_ui4 lcid,   /* local communicator id on process */
                                elg_ui4 lrank,  /* local rank of process */
                                elg_ui4 size,   /* size of communicator */
                                void* userdata )
{}

void elg_readcb_MPI_COMM_REF( elg_ui4 cid,  /* communicator identifier */
                              elg_ui4 gid,  /* group identifier */
                              void* userdata )
{}

void elg_readcb_MPI_COMM( elg_ui4 cid,      /* communicator identifier */
			elg_ui1 mode,       /* mode flags */
			elg_ui4 grpc,       /* size of bitstring in bytes */
			elg_ui1 grpv[],     /* bitstring defining the group */
                        void* userdata ) 
{}

void elg_readcb_CART_TOPOLOGY(elg_ui4 topid,     /* topology id */
			      elg_ui4 tnid,      /* topology name id */
			      elg_ui4 cid,       /* communicator id */
			      elg_ui1 ndims,     /* number of dimensions */
			      elg_ui4 dimv[],    /* number of processes in each dim */  
			      elg_ui1 periodv[], /* periodicity in each dim */
			      elg_ui4 dimids[],  /* string ids of dimension names */
			      void* userdata)      
{}

void elg_readcb_CART_COORDS   ( elg_ui4 topid,  /* topology id */
			      elg_ui4 lid,      /* location id */
			      elg_ui1 ndims,    /* number of dimensions */
			      elg_ui4 coordv[], /* coordinates in each dimension */
			      void* userdata)
{}

void elg_readcb_OFFSET( elg_d8  ltime,      /* local time */
		      elg_d8  offset,       /* offset to global time */
                      void* userdata ) 
{}

void elg_readcb_NUM_EVENTS( elg_ui4 eventc, /* number of event records */
                          void* userdata ) 
{}

void elg_readcb_EVENT_TYPES( elg_ui4 ntypes,    /* number of event types */
                             elg_ui1 typev[],   /* vector of event types */
                             void* userdata ) 
{}

void elg_readcb_EVENT_COUNTS( elg_ui4 ntypes,   /* number of event types */
                              elg_ui4 countv[], /* vector of event counts */
                              void* userdata ) 
{}

EXTERN void elg_readcb_MAP_SECTION( elg_ui4 rank, /* rank number the section relates to */
                                    void* userdata )
{}

EXTERN void elg_readcb_MAP_OFFSET( elg_ui4 rank,   /* rank number the offset relates to */
                                   elg_ui4 offset, /* file offset in mapping file */
                                   void* userdata )
{}

EXTERN void elg_readcb_IDMAP( elg_ui1 type,   /* object type to be mapped */
                              elg_ui1 mode,   /* mapping mode (dense/sparse) */
                              elg_ui4 count,  /* number of entries */
                              elg_ui4 mapv[], /* vector of mappings */
                              void* userdata )
{}

void elg_readcb_LAST_DEF( void* userdata ) 
{}

void elg_readcb_MPI_WIN( elg_ui4 wid,       /* window identifier */
                         elg_ui4 cid,       /* communicator identifier */
                         void* userdata )
{}

#include "elg_readcb_dummies.c.gen"

/*
 * Example function reading an EPILOG trace file
 */
void read_epilog_trace(char *file) 
{
  ElgRCB* handle;

  /* Open EPILOG trace stored in `file' */
  handle = elg_read_open(file);
  if (handle == NULL) return;

  /* Process every event record of the trace */
  /* Calls read callback functions above     */
  while ( elg_read_next(handle, NULL) );

  /* Close trace file and cleanup */
  elg_read_close(handle);
}
