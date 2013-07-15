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

#ifndef EARL_CALLTREE_H
#define EARL_CALLTREE_H

/*
 *----------------------------------------------------------------------------
 *
 * class Calltree
 *
 *----------------------------------------------------------------------------
 */

#include <map>
#include <vector>

#include "Event.h"

namespace earl 
{
  class Calltree 
  {    
  public:
  
    Calltree() : last(-1) {}
    virtual ~Calltree(){}

    // functions needed for internal purposes

    // provides the cnodptr that a new (enter) event with this region
    // and call site would have. If the event's cnodeptr points to
    // itself the function will return a null event
    Event get_cnodeptr(Event cedgeptr, Region* region, Callsite* csite);

    // add the new event to the tree
    void  add_node(Event enter);

    // functions provided by the external EARL interface

    Event get_parent(long cnode);

    void calltree(std::vector<Event>& out);
    void callpath(std::vector<Event>& out, long cnode);
    long ctsize() const { return nodem.size(); }
    void ctroots(std::vector<Event>& out);
    void ctchildren(std::vector<Event>& out, long cnode);
    long ctvisits(long cnode);
                     
  private:

    bool      iscnode(long cnode);

    std::map<long, Event>              nodem;
    std::vector<long>                  rootv;
    std::map<long, std::vector<long> > childvm;
    std::map<long, long>               nvisitsm;

    long last;          // pos of the most recent event in the tree
  };
}
#endif













