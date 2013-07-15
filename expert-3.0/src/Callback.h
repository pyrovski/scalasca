/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EXP_CALLBACK_H
#define EXP_CALLBACK_H

/*
 *----------------------------------------------------------------------------
 *
 * class Callback
 *
 * Defines interface with callback methods to be called upon the
 * detection of a particular event in an event trace.
 *
 *---------------------------------------------------------------------------- 
 */

#include <earl.h>


class Callback
{
 public:

  virtual struct cmp_event* ompalock(earl::Event& event)         { return NULL; }
  virtual struct cmp_event* collexit(earl::Event& event)      { return NULL; }
  virtual struct cmp_event* enter(earl::Event& event)         { return NULL; }
  virtual struct cmp_event* exit(earl::Event& event)          { return NULL; }
  virtual struct cmp_event* fork(earl::Event& event)          { return NULL; }
  virtual struct cmp_event* join(earl::Event& event)          { return NULL; }
  virtual struct cmp_event* mpicexit(earl::Event& event)      { return NULL; }
  virtual struct cmp_event* ompcexit(earl::Event& event)      { return NULL; }
  virtual struct cmp_event* recv(earl::Event& event)          { return NULL; }
  virtual struct cmp_event* omprlock(earl::Event& event)         { return NULL; }
  virtual struct cmp_event* rmawcexit(earl::Event& event)     { return NULL; }
  virtual struct cmp_event* rmawexit(earl::Event& event)      { return NULL; }
  virtual struct cmp_event* send(earl::Event& event)          { return NULL; }

  virtual struct cmp_event* cmpnd(const struct cmp_event* ce) { return NULL; }
  virtual ~Callback() {}
};


#endif






