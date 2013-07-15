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

#include "esd_gen.h"
#include "esd_def.h"
#include "elg_error.h"
#include "platform/elg_pform.h"
#include "esd_run.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
 *-----------------------------------------------------------------------------
 * Macro functions
 *-----------------------------------------------------------------------------
 */

#define ESDGEN_CHECK(gen)                                           \
  if (gen == NULL) elg_error_msg("Abort: Uninitialized defs buffer")

#define ESDGEN_ALLOC(gen, bytes)                                    \
  gen->total_bytes += (bytes);                                      \
  if (gen->disabled) return;                                        \
  if ((gen->buf->pos - gen->buf->mem) > (gen->buf->size - (bytes))) { \
      elg_warning("ESD buffer full - definitions being lost!");     \
      gen->disabled=1; /* deactivate recording */                   \
      return;                                                       \
  }


/*    EsdGen_flush(gen) */
 
#define ESDGEN_COPY_NUM(gen, var)           \
  memcpy(gen->buf->pos, &var, sizeof(var)); \
  gen->buf->pos += sizeof(var)       

#define ESDGEN_COPY_STR(gen, str)           \
  strcpy((char*) gen->buf->pos, str);       \
  gen->buf->pos += strlen(str) + 1     

#define ESDGEN_COPY_STRN(gen, str, n)       \
  strncpy((char*) gen->buf->pos, str, n);   \
  gen->buf->pos += n     
                         
/*
 *-----------------------------------------------------------------------------
 * EsdGen
 *-----------------------------------------------------------------------------
 */

/* Data types (currently copies of ELG equivalents) */

typedef struct 
{
  buffer_t  mem;
  buffer_t  pos;
  size_t    size;
} EsdBuf;

struct EsdGen_struct
{
  FILE*             file;
  char              name[PATH_MAX];
  EsdBuf*           buf;
  size_t            total_bytes;
  int               disabled;
};

EsdGen* EsdGen_open(const char* path_name, 
                    elg_ui4 tid, size_t buffer_size)
{
  EsdGen* gen;

  elg_ui1 major_vnr  = ELG_MAJOR_VNR;
  elg_ui1 minor_vnr  = ELG_MINOR_VNR;
  elg_ui1 byte_order = ELG_BYTE_ORDER;

  size_t header_size = strlen(ELG_HEADER) + 1 + 
	       sizeof(major_vnr) + sizeof(minor_vnr) + sizeof(byte_order); 

  /* allocate EsdGen record */

  gen = calloc(1, sizeof(EsdGen));
  if (gen == NULL) 
    elg_error();
    
  /* define file name */

  if (tid != ELG_NO_ID)
      snprintf(gen->name, PATH_MAX, "%s.%u.%s", path_name, tid, "esd");
  else
      snprintf(gen->name, PATH_MAX, "%s.%s", path_name, "esd");

  gen->file = NULL; /* open defered */

  /* allocate buffer record */

  gen->buf = malloc(sizeof(EsdBuf));  
  if (gen->buf == NULL) 
    elg_error();

  /* allocate buffer */

  if (buffer_size < header_size) buffer_size = header_size;
  gen->buf->mem = malloc(buffer_size);  
  if (gen->buf->mem == NULL) 
    elg_error();
  
  /* initialize buffer */
  
  gen->buf->pos  = gen->buf->mem;
  gen->buf->size = buffer_size;  

  /* write file header */

  gen->total_bytes = header_size;

  ESDGEN_COPY_STR(gen, ELG_HEADER);
  ESDGEN_COPY_NUM(gen, major_vnr);
  ESDGEN_COPY_NUM(gen, minor_vnr);
  ESDGEN_COPY_NUM(gen, byte_order);
  
  /* return */
  return gen;
}

