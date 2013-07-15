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

#ifndef EARL_PUT1TE_REP_H
#define EARL_PUT1TE_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Put1TE_rep
 *
 * transfer end of an 'PUT' operation. 
 * Occurs only on target location. 
 * Corresponding Put1TS has the same 'rmaid' value
 *
 *---------------------------------------------------------------------------- 
 */

#include "RmaComm_rep.h"


namespace earl
{
  class Put1TE_rep : public RmaComm_rep 
  {
  public:
  
    Put1TE_rep(State&        state,
               EventBuffer*  buffer,
               Location*     loc, 
               double        time,
               Location*     src,
               long          rmaid);
    
    // accessor functions
    Location* get_dest()       const { return get_loc(); }
    Location* get_src()        const { return src;       }
    Location* get_origin()     const { return src;       }
    Location* get_target()     const { return get_loc(); }
    Event get_transfer_start() const { return put1TSptr; }
    long get_length()          const { return put1TSptr->get_length(); }
    
    virtual bool is_type(etype type) const
           { return RmaComm_rep::is_type(type) || type == PUT1TE; }
    virtual etype       get_type()    const { return PUT1TE; }
    virtual std::string get_typestr() const { return "RMAPUT1TE"; }

    virtual void trans(State& state);
  
  private:
    Location* src;
    Event     put1TSptr;
  };
}
#endif

