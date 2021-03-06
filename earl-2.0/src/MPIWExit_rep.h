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

#ifndef EARL_MPI_WEXIT_REP_H
#define EARL_MPI_WEXIT_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class RMAWExit_rep
 *
 * Event of leaving an MPI window related operation.
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <vector>

#include "Exit_rep.h"

namespace earl
{
  class MPIWindow;

  class MPIWExit_rep : public Exit_rep
  {
  public:

    MPIWExit_rep(State& state,
                 EventBuffer*  buffer,
                 Location*     loc, 
                 double        time,
                 std::map<long, Metric*>&  metm,
                 std::vector<elg_ui8>& metv,
                 MPIWindow*    win,
                 Communicator* com,
                 bool          synex
                 ) :
      Exit_rep(state, buffer, loc, time, metm, metv),
      win(win),
      com(com),
      synex(synex)
      {}

    virtual bool        is_type(etype type) const
      { return  Exit_rep::is_type(type) || type == MPIWEXIT; }
    virtual etype       get_type()          const { return MPIWEXIT;   }
    virtual std::string get_typestr()       const { return "MPIWEXIT"; }

    MPIWindow*     get_win()   const { return win;   }
    Communicator*  get_com()      const { return com;   }
    long           get_synex()    const { return synex; }

    virtual void trans(State& state);

  private:

    MPIWindow*    win;
    Communicator* com;
    long          synex;
  };  
}
#endif


