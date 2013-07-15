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

#include <unistd.h>
#include <stdlib.h>
#include "elg_readcb.h"
#include "epk_archive.h"

#include <stdio.h>

static int printrechdr = 1;
static int printfilehdr = 0;

void elg_readcb_ALL( elg_ui1 type, 
		     elg_ui1 length,
                     void* userdata )
{
  if (printrechdr) printf("(%3u,%3u): ", type, length); 
}

void elg_readcb_STRING( elg_ui4 strid,
		      elg_ui1 cntc,
		      const char* str,
                      void* userdata ) 
{
  printf("%-20s", "<STRING>");    
  printf("  strid  : %3u,", strid);
  printf("  cntc   : %3u,", cntc);
  printf("  str    : %.250s ", str); 
  printf("\n");
}

void elg_readcb_STRING_CNT( const char* str, void* userdata ) 
{
  printf("%-20s", "<STRING_CNT>");
  printf("  str    : %.255s ", str);   
  printf("\n");
}

void elg_readcb_MACHINE( elg_ui4 mid,
		       elg_ui4 nodec,
		       elg_ui4 mnid,
                       void* userdata ) 
{
  printf("%-20s", "<MACHINE>");
  printf("  mid    : %3u,", mid);
  printf("  nodec  : %3u,", nodec);
  printf("  mnid   : %3u", mnid);
  printf("\n");
}

void elg_readcb_NODE( elg_ui4 nid,
		    elg_ui4 mid,
		    elg_ui4 cpuc,
		    elg_ui4 nnid, 
		    elg_d8  cr,
                    void* userdata ) 
{
  printf("%-20s", "<NODE>");
  printf("  nid    : %3u,", nid);
  printf("  mid    : %3u,", mid);
  printf("  cpuc   : %3u,", cpuc);
  printf("  nnid   : %3u,", nnid);
  printf("  cr     : %10.6f", cr);
  printf("\n");
}

void elg_readcb_PROCESS( elg_ui4 pid,
		       elg_ui4 pnid,
                       void* userdata ) 
{
  printf("%-20s", "<PROCESS>");
  printf("  pid    : %3u,", pid);
  printf("  pnid   : %3u", pnid);
  printf("\n");
}

void elg_readcb_THREAD( elg_ui4 tid,
		      elg_ui4 pid,
		      elg_ui4 tnid,
                      void* userdata ) 
{
  printf("%-20s", "<THREAD>");
  printf("  tid    : %3u,", tid);
  printf("  pid    : %3u,", pid);
  printf("  tnid   : %3u", tnid);
  printf("\n");
}

void elg_readcb_LOCATION( elg_ui4 lid,
			elg_ui4 mid,
			elg_ui4 nid,
			elg_ui4 pid,
			elg_ui4 tid,
                        void* userdata ) 
{
  printf("%-20s", "<LOCATION>");
  printf("  lid    : %3u,", lid);
  printf("  mid    : %3u,", mid);
  printf("  nid    : %3u,", nid);
  printf("  pid    : %3u,", pid);
  printf("  tid    : %3u", tid);
  printf("\n");
}

void elg_readcb_FILE( elg_ui4 fid,
		    elg_ui4 fnid,
                    void* userdata ) 
{
  printf("%-20s", "<FILE>");
  printf("  fid    : %3u,", fid);
  printf("  fnid   : %3u", fnid);
  printf("\n");  
}

void elg_readcb_REGION( elg_ui4 rid,
		      elg_ui4 rnid,
		      elg_ui4 fid,
		      elg_ui4 begln,
		      elg_ui4 endln,
		      elg_ui4 rdid,
		      elg_ui1 rtype,
                      void* userdata ) 
{
  printf("%-20s", "<REGION>");
  printf("  rid    : %3u,", rid);
  printf("  rnid   : %3u,", rnid);
  printf("  fid    : %3u,", fid);
  printf("  begln  : %3u,", begln);
  printf("  endln  : %3u,", endln);
  printf("  rdid   : %3u,", rdid);
  printf("  rtype  : %3u", rtype);
  printf("\n");
}

