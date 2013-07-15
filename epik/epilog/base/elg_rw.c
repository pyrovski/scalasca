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

#include "elg_rw.h"
#include "elg_impl.h"
#include "elg_error.h"
#include "epk_archive.h"

#if defined(ELG_COMPRESSED)
#  include "zlib.h"
#endif

#ifdef USE_SIONLIB
#  include "sion.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>


#define ELGIN_MODE_FILE      0
#define ELGIN_MODE_BUFFER    1

#define ELGOUT_MODE_FILE     0
#define ELGOUT_MODE_BUFFER   1


/*
 *-----------------------------------------------------------------------------
 * Helper functions
 *-----------------------------------------------------------------------------
 */

static void elg_swap(void* buffer, size_t length)
{
  /* length must be an even number */

  char   tmp;
  size_t i;

  if (length > 1)
    for (i = 0; i < length / 2; i++)
      {
	tmp = ((char*) buffer)[i];
	((char*) buffer)[i] = ((char*) buffer)[length - 1 - i];
	((char*) buffer)[length - 1 - i] = tmp;
      }  
}

/*
 *-----------------------------------------------------------------------------
 * Macro functions
 *-----------------------------------------------------------------------------
 */

#define ELGREC_GET_NUM(rec, var)                                                     \
  memcpy(&var, rec->pos, sizeof(var));                                               \
  rec->pos += sizeof(var);                                                           \
  if ((rec->byte_order == ELG_LITTLE_ENDIAN && ELG_BYTE_ORDER == ELG_BIG_ENDIAN) ||  \
      (rec->byte_order == ELG_BIG_ENDIAN && ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN))    \
      elg_swap(&var, sizeof(var))

#define ELGREC_WRITE_NUM(rec, var, offset)                                           \
  if ((rec->byte_order == ELG_LITTLE_ENDIAN && ELG_BYTE_ORDER == ELG_BIG_ENDIAN) ||  \
      (rec->byte_order == ELG_BIG_ENDIAN && ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN))    \
      elg_swap(&var, sizeof(var));                                                   \
  memcpy(rec->body + offset, &var, sizeof(var))  

#define ELGREC_WRITE_VAR(rec, var)                                                   \
  if ((rec->byte_order == ELG_LITTLE_ENDIAN && ELG_BYTE_ORDER == ELG_BIG_ENDIAN) ||  \
      (rec->byte_order == ELG_BIG_ENDIAN && ELG_BYTE_ORDER == ELG_LITTLE_ENDIAN))    \
      elg_swap(&var, sizeof(var));                                                   \
  memcpy(rec->pos, &var, sizeof(var));                                               \
  rec->pos+=sizeof(var);

/*
 *-----------------------------------------------------------------------------
 * ElgRec
 *-----------------------------------------------------------------------------
 */

/* Data type */
struct ElgRec_struct
{
  elg_ui1   type;
  elg_ui1   length;
  elg_ui1   byte_order;
  buffer_t  pos;
  buffer_t  body;
};


ElgRec* ElgRec_create_empty(elg_ui1 length,
			    elg_ui1 type)
{
  ElgRec* rec;
  
  rec = malloc(sizeof(ElgRec));
  if (rec == NULL)
    elg_error();
 
  rec->length = length;
  rec->type   = type;

  if (rec->length)
    {
      rec->body = malloc(rec->length);
      if (rec->body == NULL)
	elg_error();
    }
  else
    {
      rec->body = NULL;
    }
  
  rec->byte_order = ELG_BYTE_ORDER;
  rec->pos        = rec->body;

  return rec;
}

ElgRec* ElgRec_copy( ElgRec* rec )
{
  ElgRec* newrec;
  
  newrec = (ElgRec*) malloc(sizeof(ElgRec));
  if (newrec == NULL)
    elg_error();
 
  newrec->length = rec->length;
  newrec->type   = rec->type;

  if (newrec->length)
    {
      newrec->body = (buffer_t) malloc(newrec->length);
      if (newrec->body == NULL)
	elg_error();
      memcpy( newrec->body, rec->body, rec->length );
    }
  else
    {
      newrec->body = NULL;
    }
  
  newrec->byte_order = ELG_BYTE_ORDER;
  newrec->pos        = newrec->body;

  return newrec;
}

void ElgRec_reset_type(ElgRec* rec, elg_ui1 type)
{
  rec->type   = type;
  rec->pos    = rec->body;
  rec->length = 255;
}

ElgRec* ElgRec_create_NODE(elg_ui4 nid,
			   elg_ui4 mid,
			   elg_ui4 cpuc,
			   elg_ui4 nnid, 
			   elg_d8  cr)
{
  ElgRec* rec;

  rec = ElgRec_create_empty(sizeof(nid) + 
			    sizeof(mid) + 
			    sizeof(cpuc) + 
			    sizeof(nnid) + 
			    sizeof(cr), 
			    ELG_NODE);
  ELGREC_WRITE_VAR(rec, nid);  
  ELGREC_WRITE_VAR(rec, mid);
  ELGREC_WRITE_VAR(rec, cpuc);
  ELGREC_WRITE_VAR(rec, nnid);
  ELGREC_WRITE_VAR(rec, cr);
  return rec;
}

ElgRec* ElgRec_create_NUM_EVENTS(elg_ui4 eventc)
{
  ElgRec* rec;

  rec = ElgRec_create_empty(sizeof(eventc), ELG_NUM_EVENTS);
  ELGREC_WRITE_VAR(rec, eventc);
  return rec;
}

ElgRec* ElgRec_create_LAST_DEF()
{
  ElgRec* rec;

  rec = ElgRec_create_empty(0, ELG_LAST_DEF);
  return rec;
}

void ElgRec_free(ElgRec* rec)
{
  free(rec->body);
  free(rec);
}

/* Reading record header */

elg_ui1 ElgRec_get_type(ElgRec* rec) 
{
  return rec->type;
}

/* Writing record header */
void ElgRec_set_type(ElgRec* rec, elg_ui1 type )
{
  rec->type = type;
}  

/* Reading record byteorder */
elg_ui1 ElgRec_get_byteorder(ElgRec* rec) 
{
  return rec->byte_order;
}

/* Set record byteorder */
void ElgRec_set_byteorder(ElgRec* rec, elg_ui1 byteorder)
{
  rec->byte_order = byteorder;
}


int ElgRec_is_event(ElgRec* rec)
{
  elg_ui1 type = ElgRec_get_type(rec);
  
  return (type >= 100);
} 

int ElgRec_is_attribute(ElgRec* rec)
{
  elg_ui1 type = ElgRec_get_type(rec);
  
  return (type >= ELG_FIRST_ATTR && type <= ELG_LAST_ATTR);
}

elg_ui1 ElgRec_get_length(ElgRec* rec) 
{
  return rec->length;
}

elg_d8 ElgRec_read_time(ElgRec* rec)
{
    if (!ElgRec_is_event(rec))
        return ELG_NO_TIME;

    /* seek time field and read it */
    ElgRec_seek(rec, sizeof(elg_ui4));
    return ElgRec_read_d8(rec);
}

elg_ui4 ElgRec_read_location(ElgRec* rec)
{
    if (!ElgRec_is_event(rec))
        return ELG_NO_ID;

    ElgRec_seek(rec, 0);
    return ElgRec_read_ui4(rec);
}

/* Reading record data */

elg_ui1 ElgRec_read_ui1(ElgRec* rec) 
{
  elg_ui1 ui1;

  ELGREC_GET_NUM(rec, ui1);  
  return ui1;
}

elg_ui4 ElgRec_read_ui4(ElgRec* rec) 
{
  elg_ui4 ui4;

  ELGREC_GET_NUM(rec, ui4);  
  return ui4;
}

elg_ui8 ElgRec_read_ui8(ElgRec* rec) 
{
  elg_ui8 ui8;

  ELGREC_GET_NUM(rec, ui8);  
  return ui8;
}

elg_d8 ElgRec_read_d8(ElgRec* rec)
{
  elg_d8 d8;

  ELGREC_GET_NUM(rec, d8);  
  return d8;
}

