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

#include "elg_mrgthrd.h"
#include "elg_rw.h"
#include "elg_error.h"
#include "epk_conf.h"


static void elg_mrg_recs(ElgOut* out, int infilec, ElgIn** inv)
{
  int i;
  ElgRec** recv   = calloc(infilec, sizeof(ElgRec*));
  long*    posv   = calloc(infilec, sizeof(long));
  long*    attrv  = calloc(infilec, sizeof(long));

  /* init */
  for (i = 0; i < infilec; i++)
    {
      posv[i] = ElgIn_tell(inv[i]);
      recv[i] = ElgIn_read_record(inv[i]);
    }

  while(1)
    {
      ElgRec* min_rec = NULL;
      int min_pos;      
      
      for (i = 0; i < infilec ; i++)
	{
	  if (recv[i] == NULL)
	      continue;

          /* save pos of first attribute record (pos - reclen - type,len bytes) 
           * and scan over remaining attribute records */
          if (ElgRec_is_attribute(recv[i]))
            {
              attrv[i] = posv[i];

              while (ElgRec_is_attribute(recv[i]))
                {
                  ElgRec_free(recv[i]);
                  recv[i] = ElgIn_read_record(inv[i]);
                }
            }

	  /* definition records are oldest by default */
	  if (!ElgRec_is_event(recv[i]) && !ElgRec_is_attribute(recv[i]))
	    {
	      min_rec = recv[i];	      
	      min_pos = i;
	      break;
	    }
	  /* select first available record */
	  else if (min_rec == NULL)
	    {
	      min_rec = recv[i];	      
	      min_pos = i;
	      if (!ElgRec_is_event(min_rec))
		break;
	    }
	  /* compare timestamps: looking for oldest (minimum time stamp) record */     
	  else
	    {
	      double first, second;

	      ElgRec_seek(min_rec, 0);
	      ElgRec_seek(recv[i], 0);

	      first  = ElgRec_read_d8(min_rec);
	      second = ElgRec_read_d8(recv[i]);
	      
	      if (first > second)
		{
		  min_rec = recv[i];
		  min_pos = i;
		}
	    }
	}
      /* finish if all records are processed */
      if (min_rec == NULL) 
        {
          free(attrv);
          free(posv);
          free(recv);

          return;
        }

      /* write attributes, if any  */
      if (attrv[min_pos])
        {
          ElgRec* tmprec;

          ElgIn_seek(inv[min_pos], attrv[min_pos]);

          for (tmprec = ElgIn_read_record(inv[min_pos]); 
               ElgRec_is_attribute(tmprec); 
               tmprec = ElgIn_read_record(inv[min_pos]))
            {
              ElgOut_write_record(out, tmprec);
              ElgRec_free(tmprec);
            }

          ElgRec_free(tmprec);
          attrv[min_pos] = 0;
        }

      /* replace oldest record by new record */
      posv[min_pos] = ElgIn_tell(inv[min_pos]);
      recv[min_pos] = ElgIn_read_record(inv[min_pos]);

      /* correct location identifier */
      if (ElgRec_get_type(min_rec) == ELG_LOCATION)
	{
	  ElgRec_write_ui4(min_rec, min_pos, 0);
	}

      /* correct location identifier in ELG_CART_COORDS */
      if (ElgRec_get_type(min_rec) == ELG_CART_COORDS)
	{
	  ElgRec_write_ui4(min_rec, min_pos, sizeof(elg_ui4));
	}

      if (!ElgRec_is_event(min_rec))
	  /* write record to outfile */
	  ElgOut_write_record(out, min_rec);
      else
	  /* write record to outfile and add location */
	  ElgOut_localize_record(out, min_rec, min_pos);

      ElgRec_free(min_rec);
    }
}


void elg_mrgthrd(char* outfile_name, 
		 int infilec, char** infile_namev)
{
  int i;
  ElgOut* out;
  ElgIn** inv; 
  int flag = 0;

  /* open infiles */

  inv = calloc(infilec, sizeof(ElgIn*));
  for (i = 0; i < infilec; i++)
    if ( !(inv[i] = ElgIn_open(infile_namev[i])) )
      elg_error_msg("Cannot open input file %s", infile_namev[i]);

  /* open outfile */
#if defined(ELG_COMPRESSED)
  if (!epk_str2bool(epk_get(ELG_COMPRESSION)))
    flag = ELG_UNCOMPRESSED;
#endif
  if ( !(out = ElgOut_open(outfile_name, ElgIn_get_byte_order(inv[0]), flag)) )
    elg_error_msg("Cannot open output file %s", outfile_name);
   
  /* merge records */
  elg_mrg_recs(out, infilec, inv);
    
  /* close infiles */
  for (i = 0; i < infilec; i++)
    ElgIn_close(inv[i]);

  /* close outfiles */
  ElgOut_close(out);
}
