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

#ifndef _ELG_CONF_MSGQUEUE_H
#define _ELG_CONF_MSGQUEUE_H

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

#include <stdlib.h>

#include "elg_types.h"
#include "elg_conv.h"

typedef struct msg {
  elg_ui4 slid;  /* source location */
  elg_ui4 dlid;  /* destination location */
  elg_ui4 tag;  
  elg_ui4 cid;   /* communicator id */
  elg_ui4 len;   /* length */
  elg_d8  etime; /* enter timestamp */
  elg_d8  stime; /* send timestamp */
  struct msg* next;
} Message;

EXTERN int nummsg;

EXTERN void
AddMessage(elg_ui4 slid, elg_ui4 dlid, elg_ui4 tag, elg_ui4 cid, elg_ui4 len,
           elg_d8 etime, elg_d8 stime);

EXTERN Message*
FindMessage(elg_ui4 slid, elg_ui4 dlid, elg_ui4 tag, elg_ui4 cid);

#endif