char* ElgRec_read_string(ElgRec* rec)
{
  return (char*) rec->pos;                   
}

int ElgRec_seek(ElgRec* rec, size_t offset)
{
  if (offset >= rec->length)
    return 0;
    
  rec->pos = rec->body + offset;
  return offset;
}

/* Writing record data -*/

ElgRec* ElgRec_write_ui1(ElgRec* rec,
			 elg_ui1 ui1,
			 size_t offset)
{
  if (offset > rec->length)                                                          
      elg_error_msg("Write beyond record boundaries.");                              
  ELGREC_WRITE_NUM(rec, ui1, offset);
  return rec;
} 

ElgRec* ElgRec_write_ui4(ElgRec* rec,
			 elg_ui4 ui4,
			 size_t offset)
{
  if (offset > rec->length)                                                          
      elg_error_msg("Write beyond record boundaries.");                              
  ELGREC_WRITE_NUM(rec, ui4, offset);
  return rec;
} 

ElgRec* ElgRec_write_ui8(ElgRec* rec,
			 elg_ui8 ui8,
			 size_t offset)
{
  if (offset > rec->length)                                                          
      elg_error_msg("Write beyond record boundaries.");                              
  ELGREC_WRITE_NUM(rec, ui8, offset);
  return rec;
} 

ElgRec* ElgRec_write_d8(ElgRec* rec,
			elg_d8 d8,
			size_t offset)
{
  if (offset > rec->length)                                                          
      elg_error_msg("Write beyond record boundaries.");                              
  ELGREC_WRITE_NUM(rec, d8, offset);
  return rec;
}

int ElgRec_write_time(ElgRec* rec, elg_d8 time)
{
    if (!ElgRec_is_event(rec))
        return ELG_FALSE;

    /* write time */
    ElgRec_write_d8(rec, time, sizeof(elg_ui4));
    return ELG_TRUE;
}

/*
 *-----------------------------------------------------------------------------
 * ElgIn
 *-----------------------------------------------------------------------------
 */

/* Data type */

struct ElgIn_struct  
{
  int            mode;
#if !defined(ELG_COMPRESSED)
  FILE*          file;
#else
  gzFile         file;
#endif
  const elg_ui1* buffer;
  int            sionid;
  long           bytes_avail_in_block;
  size_t         size;
  size_t         offset;
  char           name[PATH_MAX];
  elg_ui1        minor_vnr; 
  elg_ui1        major_vnr; 
  elg_ui1        byte_order; 
};



/* SION: check if gzread can read next bytes in sionfile */

static int check_sion_next_read(ElgIn* in, int bytes) {
#ifndef USE_SIONLIB
  return(1);
#else 
  int bytes_in_gzbuffer=-1;

  if(in->sionid>=0) {  	/* is only set, if file is a sion file, e.g. not for defs */

#ifndef ELG_COMPRESSED
    if(in->bytes_avail_in_block<0) {
      DPRINTFP((  512,"ELG: check_sion_next_read",-1,"call sion_bytes_avail_in_block bytes=%d\n",bytes));
      in->bytes_avail_in_block=sion_bytes_avail_in_block(in->sionid); 
    }

    if(bytes > in->bytes_avail_in_block) {
      if(in->bytes_avail_in_block==0) {
	DPRINTFP((  512,"ELG: check_sion_next_read",-1,"call sion_feof bytes=%d\n",bytes));
	if(sion_feof(in->sionid)) {
	  DPRINTFP((  512,"ELG: check_sion_next_read",-1,"eof reached\n"));
	  return(0);
	}
	in->bytes_avail_in_block=sion_bytes_avail_in_block(in->sionid); 
      } else {
	  DPRINTFP((  512,"ELG: check_sion_next_read",-1,"something wrong bytes > in->bytes_avail_in_block %ld > %ld\n",bytes,in->bytes_avail_in_block));
	  return(0);
      }
    }
    in->bytes_avail_in_block-=bytes;
#else
    if(in->bytes_avail_in_block<0) {
      in->bytes_avail_in_block=sion_bytes_avail_in_block(in->sionid); 
      DPRINTFP((  512,"ELG: check_sion_next_read",-1,"call sion_bytes_avail_in_block bytes_in_gzbuffer=%d bytes=%d bytes_avail_in_block=%d result\n",
		  bytes_in_gzbuffer,bytes,in->bytes_avail_in_block));
    }
    if(in->bytes_avail_in_block<=0) {
      bytes_in_gzbuffer=gzgetbufferavailbytes(in->file); 
      
      if(bytes_in_gzbuffer<bytes) { 
	DPRINTFP((  512,"ELG: check_sion_next_read",-1,"call sion_feof bytes_in_gzbuffer=%d bytes=%d\n",bytes_in_gzbuffer,bytes));
	if(sion_feof(in->sionid)) {
	  if(bytes_in_gzbuffer<=0) {
	    DPRINTFP((  512,"ELG: check_sion_next_read",-1,"eof reached\n"));
	    return(0);
	  }
	}
	in->bytes_avail_in_block=sion_bytes_avail_in_block(in->sionid); 
	DPRINTFP((  512,"ELG: check_sion_next_read",-1,"call sion_bytes_avail_in_block2 bytes_in_gzbuffer=%d bytes=%d bytes_avail_in_block=%d result\n",
		    bytes_in_gzbuffer,bytes,in->bytes_avail_in_block));
      }
    }
#endif

  }
  return(1);
#endif
}

/* Open and close trace file */

