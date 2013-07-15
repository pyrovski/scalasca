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

#ifndef EARL_NAMEDOBJ_H
#define EARL_NAMEDOBJ_H

#include <string>
#include <map>

/*
 *----------------------------------------------------------------------------
 *
 * Abstarct base class for named objects.
 * Named Objeacts may derives from NamedObj abstract base class and
 * profit using algorithms defined for named nbjects.
 *
 * usage example:
 * map<long, NamedObj*> mymap;
 * ...
 * it = find_if((mymap.begin(), mymap.end(), NO::name_sec_eq("MPI_Send")));
 *
 * Search for objects saved in map. The search result is iterator
 * to object whos name is MPI_Send or mymap.end() if such object
 * is not found.
 * 
 *---------------------------------------------------------------------------- 
 */


namespace earl
{
  class NamedObj 
  {
  public:
    /* public iterface, which must be implemented by each named object */
    virtual std::string get_name() const = 0;
    /* ecnsure virtual destructor */
    virtual ~NamedObj(){};
  };
    
  namespace NO
  {
    /* predicate which compares name of named object */
    class name_eq {
        private:
          std::string name;
        public:
          name_eq(const std::string& name):name(name){};
          bool operator() ( const NamedObj* item ) {
            return name == item->get_name();
          }
    };

    /* predicate which compares name of second object, stored in pair */
    class name_sec_eq {
        private:
          std::string name;
        public:
          name_sec_eq(const std::string& name):name(name){};
          bool operator() ( const std::pair<long, NamedObj*>& item ) {
            return name == item.second->get_name();
          }
    };
  }
}
#endif






