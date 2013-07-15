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

#include "Alock_rep.h"

using namespace earl;

bool 
Alock_rep::is_type(etype type) const
{
  if ( Sync_rep::is_type(type) || type == ALOCK )
    return true;
  else
    return false;
}