ElgIn* ElgIn_open(const char* path_name)
{
  ElgIn*   in;
  char     label[7];
  struct stat sbuf;
  int status; 

#ifdef USE_SIONLIB
  int   open_with_sion=0; 
  int   opened_with_sion=0; 
  int   sion_id=-1; 
  int   sionrank=-1;
  char *sionfname=NULL;
#endif

#ifdef USE_SIONLIB
  /* check if file is in a SION file */
 { 
   int rc;
   char *cptr=NULL;

   if ( (cptr=strstr(path_name,"SID="))!=NULL) {
     rc=sscanf(cptr,"SID=%d:",&sion_id);
     opened_with_sion=1;
     open_with_sion=1;
     DPRINTFP((  512,"ELG: ElgIn_open",-1,"path_name= %s  sion_id=%d opened_with_sion=%d\n",path_name,sion_id,opened_with_sion));
   } else {
     
     if ( (cptr=strstr(path_name,"ELG/"))!=NULL) {
       rc=sscanf(cptr,"ELG/%d",&sionrank);
       sionfname=epk_archive_filename(EPK_TYPE_SION,epk_archive_directory(EPK_TYPE_ELG));
       open_with_sion=_sion_stat_file(sionfname);
       DPRINTFP((  512,"ELG: ElgIn_open",-1,"path_name= %s  sionrank=%d sionfname=%s open_with_sion=%d\n",path_name,sionrank,sionfname,open_with_sion));
     }
   }
 }
#endif

#ifdef USE_SIONLIB
 if(!open_with_sion) {
#endif
  /* check file status */
  status=stat(path_name, &sbuf);
  if (status) {
    elg_cntl_msg("stat(%s): %s", path_name, strerror(errno));
    return NULL;
  }
  if (sbuf.st_blocks == 0) { /* number of 512-byte blocks allocated */
    elg_warning("%u blocks allocated for %u bytes of %s",
        sbuf.st_blocks, sbuf.st_size, path_name);
  }

#ifdef USE_SIONLIB
 }
#endif

  /* allocate ElgIn record */
  in = (ElgIn*)malloc(sizeof(ElgIn));
  if (in == NULL)
    elg_error();


#ifdef USE_SIONLIB
 /* open SION file */
 if(open_with_sion) {
   FILE *fp, *secondfp=NULL;
   sion_int32 fsblocksize;
   sion_int64 localsize;
   int secondfd;
   if(!opened_with_sion) {
     in->sionid=sion_open_rank( sionfname, "rb", &localsize, &fsblocksize, &sionrank, &fp);
     strcpy(in->name, path_name);
   } else {
     in->sionid=sion_id;
     fp=sion_get_fp(sion_id);
     strcpy(in->name, path_name+10);
   }
   in->mode = ELGIN_MODE_FILE;

#if !defined(ELG_COMPRESSED)
   in->file = fp;
#else
   secondfd=dup(fileno(fp));
   in->file = gzdopen(secondfd, "r");

   secondfp = gzgetfileptr(in->file);
   sion_set_second_fp(in->sionid,secondfp);
   sion_optimize_fp_buffer(in->sionid);
#endif
   in->bytes_avail_in_block=-1;

   if(in->file==NULL) {
      elg_warning("Cannot open %s file %s", epk_archive_filetype(path_name), path_name);
      free(sionfname);
      free(in);
      return NULL;
   } else {
     elg_cntl_msg("Opened %s file %s for reading %d bytes",
		  epk_archive_filetype(path_name), path_name, -1);
   }
   if(sionfname!=NULL) free(sionfname);
 } else {
   /* open single file */
   if(sionfname!=NULL) free(sionfname);
   in->sionid=-1;
   in->bytes_avail_in_block=-1;
#endif

  /* open file */
  in->mode = ELGIN_MODE_FILE;
#if !defined(ELG_COMPRESSED)
  if ((in->file = fopen(path_name, "r")) == NULL)
#else
  if ((in->file = gzopen(path_name, "r")) == NULL)
#endif
    {
      elg_warning("Cannot open %s file %s", epk_archive_filetype(path_name), path_name);
      free(in);
      return NULL;
    }
  else
    elg_cntl_msg("Opened %s file %s for reading %d bytes",
        epk_archive_filetype(path_name), path_name, sbuf.st_size);

  /* store file name */
  strcpy(in->name, path_name);

#ifdef USE_SIONLIB
 }
#endif

#ifdef USE_SIONLIB
  /* read header and check file format */ 
 check_sion_next_read(in,strlen(ELG_HEADER) + 1 + sizeof(in->major_vnr) + sizeof(in->minor_vnr) + sizeof(in->byte_order));
#endif

#if !defined(ELG_COMPRESSED)
  if (fread(label, strlen(ELG_HEADER) + 1, 1, in->file)           != 1 ||
      fread(&in->major_vnr, sizeof(in->major_vnr), 1, in->file)   != 1 ||
      fread(&in->minor_vnr, sizeof(in->minor_vnr), 1, in->file)   != 1 ||
      fread(&in->byte_order, sizeof(in->byte_order), 1, in->file) != 1 ||
      strcmp(label, ELG_HEADER) != 0)
    {
      fclose(in->file);
      free(in);
      return NULL;
    }
#else
#ifndef USE_SIONLIB
  if (gzread(in->file, label, strlen(ELG_HEADER) + 1)           != strlen(ELG_HEADER) + 1 ||
      gzread(in->file, &in->major_vnr, sizeof(in->major_vnr))   != sizeof(in->major_vnr) ||
      gzread(in->file, &in->minor_vnr, sizeof(in->minor_vnr))   != sizeof(in->minor_vnr) ||
      gzread(in->file, &in->byte_order, sizeof(in->byte_order)) != sizeof(in->byte_order) ||
      strcmp(label, ELG_HEADER) != 0)
#else
  if (gzread_bs(in->file, label, strlen(ELG_HEADER) + 1,&in->bytes_avail_in_block)           != strlen(ELG_HEADER) + 1 ||
      gzread_bs(in->file, &in->major_vnr, sizeof(in->major_vnr),&in->bytes_avail_in_block)   != sizeof(in->major_vnr) ||
      gzread_bs(in->file, &in->minor_vnr, sizeof(in->minor_vnr),&in->bytes_avail_in_block)   != sizeof(in->minor_vnr) ||
      gzread_bs(in->file, &in->byte_order, sizeof(in->byte_order),&in->bytes_avail_in_block) != sizeof(in->byte_order) ||
      strcmp(label, ELG_HEADER) != 0)
#endif
    {
      gzclose(in->file);
      free(in);
      return NULL;
    }

#endif   /* !ELG_COMPRESSED */

  return in;  
}


ElgIn* ElgIn_open_buffer(const elg_ui1* buffer, size_t size)
{
  ElgIn* in;

  /* Validate arguments */
  if (buffer == NULL || size < 10)
    return NULL;

  /* allocate ElgIn record */
  in = (ElgIn*)malloc(sizeof(ElgIn));
  if (in == NULL)
    elg_error();

  /* "open" buffer */
  in->mode   = ELGIN_MODE_BUFFER;
  in->buffer = buffer;
  in->size   = size;
  in->offset = 10;
#ifdef USE_SIONLIB
  in->sionid = -1;
#endif

  /* read header and check file format */
  if (strncmp((char*)buffer, ELG_HEADER, strlen(ELG_HEADER) + 1) != 0)
    {
      free(in);
      return NULL;
    }
  in->major_vnr  = buffer[7];
  in->minor_vnr  = buffer[8];
  in->byte_order = buffer[9];

  /*elg_cntl_msg("Opened buffer for reading");*/

  return in;
}


int ElgIn_close(ElgIn* in)
{
  int result;

  if (in->mode == ELGIN_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
#ifdef USE_SIONLIB
      if(in->sionid>=0) {
	if(sion_is_serial_opened(in->sionid)) result = sion_close(in->sionid);
      } else {
	result = fclose(in->file);
      }
#else 
      result = fclose(in->file);
#endif
#else
      result = gzclose(in->file);
#ifdef USE_SIONLIB
      if(in->sionid>=0) {
	sion_unset_second_fp(in->sionid);
	if(sion_is_serial_opened(in->sionid)) result=sion_close(in->sionid);
      }
#endif

#endif
      if (result == 0)
        elg_cntl_msg("Closed %s file %s", epk_archive_filetype(in->name), in->name);
    }
  else   /* ELGIN_MODE_BUFFER */
    {
      /* elg_cntl_msg("Closed buffer"); */
      result=0;
    }

  free(in);

  return result;
}

/* EPILOG version */

EXTERN elg_ui4  ElgIn_get_version(ElgIn* in)
{
  return in->major_vnr * 1000 + in->minor_vnr;
}

/* Byte order */

EXTERN elg_ui1  ElgIn_get_byte_order(ElgIn* in)
{
  return in->byte_order;
}

/* File position and error management */

int ElgIn_setpos(ElgIn* in, 
		 const fpos_t* pos)
{
  if (in->mode == ELGIN_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      return fsetpos(in->file, pos);
#else
      return gzsetpos(in->file, pos);
#endif
    }
  else   /* ELGIN_MODE_BUFFER */
    {
      elg_warning("ElgIn_setpos() not supported for buffers.");
    }

  return -1;
}

int ElgIn_getpos(ElgIn* in, 
		 fpos_t* pos)
{
  if (in->mode == ELGIN_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      return fgetpos(in->file, pos);
#else
      return gzgetpos(in->file, pos);
#endif
    }
  else   /* ELGIN_MODE_BUFFER */
    {
      elg_warning("ElgIn_getpos() not supported for buffers.");
    }

  return -1;
}

int ElgIn_rewind(ElgIn* in)
{
  if (in->mode == ELGIN_MODE_FILE)
    {
  /* sets position to begin of first record */

#ifdef USE_SIONLIB
      /* rewind sion file */
      if(in->sionid>=0) {  	/* is only set, if file is a sion file, e.g. not for defs */
#if defined(ELG_COMPRESSED)
	int rc=sion_seek(in->sionid,SION_CURRENT_RANK,0,0);
	gzrewind(in->file);
	rc=gzseek(in->file, 10, SEEK_CUR);
#else
	int rc=sion_seek(in->sionid,SION_CURRENT_RANK,0,10);
#endif
	return(rc);
      } 
#endif

#if !defined(ELG_COMPRESSED)
      return fseek(in->file, 10, SEEK_SET);
#else
      return gzseek(in->file, 10, SEEK_SET);
#endif
    }

  /* ELGIN_MODE_BUFFER */
  in->offset = 10;
  return 0;
}

