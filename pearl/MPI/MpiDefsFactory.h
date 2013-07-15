/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIDEFSFACTORY_H
#define PEARL_MPIDEFSFACTORY_H


#include "DefsFactory.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiDefsFactory.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiDefsFactory.
 *
 * This header file provides the declaration of the class MpiDefsFactory.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiDefsFactory
 * @ingroup PEARL_mpi
 * @brief   Abstract factory class for MPI-specific global definition
 *          objects.
 */
/*-------------------------------------------------------------------------*/

class MpiDefsFactory : public DefsFactory
{
  public:
    /// @name MPI-specific factory methods
    /// @{

    virtual void createMpiGroup    (GlobalDefs&        defs,
                                    ident_t            id,
                                    process_group&     process_ids,
                                    bool               is_self,
                                    bool               is_world) const;
    virtual void createMpiComm     (GlobalDefs&        defs,
                                    ident_t            id,
                                    ident_t            group_id) const;
    virtual void createMpiWindow   (GlobalDefs&        defs,
                                    ident_t            id,
                                    ident_t            comm_id) const;
    virtual void createMpiCartesian(GlobalDefs&        defs,
                                    ident_t            id,
                                    const cart_dims&   dimensions,
                                    const cart_period& periodic,
                                    ident_t            comm_id,
                                    const std::string cart_name,
                                    const std::vector<std::string> dim_names) const;

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIDEFSFACTORY_H */
