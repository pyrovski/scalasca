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

#include "OMPAlock_rep.h"

using namespace earl;

bool 
OMPAlock_rep::is_type(etype type) const
{
  if ( OMPSync_rep::is_type(type) || type == OMPALOCK )
    return true;
  else
    return false;
}