long ElgIn_tell(ElgIn* in)
{
  if (in->mode == ELGIN_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      return ftell(in->file);
#else
      return gztell(in->file);
#endif
    }

  /* ELGIN_MODE_BUFFER */
  return in->offset;
}

int ElgIn_seek(ElgIn* in, long offset)
{
  if (in->mode == ELGIN_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      return fseek(in->file, offset, SEEK_SET);
#else
      return gzseek(in->file, offset, SEEK_SET);
#endif
    }

  /* ELGIN_MODE_BUFFER */
  if (offset < 0 || offset >= in->size)
    return -1;

  in->offset = offset;
  return 0;
}

int ElgIn_ferror(ElgIn* in)
{
  if (in->mode == ELGIN_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      return ferror(in->file);
#else
      int error;
      gzerror(in->file, &error);
      return error;
#endif
    }

  /* ELGIN_MODE_BUFFER */
  return 0;
}

int ElgIn_feof(ElgIn* in)
{
  if (in->mode == ELGIN_MODE_FILE)
    {

#ifdef USE_SIONLIB
      if(in->sionid>=0) {  	/* is only set, if file is a sion file, e.g. not for defs */
	if(sion_feof(in->sionid)) {
#if defined(ELG_COMPRESSED)
	  return(gzgetbufferavailbytes(in->file)==0);  /* check if buffer of zlib is also empty */
#else 
	  return(1);
#endif
	}
      }
#endif

#if !defined(ELG_COMPRESSED)
      return feof(in->file);
#else
      return gzeof(in->file);
#endif
    }
  else   /* ELGIN_MODE_BUFFER */
    {
      return (in->offset == in->size);
    }
}

/* Reading trace records */

ElgRec* ElgIn_read_record(ElgIn* in)
{
  ElgRec* rec;

  /* allocate record */
  rec = calloc(1,sizeof(ElgRec));
  if (rec == NULL)
    elg_error();

  if (in->mode == ELGIN_MODE_FILE)
    {

      /* complete record is stored in a block, 
	 recalulation not necessary for the gzreads of one record  */
      if(!check_sion_next_read(in,sizeof(rec->length)+sizeof(rec->type))) {
          free(rec);
          return NULL;
      }

#if !defined(ELG_COMPRESSED)
      if (fread(&rec->length, sizeof(rec->length), 1, in->file) != 1 ||
          fread(&rec->type, sizeof(rec->type), 1, in->file)     != 1 )
#else
#ifndef USE_SIONLIB
      if (gzread(in->file, &rec->length, sizeof(rec->length)) != sizeof(rec->length) ||
          gzread(in->file, &rec->type, sizeof(rec->type))     != sizeof(rec->type) )
#else
      if (gzread_bs(in->file, &rec->length, sizeof(rec->length),&in->bytes_avail_in_block) != sizeof(rec->length) ||
          gzread_bs(in->file, &rec->type, sizeof(rec->type),&in->bytes_avail_in_block)     != sizeof(rec->type) )
#endif
#endif
        {
          free(rec);

          return NULL;
        }
    }
  else   /* ELGIN_MODE_BUFFER */
    {
      if (in->size - in->offset < sizeof(rec->length) + sizeof(rec->type))
        {
          free(rec);
          return NULL;
        }

      rec->length = in->buffer[in->offset++];
      rec->type   = in->buffer[in->offset++];
    }

  /* allocate record body */
  if (rec->length)
    {
      rec->body = malloc(rec->length);
      if (rec->body == NULL)
        elg_error();

      /* read record body */
      if (in->mode == ELGIN_MODE_FILE)
        {
	  /* complete record is stored in a block, 
	     recalulation not necessary for the gzreads of one record  */
	  if(!check_sion_next_read(in,rec->length)) {
              ElgRec_free(rec);
              return NULL;
	  } 

#if !defined(ELG_COMPRESSED)
          if (fread(rec->body, rec->length, 1, in->file) != 1)
#else
#ifndef USE_SIONLIB
          if (gzread(in->file, rec->body, rec->length) != rec->length)
#else
          if (gzread_bs(in->file, rec->body, rec->length, &in->bytes_avail_in_block) != rec->length)
#endif
#endif
            {
              ElgRec_free(rec);
              return NULL;
            }
        }
      else   /* ELGIN_MODE_BUFFER */
        {
          if (in->size - in->offset < rec->length)
            {
              ElgRec_free(rec);
              return NULL;
            }

          memcpy(rec->body, &in->buffer[in->offset], rec->length);
          in->offset += rec->length;
        }
    }
  else
    {
      rec->body = NULL;
    }

  rec->byte_order = in->byte_order;
  rec->pos        = rec->body;

  return rec;
}

ElgRec* ElgIn_localize_record(ElgIn* in, elg_ui4 lid)
{
  ElgRec* rec;
  
  /* allocate record */
  rec = calloc(1,sizeof(ElgRec));
  if (rec == NULL)
    elg_error();

  if (in->mode == ELGIN_MODE_FILE)
    {
      /* complete record is stored in a block, 
	 recalulation not necessary for the gzreads of one record  */
      if(!check_sion_next_read(in,sizeof(rec->length)+sizeof(rec->type))) {
          free(rec);
          return NULL;
      }

#if !defined(ELG_COMPRESSED)
      if (fread(&rec->length, sizeof(rec->length), 1, in->file) != 1 ||
          fread(&rec->type, sizeof(rec->type), 1, in->file)     != 1 )
#else
#ifndef USE_SIONLIB
      if (gzread(in->file, &rec->length, sizeof(rec->length)) != sizeof(rec->length) ||
          gzread(in->file, &rec->type, sizeof(rec->type))     != sizeof(rec->type) )
#else
      if (gzread_bs(in->file, &rec->length, sizeof(rec->length),&in->bytes_avail_in_block) != sizeof(rec->length) ||
          gzread_bs(in->file, &rec->type, sizeof(rec->type),&in->bytes_avail_in_block)     != sizeof(rec->type) )
#endif
#endif
        {
          free(rec);
          return NULL;
        }
    }
  else   /* ELGIN_MODE_BUFFER */
    {
      if (in->size - in->offset < sizeof(rec->length) + sizeof(rec->type))
        {
          free(rec);
          return NULL;
        }

      rec->length = in->buffer[in->offset++];
      rec->type   = in->buffer[in->offset++];
    }

  /* set byte order before writing any data */
  rec->byte_order = in->byte_order;

  /* allocate record body */
  if (rec->length)
    {
      /* Quick hack: avoid lid write for attribute records */
      if (ElgRec_is_attribute(rec))
        lid = ELG_NO_ID;

      size_t lid_bytes = (lid != ELG_NO_ID) ? sizeof(lid) : 0;

      rec->body = malloc(rec->length + lid_bytes);
      if (rec->body == NULL)
	elg_error();

      if (lid != ELG_NO_ID) ELGREC_WRITE_NUM(rec, lid, 0);

      /* read record body */
      if (in->mode == ELGIN_MODE_FILE)
        {

	  /* complete record is stored in a block, 
	     recalulation not necessary for the gzreads of one record  */
	  if(!check_sion_next_read(in,rec->length)) {
              ElgRec_free(rec);
              return NULL;
	  } 

#if !defined(ELG_COMPRESSED)
          if (fread(rec->body + lid_bytes, rec->length, 1, in->file) != 1)
#else
#ifndef USE_SIONLIB
          if (gzread(in->file, rec->body + lid_bytes, rec->length) != rec->length)
#else
          if (gzread_bs(in->file, rec->body + lid_bytes, rec->length,&in->bytes_avail_in_block) != rec->length)
#endif
#endif
            {
              ElgRec_free(rec);
              return NULL;
            }
        }
      else   /* ELGIN_MODE_BUFFER */
        {
          if (in->size - in->offset < rec->length)
            {
              ElgRec_free(rec);
              return NULL;
            }

          memcpy(rec->body + lid_bytes, &in->buffer[in->offset], rec->length);
          in->offset += rec->length;
        }
      rec->length += lid_bytes;
    }
  else
    {
      rec->body = NULL;
    }

  rec->pos = rec->body;

  return rec;
}

