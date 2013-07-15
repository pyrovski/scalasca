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

#ifndef EARL_GET1TS_REP_H
#define EARL_GET1TS_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Get1TS_rep
 *
 * transfer start of an 'GET' operation. 
 * Occurs only on target location.
 * Corresponding Get1TE_rep has the same 'rmaid' value
 *
 *---------------------------------------------------------------------------- 
 */

#include "State.h"
#include "RmaComm_rep.h"

namespace earl
{
  class Get1TS_rep : public RmaComm_rep
  {
  public:
  
    Get1TS_rep(State&        state,
               EventBuffer*  buffer,
               Location*     loc, 
               double        time,
               Location*     dst,
               long          rmaid,
               long          length ) :
    RmaComm_rep(state, buffer, loc, time, rmaid), 
    dst(dst), length(length)
    {}
    
    // accessor functions
    Location* get_dest()   const { return dst;  }
    Location* get_src()    const { return get_loc();  }
    Location* get_origin() const { return dst;  }
    Location* get_target() const { return get_loc();  }
    long      get_length() const       { return length; }
    
    virtual bool is_type(etype type) const
      { return RmaComm_rep::is_type(type) || type == GET1TS; }
    virtual etype       get_type()    const { return GET1TS; }
    virtual std::string get_typestr() const { return "GET1TS"; }

    virtual void trans(State& state);
  
  private:
    // destination of data transfer
    Location* dst;
    long      length;
  };
}
#endif






