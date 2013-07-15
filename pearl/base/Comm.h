/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_COMM_H
#define PEARL_COMM_H


#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Comm.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Comm.
 *
 * This header file provides the declaration of the class Comm.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Group;
class Process;


/*-------------------------------------------------------------------------*/
/**
 * @class   Comm
 * @ingroup PEARL_base
 * @brief   Stores information related to an MPI communicator.
 *
 * Instances of the Comm class provide information about MPI communicators,
 * specifically the associated group of process identifiers.
 *
 * The numerical identifiers of the individual communicators are globally
 * defined and continuously enumerated, i.e., the ID is element of
 * [0,@#communicators-1].
 */
/*-------------------------------------------------------------------------*/

class Comm : public IdObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    Comm(ident_t id, Group* group);

    /// @}
    /// @name Get communicator information
    /// @{

    Group* get_group() const;

    /// @}


  private:
    /// Associated group of processes identifiers
    Group* m_group;
};


}   /* namespace pearl */


#endif   /* !PEARL_GROUP_H */