int ElgIn_skip_record(ElgIn* in)
{
  if (in->mode == ELGIN_MODE_FILE)
    {
      int length;

#if !defined(ELG_COMPRESSED)
      if ((length = fgetc(in->file)) == EOF)
        return EOF;

      return fseek(in->file, length + 1, SEEK_CUR);
#else
      if ((length = gzgetc(in->file)) == EOF)
        return EOF;

      return gzseek(in->file, length + 1, SEEK_CUR);
#endif
    }
  else   /* ELGIN_MODE_BUFFER */
    {
      elg_ui1 buflength;

      if (in->offset == in->size)
        return EOF;

      buflength = in->buffer[in->offset];
      if (in->size - in->offset < buflength)
        return EOF;

      in->offset += buflength;
    }

  return 0;
}

/*
 *-----------------------------------------------------------------------------
 * ElgOut
 *-----------------------------------------------------------------------------
 */

/* Data type */

struct ElgOut_struct
{
  int       mode;
#if !defined(ELG_COMPRESSED)
  FILE*     file;
#else
  gzFile    file;
#endif
  elg_ui1*  buffer;
  size_t    size;
  size_t    offset;
  char      name[PATH_MAX];
  ElgRec*   rec;
  long      num_event_pos;
  elg_ui4   num_event_user;
  elg_ui4   num_event_real;
};

/* Open and close defs/trace file */

ElgOut* ElgOut_open(const char* path_name, elg_ui1 byte_order, elg_ui1 flags)
{
  ElgOut* out;
  elg_ui1 major_vnr  = ELG_MAJOR_VNR;
  elg_ui1 minor_vnr  = ELG_MINOR_VNR;
  
  /* allocate ElgOut file rec */

  out = malloc(sizeof(ElgOut));
  if (out == NULL)
    return NULL;
  
  /* allocate buffer for output record */
  out->rec = ElgRec_create_empty(255, ELG_UNKNOWN);
  if (out->rec == NULL)
    {
      free(out);
      return NULL;
    }
  out->rec->byte_order = byte_order;
  
  /* open file */

  out->mode = ELGOUT_MODE_FILE;
#if !defined(ELG_COMPRESSED)
  out->file = fopen(path_name, "w");
#else
  if (flags & ELG_UNCOMPRESSED)
    out->file = gzopen(path_name, "wu");
  else
    out->file = gzopen(path_name, "w");
#endif
  if (out->file == NULL)
    {
      free(out);
      elg_warning("Cannot open %s output file %s", epk_archive_filetype(path_name), path_name);
      return NULL;
    }
  else
    elg_cntl_msg("Opened %s file %s for writing", epk_archive_filetype(path_name), path_name);
  
  /* store file name */
  strcpy(out->name, path_name);
  out->num_event_pos  = -1;
  out->num_event_user = ELG_NO_ID;
  out->num_event_real = 0;

  /* write file header */

#if !defined(ELG_COMPRESSED)
  if (fwrite(ELG_HEADER, strlen(ELG_HEADER) + 1, 1, out->file) != 1 ||
      fwrite(&major_vnr, sizeof(major_vnr), 1, out->file)      != 1 ||
      fwrite(&minor_vnr, sizeof(minor_vnr), 1, out->file)      != 1 ||
      fwrite(&byte_order, sizeof(byte_order), 1, out->file)    != 1 )
    {
      fclose(out->file);
      free(out);
      return NULL;
    }
#else
  if (gzwrite(out->file, ELG_HEADER, strlen(ELG_HEADER) + 1) != strlen(ELG_HEADER) + 1 ||
      gzwrite(out->file, &major_vnr, sizeof(major_vnr))      != sizeof(major_vnr) ||
      gzwrite(out->file, &minor_vnr, sizeof(minor_vnr))      != sizeof(minor_vnr) ||
      gzwrite(out->file, &byte_order, sizeof(byte_order))    != sizeof(byte_order) )
    {
      gzclose(out->file);
      free(out);
      return NULL;
    }
#endif   /* !ELG_COMPRESSED */

  /* return */

  return out;
}

ElgOut* ElgOut_open_buffer(elg_ui1* buffer, size_t size, elg_ui1 byte_order)
{
  ElgOut* out;

  /* Validate arguments */
  if (buffer == NULL || size < 10)
    return NULL;

  /* allocate ElgOut file rec */
  out = malloc(sizeof(ElgOut));
  if (out == NULL)
    return NULL;
  
  /* allocate buffer for output record */
  out->rec = ElgRec_create_empty(255, ELG_UNKNOWN);
  if (out->rec == NULL)
    {
      free(out);
      return NULL;
    }
  out->rec->byte_order = byte_order;
  
  /* "open" buffer */
  out->mode   = ELGOUT_MODE_BUFFER;
  out->buffer = buffer;
  out->size   = size;
  out->offset = 10;

  /* write header */
  memcpy(out->buffer, ELG_HEADER, strlen(ELG_HEADER) + 1);
  out->buffer[7] = ELG_MAJOR_VNR;
  out->buffer[8] = ELG_MINOR_VNR;
  out->buffer[9] = byte_order;

  out->num_event_pos  = -1;
  out->num_event_user = ELG_NO_ID;
  out->num_event_real = 0;

  return out;
}

int ElgOut_close(ElgOut* out)
{
  int result;

  if (out->mode == ELGOUT_MODE_FILE)
    {
      /* correct NUM_EVENT record if needed */
      if ( out->num_event_real != out->num_event_user &&
           out->num_event_pos != -1 )
         ElgOut_write_NUM_EVENTS(out, out->num_event_real);

#if !defined(ELG_COMPRESSED)
      result = fclose(out->file);
#else
      result = gzclose(out->file);
#endif

      if (result == 0)
        elg_cntl_msg("Closed %s file %s", epk_archive_filetype(out->name), out->name);
    }

  if (out->rec)
    ElgRec_free(out->rec);
  free(out);

  return result;
}

/* File position management */
long ElgOut_tell( ElgOut* out )
{
  if (out->mode == ELGOUT_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      return ftell(out->file);
#else
      return gztell(out->file);
#endif
    }

  /* ELGOUT_MODE_BUFFER */
  return out->offset;
}

int ElgOut_seek( ElgOut* out, long offset )
{
  if (out->mode == ELGOUT_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      return fseek(out->file, offset, SEEK_SET);
#else
      return gzseek(out->file, offset, SEEK_SET);
#endif
    }

  /* ELGOUT_MODE_BUFFER */
  if (offset < 0 || offset >= out->size)
    return -1;

  out->offset = offset;
  return 0;
}

/* convenience functions */
#define STR_HASH_MAX 37

typedef struct shi {
  const char* str;
  elg_ui4     id;
  elg_ui4     fid;
  struct shi* next;
} stringHashItem;

static stringHashItem* stringHashTab[STR_HASH_MAX];

static stringHashItem* hashItem = 0;

static elg_ui4 nextStrId = 0;

elg_ui4 ElgOut_define_string(ElgOut* out, const char* str) {
  stringHashItem* ptr = stringHashTab[str[0] % STR_HASH_MAX];

  while ( ptr ) {
    /* -- found -- */
    if ( strcmp(ptr->str, str) == 0 ) {
      hashItem = ptr;
      return ptr->id;
    }
    ptr = ptr->next;
  }

  /* -- not found -- */
  ptr = malloc(sizeof(stringHashTab));
  ptr->str  = strdup(str);
  ptr->id   = nextStrId++;
  ptr->fid  = ELG_NO_ID;
  ptr->next = stringHashTab[str[0] % STR_HASH_MAX];
  stringHashTab[str[0] % STR_HASH_MAX] = ptr;

  ElgOut_write_string(out, ptr->id, str);
  hashItem = ptr;
  return ptr->id;
}

static elg_ui4 nextFileId = 0;

