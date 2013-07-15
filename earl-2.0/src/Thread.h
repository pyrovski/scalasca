/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef EARL_THREAD_H
#define EARL_THREAD_H

/*
 *----------------------------------------------------------------------------
 *
 * class Thread
 *
 *----------------------------------------------------------------------------
 */

#include <string>

namespace earl
{
  class Location;
  class Process;

  class Thread
  {
  public:

    Thread(long proc_id,
	   long thrd_id,
	   std::string name) :
      proc_id(proc_id), thrd_id(thrd_id), name(name) {}

    long        get_thrd_id()   const { return thrd_id; }
    long        get_proc_id()   const { return proc_id; }
    std::string get_name()      const { return name; }
    Process*    get_proc()      const { return proc; }
    Location*   get_loc()       const { return loc; }


  private:
  
    friend class Grid;
    void set_name(std::string str) { name = str; } 
    void set_proc(Process* p)      { this->proc = p; }
    void set_loc(Location* l)      { this->loc = l; }

    long                   proc_id;
    long                   thrd_id;
    std::string            name;
    Process*               proc;
    Location*              loc;
  };
}
#endif


