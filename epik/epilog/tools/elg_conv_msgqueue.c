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

/*===========================================================================*/
/*                                                                           */
/* EPILOG Converter Message Queue Utility module                             */
/*                                                                           */
/*===========================================================================*/

#include "elg_conv_msgqueue.h"

int nummsg = 0;

static Message* freeList = 0;

#define SRC_MAX  256
#define RECV_MAX  16

typedef struct {
  Message* first;
  Message* last;
} MsgList;

static MsgList** mqueue = 0;

void AddMessage(elg_ui4 slid, elg_ui4 dlid, elg_ui4 tag,
                elg_ui4 cid, elg_ui4 len, elg_d8 etime, elg_d8 stime) {
  Message* ptr;
  int shash = slid % SRC_MAX;
  int dhash = dlid % RECV_MAX;
  int i;

  /* get message descriptor */
  if ( freeList ) {
    ptr = freeList;
    freeList = freeList->next;
  } else {
    ptr = x_malloc(sizeof(Message));
  }

  /* fill message descriptor */
  ptr->slid  = slid;
  ptr->dlid  = dlid;
  ptr->tag   = tag;
  ptr->cid   = cid;
  ptr->len   = len;
  ptr->etime = etime;
  ptr->stime = stime;
  ptr->next  = 0;

  /* find place in message queue */
  if ( mqueue == 0 ) {
    /* initialize message queue */
    mqueue = x_malloc(SRC_MAX * sizeof(MsgList*));
    for (i=0; i<SRC_MAX; i++) mqueue[i] = 0;
  }

  if ( mqueue[shash] == 0 ) {
    /* initialize message queue for source location == shash */
    mqueue[shash] = x_malloc(RECV_MAX * sizeof(MsgList));
    for (i=0; i<RECV_MAX; i++) {
      mqueue[shash][i].first = 0;
      mqueue[shash][i].last  = 0;
    }
  }

  /* insert new message */
  if ( mqueue[shash][dhash].first == 0 ) {
    mqueue[shash][dhash].first = mqueue[shash][dhash].last = ptr;
  } else {
    mqueue[shash][dhash].last = mqueue[shash][dhash].last->next = ptr;
  }
  ++nummsg;
}

Message* FindMessage(elg_ui4 slid, elg_ui4 dlid, elg_ui4 tag, elg_ui4 cid) {
  int shash = slid % SRC_MAX;
  int dhash = dlid % RECV_MAX;

  if ( mqueue != 0 && mqueue[shash] != 0 && mqueue[shash][dhash].first != 0 ) {
    Message* curr =  mqueue[shash][dhash].first;
    Message* prev = 0;

    while ( curr ) {
      if ( curr->slid == slid && curr->dlid == dlid &&
           curr->tag == tag && curr->cid == cid ) {
	/* found: dequeue "curr" and return */
	if ( prev ) {
	  prev->next = curr->next;
	  if ( mqueue[shash][dhash].last == curr ) {
	    mqueue[shash][dhash].last = prev;
	  }
	} else {
	  mqueue[shash][dhash].first = curr->next;
	}

	/* return to freeList */
	curr->next = freeList;
	freeList = curr;
	--nummsg;
	return curr;
      }

      /* skip to next */
      prev = curr;
      curr = curr->next;
    }
  }
  return 0;
}