void elg_readcb_CALL_SITE( elg_ui4 csid,
			 elg_ui4 fid,
			 elg_ui4 lno,
			 elg_ui4 erid,
			 elg_ui4 lrid,
                         void* userdata ) 
{
  printf("%-20s", "<CALL_SITE>");
  printf("  csid   : %3u,", csid);
  printf("  fid    : %3u,", fid);
  printf("  lno    : %3u,", lno);
  printf("  erid   : %3u,", erid);
  printf("  lrid   : %3u", lrid);
  printf("\n");
}

void elg_readcb_CALL_PATH( elg_ui4 cpid,
			 elg_ui4 rid,
			 elg_ui4 ppid,
			 elg_ui8 order,
                         void* userdata ) 
{
  printf("%-20s", "<CALL_PATH>");
  printf("  cpid   : %3u,", cpid);
  printf("  rid    : %3u,", rid);
  printf("  ppid   : %3u,", ppid);
  printf("  order  : %llu", (long long unsigned) order); /* might be elg_d8? */
  printf("\n");
}

void elg_readcb_METRIC( elg_ui4 metid,
		      elg_ui4 metnid,
		      elg_ui4 metdid,
		      elg_ui1 metdtype,
		      elg_ui1 metmode,
		      elg_ui1 metiv,
                      void* userdata ) 
{
  printf("%-20s", "<METRIC>");
  printf("  metid  : %3u,", metid);
  printf("  metnid : %3u,", metnid);
  printf("  metdid : %3u,", metdid);
  printf("  metdtype : %3u,", metdtype);
  printf("  metmode : %3u,", metmode);
  printf("  metiv  : %3u", metiv);
  printf("\n");
}

void elg_readcb_MPI_GROUP( elg_ui4 gid,
                           elg_ui1 mode,
                           elg_ui4 grpc,
                           elg_ui4 grpv[],
                           void*   userdata )
{
  elg_ui4 i;

  printf("%-20s", "<MPI_GROUP>");
  printf("  gid    : %3u,", gid);
  printf("  mode   : %3u,", mode);
  printf("  grpc   : %3u,", grpc);

  printf("  group  : (");
  for (i = 0; i < grpc; i++)
  {
    printf("%u", grpv[i]);
    if (i + 1 < grpc)
      printf(",");
  }
  printf(")");
  printf("\n");
}


void elg_readcb_MPI_COMM_DIST ( elg_ui4 cid,    /* communicator id unique to root */
                                elg_ui4 root,   /* global rank of root process */
                                elg_ui4 lcid,   /* local communicator id on process */
                                elg_ui4 lrank,  /* local rank of process */
                                elg_ui4 size,   /* size of communicator */
                                void* userdata )
{
  printf("%-20s", "<MPI_COMM_DIST>");
  printf("  cid    : %3u,", cid);
  printf("  root   : %3u,", root);
  printf("  lcid   : %3u,", lcid);
  printf("  lrank  : %3u,", lrank);
  printf("  size   : %3u", size);
  printf("\n");
}


void elg_readcb_MPI_COMM_REF( elg_ui4 cid,
                              elg_ui4 gid,
                              void*   userdata )
{
  printf("%-20s", "<MPI_COMM_REF>");
  printf("  cid    : %3u,", cid);
  printf("  gid    : %3u", gid);
  printf("\n");
}

void elg_readcb_MPI_COMM( elg_ui4 cid,
                        elg_ui1 mode,
			elg_ui4 grpc,
			elg_ui1 grpv[],
                        void* userdata ) 
{
  elg_ui4 i, j;

  printf("%-20s", "<MPI_COMM>");
  printf("  cid    : %3u,", cid);	  
  printf("  mode   : %3u,", mode);	  
  printf("  grpc   : %3u,", grpc);	  

  printf("  group  : ( ");
  for (i = 0; i < grpc; i++)
    for (j = 0; j < 8; j++)
      printf("%u", (grpv[i] & (1 << (j % 8))  ? 1 : 0 ));
  
  printf(" )");
  printf("\n");
}

