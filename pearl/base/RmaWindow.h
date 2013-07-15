/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_RMAWINDOW_H
#define PEARL_RMAWINDOW_H


#include "IdObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    RmaWindow.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class RmaWindow.
 *
 * This header file provides the declaration of the class RmaWindow.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Comm;


/*-------------------------------------------------------------------------*/
/**
 * @class   RmaWindow
 * @ingroup PEARL_base
 * @brief   Base class for remote memory access windows.
 *
 * The class RmaWindow serves as a base class for all classes related to
 * memory access windows. It is independent from the parallel-programming
 * paradigm used, but always associated to a communicator.
 *
 * The numerical identifiers of the individual RMA windows are globally
 * defined and continuously enumerated, i.e., the ID is element of
 * [0,@#windows-1].
 */
/*-------------------------------------------------------------------------*/

class RmaWindow : public IdObject
{
  public:
    /// @name Constructors & destructor
    /// @{

    RmaWindow(ident_t id, Comm* communicator);

    /// @}
    /// @name Get window information
    /// @{

    Comm* get_comm() const;

    /// @}


  private:
    /// Associated communicator
    Comm* m_communicator;
};


}   /* namespace pearl */


#endif   /* !PEARL_RMAWINDOW_H */
