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

#include "elg_gen.h"
#include "epk_conf.h"
#include "elg_impl.h"
#include "elg_error.h"
#include "esd_event.h"
#include "epk_metric.h"
#include "elg_pform.h"

#if defined(ELG_COMPRESSED)
#  include "zlib.h"
#endif

#if defined(USE_SIONLIB)
#  include "sion.h"
#endif

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

extern elg_ui4 esd_tracing_rid;

/*
 *-----------------------------------------------------------------------------
 * Macro functions
 *-----------------------------------------------------------------------------
 */

#define ELGGEN_CHECK(gen, type)                                     \
  if (gen != NULL) gen->stats[type]++;                              \
  else elg_error_msg("Abort: Uninitialized trace buffer")

#define ELGGEN_ALLOC(gen, bytes)                                    \
  gen->total_bytes += (bytes);                                      \
  if ((gen->buf->pos - gen->buf->mem) > (gen->buf->size - (bytes))) \
      ElgGen_flush(gen, 0) /* intermediate flush */
 
#define ELGGEN_COPY_NUM(gen, var)           \
  memcpy(gen->buf->pos, &var, sizeof(var)); \
  gen->buf->pos += sizeof(var)       

#define ELGGEN_COPY_STR(gen, str)           \
  strcpy((char*) gen->buf->pos, str);       \
  gen->buf->pos += strlen(str) + 1     

#define ELGGEN_COPY_STRN(gen, str, n)       \
  strncpy((char*) gen->buf->pos, str, n);   \
  gen->buf->pos += n     
                         
#ifdef USE_ELG_ENTER_TRACING
/* reserved buffer space for ELG_ENTER_TRACING record */
#define ELG_BUF_RES (2*sizeof(elg_ui1) + (ELG_METRIC_MAXNUM+1)*sizeof(elg_ui8))
#else
/* reserved buffer space for ELG_ENTER(esd_tracing_rid) record */
#define ELG_BUF_RES (2*sizeof(elg_ui1) + sizeof(elg_ui4) + (ELG_METRIC_MAXNUM+1)*sizeof(elg_ui8))
#endif

/*
 *-----------------------------------------------------------------------------
 * ElgGen
 *-----------------------------------------------------------------------------
 */

/* Data types */

typedef struct 
{
  buffer_t  mem;
  buffer_t  pos;
  size_t    size;
} ElgBuf;

struct ElgGen_struct
{
#if !defined(ELG_COMPRESSED)
  FILE*             file;
#else
  gzFile*           file;
#endif
#if defined(USE_SIONLIB)
  int               sionid;
#endif
  char              name[PATH_MAX];
  ElgBuf*           buf;
  size_t            total_bytes;
  elg_ui4           stats[ELG_ALL_EVENTS];
};

ElgGen* ElgGen_open(const char* path_name, 
		    elg_ui4 tid, size_t buffer_size)
{
  ElgGen* gen;

  elg_ui1 major_vnr  = ELG_MAJOR_VNR;
  elg_ui1 minor_vnr  = ELG_MINOR_VNR;
  elg_ui1 byte_order = ELG_BYTE_ORDER;

  /* allocate ElgGen record */

  gen = calloc(1, sizeof(ElgGen));
  if (gen == NULL) 
    elg_error();
    
  /* define file name */

  if (tid != ELG_NO_ID)
      snprintf(gen->name, PATH_MAX, "%s.%u.%s", path_name, tid, "elg");
  else
      strncpy(gen->name, path_name, PATH_MAX);

  gen->file = NULL; /* open defered until ElgGen_create */

#if defined(USE_SIONLIB)
  gen->sionid=-1; 
#endif

  /* allocate buffer record */

  gen->buf = malloc(sizeof(ElgBuf));  
  if (gen->buf == NULL) 
    elg_error();

  /* allocate buffer */

  gen->buf->mem = malloc(buffer_size);  
  if (gen->buf->mem == NULL) 
    elg_error();
  
  /* initialize buffer */
  
  gen->buf->pos  = gen->buf->mem;
  /* subtraction leaves space for size of ELG_ENTER_TRACING record */
  gen->buf->size = buffer_size - ELG_BUF_RES;  

  /* write file header */

  ELGGEN_ALLOC(gen, strlen(ELG_HEADER) + 1 + 
	       sizeof(major_vnr) + sizeof(minor_vnr) + sizeof(byte_order)); 

  ELGGEN_COPY_STR(gen, ELG_HEADER);
  ELGGEN_COPY_NUM(gen, major_vnr);
  ELGGEN_COPY_NUM(gen, minor_vnr);
  ELGGEN_COPY_NUM(gen, byte_order);
  
  /* return */
  return gen;
}

