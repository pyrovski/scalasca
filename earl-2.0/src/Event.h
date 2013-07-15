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

#ifndef EARL_EVENT_H
#define EARL_EVENT_H

/*
 *----------------------------------------------------------------------------
 *
 * class Event
 *
 * A container of objects of class Event_rep with memory management
 * functionality
 *
 *----------------------------------------------------------------------------
 */

#include <string>

namespace earl
{
  class Callsite;
  class Communicator;
  class Event_rep;
  class Location;
  class Metrics;
  class Region;
  class State;
  class MPIWindow;

  enum etype { ALOCK, OMPALOCK, ENTER, EVENT, EXIT, FORK, FLOW, JOIN, MPICEXIT, 
	       OMPCEXIT, P2P, RECV, OMPRLOCK, RLOCK, SEND, SYNC, OMPSYNC, TEAM,
               MPIWCEXIT, MPIWEXIT, MPIRMA, MPIRMACOMM, MPIRMASYNC, MPIWLOCK, MPIWUNLOCK,
               MPIGET1TS, MPIGET1TO, MPIGET1TE, MPIPUT1TS, MPIPUT1TE,
               RMACOMM, GET1TS, GET1TE, PUT1TS, PUT1TE, COLLEXIT
  };

  class Event
  {
  public:
  
    Event();           
    Event(Event_rep* event);           
    Event(const Event& rhs);
 
    ~Event();

    Event&         operator=(const Event& rhs);
    bool           null() const; 
    Event_rep*     operator*() const;
    const Event_rep* operator->() const;
    Event_rep*       operator->();
  
    bool           is_type(etype type) const;

    long           get_pos() const;
    Location*      get_loc() const;
    double         get_time() const;
    Event          get_enterptr() const;
    etype          get_type() const;
    std::string    get_typestr() const;
    Event          get_cnodeptr() const;
    Event          get_cedgeptr() const;
    Region*        get_reg() const;
    Callsite*      get_csite() const;
    Event          get_sendptr() const;
    Event          get_lockptr() const;
    Event          get_forkptr() const;
    Location*      get_src() const;
    Location*      get_dest() const;
    Communicator*  get_com() const;
    long           get_tag() const;
    long           get_length() const;
    Location*      get_root() const;
    long           get_sent() const;
    long           get_recvd() const;
    long           get_lock_id() const;

    long           get_nmets()         const;
    long long      get_metval(long i)  const;
    std::string    get_metname(long i) const; 

    MPIWindow*     get_win() const;

    // should take S(pos-1) as argument
    void trans(State& state); 

  private:

    Event_rep* event;
  };

  bool operator==(const earl::Event& lhs, const earl::Event& rhs);
  bool operator!=(const earl::Event& lhs, const earl::Event& rhs);
  bool operator<(const earl::Event& lhs, const earl::Event& rhs);
  bool operator>(const earl::Event& lhs, const earl::Event& rhs);
  bool operator<=(const earl::Event& lhs, const earl::Event& rhs);
  bool operator>=(const earl::Event& lhs, const earl::Event& rhs);
}

#endif






