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

#include "Cartesian.h"

#include "Error.h"
#include "Grid.h"
#include "Location.h"
#include "Communicator.h"

using namespace earl;
using namespace std;

void Cartesian::get_dimv(std::vector<long>& out) const
{
  for( size_t i = 0 ; i < dimv.size() ; i++ )
    {
      out.push_back(dimv[i]);
    }
}

void Cartesian::get_periodv(std::vector<bool>& out) const
{
  for( size_t i = 0 ; i < periodv.size() ; i++ )
    {
      out.push_back(periodv[i]);
    }
}

void Cartesian::get_coords(std::vector<long>& out, Location* loc) const
{
  std::map<Location*, vector<long> >::const_iterator it = loc2coords.find(loc);

  if (it == loc2coords.end())
    {
      // throw RuntimeError("Error in Cartesian::get_coords(Location*).");
      // the location which is not mapped to any coords is not a part
      // of this topology
      // out remains empty for such locations
    }
  else
    {
      for ( size_t i = 0 ; i < it->second.size() ; i++ )
	    {
	      out.push_back(it->second[i]);
	    }
    }
}

Location* 
Cartesian::get_loc(std::vector<long>& coordv) const
{
  std::map<Location*, vector<long> >::const_iterator it;
  
  for ( it = loc2coords.begin() ; it != loc2coords.end() ; it++ ) 
    {
      if ( it->second == coordv ) 
	return it->first;
    }

  return NULL;
}

void
Cartesian::set_coords(Location* loc, std::vector<long> coordv)
{
  this->loc2coords[loc] = coordv;
}