#ifdef USE_SIONLIB
int ElgGen_set_sionid(ElgGen* gen, int sionid) 
{
  gen->sionid=sionid;
  DPRINTFP((  1024,"ElgGen_set_sionid",-1,"sid=%d \n",gen->sionid));
  return 1;
}

int ElgGen_get_sionid(ElgGen* gen)
{
  DPRINTFP((  1024,"ElgGen_get_sionid",-1,"sid=%d \n",gen->sionid));
  return gen->sionid;
}
#endif

int ElgGen_create(ElgGen* gen)
{
  if (gen->file) {
      elg_cntl_msg("Trace file %s is already created", gen->name);
      return 1;
  }

#ifdef USE_SIONLIB
  /* sionid was set before call by ElgGen_set_sionid */
  if(gen->sionid==-1) {
    /* non-SION version */
#endif

#if !defined(ELG_COMPRESSED)
  gen->file = fopen(gen->name, "w");
#else
  char gzflags[8];
  snprintf(gzflags, 8, "w%s",
           strcmp(epk_get(ELG_COMPRESSION),"0") ? epk_get(ELG_COMPRESSION) : "u");
  gen->file = gzopen(gen->name, gzflags);
#endif

#ifdef USE_SIONLIB
  } else {
    FILE *fp=NULL, *secondfp=NULL;
    
    /* SION version */
    fp=sion_get_fp(gen->sionid);
    
#if !defined(ELG_COMPRESSED)
    DPRINTFP((  1024,"ElgGen_create_sion",-1,"sid=%d open file in %s\n",gen->sionid,gen->name));
    gen->file = fp;
#else
    char gzflags[8];
    snprintf(gzflags, 8, "w%s",
	     strcmp(epk_get(ELG_COMPRESSION),"0") ? epk_get(ELG_COMPRESSION) : "u");
    gen->file = gzdopen(dup(fileno(fp)), gzflags);
    secondfp  = gzgetfileptr(gen->file);
    sion_set_second_fp(gen->sionid,secondfp);
    DPRINTFP((  1024,"ElgGen_create_sion",-1,"sid=%d gz open file %s with flags %s pos=%lld\n",
		gen->sionid,gen->name,gzflags,sion_get_position(gen->sionid)));
#endif
    /* healthy check if file is ok is already done in sion_paropen  */
  }
#endif

  if (gen->file == NULL)
      elg_error_msg("Cannot open trace file %s", gen->name);
  else
      elg_cntl_msg("Opened trace file %s [buffer %d bytes]", 
                    gen->name, gen->buf->size + ELG_BUF_RES);
  return (gen->file != NULL);
}

void ElgGen_flush(ElgGen* gen, int mode)
{
  size_t bytes;

  if (!gen->file) ElgGen_create(gen);

  if (mode==0) elg_warning("Enforced flush to disk of ELG_BUFFER_SIZE=%s event trace data!",
                           epk_get(ELG_BUFFER_SIZE));

  /* write ELG_ENTER_TRACING record in full buffer */
  if (mode!=1) esd_enter_tracing();

  bytes = gen->buf->pos - gen->buf->mem; /* buffer containing ELG_ENTER_TRACING */

#ifdef USE_SIONLIB
  if(gen->sionid>=0) {
    DPRINTFP((  1024,"ElgGen_flush",-1,"pos=%lld bytes=%d\n",sion_get_position(gen->sionid),bytes));
    if(!sion_ensure_free_space(gen->sionid,(long) bytes)) {
      elg_warning("Not enough space in sion file fo  %d bytes to file %s", bytes, gen->name);
      elg_error();
    }
    DPRINTFP((  1024,"ElgGen_flush",-1,"pos=%lld\n",sion_get_position(gen->sionid)));
  }
#endif

  /* flush buffer */
#if !defined(ELG_COMPRESSED)
  if (write(fileno(gen->file), gen->buf->mem, bytes) != bytes)
#else
  if (gzwrite(gen->file, gen->buf->mem, bytes) != bytes)
#endif
    elg_error();
  
  /* reset buffer */
  gen->buf->pos = gen->buf->mem;

  /* write ELG_EXIT_TRACING record in fresh buffer */
  if (mode!=1) esd_exit_tracing();

  elg_warning("Flushed %d bytes to file %s", bytes, gen->name);
}

