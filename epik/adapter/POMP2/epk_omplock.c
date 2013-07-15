/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include <stdlib.h>

#include "epk_omplock.h"

struct EpkLock {
  const void *lock;
  long lkid;
};

#define EPK_LOCKBLK_SIZE 100

struct EpkLockBlock {
  struct EpkLock lock[EPK_LOCKBLK_SIZE];
  struct EpkLockBlock *next;
  struct EpkLockBlock *prev;
};

static struct EpkLockBlock *head_block = 0;
static struct EpkLockBlock *last_block = 0;
static struct EpkLock *lastlock = 0;
static int lastidx = EPK_LOCKBLK_SIZE;

static elg_ui4 curlkid = 0;

void epk_lock_close()
{
  struct EpkLockBlock *block;

  /* free lock blocks */

  while (head_block) {
    block = head_block;
    head_block = head_block->next;
    free(block);
  }
}

elg_ui4 epk_lock_init(const void* lock)
{
  struct EpkLockBlock *new_block;

  lastidx++;
  if (lastidx >= EPK_LOCKBLK_SIZE) 
    {
      if (head_block == 0 ) 
	{
	  /* first time: allocate and initialize first block */
	  new_block = malloc(sizeof(struct EpkLockBlock));
	  new_block->next = 0;
	  new_block->prev = 0;
	  head_block = last_block = new_block;
	} 
      else if (last_block == 0 ) 
	{
	  /* lock list empty: re-initialize */
	  last_block = head_block;
	} 
      else 
	{
	  if (last_block->next == 0 ) 
	    {
	      /* lock list full: expand */
	      new_block = malloc(sizeof(struct EpkLockBlock));
	      new_block->next = 0;
	      new_block->prev = last_block;
	      last_block->next = new_block;
	    }
	  /* use next available block */
	  last_block = last_block->next;
	}
      lastlock = &(last_block->lock[0]);
      lastidx  = 0;
    } 
  else 
    {
      lastlock++;
    }
  /* store lock information */
  lastlock->lock = lock;
  lastlock->lkid = curlkid++;
  return lastlock->lkid;
}

static struct EpkLock* epk_lock_get(const void* lock)
{
  int i;
  struct EpkLockBlock *block;
  struct EpkLock *curr;

  /* search all locks in all blocks */
  block = head_block;
  while (block) {
    curr = &(block->lock[0]);
    for (i = 0; i < EPK_LOCKBLK_SIZE; ++i) 
      {
	if (curr->lock == lock) 
	  return curr;

	curr++;
      }
    block = block->next;
  }
  return 0;
}

elg_ui4 epk_lock_id(const void* lock)
{
  struct EpkLock* lockData = epk_lock_get(lock);

  if (lockData)
    return lockData->lkid;

  return ELG_NO_ID;
}

void epk_lock_destroy(const void* lock)
{
  /* delete lock by copying last lock in place of lock */ 

  *epk_lock_get(lock) = *lastlock;
  
  /* adjust pointer to last lock  */
  lastidx--;
  if (lastidx < 0) 
    {
      /* reached low end of block */
      if (last_block->prev) 
	{
	  /* goto previous block if existing */
	  lastidx = EPK_LOCKBLK_SIZE-1;
	  lastlock = &(last_block->prev->lock[lastidx]);
	} 
      else 
	{
	  /* no previous block: re-initialize */
	  lastidx = EPK_LOCKBLK_SIZE;
	  lastlock = 0;
	}
      last_block = last_block->prev;
    } 
  else 
    {
      lastlock--;
    }  
}
