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

#ifndef EARL_COLLEXIT_REP_H
#define EARL_COLLEXIT_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class CollExit_rep
 *
 * Event of leaving an MPI collective operation.
 *
 *----------------------------------------------------------------------------
 */

#include <string>
#include <vector>

#include "Exit_rep.h"

namespace earl
{
  class CollExit_rep : public Exit_rep
  {
  public:
    
    CollExit_rep(State& state,
		 EventBuffer*  buffer,
		 Location*     loc, 
		 double        time,
		 std::map<long, Metric*>&  metm,
		 std::vector<elg_ui8>& metv,
		 Location*     root, 
		 Communicator* com,
		 long          sent,
		 long          recvd) :
      Exit_rep(state, buffer, loc, time, metm, metv),
      com(com),
      root(root),
      sent(sent),
      recvd(recvd) {}
  
    virtual bool        is_type(etype type) const;
    virtual etype       get_type()    const { return COLLEXIT; }
    virtual std::string get_typestr() const { return "COLLEXIT"; }
  
    Communicator*  get_com()      const { return com; }
    Location*      get_root()     const { return root; }
    long           get_sent()     const { return sent; }
    long           get_recvd()    const { return recvd; }
  
    virtual void trans(State& state);
  
  private:
  
    Communicator* com;
    Location*     root;
    long sent;
    long recvd;
  };  
}
#endif