void ElgGen_close(ElgGen* gen)
{
  ElgGen_flush(gen, 1); /* final flush */

#ifdef USE_SIONLIB
  /* file will be closed by sion_parclose in elg_run.c,
   gzclose close only the fd created with dup() when opening 
   sion file with gzdopen() */
#if defined(ELG_COMPRESSED)
  if (gzclose(gen->file) == EOF)
    elg_error();
  else
    elg_cntl_msg("Closed gz trace file %s", gen->name);

  if(gen->sionid>=0) {
    sion_unset_second_fp(gen->sionid);
  }

#else
  elg_cntl_msg("Delayed Close of trace file %s", gen->name);
#endif


  /* gzclose flushes zlib internal memory buffer to file, sion flush has to be done afterwards */
  if(gen->sionid>=0) sion_flush(gen->sionid);

#else

  /* close file */
#if !defined(ELG_COMPRESSED)
  if (fclose(gen->file) == EOF)
#else
  if (gzclose(gen->file) == EOF)
#endif
    elg_error();
  else
    elg_cntl_msg("Closed trace file %s", gen->name);

#endif

  /* free buffer memory */
  free(gen->buf->mem); 

  /* free buffer record */
  free(gen->buf); 
}  

void ElgGen_delete(ElgGen* gen)
{
  int result;

  if (epk_str2bool(epk_get(EPK_CLEAN))) {
    /* delete the temporary trace file */
    result = remove(gen->name);
    if (result == 0)
      elg_cntl_msg("Removed trace file %s", gen->name);
    else
      elg_error_msg("Cannot remove trace file %s", gen->name);
  } else {
      elg_cntl_msg("*Left* trace file %s", gen->name);
  }
  
  /* free gen record */
  free(gen); 
}

int ElgGen_reset_name(ElgGen* gen, const char* name)
{
  int reset = !gen->file;
  if (reset) strncpy(gen->name, name, PATH_MAX);
  else elg_warning("Couldn't reset active trace file %s", gen->name);
  return reset;
}

char* ElgGen_get_name(ElgGen* gen)
{
  return gen->name;
}

size_t ElgGen_get_bsize(ElgGen* gen)
{
  return gen->buf->size;
}

size_t ElgGen_get_bytes(ElgGen* gen)
{
  return gen->total_bytes;
}

elg_ui4* ElgGen_get_stats(ElgGen* gen)
{
  return gen->stats;
}

/* -- Attributes -- */

void ElgGen_write_ATTR_UI1(ElgGen* gen, elg_ui1 attrtype, elg_ui1 val) {
  elg_ui1 type   = ELG_ATTR_UI1;
  elg_ui1 length =
    sizeof(attrtype) +
    sizeof(val);

  ELGGEN_CHECK(gen, type);
  ELGGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ELGGEN_COPY_NUM(gen, length);
  ELGGEN_COPY_NUM(gen, type);
  ELGGEN_COPY_NUM(gen, attrtype);
  ELGGEN_COPY_NUM(gen, val);
}

void ElgGen_write_ATTR_UI4(ElgGen* gen, elg_ui1 attrtype, elg_ui4 val) {
  elg_ui1 type   = ELG_ATTR_UI4;
  elg_ui1 length =
    sizeof(attrtype) +
    sizeof(val);

  ELGGEN_CHECK(gen, type);
  ELGGEN_ALLOC(gen, sizeof(length) + sizeof(type) + length);

  ELGGEN_COPY_NUM(gen, length);
  ELGGEN_COPY_NUM(gen, type);
  ELGGEN_COPY_NUM(gen, attrtype);
  ELGGEN_COPY_NUM(gen, val);
}

/* -- Writing trace event records -- */

#include "elg_gen.c.gen"
