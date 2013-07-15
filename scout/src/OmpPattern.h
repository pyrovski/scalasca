/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_OMPPATTERN_H
#define SCOUT_OMPPATTERN_H


#include "Pattern.h"


/*-------------------------------------------------------------------------*/
/**
 * @file  OmpPattern.h
 * @brief Declaration of the OmpPattern class.
 *
 * This header file provides the declaration of the OmpPattern class which
 * serves as an abstract base class for all OpenMP-specific performance
 * properties calculated by SCOUT.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{


/*-------------------------------------------------------------------------*/
/**
 * @class OmpPattern
 * @brief Abstract base class for all OpenMP-specific properties calculated
 *        by SCOUT.
 *
 * This class serves as an abstract base class for all OpenMP-specific
 * performance properties calculated by SCOUT.
 */
/*-------------------------------------------------------------------------*/

class OmpPattern : public Pattern
{
  protected:
    /// @name Data collation control
    /// @{

    virtual bool skip_cnode(const pearl::CNode& cnode) const;

    /// @}
};


}   // namespace scout


#endif   // !SCOUT_OMPPATTERN_H
