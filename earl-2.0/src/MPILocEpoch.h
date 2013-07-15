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

#ifndef EARL_MPI_LOCEPOCH_H
#define EARL_MPI_LOCEPOCH_H

#include <list>
#include <vector>
//#include <iostream>

#include "Event_rep.h"
#include "CountedPtr.h"

namespace earl
{
  class Communicator;
  class Location;
  class MPIWindow;
  
  
  class MPILocEpoch
  {
  public: 
    MPILocEpoch ();
    MPILocEpoch (Event event);
    MPILocEpoch (const MPILocEpoch& epoch);
    //CountedPtr<MPILocEpoch> clone () const;
    virtual ~MPILocEpoch(){};
    const MPILocEpoch& operator= (const MPILocEpoch& epoch);


   
    bool is_access_epoch() const;
    bool is_exposure_epoch() const;
    bool is_lock_epoch() const;

    // check if epoch contains some evnets
    bool is_empty() const;

    // get all epoch events
    void get_all_events(std::vector<Event>& out ) const;

    // tests if epoch is complete
    bool is_complete() const;

    // communication partners of this local epoch
    const Communicator* get_partners() const {return partners;}

    // location of an epoch
    const Location*     get_loc() const {return loc;}

    // RMA window ralted to this epoch
    const MPIWindow*    get_win() const {return win;}
    
    // get first transfer operation ocurred in this epoch    
    Event get_first_transfer() const { return first_transfer;}; 
    
    // get lastr transfer operation occured in this epoch    
    Event get_last_transfer () const { return last_transfer; }; 
    
    Event get_mostrcnt(const earl::etype type, const earl::Region* reg=NULL) const;

    // event which starts the local epoch
    Event get_epoch_begin() const;
    
    // event which ends the local epoch
    Event get_epoch_end()   const;
   
    // find exit from region which contain this time
    Event get_reg_exit(double time) const;
    
    // push event into epoch 
    void push_back(Event event);
   
    
    //std::ostream& dump(std::ostream& out) const;
    
  private:
    typedef std::list<Event> events_t;
    
    events_t events;
    Event    first_transfer;
    Event    last_transfer;
    Communicator* partners;
    Location* loc;
    MPIWindow* win;
    
    static std::string lower(std::string str);
  };
}
#endif

