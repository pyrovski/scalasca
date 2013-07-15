/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/**
* \file Cartesian.h
* \brief Provides a cartesian topology of the application.
*
* \details Topology means "(x,y,z) -> threas" in 3D case.
*/
#ifndef CUBE_CARTESIAN_H
#define CUBE_CARTESIAN_H


#include <iosfwd>


/*
 *----------------------------------------------------------------------------
 *
 * class Cartesian
 *
 *----------------------------------------------------------------------------
 */

#include <vector>
#include <map>
#include <string>

namespace cube {

  class Sysres;
  class Thread;
/**
* Defines and provides cartesian ( x, y, z )-> thread topology
*/
  class Cartesian {

  public:

    Cartesian (long ndims, const std::vector<long>& dimv, const std::vector<bool>& periodv)
      : ndims(ndims), dimv(dimv), periodv(periodv) {name = ""; }

    void def_coords(Sysres* sys, const std::vector<long>& coordv);

    void writeXML  (std::ostream& out) const;

    const std::vector<std::string>& get_namedims() const { return namedims; } // Returns the vector with the names of each dimension.
    long                     get_ndims()   const { return ndims; } ///< Returns a number of dimensions of the topology.
    const std::vector<long>& get_dimv()    const { return dimv; } ///< Returns a vector of number of indexes in every dimension.
    const std::vector<bool>& get_periodv() const { return periodv; } ///< Returns a vector of periodicities in every dimension.
    const std::vector<long>& get_coordv(Sysres* sys) const;
    
    const std::vector<std::vector<long> >  get_all_coordv( Sysres* sys ) const; ///< Returns all coordinates belong to the given sysres
    
    const std::multimap<const Sysres*, std::vector<long> >&  get_cart_sys() const { return sys2coordv; } ///< Gets a mapping "resource -> coordinate"

    Cartesian* clone(const std::vector<Thread*>&);      /// creates a deep copy of himself. (using gibven Threads for cloned copy)


    void  set_name (const std::string&  ); ///<  Sets a name of the topology
    const std::string& get_name() { return name; };

    bool set_namedims(const std::vector<std::string>); // Sets the name of the dimensions inside a topology
    const std::vector<std::string>& get_namedims() { return namedims; };


    bool operator== (const Cartesian& b);
    inline bool operator!= (const Cartesian& b) { return !(*this == b);};

  private:
    std::string name;     ///< Topology name
    std::vector<std::string> namedims; // Vector of topology names.
    unsigned long ndims; ///< A number of dimensions in the topology.
    std::vector<long> dimv; ///< Number of indicies in  everydimension
    std::vector<bool> periodv; ///< Periodicity in  everydimension
    std::multimap<const Sysres*, std::vector<long> > sys2coordv; ///< Mapping "resource -> coordinate".




  };


}

#endif
