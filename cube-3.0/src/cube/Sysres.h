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
* \file Sysres.h
* \brief Provides a general system resource with a tree like hierarchy.
*/
#ifndef CUBE_SYSRES_H
#define CUBE_SYSRES_H   


#include "Vertex.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Sysres
 *
 *----------------------------------------------------------------------------
 */

namespace cube {
/**
* System resources have a treelike structure -> therefore it is a kind of Vertex. And every Sysres has an id and name.
*/
  class Sysres : public Vertex {
    
  public:

    Sysres(const std::string& name, uint32_t id=0, uint32_t sysid=0)
      : Vertex(id), name(name), sys_id(sysid) {}
    Sysres(Sysres* parent, const std::string& name, uint32_t id=0, uint32_t sysid=0)
      : Vertex(parent, id), name(name), sys_id(sysid) {}
    Sysres(const Sysres& copy)
      :  Vertex(copy.get_parent(), copy.get_id()), name(copy.name), sys_id(copy.sys_id) 
        {
            for (unsigned i = 0; i< copy.num_children(); i++)
            {
                add_child(copy.get_child(i));
            }
        }


    std::string get_name() const { return name; }///< Name of the resource.
    void set_name(std::string newname) { name = newname; }///< Set a name.
    uint32_t get_sys_id() const { return sys_id; }
/**
* This sysres is a part of which sysres? 
*/
    Sysres* get_parent() const {
      return static_cast<Sysres*>(Vertex::get_parent());
    }
/**
* This sysres contains i-th sysres.... which is ?
*/
    Sysres* get_child(unsigned int i) const {
      return static_cast<Sysres*>(Vertex::get_child(i));
    }

  private:

    std::string name;
    uint32_t sys_id;
  };
}

#endif