void EsdGen_flush(EsdGen* gen)
{
  size_t bytes = gen->buf->pos - gen->buf->mem;

  if (!gen->file) { /* open file */
      gen->file = fopen(gen->name, "w");
      if (gen->file == NULL)
          elg_error_msg("Cannot open defs file %s", gen->name);
      else
          elg_cntl_msg("Opened defs file %s [buffer %d bytes]", 
                        gen->name, gen->buf->size);
  }

  /* flush buffer */
  if (write(fileno(gen->file), gen->buf->mem, bytes) != bytes)
    elg_error();

#if defined(ESD_DEBUG_DEFINITIONS)  
  /* close file */
  if (fclose(gen->file) == EOF)
      elg_error();
  else
      elg_cntl_msg("Closed defs file %s", gen->name);    
#else
  /* reset buffer */
  gen->buf->pos = gen->buf->mem;
#endif

  elg_cntl_msg("Flushed %d bytes to file %s", bytes, gen->name);
}

void EsdGen_close(EsdGen* gen)
{
  gen->disabled=1; /* deactivate recording */

  EsdGen_flush(gen);

  /* close file */
  if (fclose(gen->file) == EOF)
      elg_error();
  else
      elg_cntl_msg("Closed defs file %s", gen->name);    

  /* free buffer memory */
  free(gen->buf->mem); 

  /* free buffer record */
  free(gen->buf); 
  
  /* free gen record */
  free(gen); 
}

int EsdGen_reset_name(EsdGen* gen, const char* name)
{
  int reset = !gen->file;
  if (reset) strncpy(gen->name, name, PATH_MAX);
  else elg_warning("Couldn't reset active defs file %s", gen->name);
  return reset;
}

char* EsdGen_get_name(EsdGen* gen)
{
  return gen->name;
}

buffer_t EsdGen_get_data(EsdGen* gen)
{
  return gen->buf->mem;
}

size_t EsdGen_get_held(EsdGen* gen)
{
  return gen->buf->pos - gen->buf->mem;
}

size_t EsdGen_get_bytes(EsdGen* gen)
{
  return gen->total_bytes;
}

/* - Definition records - (renamed from ElgGen) */


void EsdGen_write_STRING(EsdGen* gen,
			 elg_ui4 strid,
			 elg_ui1 cntc,
			 const char* str) 
{
  elg_ui1 length;
  elg_ui1 type    = ELG_STRING;
  
  /* Does string exceed record size ? */
  if (!cntc) 
    {
      length = sizeof(strid) + sizeof(cntc) + strlen(str) + 1;
      
      ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);
      
      ESDGEN_COPY_NUM(gen, length);
      ESDGEN_COPY_NUM(gen, type);
      ESDGEN_COPY_NUM(gen, strid);
      ESDGEN_COPY_NUM(gen, cntc);
      ESDGEN_COPY_STR(gen, str);
    }
  else
    {
      length = 255;

      ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + 255);
      
      ESDGEN_COPY_NUM(gen, length);
      ESDGEN_COPY_NUM(gen, type);
      ESDGEN_COPY_NUM(gen, strid);
      ESDGEN_COPY_NUM(gen, cntc);
      ESDGEN_COPY_STRN(gen, str, 255 - (sizeof(strid) + sizeof(cntc)));
    }
}

void EsdGen_write_STRING_CNT(EsdGen* gen,
			     const char* str) 
{
  elg_ui1 length;
  elg_ui1 type      = ELG_STRING_CNT;
  int     stringlen = strlen(str) + 1;

  /* Does string exceed record size ? */
  if (stringlen <= 255) 
    {
      length = stringlen;
      
      ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);
      
      ESDGEN_COPY_NUM(gen, length);
      ESDGEN_COPY_NUM(gen, type);
      ESDGEN_COPY_STR(gen, str);
    }
  else
    {
      length = 255;

      ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + 255);
      
      ESDGEN_COPY_NUM(gen, length);
      ESDGEN_COPY_NUM(gen, type);
      ESDGEN_COPY_STRN(gen, str, 255);
    }  
}