elg_ui4 ElgOut_define_file(ElgOut* out, const char* fname) {
  elg_ui4 fnid = ElgOut_define_string(out, fname);
  elg_ui4 fid;

  if ( hashItem->fid == ELG_NO_ID ) {
    /* new */
    fid = hashItem->fid = nextFileId++;
    ElgOut_write_FILE(out, fid, fnid);
  } else {
    /* old */
    fid = hashItem->fid;
  }
  return fid;
}

static elg_ui4 nextRegionId = 0;

elg_ui4 ElgOut_define_region(ElgOut* out, const char* rname,
       elg_ui4 fid, elg_ui4 begln, elg_ui4 endln,
       const char* rdesc, elg_ui1 rtype) {
  elg_ui4 rid  = nextRegionId++;

  ElgOut_write_REGION(out, rid, ElgOut_define_string(out, rname),
         fid, begln, endln, ElgOut_define_string(out, rdesc), rtype);
  return rid;
}

int ElgOut_write_string(ElgOut* out, elg_ui4 strid, const char* str)
{
  int stringlen = strlen(str) + 1;
  int res = 0;

  if (stringlen <= 250)
    {
      res = ElgOut_write_STRING(out, strid, 0, str);
    }
  else
    {
      int i;
      int restlen = stringlen - 250;
      elg_ui1 cntc  = (restlen / 255) + (restlen % 255 ? 1 : 0);

      res = ElgOut_write_STRING(out, strid, cntc, str);
      str += 250; 
      
      for (i = 1; i < cntc; i++)
	{
          res += ElgOut_write_STRING_CNT(out, str);
	  str += 255;
	}
      /* last contination record */
      res += ElgOut_write_STRING_CNT(out, str);
    }
  return res;  
}

/* Write single records */
int ElgOut_write_STRING(ElgOut* out, elg_ui4 strid, elg_ui1 cntc,
                        const char* str)
{
  ElgRec* rec = out->rec;
  size_t len = 255 - sizeof(strid) -  sizeof(cntc);
  ElgRec_reset_type(rec, ELG_STRING);
  ELGREC_WRITE_VAR(rec, strid);
  ELGREC_WRITE_VAR(rec, cntc);
  if (cntc==0)
    len = strlen(str)+1; /* strlen() + '\0' */
  memcpy(rec->pos, str, len);
  rec->pos += len;
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_STRING_CNT(ElgOut* out, const char* str)
{
  size_t len = 255;
  char*  end;
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_STRING_CNT);
  /* detect string length if possible*/
  end = memchr(str,'\0', len);
  if (end!=0)
    len = end-str+1;
  memcpy(rec->body, str, len);
  rec->pos += len;
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_MACHINE(ElgOut* out, elg_ui4 mid,
                         elg_ui4 nodec, elg_ui4 mnid)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_MACHINE);
  ELGREC_WRITE_VAR(rec, mid);
  ELGREC_WRITE_VAR(rec, nodec);
  ELGREC_WRITE_VAR(rec, mnid);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_NODE(ElgOut* out, elg_ui4 nid,
                      elg_ui4 mid,
                      elg_ui4 cpuc, elg_ui4 nnid, elg_d8 cr)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_NODE);
  ELGREC_WRITE_VAR(rec, nid);
  ELGREC_WRITE_VAR(rec, mid);
  ELGREC_WRITE_VAR(rec, cpuc);
  ELGREC_WRITE_VAR(rec, nnid);
  ELGREC_WRITE_VAR(rec, cr);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_PROCESS (ElgOut* out, elg_ui4 pid,
                          elg_ui4 pnid)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_PROCESS);
  ELGREC_WRITE_VAR(rec, pid);
  ELGREC_WRITE_VAR(rec, pnid);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_THREAD (ElgOut* out, elg_ui4 tid,
                         elg_ui4 pid,
                         elg_ui4 tnid)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_THREAD);
  ELGREC_WRITE_VAR(rec, tid);
  ELGREC_WRITE_VAR(rec, pid);
  ELGREC_WRITE_VAR(rec, tnid);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_LOCATION (ElgOut* out, elg_ui4 lid,
                           elg_ui4 mid,
                           elg_ui4 nid,
                           elg_ui4 pid,
                           elg_ui4 tid)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_LOCATION);
  ELGREC_WRITE_VAR(rec, lid);
  ELGREC_WRITE_VAR(rec, mid);
  ELGREC_WRITE_VAR(rec, nid);
  ELGREC_WRITE_VAR(rec, pid);
  ELGREC_WRITE_VAR(rec, tid);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_FILE(ElgOut* out, elg_ui4 fid, elg_ui4 fnid)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_FILE);
  ELGREC_WRITE_VAR(rec, fid);
  ELGREC_WRITE_VAR(rec, fnid);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_REGION(ElgOut* out, elg_ui4 rid, 
                        elg_ui4 rnid,
                        elg_ui4 fid,
                        elg_ui4 begln,
                        elg_ui4 endln,
                        elg_ui4 rdid,
                        elg_ui1 rtype) 
                        
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_REGION);
  ELGREC_WRITE_VAR(rec, rid);
  ELGREC_WRITE_VAR(rec, rnid);
  ELGREC_WRITE_VAR(rec, fid);
  ELGREC_WRITE_VAR(rec, begln);
  ELGREC_WRITE_VAR(rec, endln);
  ELGREC_WRITE_VAR(rec, rdid);
  ELGREC_WRITE_VAR(rec, rtype);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_CALL_SITE (ElgOut* out, elg_ui4 csid,
                            elg_ui4 fid,
                            elg_ui4 lno,
                            elg_ui4 erid,
                            elg_ui4 lrid)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_CALL_SITE);
  ELGREC_WRITE_VAR(rec, csid);
  ELGREC_WRITE_VAR(rec, fid);
  ELGREC_WRITE_VAR(rec, lno);
  ELGREC_WRITE_VAR(rec, erid);
  ELGREC_WRITE_VAR(rec, lrid);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);

}

int ElgOut_write_CALL_PATH (ElgOut* out, elg_ui4 cpid,
                            elg_ui4 rid,
                            elg_ui4 ppid,
                            elg_ui8 order)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_CALL_PATH);
  ELGREC_WRITE_VAR(rec, cpid);
  ELGREC_WRITE_VAR(rec, rid);
  ELGREC_WRITE_VAR(rec, ppid);
  ELGREC_WRITE_VAR(rec, order);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);

}

int ElgOut_write_METRIC(ElgOut* out, elg_ui4 metid,
                        elg_ui4 metnid,
                        elg_ui4 metdid,
                        elg_ui1 metdtype, elg_ui1 metmode, elg_ui1 metiv)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_METRIC);
  ELGREC_WRITE_VAR(rec, metid); 
  ELGREC_WRITE_VAR(rec, metnid); 
  ELGREC_WRITE_VAR(rec, metdid); 
  ELGREC_WRITE_VAR(rec, metdtype);
  ELGREC_WRITE_VAR(rec, metmode);
  ELGREC_WRITE_VAR(rec, metiv);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_MPI_GROUP(ElgOut* out, elg_ui4 gid, elg_ui1 mode,
                           elg_ui4 grpc, elg_ui4 grpv[])
{
  elg_ui4 i;
  elg_ui4 nranks      = grpc;   /* grpc may be modified by ELGREC_WRITE_VAR */
  elg_ui4 pos         = 0;
  elg_ui4 numRecs     = 0;
  elg_ui4 maxLength   = (255 - sizeof(gid) - sizeof(mode) - sizeof(grpc)) /
                        sizeof(elg_ui4);
  elg_ui4 blockLength = (grpc > maxLength) ? maxLength : grpc;

  /* Write record header */
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_MPI_GROUP);
  ELGREC_WRITE_VAR(rec, gid);
  ELGREC_WRITE_VAR(rec, mode);
  ELGREC_WRITE_VAR(rec, nranks);

  /* Special case: GROUP_WORLD */
  if (mode & ELG_GROUP_WORLD)
  {
    rec->length = rec->pos - rec->body;
    ElgOut_write_record(out, rec);
    numRecs++;

    return rec->length + sizeof(rec->type) + sizeof(rec->length);
  }

  /* Write global rank mapping */
  for (i = 0; i < blockLength; i++)
  {
    ELGREC_WRITE_VAR(rec, grpv[pos]);
    pos++;
  }
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out, rec);
  numRecs++;

  /* Potentially write continuation records */
  maxLength = 255 / sizeof(elg_ui4);
  while (pos < grpc)
  {
    ElgRec_reset_type(rec, ELG_MPI_GROUP_CNT);

    /* Determine number of entries in this record */
    blockLength = grpc - pos;
    if (blockLength > maxLength)
      blockLength = maxLength;

    for (i = 0; i < blockLength; i++)
    {
      ELGREC_WRITE_VAR(rec, grpv[pos]);
      pos++;
    }
    rec->length = rec->pos - rec->body;
    ElgOut_write_record(out, rec);
    numRecs++;
  }

  return sizeof(gid) + sizeof(mode) + sizeof(grpc) +
         numRecs * (sizeof(rec->type) + sizeof(rec->length)) +
         grpc * sizeof(elg_ui4);
}


