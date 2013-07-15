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

#ifndef EARL_CARTESIAN_H
#define EARL_CARTESIAN_H

/*
 *----------------------------------------------------------------------------
 *
 * class Cartesian
 *
 * Defines the Cartesian grid and assigns coordinates to each Location
 *
 *----------------------------------------------------------------------------
 */

#include <map>
#include <vector>

namespace earl
{
  class Location;
  class Communicator;

  class Cartesian
  {
  public:
    Cartesian(long               id,
	      Communicator*      com,
	      long               ndims, 
	     std::vector<long>  dimv,
	     std::vector<bool>  periodv):
      id(id), com(com), ndims(ndims), dimv(dimv), periodv(periodv) {}

    long           get_id()   const { return id; }
    Communicator*  get_com()   const { return com; }
    long           get_ndims()   const { return ndims; }
    void           get_dimv(std::vector<long> &out) const; 
    void           get_periodv(std::vector<bool> &out) const;  
    void           get_coords(std::vector<long> &out, Location* loc) const;
    Location*      get_loc(std::vector<long>& coordv) const;
    
  private: 
    friend class SeqTrace;
    void   set_coords(Location* loc, std::vector<long> coordv);

    long              id;
    Communicator*     com;
    long              ndims; 
    std::vector<long> dimv;
    std::vector<bool> periodv;
    std::map <Location*, std::vector<long> > loc2coords;    
  };
}
#endif
