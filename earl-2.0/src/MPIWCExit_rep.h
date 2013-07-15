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

#ifndef EARL_MPI_WCEXIT_REP_H
#define EARL_MPI_WCEXIT_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class MPIWCExit_rep
 *
 * Event of leaving an MPI window collective operation.
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <vector>

#include "Exit_rep.h"
//#include "Window.h"

namespace earl
{
  class MPIWindow;

  class MPIWCExit_rep : public Exit_rep
  {
  public:

    MPIWCExit_rep(State& state,
                  EventBuffer*  buffer,
                  Location*     loc, 
                  double        time,
                  std::map<long, Metric*>&  metm,
                  std::vector<elg_ui8>&     metv,
                  MPIWindow*    win
                  ) :
      Exit_rep(state, buffer, loc, time, metm, metv),
      win(win)
      {}

    virtual bool        is_type(etype type) const
      { return Exit_rep::is_type(type) || type == MPIWCEXIT;}

    virtual etype       get_type()    const { return MPIWCEXIT; }
    virtual std::string get_typestr() const { return "MPIWCEXIT"; }

    MPIWindow* get_win() const { return win; }

    virtual void trans(State& state);

  private:
    MPIWindow* win; 
  };  
}
#endif








