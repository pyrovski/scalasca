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

#ifndef EARL_P2P_REP_H
#define EARL_P2P_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class P2P_rep
 *
 * Event of sending or receiving a point-to-point message.
 *
 *----------------------------------------------------------------------------
 */

#include "Event_rep.h"

namespace earl
{
  class P2P_rep : public Event_rep
  {
  public:
  
    P2P_rep(State&        state,
	    EventBuffer*  buffer,
	    Location*     loc, 
	    double        time,
	    Communicator* com,
	    long          tag, 
	    long          length) :
      Event_rep(state, buffer, loc, time), com(com), tag(tag), length(length) {}

    virtual bool   is_type(etype type) const;
  
    Communicator*  get_com()     const { return com; }
    long           get_tag()     const { return tag; }
    long           get_length()  const { return length; }
  
  private:
  
    Communicator* com;
    long tag;
    long length;
  };
}
#endif




