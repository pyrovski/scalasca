/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_NAMEDOBJECT_H
#define PEARL_NAMEDOBJECT_H


#include <iosfwd>
#include <string>

#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    NamedObject.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class NamedObject and related functions.
 *
 * This header file provides the declaration of the class NamedObject and
 * all related functions.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   NamedObject
 * @ingroup PEARL_base
 * @brief   Base class for named entities with a numerical identifier.
 *
 * The class NamedObject serves as a base class for all entities that are
 * named and have a numerical identifier. It provides the basic functionality
 * to get and set the object ID (inherited from IdObject) as well as the
 * name of the object.
 */
/*-------------------------------------------------------------------------*/

class NamedObject : public IdObject
{
  public:
    /// @name Get & set object name
    /// @{

    std::string get_name() const;
    void        set_name(const std::string& name);

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    NamedObject(ident_t id, const std::string& name);

    /// @}


  private:
    /// Object name
    std::string m_name;
};


//--- Related functions -----------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const NamedObject& item);


}   /* namespace pearl */


#endif   /* !PEARL_NAMEDOBJECT_H */
