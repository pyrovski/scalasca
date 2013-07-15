/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Cartesian.h"
#include "Error.h"
#include <iostream>

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Cartesian
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets data to the given values.
 *
 * @param id            Topology identifier
 * @param num_locations Contains the number of locations in each dimension
 * @param is_periodic   Contains boolean values defining whether the
 *                      corresponding dimension is periodic or not
 */
Cartesian::Cartesian(ident_t            id,
                     const cart_dims&   num_locations,
                     const cart_period& is_periodic,
                     const std::string cart_name,
                     const std::vector<std::string> dim_names)
  : Topology(id),
    m_num_locations(num_locations),
    m_is_periodic(is_periodic),
    m_cart_name(cart_name),
    m_dim_names(dim_names)
{

}


//--- Get & set cartesian topology information ------------------------------

/**
 * Returns the number of dimensions of the cartesian grid.
 *
 * @return Number of dimensions
 */
uint32_t Cartesian::num_dimensions() const
{
  return m_num_locations.size();
}


/**
 * Stores the number of locations in each dimension in the given vector
 * @a outv.    
 *
 * @param[out] outv Number of locations per dimension
 */
void Cartesian::get_dimensionv(cart_dims& outv) const
{
  outv = m_num_locations;
}


/**
 * Stores the periodicity of each dimension in the given vector @a outv.
 *
 * @param[out] outv Periodicity per dimension
 */
void Cartesian::get_periodv(cart_period& outv) const
{
  outv = m_is_periodic;
}


/**
 * Stores the coordinate of the given @a location in the vector @a coords.
 *
 * @param      location %Location in the system hierarchy
 * @param[out] coords   Coordinate of the given location
 *
 * @exception RuntimeError if the given @a location is not part of this
 *                         topology
 */
void Cartesian::get_coords(Location* location, cart_coords& coords) const
{
  CoordinateMap::const_iterator it = m_coords.find(location);
  if (it == m_coords.end())
    throw RuntimeError("Cartesian::get_coords(Location*,cart_coords&) -- "
                       "Coordinate not found.");

  coords = it->second;
}

/**
 * Returns the topology name or "" if none
 *
 * @param[out] name a string pointer with the name of this topology.
 *
 */
void Cartesian::get_name(std::string& name) const
{
  name=m_cart_name;
}


/**
 * Returns the dimensions' names vector or an empty vector if dimensions are not named.
 *
 * @param[out] name a string pointer with the name of this topology.
 *
 */
void Cartesian::get_dim_names(cart_dim_names& out_dim_names) const
{
  out_dim_names=m_dim_names;
}


/**
 * Sets the topology name
 *
 * @param[in] name a string with the topology's name
 *
 */
void Cartesian::set_name(std::string name)
{
  m_cart_name=name;
}

/**
 * Sets the topology dimensions
 *
 * @param[in] in_dim_names vector of strings containing the dimension names. Should check for the right number of dimensions.
 *
 */
void Cartesian::set_dim_names(cart_dim_names in_dim_names)
{
  m_dim_names=in_dim_names;
  // remember to remove include
  for(uint32_t i=0; i< num_dimensions();i++ ) {
  cout << "Calling set_dim_names with " << in_dim_names[i] << endl;
  }
}





//--- Private methods -------------------------------------------------------

/**
 * Assigns the coordinate @a coords to the given @a location.
 *
 * @param location %Location in the system hierarchy
 * @param coords   Coordinate of the given location
 *
 * @exception RuntimeError if one of the following conditions is met:
 *                           - The number of dimensions of the coordinate
 *                             and the cartesian topology do not match.
 *                           - The coordinate is out of bounds.
 *                           - The location coordinate is already defined.
 */
void Cartesian::set_coords(Location* location, const cart_coords& coords)
{
  size_t count = coords.size();

  if (count != m_num_locations.size())
    throw RuntimeError("Cartesian::set_coords(Location*,const cart_coords&) -- "
                       "Dimensions do not match.");

  for (size_t i = 0; i < count; ++i)
    if (coords[i] >= m_num_locations[i])
      throw RuntimeError("Cartesian::set_coords(Location*,const cart_coords&) -- "
                         "Coordinate out of bounds");

  CoordinateMap::iterator it = m_coords.find(location);
  if (it != m_coords.end())
    throw RuntimeError("Cartesian::set_coords(Location*,const cart_coords&) -- "
                       "Location coordinate already defined.");

  m_coords.insert(CoordinateMap::value_type(location, coords));
}
