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

#include <cstdlib>

#include "Bookmarks.h"

#include "Error.h"

#define EARL_DEFAULT_BOOKMARK_DISTANCE 10000

using namespace earl;
using namespace std;

Bookmarks::Bookmarks() 
{
  char* tmp = getenv("EARL_BOOKMARK_DISTANCE");
  if (tmp != NULL)
    {
      bdist = atoi(tmp);
      if (bdist <= 0)
	throw RuntimeError("Bad value for environment variable EARL_BOOKMARK_DISTANCE.");
      disabled = false;
      // cerr << "EARL: Bookmark buffer with bookmark distance " << bdist << "." << endl;
    }
  else
    {
      disabled = true;
      // cerr << "EARL: Bookmark buffer disabled." << endl;
    }
  // next insertion at zero
  last_insertion = -bdist;
}

bool
Bookmarks::save(long pos) 
{
  if (!is_eligible(pos)) 
    return false;

  last_insertion = pos;
  return true;
}

long
Bookmarks::nearest(long pos) 
{
  if (pos < 0)
    throw FatalError("Error in Bookmarks::nearest(long).");

  if (pos == 0)
    return 0;
  else if (pos >= last_insertion)
    return last_insertion;
  else if (pos % bdist > 0) 
    return (pos / bdist) * bdist;
  else
    return ((pos / bdist) - 1) * bdist;
}

bool 
Bookmarks::is_eligible(long pos) 
{  
  if ( disabled )
    return pos == 0;
  else
    return (pos == (last_insertion + bdist));
}











