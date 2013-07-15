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

#ifndef EARL_RMACOMM_REP_H
#define EARL_RMACOMM_REP_H


#include "Event_rep.h"

namespace earl
{
  class RmaComm_rep : public Event_rep 
  {
  public:

    RmaComm_rep(State&        state,
                EventBuffer*  buffer,
                Location*     loc, 
                double        time,
                long          rmaid) :
      Event_rep(state, buffer, loc, time), rmaid(rmaid)
    {}

    virtual bool is_type(etype type) const
      { return Event_rep::is_type(type) || type == RMACOMM; }
    virtual etype       get_type()    const { return RMACOMM; }
    virtual std::string get_typestr() const { return "RMACOMM"; }

    long  get_rmaid() const { return rmaid; }

    virtual void trans(State& state);

  protected:
    long rmaid;
  };
}
#endif






