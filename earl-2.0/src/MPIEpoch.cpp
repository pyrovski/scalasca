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

#include "MPIEpoch.h"


#include "Error.h"
#include "Region.h"
#include "State.h"
#include "MPIWindow.h"


using namespace earl;
using namespace std;

CountedPtr<MPILocEpoch>
MPIEpoch::get_exp_epoch() const
{
  return exp_epoch;
}

void
MPIEpoch::get_acc_epoches(std::vector<CountedPtr<MPILocEpoch> >& out ) const
{
    out.clear();
    out.reserve(acc_epoches.size());
    
    std::map<long, CountedPtr<MPILocEpoch> >::const_iterator it;
    for ( it=acc_epoches.begin(); it!=acc_epoches.end(); it++ )
      out.push_back(it->second);
}
    
bool
MPIEpoch::is_complete() const
{
  // check if exposure epoche i present
  if ( ! exp_epoch)
    return false;

  // check if all coresponding access epoches are present
  const Communicator* com = exp_epoch->get_partners();
  bool acc_epoches_complete = size_t(com->get_nprocs()) == acc_epoches.size();

  return acc_epoches_complete;

}

bool
MPIEpoch::is_empty() const
{
  if ( ! exp_epoch && acc_epoches.empty() )
    return true;

  return false;
}

bool 
MPIEpoch::match(CountedPtr<MPILocEpoch>& ep ) const
{
  if (!ep)
    throw RuntimeError("MPIEpoch::match() invalid input");
  if (!ep->is_exposure_epoch() && !ep->is_access_epoch() )
      throw RuntimeError("MPIEpoch::match(): unknown epoch");
      
  // if exposure epoch already present  
  if ( ep->is_exposure_epoch() && !!exp_epoch)
      return false;
  
  if (ep->is_access_epoch() ) 
    {
      long loc_id = ep->get_loc()->get_id();
      // if access epoch already present  
      if ( acc_epoches.find(loc_id) != acc_epoches.end() )
        return false;
    } 
  
  return true;
}

void
MPIEpoch::insert( CountedPtr<MPILocEpoch>& ep)
{
  if ( !ep )
    throw RuntimeError("MPIEpoch::insert(): empty input epoch");
  if ( !ep->is_complete() ) 
    throw RuntimeError("MPIEpoch::insert(): incomplete input epoch");
    
  if ( !!ep->is_exposure_epoch() ) 
    {
      if ( !!exp_epoch )
        throw RuntimeError("MPIEpoch::insert(): expouse epoch duplicate");
      
      exp_epoch = ep; 
      return; 
    }
  else if (!!ep->is_access_epoch() ) 
    {
      long loc_id = ep->get_loc()->get_id();
      if ( acc_epoches.find(loc_id) != acc_epoches.end() )
        throw RuntimeError("MPIEpoch::insert(): access epoch duplicate");
      
      acc_epoches[loc_id] = ep;
    }
  else
    {
      throw RuntimeError("MPIEpoch::insert(): unknown epoch");
    }
}

const Location*  
MPIEpoch::get_loc() const
{ 
    return !!exp_epoch ? exp_epoch->get_loc() : NULL; 
}
const MPIWindow* 
MPIEpoch::get_win() const
{ 
    return !!exp_epoch ? exp_epoch->get_win() : NULL; 
}


/*
std::ostream& 
MPIEpoch::dump( ostream& out ) const
{
  return out;
}
*/
