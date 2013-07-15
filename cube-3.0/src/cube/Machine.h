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
* \file Machine.h
* \brief Describes a machine, on which an application was running.
*/
#ifndef CUBE_MACHINE_H
#define CUBE_MACHINE_H   


#include "Sysres.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Machine
 *
 *----------------------------------------------------------------------------
 */

namespace cube {

  class Node;
/**
* A machine as a kind of system resources. 
*/ 
  class Machine : public Sysres {
    
  public:

    Machine(const std::string& name, const std::string& desc, uint32_t id, uint32_t sysid)
      : Sysres(name, id, sysid), desc(desc) {}
    
    Node* get_child(unsigned int i) const {
      return (Node*) Vertex::get_child(i);
    }
    Sysres* get_parent() const { return NULL; }///< Machine is a upper object of abstraction. (GRID?)
    std::string get_desc() const { return desc; }///< Gets a description of this machine. 
    void set_desc(std::string newDesc) { desc = newDesc; }///< Describe the machine.

    void writeXML(std::ostream& out) const;///< Write a xml-output for the machine in .cube file.

  private:

    std::string desc;///< Description of the machine.

  };
  
  inline bool operator== (const Machine& a, const Machine& b) {
    return a.get_name() == b.get_name();
  }
  
}

#endif
