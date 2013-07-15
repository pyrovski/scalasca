/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRMACOLLEXIT_REP_H
#define PEARL_MPIRMACOLLEXIT_REP_H


#include "Exit_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRmaCollExit_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRmaCollExit_rep.
 *
 * This header file provides the declaration of the class MpiRmaCollExit_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl {

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRmaCollExit_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for EXIT events of MPI RMA collective
 *          operations.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaCollExit_rep : public Exit_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiRmaCollExit_rep(timestamp_t timestamp,
                       MpiWindow*  window,
                       uint64_t*   metrics);
    MpiRmaCollExit_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual MpiWindow* get_window() const;

    /// @}


  protected:
    /// @name Serialize event data
    /// @{

    virtual void pack(Buffer& buffer) const;

    /// @}
    /// @name Generate human-readable output of event data
    /// @{

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}


  private:
    /// Memory window
    MpiWindow* m_window;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIRMACOLLEXIT_REP_H */
