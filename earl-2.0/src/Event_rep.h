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

#ifndef EARL_EVENT_REP_H
#define EARL_EVENT_REP_H

/*
 *----------------------------------------------------------------------------
 *
 * class Event_rep
 *
 * This class represents the actual EARL events. Its an abstract class
 * containing the base attributes and defining a common interface to
 * events.
 *
 *----------------------------------------------------------------------------
 */

#include <string>

#include "Error.h"
#include "Event.h"
#include "EventBuffer.h"

namespace earl 
{
  class Callsite;
  class Communicator;
  class EventBuffer;
  class Location;
  class Region;
  class State;
  class MPIWindow;

  class Event_rep
  {
  public:
  
    Event_rep(State&        state,
	      EventBuffer*  buffer,
	      Location*     loc, 
	      double        time);
  
    virtual ~Event_rep() {} 
  
    long add_ref();    
    long rem_ref(); 
  
    long         get_pos()      const { return pos; }
    Location*    get_loc()      const { return loc; }
    double       get_time()     const { return time; }
    Event        get_enterptr() const { return enterptr; }

    virtual bool           is_type(etype type) const; 

    virtual etype          get_type()         const { throw RuntimeError("No such type."); }
    virtual std::string    get_typestr()      const { throw RuntimeError("No such type string."); }
    virtual Event          get_cnodeptr()     const { return NULL; }
    virtual Event          get_cedgeptr()     const { return NULL; }
    virtual Region*        get_reg()          const { return NULL; }
    virtual Callsite*      get_csite()        const { return NULL; }
    virtual Event          get_sendptr()      const { return NULL; }
    virtual Event          get_lockptr()      const { return NULL; }
    virtual Event          get_forkptr()      const { return NULL; }
    virtual Communicator*  get_com()          const { return NULL; }
    virtual long           get_tag()          const { return -1; }
    virtual long           get_length()       const { return -1; }
    virtual Location*      get_root()         const { return NULL; }
    virtual long           get_sent()         const { return -1; }
    virtual long           get_recvd()        const { return -1; }
    virtual long           get_lock_id()      const { return -1; }
    virtual long           get_nmets()         const { return 0; }
    virtual long long      get_metval(long i)  const { throw RuntimeError("No such metric."); }
    virtual std::string    get_metname(long i) const { throw RuntimeError("No such metric."); }
    virtual MPIWindow*     get_win()           const { return NULL; /*throw RuntimeError("No such RMA-window.");*/}
    virtual void           set_win(MPIWindow*)       { throw RuntimeError("No MPIWindow."); }
    virtual Location*      get_origin()        const { throw RuntimeError("No origin location pointer in event: " + get_typestr()); }
    virtual Location*      get_target()        const { throw RuntimeError("No target location pointer in event: " + get_typestr()); }
    virtual Location*      get_src()           const { return NULL;/*(be compatible) throw RuntimeError("No source location pointer in event: " + get_typestr());*/ }
    virtual Location*      get_dest()          const { return NULL;/*(be compatible) throw RuntimeError("No destination loaction pointer in event: " + get_typestr());*/ }
    virtual Event          get_TO_event()      const { throw RuntimeError("No tranfer origin event pointer in event: " + get_typestr()); }
    virtual long           get_rmaid()         const { throw RuntimeError("No rma id."); }
    virtual long           get_synex()         const { throw RuntimeError("No synchronization flag."); }
    virtual Event          get_transfer_start()const { throw RuntimeError("No transfer start poiner."); }
    //virtual void           set_origin(Event e) const { throw RuntimeError("No origin pointer. "); }

    // should take S(pos-1) as argument
    virtual void trans(State& state);

    void detach_buffer() { buffer = NULL; }

  private:

    long         pos;
    Location*    loc;
    double       time;
  
    Event  enterptr;
  
    EventBuffer* buffer;
    long         refc;  
  };

#ifndef NO_INLINE
#  include "Event_rep.inl"
#endif
}
#endif






