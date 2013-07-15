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

#ifndef EARL_MPI_EPOCH_H
#define EARL_MPI_EPOCH_H

#include <list>
#include <vector>
//#include <iostream>

#include "Event_rep.h"

#include "CountedPtr.h"

#include "Location.h"

namespace earl
{
  class MPILocEpoch;
  class Location;
  class MPIWindow;
  
  class MPIEpoch
  {
  public: 
    //MPIEpoch (){};
    //virtual ~MPIEpoch(){};
    //MPIEpoch (CountedPtr<MPILocEpoch>& loc_epoch_ptr){};
    //MPIEpoch (const MPIEpoch& epoch){};
    //MPIEpoch& operator= (const MPIEpoch& epoch);

    void insert( CountedPtr<MPILocEpoch>& loc_epoch);

    bool is_complete() const;
    bool is_empty() const;
    bool match(CountedPtr<MPILocEpoch>& loc_ep ) const;
      
    const Location*  get_loc() const;
    const MPIWindow* get_win() const;
    
    //std::ostream& dump( std::ostream& out ) const;

    CountedPtr<MPILocEpoch> get_exp_epoch() const;
    void get_acc_epoches(std::vector<CountedPtr<MPILocEpoch> >& out ) const;

  private:
      
    CountedPtr<MPILocEpoch> exp_epoch;
    std::map<long, CountedPtr<MPILocEpoch> > acc_epoches;
    
  };
}
#endif

