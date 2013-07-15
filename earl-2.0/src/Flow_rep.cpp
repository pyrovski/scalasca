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

#include "Flow_rep.h"

#include "Error.h"
#include "HWMetric.h"

using namespace earl;
using namespace std;

bool 
Flow_rep::is_type(etype type) const
{
  if ( Event_rep::is_type(type) || type == FLOW )
    return true;
  else
    return false;
}


long long  
Flow_rep::get_metval(long i) const 
{ 
  if ( 0 <= i && i < long(metv.size()) )
    return metv[i]; 
  else
    throw RuntimeError("Error in Flow_rep::get_metval(long).");     
}

string
Flow_rep::get_metname(long i) const 
{ 
  map<long, Metric*>::const_iterator it = metm.find(i);

  if (it == metm.end())
    throw RuntimeError("Error in Flow_rep::get_metname(long)."); 
  else
    return it->second->get_name(); 
}
