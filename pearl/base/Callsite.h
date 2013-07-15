/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_CALLSITE_H
#define PEARL_CALLSITE_H


#include <string>

#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Callsite.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Callsite.
 *
 * This header file provides the declaration of the class Callsite.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Region;


/*-------------------------------------------------------------------------*/
/**
 * @class   Callsite
 * @ingroup PEARL_base
 * @brief   Stores information related to a call site.
 *
 * The instances of the Callsite class provide information about source-code
 * locations where the control flow may move from one region to another. It
 * does not necessarily have to be a function call site, instead, it can
 * also be a loop entry, where the control flow may move from the enclosing
 * region to the loop region.
 *
 * The numerical identifiers of the individual call sites are globally
 * defined and continuously enumerated, i.e., the ID is element of
 * [0,@#call_sites-1].
 *
 * @note The associated Region object is not owned by the Callsite object.
 *       Therefore, it is not deleted if the instance is released.
 */
/*-------------------------------------------------------------------------*/

class Callsite : public IdObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    Callsite(ident_t            id,
             const std::string& file,
             line_t             line,
             Region*            callee);

    /// @}
    /// @name Get call site information
    /// @{

    std::string get_file() const;
    line_t      get_line() const;
    Region*     get_callee() const;

    /// @}


  private:
    /// File name of the source-code location
    std::string m_file;

    /// Line number of the source-code location
    line_t m_line;

    /// Source-code region to be entered
    Region* m_callee;
};


}   /* namespace pearl */


#endif   /* !PEARL_CALLSITE_H */