void EsdGen_write_MACHINE(EsdGen* gen,
			  elg_ui4 mid,
			  elg_ui4 nodec,
			  elg_ui4 mnid) 
{
  elg_ui1 length = sizeof(mid) + sizeof(nodec) + sizeof(mnid);
  elg_ui1 type   = ELG_MACHINE;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, mid);
  ESDGEN_COPY_NUM(gen, nodec);
  ESDGEN_COPY_NUM(gen, mnid);
}

void EsdGen_write_NODE(EsdGen* gen,
		       elg_ui4 nid,
		       elg_ui4 mid,
		       elg_ui4 cpuc,
		       elg_ui4 nnid,
		       elg_d8 cr) 
{
  elg_ui1 length = sizeof(nid) + sizeof(mid) + sizeof(cpuc) + sizeof(nnid) + sizeof(cr);
  elg_ui1 type   = ELG_NODE;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, nid);
  ESDGEN_COPY_NUM(gen, mid);
  ESDGEN_COPY_NUM(gen, cpuc);
  ESDGEN_COPY_NUM(gen, nnid);
  ESDGEN_COPY_NUM(gen, cr);
}

void EsdGen_write_PROCESS(EsdGen* gen,
			  elg_ui4 pid,
			  elg_ui4 pnid) 
{
  elg_ui1 length = sizeof(pid) + sizeof(pnid);
  elg_ui1 type   = ELG_PROCESS;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, pid);
  ESDGEN_COPY_NUM(gen, pnid);
}

void EsdGen_write_THREAD(EsdGen* gen,
			 elg_ui4 tid,
			 elg_ui4 pid,
			 elg_ui4 tnid) 
{
  elg_ui1 length = sizeof(tid) + sizeof(pid) + sizeof(tnid);
  elg_ui1 type   = ELG_THREAD;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, tid);
  ESDGEN_COPY_NUM(gen, pid);
  ESDGEN_COPY_NUM(gen, tnid);
}

void EsdGen_write_LOCATION(EsdGen* gen,
			   elg_ui4 lid,
			   elg_ui4 mid,
			   elg_ui4 nid,
			   elg_ui4 pid,
			   elg_ui4 tid) 
{
  elg_ui1 length = 
    sizeof(lid) + 
    sizeof(mid) + 
    sizeof(nid) + 
    sizeof(pid) + 
    sizeof(tid);

  elg_ui1 type   = ELG_LOCATION;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, lid);
  ESDGEN_COPY_NUM(gen, mid);
  ESDGEN_COPY_NUM(gen, nid);
  ESDGEN_COPY_NUM(gen, pid);
  ESDGEN_COPY_NUM(gen, tid);
}

void EsdGen_write_FILE(EsdGen* gen,
		       elg_ui4 fid,
		       elg_ui4 fnid) 
{
  elg_ui1 length = sizeof(fid) + sizeof(fnid);
  elg_ui1 type   = ELG_FILE;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, fid);
  ESDGEN_COPY_NUM(gen, fnid);
}

void EsdGen_write_REGION(EsdGen* gen,
			 elg_ui4 rid,
			 elg_ui4 rnid,
			 elg_ui4 fid,
			 elg_ui4 begln,
			 elg_ui4 endln,
			 elg_ui4 rdid,
			 elg_ui1 rtype) 
{
  elg_ui1 length = 
    sizeof(rid) + 
    sizeof(rnid) + 
    sizeof(fid) + 
    sizeof(begln) + 
    sizeof(endln) + 
    sizeof(rdid) + 
    sizeof(rtype);

  elg_ui1 type   = ELG_REGION;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, rid);
  ESDGEN_COPY_NUM(gen, rnid);
  ESDGEN_COPY_NUM(gen, fid);
  ESDGEN_COPY_NUM(gen, begln);
  ESDGEN_COPY_NUM(gen, endln);
  ESDGEN_COPY_NUM(gen, rdid);
  ESDGEN_COPY_NUM(gen, rtype);
}

