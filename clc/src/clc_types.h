/****************************************************************************
 **  SCALASCA    http://www.scalasca.org                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef CLC_TYPES_H
#define CLC_TYPES_H


/// Enumeration defining available event types
enum clc_event_t {

  CLC_P2P,
  CLC_COLL,
  CLC_OMP,
  CLC_NUM_EVENT_TYPE

};

struct clc_location { 

  int machine;
  int node;
  int process;
  int thread;

};

enum stat_t {

  STAT_REL,
  STAT_ABS,
  CLC_STAT_TYPE

};

#endif   /* !CLC_TYPES_H */