void elg_readcb_CART_TOPOLOGY( elg_ui4 topid,
			       elg_ui4 tnid,
			       elg_ui4 cid,
			       elg_ui1 ndims,
			       elg_ui4 dimv[], 
			       elg_ui1 periodv[],
			       elg_ui4 dimids[],
			       void* userdata ) 
{
  int i;

  printf("%-20s", "<CART_TOPOLOGY>");
  printf("  topid  : %3u,", topid);	  
  printf("  cid    : %3u,", cid);
  printf("  ndims  : %3u,", ndims);	  
  printf("  dimv   : ( "); 
  for ( i = 0 ; i < ndims-1 ; i++ ) 
    printf("%u, ", dimv[i]);
  printf("%u", dimv[ndims-1]);
  printf(" ),");

  printf(" periodv : ( ");
  for ( i = 0 ; i < ndims-1 ; i++ ) 
    printf("%u, ", periodv[i]);
  printf("%u", periodv[ndims-1]);
  printf(" )");
  printf("  tnid   : %3u", tnid);
  printf(" dimids : ( ");
  for ( i = 0 ; i < ndims-1 ; i++ ) 
    printf("%u, ", dimids[i]);
  printf("%u", dimids[ndims-1]);
  printf(" )");

  printf("\n");
}

void elg_readcb_CART_COORDS( elg_ui4 topid,
			     elg_ui4 lid,
			     elg_ui1 ndims,
			     elg_ui4 coordv[],
			     void* userdata)
{
  int i;

  printf("%-20s", "<CART_COORDS>");
  printf("  topid  : %3u,", topid);	  
  printf("  lid    : %3u,", lid);	  
  printf("  ndims  : %3u,", ndims);	 

  printf("  coordv : ( ");
  for ( i = 0 ; i < ndims-1 ; i++ ) 
    printf("%u, ", coordv[i]);
  printf("%u", coordv[ndims-1]);
  printf(" )");

  printf("\n");
}

void elg_readcb_OFFSET( elg_d8  ltime,
		      elg_d8  offset,
                      void* userdata ) 
{
  printf("%-20s", "<OFFSET>");
  printf("  ltime  : %10.6f,", ltime);
  printf("  offset : %10.6f", offset);
  printf("\n");
}	

void elg_readcb_MPI_WIN(elg_ui4 wid, elg_ui4 cid, void* userdata) {
  printf("%-20s", "<MPI_WIN>");
  printf("  wid    : %3u,", wid);
  printf("  cid    : %3u", cid);
  printf("\n");
}

void elg_readcb_NUM_EVENTS( elg_ui4 eventc, void* userdata ) 
{
  printf("%-20s", "<NUM_EVENTS>");
  printf("  eventc : %3u", eventc);
  printf("\n");
}

void elg_readcb_EVENT_TYPES( elg_ui4 ntypes, elg_ui1 typev[], void* userdata ) 
{
  unsigned i;
  printf("%-20s", "<EVENT_TYPES>");
  printf("  ntypes : %3u,", ntypes);

  printf("  typev : ( ");
  for ( i = 0 ; i < ntypes-1 ; i++ ) 
    printf("%u, ", typev[i]);
  printf("%u", typev[ntypes-1]);
  printf(" )");

  printf("\n");
}

void elg_readcb_EVENT_COUNTS( elg_ui4 ntypes, elg_ui4 countv[], void* userdata ) 
{
  unsigned i;
  printf("%-20s", "<EVENT_COUNTS>");
  printf("  ntypes : %3u,", ntypes);

  printf("  countv : ( ");
  for ( i = 0 ; i < ntypes-1 ; i++ ) 
    printf("%u, ", countv[i]);
  printf("%u", countv[ntypes-1]);
  printf(" )");

  printf("\n");
}