void EsdGen_write_CALL_SITE( EsdGen* gen,
			     elg_ui4 csid,
			     elg_ui4 fid,
			     elg_ui4 lno,
			     elg_ui4 erid,
			     elg_ui4 lrid )
{
  elg_ui1 length = 
    sizeof(csid) + 
    sizeof(fid) + 
    sizeof(lno) + 
    sizeof(erid) + 
    sizeof(lrid); 

  elg_ui1 type   = ELG_CALL_SITE;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, csid);
  ESDGEN_COPY_NUM(gen, fid);
  ESDGEN_COPY_NUM(gen, lno);
  ESDGEN_COPY_NUM(gen, erid);
  ESDGEN_COPY_NUM(gen, lrid);
}

void EsdGen_write_CALL_PATH( EsdGen* gen,
			     elg_ui4 cpid,
			     elg_ui4 rid,
			     elg_ui4 ppid,
			     elg_ui8 order )
{
  elg_ui1 length = 
    sizeof(cpid) + 
    sizeof(rid) + 
    sizeof(ppid) + 
    sizeof(order); 

  elg_ui1 type   = ELG_CALL_PATH;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, cpid);
  ESDGEN_COPY_NUM(gen, rid);
  ESDGEN_COPY_NUM(gen, ppid);
  ESDGEN_COPY_NUM(gen, order);
}

void EsdGen_write_METRIC( EsdGen* gen,
			  elg_ui4 metid,
			  elg_ui4 metnid,
			  elg_ui4 metdid,
			  elg_ui1 metdtype,
			  elg_ui1 metmode,
			  elg_ui1 metiv )
{
  elg_ui1 length = 
    sizeof(metid) + 
    sizeof(metnid) + 
    sizeof(metdid) + 
    sizeof(metdtype) + 
    sizeof(metmode) + 
    sizeof(metiv); 

  elg_ui1 type   = ELG_METRIC;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, metid);
  ESDGEN_COPY_NUM(gen, metnid);
  ESDGEN_COPY_NUM(gen, metdid);
  ESDGEN_COPY_NUM(gen, metdtype);
  ESDGEN_COPY_NUM(gen, metmode);
  ESDGEN_COPY_NUM(gen, metiv);
}

void EsdGen_write_MPI_COMM(EsdGen* gen,
			   elg_ui4 cid,
			   elg_ui1 mode,
			   elg_ui4 grpc,
			   elg_ui1 grpv[]) 
{
  elg_ui4 i;
  elg_ui4 pos         = 0;
  elg_ui4 maxLength   = 255 - sizeof(cid) - sizeof(mode) - sizeof(grpc);
  elg_ui4 blockLength = (grpc > maxLength) ? maxLength : grpc;
  elg_ui1 length      = sizeof(cid) + sizeof(mode) + sizeof(grpc)
                        + blockLength * sizeof(elg_ui1);
  elg_ui1 type        = ELG_MPI_COMM;

  /* Write communicator record */
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, cid); 
  /* this is added here and modified in elg_merge.cpp */
  ESDGEN_COPY_NUM(gen, mode);
  ESDGEN_COPY_NUM(gen, grpc);

  for (i = 0; i < blockLength; i++)
    {
      ESDGEN_COPY_NUM(gen, grpv[pos]);
      pos++;
    }

  /* Eventually write continuation records */
  type = ELG_MPI_COMM_CNT;
  while (pos < grpc)
    {
      blockLength = grpc - pos;
      if (blockLength > 255)
        blockLength = 255;
      length = blockLength * sizeof(elg_ui1);

      ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

      ESDGEN_COPY_NUM(gen, length);
      ESDGEN_COPY_NUM(gen, type);

      for (i = 0; i < blockLength; i++)
        {
          ESDGEN_COPY_NUM(gen, grpv[pos]);
          pos++;
        }
    }
}


