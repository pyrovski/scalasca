/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/**
 * @file  epk_mpireg.c
 *
 * @brief Request handling for MPI adapter
 */

#include "epk_mpireq.h"
#include "epk_mpicom.h"
#include "epk_mpireg.h"
#include "elg_error.h"

#include "esd_def.h"
#include "esd_event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 *-----------------------------------------------------------------------------
 *
 * Request management
 *
 *-----------------------------------------------------------------------------
 */

/** @internal size of element list behind a hash entry */
#define EPK_REQBLK_SIZE  16
/** @internal size of hash table */
#define EPK_REQHASH_SIZE 256

/** @internal Block of linked list of request blocks */
struct EpkRequestBlock {
  struct EpkRequest req[EPK_REQBLK_SIZE];
  struct EpkRequestBlock *next;
  struct EpkRequestBlock *prev;
};

/** @internal Hash access structure */
struct EpkRequestHash {
  struct EpkRequestBlock *head_block;
  struct EpkRequestBlock *last_block;
  struct EpkRequest      *lastreq;
  int lastidx;
};

/** @internal Data structure to store request info for effective request
 *            tracking */
static struct EpkRequestHash request_hash[EPK_REQHASH_SIZE] = { 
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  32 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  32 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  32 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  32 */ /* 128 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  32 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  32 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  32 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   2 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   4 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   6 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*   8 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  10 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  12 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  14 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  16 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  18 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  20 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  22 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  24 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  26 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  28 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }, /*  30 */
  { 0, 0, 0, EPK_REQBLK_SIZE }, { 0, 0, 0, EPK_REQBLK_SIZE }  /*  32 */ /* 256 */
};

/** @internal
 * Request id counter
 */
static elg_ui4 lastreqid = 0;


/** @internal
 * @brief  Return entry in the hash table that holds the request tracking data
 * @param  req MPI request handle
 * @return Pointer to request hash entry
 */
static struct EpkRequestHash* epk_get_request_hash_entry(MPI_Request req)
{
  unsigned char* cptr = (unsigned char*) &req;
  unsigned char  h    = cptr[0];
  /* int i; */

  /*
   * The hash function.
   * At least on BlueGene and Jump, MPI_Request is a 32-bit integer, which
   * more-or-less seems to represent a request count. Hence we can use a 
   * simple and fast hash like msb^lsb here.
   *
   * On Linux/LAM, MPI_Request is still 4 bytes, but the representation is
   * different. The simple hash function would not use all hash bits; here, 
   * the loop over all bytes sketched below would be better:
   *
   *   for (i = 1; i < sizeof(MPI_Request); ++i)
   *     h ^= cptr[i];
   */

  h ^= cptr[sizeof(MPI_Request)-1];

  return &request_hash[h];
}

/** @internal
 * @brief Clean up request tracking infrastructure
 */
void epk_request_finalize()
{
  struct EpkRequestBlock* block;
  int i;

  /* free request blocks */

  for (i = 0; i < EPK_REQHASH_SIZE; ++i)
    while (request_hash[i].head_block) {
      block = request_hash[i].head_block;
      request_hash[i].head_block = request_hash[i].head_block->next;
      free(block);
    }
}

/** @internal
 * @brief Return a new request id
 */

elg_ui4 epk_get_request_id()
{
  return ++lastreqid;
}

/** @internal
 * @brief Create entry for a given MPI request handle
 * @param request  MPI request handle
 * @param flags    Bitmask containing flags set for this request
 * @param tag      MPI tag for this request
 * @param dest     Destination rank of request
 * @param bytes    Number of bytes transfered in request
 * @param datatype MPI datatype handle
 * @param comm     MPI communicator handle
 * @param id       Request id
 */
void epk_request_create(MPI_Request request, 
			unsigned flags,
			int tag, 
			int dest, 
			int bytes, 
			MPI_Datatype datatype,
			MPI_Comm comm,
                        elg_ui4 id)
{
  struct EpkRequestBlock *new_block;
  struct EpkRequestHash  *hash_entry = epk_get_request_hash_entry(request);

  hash_entry->lastidx++;
  if (hash_entry->lastidx >= EPK_REQBLK_SIZE) 
    {
      if (hash_entry->head_block == 0 ) 
	{
	  /* first time: allocate and initialize first block */
	  new_block = malloc(sizeof(struct EpkRequestBlock));
	  new_block->next = 0;
	  new_block->prev = 0;
	  hash_entry->head_block = hash_entry->last_block = new_block;
	} 
      else if (hash_entry->last_block == 0 ) 
	{
	  /* request list empty: re-initialize */
	  hash_entry->last_block = hash_entry->head_block;
	} 
      else 
	{
	  if (hash_entry->last_block->next == 0 ) 
	    {
	      /* request list full: expand */
	      new_block = malloc(sizeof(struct EpkRequestBlock));
	      new_block->next = 0;
	      new_block->prev = hash_entry->last_block;
	      hash_entry->last_block->next = new_block;
	    }
	  /* use next available block */
	  hash_entry->last_block = hash_entry->last_block->next;
	}
      hash_entry->lastreq = &(hash_entry->last_block->req[0]);
      hash_entry->lastidx = 0;
    } 
  else 
    {
      hash_entry->lastreq++;
    }
  /* store request information */
  hash_entry->lastreq->request  = request;
  hash_entry->lastreq->flags    = ERF_NONE;
  hash_entry->lastreq->flags   |= flags;
  hash_entry->lastreq->tag      = tag;
  hash_entry->lastreq->dest     = dest;
  hash_entry->lastreq->bytes    = bytes;
  hash_entry->lastreq->datatype = datatype;
  hash_entry->lastreq->comm     = comm;
  hash_entry->lastreq->id       = id;
}

