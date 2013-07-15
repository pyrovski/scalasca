/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EXP_RecordBuffer_H
#define EXP_RecordBuffer_H

#include <earl.h>
#include <elg_rw.h>
#include <list>
#include <vector>
#include <limits>
#include "EventRecord.h"

/*
 *----------------------------------------------------------------------------
 *
 * class RecordBuffer / class LocationBuffer
 *
 * Pattern Analysis process
 *
 *---------------------------------------------------------------------------- 
 */
class RecordBuffer;


class LocationBuffer{

public:
LocationBuffer(): _LastInsert(_EventList.end()) {}
friend class RecordBuffer;
typedef std::list<EventRecord*> elist_t;

private:  
  elist_t _EventList;
  elist_t::iterator _LastInsert;
  std::set <long> _OpenEnters;
};


class RecordBuffer{

public:

  RecordBuffer(earl::EventTrace* trace);

  ~RecordBuffer();

  //add event-record to vector _loc_vec
  void AddEvent(long location, EventRecord* newElem);
  //append event-record to vector _loc_vec at position _LastInsert
  void PrependEvent(long location, EventRecord* newElem);
  //write records from _loc_vec to trace file _p_trace as far as possible
  void WriteAFAP(ElgOut* _p_trace);
  //write all records from _loc_vec to trace file _p_trace
  void WriteAll(ElgOut* _p_trace, double limit=std::numeric_limits<double>::max());
  // Push Enter to OPENENTER list in RecordBuffer
  void PushOpenEnter(long position, long location);
  // Search for OPENENTER and delete if sucessfully
  bool FindDeleteOpenEnter(long position, long location);
  
private:
  earl::EventTrace * _trace;
  std::vector <LocationBuffer> _loc_vec;
  class OldestEvent{
  public:
    OldestEvent(const LocationBuffer::elist_t::iterator& it, long location ): _it(it), _location(location) {};
    bool operator<(const OldestEvent& rhs) const{
      return (*_it)->GetTime() > (*(rhs._it))->GetTime(); 
    }
    LocationBuffer::elist_t::iterator GetIt()const { return _it;}
    long GetLocation()const{ return _location;}
    
    void SetIt(const LocationBuffer::elist_t::iterator& it){ _it=it;}
  private:
    LocationBuffer::elist_t::iterator _it;
    long _location; 

  };

};



#endif

