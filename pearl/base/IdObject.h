/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_IDOBJECT_H
#define PEARL_IDOBJECT_H


#include <iosfwd>

#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    IdObject.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class IdObject and related functions.
 *
 * This header file provides the declaration of the class IdObject and all
 * related functions.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @class   IdObject
 * @ingroup PEARL_base
 * @brief   Base class for entities with a numerical identifier.
 *
 * The class IdObject serves as a base class for all entities that have a
 * numerical identifier. The meaning of this ID, however, is defined by
 * the individual derived classes.
 *
 * The IdObject class provides the basic functionality to get and set the
 * object identifier. In addition, instances can be compared based on their
 * ID (equivalence test).
 */
/*-------------------------------------------------------------------------*/

class IdObject
{
  public:
    /// @name Get & set object identifier
    /// @{

    ident_t get_id() const;
    void    set_id(ident_t id);

    /// @}
    /// @name Comparison operators
    /// @{

    bool operator <(const IdObject& rhs) const;
    bool operator >(const IdObject& rhs) const;

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    IdObject(ident_t id);
    virtual ~IdObject();

    /// @}


  private:
    /// Object identifier
    ident_t m_id;
};


//--- Related functions -----------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const IdObject& item);


}   /* namespace pearl */


#endif   /* !PEARL_IDOBJECT_H */
