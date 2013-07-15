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

#include "P2P_rep.h"

using namespace earl;

bool 
P2P_rep::is_type(etype type) const
{
  if ( Event_rep::is_type(type) || type == P2P )
    return true;
  else
    return false;
}
