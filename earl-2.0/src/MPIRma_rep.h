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

#ifndef EARL_MPI_RMA_REP_H
#define EARL_MPI_RMA_REP_H


#include "Event_rep.h"

namespace earl
{
  class MPIWindow;

  class MPIRma_rep : public Event_rep 
  {
  public:

    MPIRma_rep(State&        state,
               EventBuffer*  buffer,
               Location*     loc, 
               double        time,
               MPIWindow*    win) :
      Event_rep(state, buffer, loc, time), win(win)
    {}

    virtual bool is_type(etype type) const
      { return Event_rep::is_type(type) || type == MPIRMA; }
    virtual etype       get_type()    const { return MPIRMA; }
    virtual std::string get_typestr() const { return "MPIRMA"; }

    MPIWindow*  get_win() const { return win; }

    virtual void trans(State& state);

  protected:
    MPIWindow* win;
  };
}
#endif






