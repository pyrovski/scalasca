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

#ifndef EARL_BOOKMARKS_H
#define EARL_BOOKMARKS_H

/*
 *----------------------------------------------------------------------------
 *
 * class Bookmarks
 *
 * This class maintains a sparse index on the event trace, which is build up
 * dynamically. So it is possible to access events and system states from the 
 * event trace without reading the trace file from the very beginning.
 *
 *----------------------------------------------------------------------------
 */

namespace earl 
{
  class Bookmarks
  {
  public:
    
    Bookmarks();
    
    bool  save(long pos);      
    long  nearest(long pos); 
    
  private:
    
      // distance of bookmarks
    long bdist;
    bool disabled;
    
    long last_insertion;
    
    bool is_eligible(long pos);
  };
}

#endif






