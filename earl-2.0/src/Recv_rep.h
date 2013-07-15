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

#ifndef EARL_RECV_REP_H
#define EARL_RECV_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Recv_rep
 *
 * Event of receiving a message.
 *
 *----------------------------------------------------------------------------
 */

#include "P2P_rep.h"

namespace earl
{
  class Recv_rep : public P2P_rep
  {
  public:
  
    Recv_rep(State&        state,
	     EventBuffer*  buffer,
	     Location*     loc, 
	     double        time,
	     Communicator* com,
	     long          tag,
	     Location*     src);
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return RECV; }
    virtual std::string get_typestr() const { return "RECV"; }
  
    Location*  get_src()      const { return src; }
    Event      get_sendptr()  const { return sendptr; }
  
    virtual void trans(State& state);
  
  private:
  
    Location*  src;
    Event sendptr;
  };
}
#endif















