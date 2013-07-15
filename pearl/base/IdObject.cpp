/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>

#include "IdObject.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class IdObject
 *
 *---------------------------------------------------------------------------
 */

//--- Get & set object ID ---------------------------------------------------

/**
 * Returns the object's numerical identifier.
 *
 * @returns Numerical identifier
 */
ident_t IdObject::get_id() const
{
  return m_id;
}


/**
 * Sets (i.e., overwrites) the object's numerical identifier with the
 * given @a id.
 *
 * @param id New identifier
 */
void IdObject::set_id(ident_t id)
{
  m_id = id;
}


//--- Comparison operators --------------------------------------------------

/**
 * Returns true, if the instance's numerical identifier is less than the
 * ID of the given object @a rhs. Returns false otherwise.
 *
 * This comparison only makes sense if the two objects are of the same type,
 * e.g., both objects are instances of the class Process. However, this is
 * not checked at run-time.
 *
 * @return True, if get_id() @< rhs.get_id()
 */
bool IdObject::operator <(const IdObject& rhs) const
{
  return m_id < rhs.m_id;
}


/**
 * Returns true, if the instance's numerical identifier is greater than
 * the ID of the given object @a rhs. Returns false otherwise.
 *
 * This comparison only makes sense if the two objects are of the same type,
 * e.g., both objects are instances of the class Process. However, this is
 * not checked at run-time.
 *
 * @return True, if get_id() @> rhs.get_id()
 */
bool IdObject::operator >(const IdObject& rhs) const
{
  return m_id > rhs.m_id;
}


//--- Constructors & destructor (protected) ---------------------------------

/**
 * Creates a new instance and sets its identifier to the given @a id.
 *
 * @param id Numerical identifier
 */
IdObject::IdObject(ident_t id)
  : m_id(id)
{
}


/**
 * Destructor. Destroys the instance.
 */
IdObject::~IdObject()
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
 * Writes the numerical identifier of the given IdObject instance @a item
 * to the output stream @a stream.
 *
 * @param stream Output stream
 * @param item   IdObject instance
 *
 * @return Output stream
 *
 * @relatesalso IdObject
 */
ostream& pearl::operator<<(ostream& stream, const IdObject& item)
{
  return stream << item.get_id();
}
