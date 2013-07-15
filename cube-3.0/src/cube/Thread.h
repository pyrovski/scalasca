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
* \file Thread.h
* \brief Provides a relevant description of a thread. 
*/ 

#ifndef CUBE_THREAD_H
#define CUBE_THREAD_H   


#include <iosfwd>

#include "Sysres.h"


/*
 *----------------------------------------------------------------------------
 *
 * class Thread
 *
 *----------------------------------------------------------------------------
 */


namespace cube {

  class Process;

/**
* Thread is a basics kind of system resources. It doesn't return any "children"
*/ 
  class Thread : public Sysres {
    
  public:

    Thread(const std::string& name, int rank, Process* proc, uint32_t id=0, uint32_t sysid=0)
      : Sysres((Sysres*)proc, name, id, sysid), rank(rank) {}

    int get_rank() const { return rank; } ///< Thread does have a rank.
    Process* get_parent() const { return (Process*) Vertex::get_parent(); }///< Thread belongs always to some process.
    void writeXML(std::ostream& out) const; ///< Writes a xml-representation of a thread in a .cube file.

  private:

    int rank;

  };
  

  inline bool operator== (const Thread& a, const Thread& b) {
    return a.get_rank() == b.get_rank();
  }
  inline bool operator< (const Thread& a, const Thread& b) {
    return a.get_rank() < b.get_rank();
  }

}

#endif