void elg_readcb_MAP_SECTION( elg_ui4 rank, void* userdata )
{
  printf("%-20s  rank: %3u\n", "<MAP_SECTION>", rank);
}

void elg_readcb_MAP_OFFSET( elg_ui4 rank, elg_ui4 offset, void* userdata )
{
  printf("%-20s", "<MAP_OFFSET>");
  printf("  rank : %3u,", rank);
  printf("  offset : %3u", offset);
  printf("\n");
}

void elg_readcb_IDMAP( elg_ui1 type,
                       elg_ui1 mode,
                       elg_ui4 count,
                       elg_ui4 mapv[],
                       void*   userdata )
{
  elg_ui4 i;
  printf("%-20s", "<IDMAP>");
  printf("  type : %3u ", type);
  switch (type) {
    case ELG_LOCATION:
      printf("(LOC),");
      break;
    case ELG_REGION:
      printf("(REG),");
      break;
    case ELG_CALL_SITE:
      printf("( CS),");
      break;
    case ELG_MPI_GROUP:
      printf("(GRP),");
      break;
    case ELG_MPI_COMM:
      printf("(COM),");
      break;
    case ELG_MPI_WIN:
      printf("(WIN),");
      break;
    case ELG_CALL_PATH:
      printf("( CP),");
      break;
    default:
      printf("( ? ),");
      break;
  }
  printf("  mode : %3u,", mode);
  printf("  count : %3u,", count);

  printf(" mapv : ( ");
  if (count > 0) {
    if (mode == ELG_MAP_DENSE) {
      for ( i = 0; i < count-1; i++ )
        printf("%u, ", mapv[i]);
      printf("%u", mapv[count-1]);
    } else {   /* ELG_MAP_SPARSE */
      for ( i = 0; i < count-2; i += 2 )
        printf("{%u,%u},", mapv[i], mapv[i+1]);
      printf("{%u,%u}", mapv[count-2], mapv[count-1]);
    }
  }
  printf(" )");

  printf("\n");
}

void elg_readcb_LAST_DEF( void* userdata ) 
{
  printf("%-20s", "<LAST_DEF>");
  printf("\n");
}

void elg_readcb_ATTR_UI1( elg_ui1 type, elg_ui1 val, void* userdata )
{
  printf("%-20s", "<ATTR_UI1>");
  printf("  type   : %3u,", type);
  printf("  value  : %3u", val);
  printf("\n");
}

void elg_readcb_ATTR_UI4( elg_ui1 type, elg_ui4 val, void* userdata )
{
  printf("%-20s", "<ATTR_UI4>");
  printf("  type   : %3u,", type);
  printf("  value  : %3u", val);
  printf("\n");
}

#include "elg_print.c.gen"

int main(int argc, char **argv) 
{
  char*   infile;
  int     ch;
  int     eflag = 0;
  ElgRCB* handle;

  while ((ch = getopt(argc, argv, "hnv?")) != EOF) {
    switch (ch) {
    case 'n': printrechdr = 0;
              break;
    case 'v': printfilehdr = 1;
              break;
    case 'h':
    case '?': eflag = 1;
              break;
    }
  }

  if ((argc-optind) == 1) {
    infile  = epk_get_elgfilename(argv[optind]);
  } else {
    eflag = 1;
  }

  if ( eflag ) {
    fprintf(stderr, "Usage:   %s [-n] [-v] "
                    "(<infile>.elg | <experiment_archive>)\n", argv[0]);
    fprintf(stderr, "Options: -n don't print record headers\n");
    fprintf(stderr, "         -v print file header\n");
    fflush(stderr);
    exit(EXIT_FAILURE);
  }

  handle = elg_read_open(infile);
  if (handle == NULL)
    exit(EXIT_FAILURE);

  if ( printfilehdr )
      printf("EPILOG: version %u byteorder %u\n",
                elg_read_version(handle), elg_read_byteorder(handle));

  while ( elg_read_next(handle, NULL) );

  elg_read_close(handle);
    
  return 0;
}
