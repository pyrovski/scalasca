/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "RecordBuffer.h"
#include <queue>

RecordBuffer::RecordBuffer(earl::EventTrace* trace) : _trace(trace), _loc_vec( trace->get_nthrds() ) {
}
RecordBuffer::~RecordBuffer(){ }

// add event-record to vector _loc_vec
void RecordBuffer::AddEvent(long location, EventRecord* newElem){
  LocationBuffer& loc_vec = _loc_vec[location];
  LocationBuffer::elist_t& elist = loc_vec._EventList;

  if ( elist.empty() ){
   loc_vec._LastInsert = elist.insert(elist.begin(), newElem);
  }else{//list is not empty, find insertion point
    if( *(*(loc_vec._LastInsert)) > *newElem ){//CASE 1: timestamp(list) > timestamp(new)
      while ( (loc_vec._LastInsert != elist.begin() ) && 
              *(*(loc_vec._LastInsert)) > *newElem ){
        --loc_vec._LastInsert;
      }
      if( *(*(loc_vec._LastInsert)) <= *newElem ){
        ++loc_vec._LastInsert;
      }
    }else{//CASE 2: timestamp(list) <= timestamp(new)
      while( (loc_vec._LastInsert != elist.end() ) && 
              ( *(*(loc_vec._LastInsert))  <= *newElem ) ){
        ++loc_vec._LastInsert;
      }
    } 
    loc_vec._LastInsert = elist.insert(loc_vec._LastInsert, newElem); 
  }
}


void RecordBuffer::PrependEvent(long location, EventRecord* newElem){
  LocationBuffer& loc_vec = _loc_vec[location];
  LocationBuffer::elist_t& elist = loc_vec._EventList;

  if ( elist.empty() ){
    loc_vec._LastInsert = elist.insert(elist.begin(), newElem);
  }else{
    loc_vec._LastInsert = elist.insert(loc_vec._LastInsert, newElem); 
  }
}




// write all records from _loc_vec to trace file _p_trace
void RecordBuffer::WriteAll(ElgOut* _p_trace, double limit){

  std::priority_queue<OldestEvent> EvQueue;
  
  // Collect oldest events from each location in queue
  for(size_t location=0; location < _loc_vec.size(); ++location){
    if (! _loc_vec[location]._EventList.empty() ){
      EvQueue.push(OldestEvent(_loc_vec[location]._EventList.begin(), location) );
    }
  }

  while(! EvQueue.empty() ){
    OldestEvent Oev = EvQueue.top();
    LocationBuffer::elist_t::iterator s_it = Oev.GetIt() ;
    if ( (*s_it)->GetTime() >= limit ) break;
    long s_loc = Oev.GetLocation(); 
    LocationBuffer& loc_vec = _loc_vec[s_loc];
    // Write oldest event
    (*s_it)->Write(_p_trace, s_loc);  
    // Delete oldest event from Record Buffer
    loc_vec._EventList.erase(s_it);
    delete *s_it;
    // Adjust last insert if necessary
    if ( loc_vec._LastInsert == s_it ) loc_vec._LastInsert= loc_vec._EventList.end();
    // Remove oldest event from queue
    EvQueue.pop();
    // Add next oldest event from same location to queue 
    if( ! loc_vec._EventList.empty() ){
      Oev.SetIt( loc_vec._EventList.begin() );
      EvQueue.push(Oev);
    }
  }
}
    

// write records from _loc_vec to trace file _p_trace as far as possible
void RecordBuffer::WriteAFAP(ElgOut* _p_trace){
  
  double limit=100;
  //find event with timestamp limit , where no event with a timestamp < limit will be created
  
  // write 
  WriteAll(_p_trace, limit);
  cout << "Write ASAP till here" << endl;
}

// Push Enter to OPENENTER list in RecordBuffer
void RecordBuffer::PushOpenEnter(long position, long location){ 
  _loc_vec[location]._OpenEnters.insert(position);
}

// Search for OPENENTER and delete if sucessfully
bool RecordBuffer::FindDeleteOpenEnter(long position, long location){
  std::set<long>::iterator oit = _loc_vec[location]._OpenEnters.find(position);
  if(oit == _loc_vec[location]._OpenEnters.end() ){
    return false;
  }else{
    _loc_vec[location]._OpenEnters.erase(oit);
    return true;
  }
} 