int ElgOut_write_MPI_COMM_DIST(ElgOut* out, elg_ui4 cid, elg_ui4 root, 
                               elg_ui4 lcid, elg_ui4 lrank, elg_ui4 size)
{
  ElgRec* rec = out->rec;

  ElgRec_reset_type(rec, ELG_MPI_COMM_DIST);
  ELGREC_WRITE_VAR(rec, cid);
  ELGREC_WRITE_VAR(rec, root);
  ELGREC_WRITE_VAR(rec, lcid);
  ELGREC_WRITE_VAR(rec, lrank);
  ELGREC_WRITE_VAR(rec, size);

  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out, rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}


int ElgOut_write_MPI_COMM_REF(ElgOut* out, elg_ui4 cid, elg_ui4 gid)
{
  ElgRec* rec = out->rec;

  ElgRec_reset_type(rec, ELG_MPI_COMM_REF);
  ELGREC_WRITE_VAR(rec, cid); 
  ELGREC_WRITE_VAR(rec, gid); 

  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_MPI_COMM(ElgOut* out, elg_ui4 cid, elg_ui1 mode,
                          elg_ui4 grpc, elg_ui1 grpv[])
{
  elg_ui4 i;
  elg_ui4 nbytes      = grpc; /* grpc may be modified by ELGREC_WRITE_VAR */
  elg_ui4 pos         = 0;
  elg_ui4 numRecs     = 1;
  elg_ui4 maxLength   = 255 - sizeof(cid) - sizeof(mode) - sizeof(grpc);
  elg_ui4 blockLength = (grpc > maxLength) ? maxLength : grpc;

  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_MPI_COMM);
  ELGREC_WRITE_VAR(rec, cid); 
  ELGREC_WRITE_VAR(rec, mode); 
  ELGREC_WRITE_VAR(rec, nbytes); 

  for (i = 0; i < blockLength; i++)
  {
    ELGREC_WRITE_VAR(rec, grpv[pos]);
    pos++;
  }
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out, rec);

  /* Potentially write continuation records */
  while (pos < grpc)
  {
    ElgRec_reset_type(rec, ELG_MPI_COMM_CNT);

    blockLength = grpc - pos;
    if (blockLength > 255)
      blockLength = 255;
    for (i = 0; i < blockLength; i++)
    {
      ELGREC_WRITE_VAR(rec, grpv[pos]);
      pos++;
    }
    rec->length = rec->pos - rec->body;
    ElgOut_write_record(out, rec);
    numRecs++;
  }
  return nbytes + numRecs * (sizeof(rec->type) + sizeof(rec->length))
                + sizeof(cid) + sizeof(mode) + sizeof(grpc);
}

int ElgOut_write_CART_TOPOLOGY(ElgOut* out, elg_ui4 topid, elg_ui4 tnid, elg_ui4 cid,
                               elg_ui1 ndims,
                               elg_ui4 dimv[], elg_ui1 periodv[], elg_ui4 dimids[])
{
  elg_ui1 i,n=ndims; /* ndims may be modified by ELGREC_WRITE_VAR */
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_CART_TOPOLOGY);
  ELGREC_WRITE_VAR(rec, topid);
  ELGREC_WRITE_VAR(rec, cid);
  ELGREC_WRITE_VAR(rec, ndims);
  for (i=0; i<n; i++ )
  {
    ELGREC_WRITE_VAR(rec, dimv[i]);
  }
  for (i=0; i<n; i++ )
  {
    ELGREC_WRITE_VAR(rec, periodv[i]);
  }
  ELGREC_WRITE_VAR(rec, tnid);
  if (dimids) {
    for (i=0; i<n; i++) {
    	ELGREC_WRITE_VAR(rec, dimids[i]);
    }
  } else {
    elg_ui4 dummy = ELG_NO_ID;
    for (i=0; i<n; i++) {
      ELGREC_WRITE_VAR(rec, dummy);
    }
  }
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_CART_COORDS(ElgOut* out, elg_ui4 topid,
                             elg_ui4 lid, elg_ui1 ndims, elg_ui4 coordv[])
{
  elg_ui1 i, n=ndims;
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_CART_COORDS);
  ELGREC_WRITE_VAR(rec, topid); 
  ELGREC_WRITE_VAR(rec, lid); 
  ELGREC_WRITE_VAR(rec, ndims); 
  for (i=0; i<n; i++ )
  {
    ELGREC_WRITE_VAR(rec, coordv[i]); 
  }
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_MPI_WIN(ElgOut* out, elg_ui4 wid, elg_ui4 cid)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_MPI_WIN);
  ELGREC_WRITE_VAR(rec, wid);
  ELGREC_WRITE_VAR(rec, cid);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_OFFSET(ElgOut* out, elg_d8 ltime, elg_d8 ofs_time)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_OFFSET);
  ELGREC_WRITE_VAR(rec, ltime); 
  ELGREC_WRITE_VAR(rec, ofs_time); 
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_EVENT_TYPES(ElgOut* out, elg_ui4 ntypes, elg_ui1 typev[])
{
  elg_ui4 i;
  elg_ui4 count = ntypes; /* ntypes may be modified by ELGREC_WRITE_VAR */
  ElgRec* rec   = out->rec;
  ElgRec_reset_type(rec, ELG_EVENT_TYPES);
  ELGREC_WRITE_VAR(rec, count);
  for (i=0; i<ntypes; i++)
  {
    ELGREC_WRITE_VAR(rec, typev[i]); 
  }
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_EVENT_COUNTS(ElgOut* out, elg_ui4 ntypes, elg_ui4 countv[])
{
  elg_ui4 i;
  elg_ui4 count = ntypes; /* ntypes may be modified by ELGREC_WRITE_VAR */
  ElgRec* rec   = out->rec;
  ElgRec_reset_type(rec, ELG_EVENT_COUNTS);
  ELGREC_WRITE_VAR(rec, count);
  for (i=0; i<ntypes; i++)
  {
    ELGREC_WRITE_VAR(rec, countv[i]); 
  }
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);
  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_MAP_SECTION(ElgOut* out, elg_ui4 rank)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_MAP_SECTION);
  ELGREC_WRITE_VAR(rec, rank);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);

  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_MAP_OFFSET(ElgOut* out, elg_ui4 rank, elg_ui4 offset)
{
  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_MAP_OFFSET);
  ELGREC_WRITE_VAR(rec, rank);
  ELGREC_WRITE_VAR(rec, offset);
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out,rec);

  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_IDMAP(ElgOut* out, elg_ui1 type, elg_ui1 mode, elg_ui4 count,
                       elg_ui4* mapv)
{
  elg_ui4 i;
  elg_ui4 pos     = 0;
  elg_ui4 numRecs = 1;
  elg_ui4 cnt     = count; /* count may be modified by ELGREC_WRITE_VAR */
  elg_ui4 max     = (255 - sizeof(type) - sizeof(mode) - sizeof(count)) /
                      sizeof(elg_ui4);
  elg_ui4 block = (count > max) ? max : count;

  ElgRec* rec = out->rec;
  ElgRec_reset_type(rec, ELG_IDMAP);
  ELGREC_WRITE_VAR(rec, type); 
  ELGREC_WRITE_VAR(rec, mode); 
  ELGREC_WRITE_VAR(rec, cnt); 

  for (i=0; i<block; i++)
  {
    ELGREC_WRITE_VAR(rec, mapv[pos]);
    pos++;
  }
  rec->length = rec->pos - rec->body;
  ElgOut_write_record(out, rec);

  /* Potentially write continuation records */
  max = 255 / sizeof(elg_ui4);
  while (pos < count)
  {
    ElgRec_reset_type(rec, ELG_IDMAP_CNT);

    block = count - pos;
    if (block > max)
      block = max;
    for (i=0; i<block; i++)
    {
      ELGREC_WRITE_VAR(rec, mapv[pos]);
      pos++;
    }
    rec->length = rec->pos - rec->body;
    ElgOut_write_record(out, rec);
    numRecs++;
  }
  return numRecs * (sizeof(rec->type) + sizeof(rec->length)) +
         sizeof(type) + sizeof(mode) + sizeof(count) + count * sizeof(elg_ui4);
}

