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

#ifndef EARL_REGIONSTACK_H
#define EARL_REGIONSTACK_H

/*
 *----------------------------------------------------------------------------
 *
 * class RegionStack
 *
 *----------------------------------------------------------------------------
 */


#include <vector>

#include "Event.h"
#include "Grid.h"

namespace earl
{
  class Location;

  class RegionStack 
  {    
  public:

    RegionStack(const Grid* grid);   
    virtual ~RegionStack(){};   

    void push(Event enter);                   
    void pop(Event enter);                           
    void fork(Event fork);                           
    void join(Event fork);      
                     
    Event top(Location* loc)       const; 
    Event last_fork(Location* loc) const;

    void stack(std::vector<Event>& out, Location* loc); 
    void istack(std::vector<Event>& out, Location* loc); 
  
  private:
  
    std::vector<std::vector<Event> >   stackv;
    std::vector<Event>                 pid2fork;  
    const Grid* grid;
  };
}
#endif








