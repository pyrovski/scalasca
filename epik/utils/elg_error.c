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

#include "elg_error.h"

#include "epk_conf.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define ELG_MSG_PFIX  "EPIK"
#define ELG_MSG_SIZE  4096

static int elg_pid = -1;
static int (*elg_tid_fn)(void) = NULL;
static int elg_verbose = 0;

void elg_error_pid(const int pid)
{
  elg_pid = pid;
  return;
}

void elg_error_thread_init(int (*tid_fn)(void))
{
  elg_tid_fn = tid_fn;
  return;
}

static void elg_print_msg(const char* fmt, va_list az)
{
  char buffer[ELG_MSG_SIZE];
  int  maxLen;
  int  msgLen;

  /* Write message prefix to buffer */
  strcpy(buffer,"[");
  if (elg_pid != -1)
    sprintf(buffer + strlen(buffer), "%05d", elg_pid);
  if (elg_tid_fn)
    sprintf(buffer + strlen(buffer), ".%d", elg_tid_fn());
  sprintf(buffer + strlen(buffer), "]%s: ", ELG_MSG_PFIX);

  /* Write message to buffer and truncate it if necessary */
  maxLen = ELG_MSG_SIZE - strlen(buffer) - 3;
  msgLen = vsnprintf(buffer + strlen(buffer), maxLen, fmt, az);
  if (msgLen < 0 || msgLen >= maxLen)
    sprintf(buffer + strlen(buffer), "...");

  /* Print message */
  fflush(stdout);
  fprintf(stderr, "%s\n", buffer);
  fflush(stderr);
}

void elg_error_impl(const char* f, int l)
{
  elg_error_msg("[%s:%d]: %s", f, l, strerror(errno));
}

void elg_error_msg(const char* fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  elg_print_msg(fmt, ap);
  exit(EXIT_FAILURE);
}

void elg_warning(const char* fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  elg_print_msg(fmt, ap);
  return;
}

void elg_cntl_msg(const char* fmt, ...)
{
  va_list ap;

//if (epk_str2bool(epk_get(EPK_VERBOSE)))
  if (elg_verbose)
    {
      va_start(ap, fmt);
      elg_print_msg(fmt, ap);
      return;
    }
}

void elg_verbosity(int val)
{
  elg_verbose = val;
}

/* determine whether specification matches given rank */
int elg_rank_match(const int rank, const char* spec)
{
    char* p;
    if (*spec == '\0') return 1; /* unqualified */
    if (*spec != ':') return 0; /* not a qualifier */
    p = (char*)spec+1;
    while (*p && (*p != '+')) { /* end */
        char *rest;
        int num=strtol(p, &rest, 10);
        if (p==rest) return 0; else p=rest;
        if (num==rank) return 1;
        if (*p == '-') { /* range */
            int upto=strtol(++p, &p, 10);
            if (p==rest) return 0; else p=rest;
            if ((num <= rank) && (rank <= upto)) return 1;
        }
        if (*p == ',') p++; /* list */
    }
    return 0;
}