int ElgOut_write_NUM_EVENTS(ElgOut* out, elg_ui4 eventc)
{
  ElgRec* rec = out->rec;
  long cur_pos=-1;
  ElgRec_reset_type(rec, ELG_NUM_EVENTS);
  ELGREC_WRITE_VAR(rec, eventc);
  rec->length = rec->pos - rec->body;

  /* save user specified event count */
  out->num_event_user = eventc;
  
  if ( out->num_event_pos == -1)
  { 
    /* write user specified NUM_EVENT and      */
    /* save NUM_EVENT record position          */
#if !defined(ELG_COMPRESSED)
    out->num_event_pos = ftell(out->file);
#else
    out->num_event_pos = gztell(out->file);
#endif
    if (out->num_event_pos == -1)
      elg_warning("ftell(...) is unsupported on this system or file is too big\n");
    ElgOut_write_record(out,rec);
  }
  else
  {
#if defined (ELG_COMPRESSED)
    if (gzdirect(out->file))
#endif
    {
      /* move to NUM_EVENT record and jump back when complete */
      /* return immediately  if error occures                 */
#if !defined(ELG_COMPRESSED)
      cur_pos = ftell(out->file);
#else
      cur_pos = gztell(out->file);
#endif
      if (cur_pos == -1) 
      {
        elg_warning("ftell(...) is unsupported on this system or file is too big\n");
        return 0;
      }
#if !defined(ELG_COMPRESSED)
      if (0 != fseek(out->file, out->num_event_pos, SEEK_SET) )
#else
      if ( gzseek(out->file, out->num_event_pos, SEEK_SET) < 0 )
#endif
      {
         elg_warning("Search for NUM_EVENT record failed. NUM_EVENT record is not overwriten\n");
         return 0;
      }
      ElgOut_write_record(out,rec);
#if !defined(ELG_COMPRESSED)
      fseek(out->file, cur_pos, SEEK_SET);
#else
      gzseek(out->file, cur_pos, SEEK_SET);
#endif
    }
#if defined(ELG_COMPRESSED)
    else
    {
      elg_warning("Cannot write NUM_EVENT records to compressed files.");
      return 0;
    }
#endif
  }

  return rec->length + sizeof(rec->type) + sizeof(rec->length);
 }

int ElgOut_write_LAST_DEF(ElgOut* out)
{
  ElgRec* rec = out->rec;

  /*insert dummy num events record if not yet seen*/
  /*will be automatically fixed ater*/
  if (out->num_event_pos == -1)
  {
    ElgOut_write_NUM_EVENTS(out, 0);
  }

  ElgRec_reset_type(rec, ELG_LAST_DEF);
  rec->length = 0;
  ElgOut_write_record(out,rec);
  return sizeof(rec->type) + sizeof(rec->type);
}

/* -- Attributes -- */

int ElgOut_write_ATTR_UI1(ElgOut* out, 
                          elg_ui1 type, 
                          elg_ui1 val) 
{
  ElgRec* rec = out->rec;

  ElgRec_reset_type(rec, ELG_ATTR_UI1);

  ELGREC_WRITE_VAR(rec, type);
  ELGREC_WRITE_VAR(rec, val);

  rec->length = rec->pos - rec->body;

  ElgOut_write_record(out,rec);

  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}

int ElgOut_write_ATTR_UI4(ElgOut* out, 
                          elg_ui1 type, 
                          elg_ui4 val) 
{
  ElgRec* rec = out->rec;

  ElgRec_reset_type(rec, ELG_ATTR_UI4);

  ELGREC_WRITE_VAR(rec, type);
  ELGREC_WRITE_VAR(rec, val);

  rec->length = rec->pos - rec->body;

  ElgOut_write_record(out,rec);

  return rec->length + sizeof(rec->type) + sizeof(rec->length);
}


#include "elg_rw.c.gen"

/* Write recs */

int ElgOut_localize_record(ElgOut* out, 
			   ElgRec* rec,
			   elg_ui4 lid)
{
  elg_ui1 new_length = rec->length + sizeof(lid);

  if (out->mode == ELGOUT_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      if (fwrite(&new_length, sizeof(new_length), 1, out->file) != 1 ||
          fwrite(&rec->type, sizeof(rec->type), 1, out->file)   != 1 ||
          fwrite(&lid, sizeof(lid), 1, out->file)               != 1 ||
          fwrite(rec->body, rec->length , 1, out->file)         != 1)
#else
      if (gzwrite(out->file, &new_length, sizeof(new_length)) != sizeof(new_length) ||
          gzwrite(out->file, &rec->type, sizeof(rec->type))   != sizeof(rec->type) ||
          gzwrite(out->file, &lid, sizeof(lid))               != sizeof(lid) ||
          gzwrite(out->file, rec->body, rec->length)          != rec->length)
#endif
        return EOF;
    }
  else   /* ELGOUT_MODE_BUFFER */
    {
      if (out->size - out->offset < new_length + 2)
        return EOF;

      out->buffer[out->offset++] = new_length;
      out->buffer[out->offset++] = rec->type;
      memcpy(&(out->buffer[out->offset]), &lid, sizeof(lid));
      out->offset += sizeof(lid);
      memcpy(&(out->buffer[out->offset]), rec->body, rec->length);
      out->offset += rec->length;
    }

  return sizeof(new_length) + sizeof(rec->type) + sizeof(lid) + new_length;  
}

int ElgOut_write_record(ElgOut* out, 
			ElgRec* rec)
{
  if (out->mode == ELGOUT_MODE_FILE)
    {
#if !defined(ELG_COMPRESSED)
      if (fwrite(&rec->length, sizeof(rec->length), 1, out->file) != 1 ||
          fwrite(&rec->type, sizeof(rec->type), 1, out->file)     != 1 ||
          fwrite(rec->body, rec->length, 1, out->file)            != 1)
#else
      if (gzwrite(out->file, &rec->length, sizeof(rec->length)) != sizeof(rec->length) ||
          gzwrite(out->file, &rec->type, sizeof(rec->type))     != sizeof(rec->type) ||
          gzwrite(out->file, rec->body, rec->length)            != rec->length)
#endif
        return EOF;
    }
  else   /* ELGOUT_MODE_BUFFER */
    {
      if (out->size - out->offset < rec->length + 2)
        return EOF;

      out->buffer[out->offset++] = rec->length;
      out->buffer[out->offset++] = rec->type;
      memcpy(&(out->buffer[out->offset]), rec->body, rec->length);
      out->offset += rec->length;
    }

  /* count successful writen event records */
  if ( ElgRec_is_event(rec) )
    out->num_event_real++;
  
  return sizeof(rec->length) + sizeof(rec->type) + rec->length;
} 
