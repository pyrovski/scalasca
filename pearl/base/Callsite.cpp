/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <cassert>

#include "Callsite.h"

using namespace std;
using namespace pearl;


/*
 *---------------------------------------------------------------------------
 *
 * class Callsite
 *
 *---------------------------------------------------------------------------
 */

//--- Constructors & destructor ---------------------------------------------

/**
 * Creates a new instance and sets data to the given values.
 *
 * @param id     Call site identifier
 * @param file   Name of the file in which the call site is located
 * @param line   Line number of the call site location
 * @param callee Source code region to be entered
 */
Callsite::Callsite(ident_t       id,
                   const string& file,
                   line_t        line,
                   Region*       callee)
  : IdObject(id),
    m_file(file),
    m_line(line),
    m_callee(callee)
{
  assert(callee);
}


//--- Get call site information ---------------------------------------------

/**
 * Returns the name of the file in which the call site is located.
 *
 * @return File name
 */
string Callsite::get_file() const
{
  return m_file;
}


/**
 * Returns the source code line number of the call site location.
 *
 * @return Line number
 */
line_t Callsite::get_line() const
{
  return m_line;
}


/**
 * Returns the source code region to be entered.
 *
 * @return Source-code region
 */
Region* Callsite::get_callee() const
{
  return m_callee;
}