void EsdGen_write_MPI_COMM_DIST ( EsdGen* gen,
                                  elg_ui4 cid,
                                  elg_ui4 root,
                                  elg_ui4 lcid,
                                  elg_ui4 lrank,
                                  elg_ui4 size)
{
  elg_ui1 length = 
    sizeof(cid) + 
    sizeof(root) + 
    sizeof(lcid) + 
    sizeof(lrank) +
    sizeof(size);

  elg_ui1 type   = ELG_MPI_COMM_DIST;
  
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, cid);
  ESDGEN_COPY_NUM(gen, root);
  ESDGEN_COPY_NUM(gen, lcid);
  ESDGEN_COPY_NUM(gen, lrank);
  ESDGEN_COPY_NUM(gen, size);
}


void EsdGen_write_MPI_GROUP(EsdGen* gen,
                            elg_ui4 gid,
                            elg_ui1 mode,
                            elg_ui4 grpc,
                            elg_ui4 grpv[])
{
  elg_ui4 i;
  elg_ui4 pos         = 0;
  elg_ui4 maxLength   = (255 - sizeof(gid) - sizeof(mode) - sizeof(grpc)) / sizeof(elg_ui4);
  elg_ui4 blockLength = (grpc > maxLength) ? maxLength : grpc;
  elg_ui1 length      = sizeof(gid) + sizeof(mode) + sizeof(grpc)
                        + ((mode & ELG_GROUP_WORLD)
                               ? 0
                               : blockLength * sizeof(elg_ui4));
  elg_ui1 type        = ELG_MPI_GROUP;

  /* Write communicator record */
  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, gid); 
  ESDGEN_COPY_NUM(gen, mode);
  ESDGEN_COPY_NUM(gen, grpc);

  if (mode & ELG_GROUP_WORLD)
    return;

  for (i = 0; i < blockLength; i++)
    {
      ESDGEN_COPY_NUM(gen, grpv[pos]);
      pos++;
    }

  /* Eventually write continuation records */
  maxLength = 255 / sizeof(elg_ui4);
  type = ELG_MPI_GROUP_CNT;
  while (pos < grpc)
    {
      blockLength = grpc - pos;
      if (blockLength > maxLength)
        blockLength = maxLength;
      length = blockLength * sizeof(elg_ui4);

      ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

      ESDGEN_COPY_NUM(gen, length);
      ESDGEN_COPY_NUM(gen, type);

      for (i = 0; i < blockLength; i++)
        {
          ESDGEN_COPY_NUM(gen, grpv[pos]);
          pos++;
        }
    }
}


void EsdGen_write_CART_TOPOLOGY ( EsdGen* gen,
				  elg_ui4 cid,
				  elg_ui4 topo_name_id,
				  elg_ui4* dim_names_ids,
				  EPIK_TOPOL * top)
{
  int i;
  elg_ui4 dim_names_len=0;

  if(!top) exit(-1); // Why I was called with no topology is beyond me.


    dim_names_len=top->num_dims*sizeof(elg_ui4); // Each string id is elg_ui4  

    elg_ui1 length = sizeof(top->topid) +
                   sizeof(topo_name_id) +
                   sizeof(cid) + 
                   sizeof(top->num_dims) +
                   top->num_dims * sizeof(elg_ui4) + 
                   top->num_dims * sizeof(elg_ui1)+dim_names_len; // dimension sizes and periods
  
  elg_ui1 type   = ELG_CART_TOPOLOGY;


  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, top->topid);
  ESDGEN_COPY_NUM(gen, cid);
  ESDGEN_COPY_NUM(gen, top->num_dims);

    for ( i = 0 ; i < top->num_dims ; i++ )
    {
      ESDGEN_COPY_NUM(gen, top->dim_sizes[i]);
    }

  for ( i = 0 ; i < top->num_dims ; i++ )
    {
      ESDGEN_COPY_NUM(gen, top->periods[i]);
    }

  ESDGEN_COPY_NUM(gen, topo_name_id); // This is either the id or ELG_NO_ID according to epilog file format
  
  if(top->dim_names!=NULL) {
    for ( i = 0 ; i < top->num_dims ; i++ )
      {	
	ESDGEN_COPY_NUM(gen, dim_names_ids[i]);	
      }
  } else {
    elg_ui4 noid=ELG_NO_ID;
    for ( i = 0 ; i < top->num_dims ; i++ )
      {	
    ESDGEN_COPY_NUM(gen,noid);
      }
  }



  
}

