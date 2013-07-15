/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include "NamedObject.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class NamedObject
 *
 *---------------------------------------------------------------------------
 */

//--- Get & set object name -------------------------------------------------

/**
 * Returns the object's name.
 *
 * @return Object name
 */
string NamedObject::get_name() const
{
  return m_name;
}


/**
 * Sets (i.e., overwrites) the object's name with the given parameter
 * @a name.
 *
 * @param name New object name
 */
void NamedObject::set_name(const string& name)
{
  m_name = name;
}


//--- Constructors & destructor (protected) ---------------------------------

/**
 * Creates a new instance and sets its identifier and name to the given
 * parameters @a id and @a name, respectively.
 *
 * @param id   Object identifier
 * @param name Object name
 */
NamedObject::NamedObject(ident_t id, const std::string& name)
  : IdObject(id),
    m_name(name)
{
}


/*
 *---------------------------------------------------------------------------
 *
 * Related functions
 *
 *---------------------------------------------------------------------------
 */

/**
 * Writes the numerical identifier and the name of the given NamedObject
 * instance @a item to the output stream @a stream.
 *
 * @param stream Output stream
 * @param item   NamedObject instance
 *
 * @return Output stream
 *
 * @relatesalso NamedObject
 */
ostream& pearl::operator<<(ostream& stream, const NamedObject& item)
{
  return stream << item.get_id() << " (" << item.get_name() << ")";
}
