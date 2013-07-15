/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRMAEXIT_REP_H
#define PEARL_MPIRMAEXIT_REP_H


#include "Exit_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRmaExit_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRmaExit_rep.
 *
 * This header file provides the declaration of the class MpiRmaExit_rep.
 */
/*-------------------------------------------------------------------------*/

namespace pearl {

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRmaExit_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for EXIT events of MPI RMA collective
 *          operations.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaExit_rep : public Exit_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiRmaExit_rep(timestamp_t timestamp,
                   MpiWindow*  window,
                   MpiGroup*   group,
                   bool        sync,
                   uint64_t*   metrics);
    MpiRmaExit_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual MpiWindow* get_window() const;
    virtual MpiGroup*  get_group() const;
    virtual bool       is_sync() const;

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

    /// Access group identifier
    MpiGroup* m_group;

    /// Synchronising flag
    bool m_sync;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIRMAEXIT_REP_H */
