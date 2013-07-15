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
* \file IdentObject.h
* \brief Provides an object with an internal ID. 
*/

#ifndef CUBE_IDENTOBJECT_H
#define CUBE_IDENTOBJECT_H   


#include <inttypes.h>


/*
 *----------------------------------------------------------------------------
 *
 * class IdentObject
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  
  class IdentObject { 

  public:
    IdentObject(uint32_t id=0) : m_id(id) {}
    virtual ~IdentObject() {}

    void set_id(uint32_t id) { m_id = id; } ///< Set an ID.
    uint32_t get_id() const { return m_id; } ///< Gets the ID of the object.

  private:
    uint32_t m_id; ///< The ID of the object.
  };


}

#endif
