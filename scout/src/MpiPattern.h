/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_MPIPATTERN_H
#define SCOUT_MPIPATTERN_H


#include "Pattern.h"
#include "scout_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file  MpiPattern.h
 * @brief Declaration of the MpiPattern class.
 *
 * This header file provides the declaration of the MpiPattern class which
 * serves as an abstract base class for all MPI-specific performance
 * properties calculated by SCOUT.
 */
/*-------------------------------------------------------------------------*/


namespace scout
{

/*-------------------------------------------------------------------------*/
/**
 * @class MpiPattern
 * @brief Abstract base class for all MPI-specific properties calculated by
 *        SCOUT.
 *
 * This class serves as an abstract base class for all MPI-specific performance
 * properties calculated by SCOUT. This includes message statistics as well as
 * the inefficiency patterns (i.e., wait states) occuring at synchronization
 * points.
 */
/*-------------------------------------------------------------------------*/

class MpiPattern : public Pattern
{
  protected:
    /// @name Data collation control
    /// @{

    virtual bool skip_cnode(const pearl::CNode& cnode) const;

    /// @}
    /// @name Exchanging remote severities
    /// @{

    virtual void exchange_severities(const pearl::GlobalDefs&   defs,
                                     const rem_sev_container_t& remote_sev);

    /// @}
};


}   // namespace scout


#endif   // !SCOUT_MPIPATTERN_H
