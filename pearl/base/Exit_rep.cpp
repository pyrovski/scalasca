/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include "Exit_rep.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Exit_rep
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

Exit_rep::Exit_rep(timestamp_t timestamp, uint64_t* metrics)
  : Flow_rep(timestamp, metrics)
{
}


Exit_rep::Exit_rep(const GlobalDefs& defs, Buffer& buffer)
  : Flow_rep(defs, buffer)
{
}


//--- Event type information ------------------------------------------------

event_t Exit_rep::get_type() const
{
  return EXIT;
}


bool Exit_rep::is_typeof(event_t type) const
{
  return type == EXIT || Flow_rep::is_typeof(type);
}
