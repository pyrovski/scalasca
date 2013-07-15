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

#ifndef EARL_SEND_REP_H
#define EARL_SEND_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Send_rep
 *
 * Event of sending a message.
 *
 *----------------------------------------------------------------------------
 */

#include "P2P_rep.h"

namespace earl
{
  class Send_rep : public P2P_rep
  {
  public:
  
    Send_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time,
	     Communicator* com,
	     long          tag, 
	     long          length,       
	     Location*     dest) : 
      P2P_rep(state, buffer, loc, time, com, tag, length), 
      dest(dest) {}
  
    Location*  get_dest() const { return dest; }
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return SEND; }
    virtual std::string get_typestr() const { return "SEND"; }
  
    // should take S(pos-1) as argument
    virtual void trans(State& state);
  
  private:
  
    Location* dest;
  };
}
#endif





