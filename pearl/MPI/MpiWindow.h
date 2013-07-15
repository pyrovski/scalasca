/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIWINDOW_H
#define PEARL_MPIWINDOW_H


#include "RmaWindow.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    MpiWindow.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiWindow.
 *
 * This header file provides the declaration of the class MpiWindow.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class MpiComm;


/*-------------------------------------------------------------------------*/
/**
 * @class   MpiWindow
 * @ingroup PEARL_mpi
 * @brief   Stores information related to an MPI-2 remote memory access
 *          window.
 *
 * The instances of the class MpiWindow provide information about MPI-2
 * remote memory access (RMA) windows.
 */
/*-------------------------------------------------------------------------*/

class MpiWindow : public RmaWindow
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiWindow(ident_t id, MpiComm* communicator);

    /// @}
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIWINDOW_H */