void EsdGen_write_CART_COORDS   ( EsdGen* gen,
				  elg_ui4 lid,
				  EPIK_TOPOL * top)
{
  int i;
  elg_ui1 length = sizeof(top->topid) +
                   sizeof(lid) +
                   sizeof(top->num_dims) + 
                   top->num_dims * sizeof(elg_ui4);

  elg_ui1 type = ELG_CART_COORDS;

  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, top->topid);
  ESDGEN_COPY_NUM(gen, lid);
  ESDGEN_COPY_NUM(gen, top->num_dims);

  for ( i = 0 ; i < top->num_dims ; i++ ) 
    {
      assert(top->coords[i]<top->dim_sizes[i]); // If coordinate cannot fit the dimension, cube library will segfault
      ESDGEN_COPY_NUM(gen, top->coords[i]);     
    }  
}


void EsdGen_write_MPI_WIN( EsdGen* gen,
                           elg_ui4 wid,
                           elg_ui4 cid )
{
  elg_ui1 type   = ELG_MPI_WIN;
  elg_ui1 length = 
    sizeof(wid)   + 
    sizeof(cid); 

  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, wid);
  ESDGEN_COPY_NUM(gen, cid);
}


void EsdGen_write_OFFSET( EsdGen* gen,
			  elg_d8  ltime,
			  elg_d8  offset) 
{
  elg_ui1 length = sizeof(ltime) + sizeof(offset);
  elg_ui1 type   = ELG_OFFSET;

  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, ltime);
  ESDGEN_COPY_NUM(gen, offset);
}

void EsdGen_write_EVENT_TYPES (EsdGen* gen, elg_ui4 ntypes, elg_ui1 typev[])
{
  unsigned i;
  elg_ui1 type   = ELG_EVENT_TYPES;
  elg_ui1 length = sizeof(ntypes) + (ntypes * sizeof(elg_ui1));

  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, ntypes);
  for ( i = 0 ; i < ntypes ; i++ ) 
    {
      ESDGEN_COPY_NUM(gen, typev[i]);
    }  
}

void EsdGen_write_EVENT_COUNTS (EsdGen* gen, elg_ui4 ntypes, elg_ui4 countv[])
{
  unsigned pos = 0;
  elg_ui1 type = ELG_EVENT_COUNTS;
  elg_ui4 maxLength = (ELG_MAX_EVENT - sizeof(ntypes)) / sizeof(elg_ui4);

  while (pos < ntypes) {
      unsigned i;
      elg_ui4 blkLength = ((ntypes-pos) > maxLength) ? maxLength : (ntypes-pos);
      elg_ui1 length = sizeof(ntypes) + (blkLength * sizeof(elg_ui4));
      
      ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

      ESDGEN_COPY_NUM(gen, length);
      ESDGEN_COPY_NUM(gen, type);
      ESDGEN_COPY_NUM(gen, blkLength);
      for (i = 0; i < blkLength; i++, pos++) {
          ESDGEN_COPY_NUM(gen, countv[pos]);
      }  
  }  
}

#if 0
void EsdGen_write_NUM_EVENTS( EsdGen* gen,
			      elg_ui4 eventc )
{
  elg_ui1 length = sizeof(eventc);
  elg_ui1 type   = ELG_NUM_EVENTS;

  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
  ESDGEN_COPY_NUM(gen, eventc);
}

void EsdGen_write_LAST_DEF(EsdGen* gen) 
{
  elg_ui1 length = 0;
  elg_ui1 type   = ELG_LAST_DEF;

  ESDGEN_ALLOC(gen, sizeof(length) + sizeof(type));

  ESDGEN_COPY_NUM(gen, length);
  ESDGEN_COPY_NUM(gen, type);
}
#endif