/** @internal
 * @brief  Retrieve internal request entry for an MPI request handle
 * @param  request MPI request handle
 * @return Pointer to corresponding internal request entry
 */
struct EpkRequest* epk_request_get(MPI_Request request)
{
  struct EpkRequestHash  *hash_entry = epk_get_request_hash_entry(request);
  int i;
  struct EpkRequestBlock *block;
  struct EpkRequest *curr;

  /* list empty */
  if (!hash_entry->lastreq) return 0;

  /* search all requests in all blocks */
  block = hash_entry->head_block;
  while (block) {
    curr = &(block->req[0]);
    for (i = 0; i < EPK_REQBLK_SIZE; ++i) 
      {
	/* found? */
	if (curr->request == request) 
	  return curr;

	/* end of list? */
	if (curr == hash_entry->lastreq)
	  return 0;

	curr++;
      }
    block = block->next;
  }
  return 0;
}

/** @internal
 * @brief Free a request entry
 * @param req Pointer to request entry to be deleted
 */
void epk_request_free(struct EpkRequest* req)
{
  struct EpkRequestHash *hash_entry = epk_get_request_hash_entry(req->request);

  /* delete request by copying last request in place of req */ 
  if (!hash_entry->lastreq) {
    elg_error_msg("INTERNAL ERROR in request handling - no last request");
  }
  *req = *(hash_entry->lastreq);
  hash_entry->lastreq->flags   = ERF_NONE;
  hash_entry->lastreq->request = 0;

  /* adjust pointer to last request */
  hash_entry->lastidx--;
  if (hash_entry->lastidx < 0) 
    {
      /* reached low end of block */
      if (hash_entry->last_block->prev) 
	{
	  /* goto previous block if existing */
	  hash_entry->lastidx = EPK_REQBLK_SIZE-1;
	  hash_entry->lastreq = &(hash_entry->last_block->prev->req[hash_entry->lastidx]);
	} 
      else 
	{
	  /* no previous block: re-initialize */
	  hash_entry->lastidx = EPK_REQBLK_SIZE;
	  hash_entry->lastreq = 0;
	}
      hash_entry->last_block = hash_entry->last_block->prev;
    } 
  else 
    {
      hash_entry->lastreq--;
    }  
}

void epk_check_request(struct EpkRequest* req, MPI_Status *status)
{
  const int event_gen_active = (epk_mpi_enabled & EPK_MPI_ENABLED_P2P);
  const int xnb_active       = (epk_mpi_enabled & EPK_MPI_ENABLED_XNONBLOCK);

  if (!req ||
      ((req->flags & ERF_IS_PERSISTENT) && !(req->flags & ERF_IS_ACTIVE)))
    return;

  if (event_gen_active)
    {
      int cancelled = 0;

      if (req->flags & ERF_CAN_CANCEL)
        PMPI_Test_cancelled(status, &cancelled);

      if (cancelled)
        {
          if (xnb_active)
            esd_mpi_cancelled(req->id);
        }
      else if ((req->flags & ERF_RECV) && (status->MPI_SOURCE != MPI_PROC_NULL))
        {
          /* if receive request, write receive trace record */

          int count, sz;

          PMPI_Type_size(req->datatype, &sz);
          PMPI_Get_count(status, req->datatype, &count);

          if (xnb_active)
            esd_attr_ui4(ELG_ATTR_REQUEST, req->id);          

          esd_mpi_recv(EPK_RANK(status->MPI_SOURCE),
                       EPK_COMM_ID(req->comm), status->MPI_TAG, count * sz);     
        }
      else if (req->flags & ERF_SEND && xnb_active)
        {
          esd_mpi_send_complete(req->id);
        }
    }
 
  if (req->flags & ERF_IS_PERSISTENT)
    {
      /* if persistent request, set to inactive,
         and, if requested delete request */
      req->flags &= ~ERF_IS_ACTIVE;
      if (req->flags & ERF_DEALLOCATE) epk_request_free(req);
    }
  else
    {
      /* if non-persistent request, delete always request */
      epk_request_free(req);
    }
}

static MPI_Request *orig_req_arr = 0;
static int orig_req_arr_size = 0;

void epk_save_request_array(MPI_Request *arr_req, int arr_req_size)
{
  if (orig_req_arr_size == 0) {
    /* -- never used: initialize -- */
    orig_req_arr = malloc(arr_req_size * sizeof(MPI_Request));
    orig_req_arr_size = arr_req_size;
  } else if (arr_req_size > orig_req_arr_size) {
    /* -- not enough room: expand -- */
    orig_req_arr = realloc(orig_req_arr, arr_req_size * sizeof(MPI_Request));
    orig_req_arr_size = arr_req_size;
  }

  /* -- copy array -- */
  /* for (i=0; i<arr_req_size; ++i) orig_req_arr[i] = arr_req[i]; */
  memcpy(orig_req_arr, arr_req, arr_req_size*sizeof(MPI_Request));
}

struct EpkRequest* epk_saved_request_get(int i)
{
  return epk_request_get(orig_req_arr[i]);
}
