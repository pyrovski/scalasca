/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRMAUNLOCK_REP_H
#define PEARL_MPIRMAUNLOCK_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRmaUnlock_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRmaUnlock_rep.
 *
 * This header file provides the declaration of the class MpiRmaUnlock_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRmaUnlock_rep
 * @ingroup PEARL_mpi
 * @brief   Class for MPI RMA unlock events
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaUnlock_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiRmaUnlock_rep(timestamp_t timestamp, 
                     uint32_t    location,
                     MpiWindow*  window);
    MpiRmaUnlock_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual MpiWindow* get_window() const;
    virtual uint32_t   get_remote() const;

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
    /// MPI window locked
    MpiWindow* m_window;

    /// Local rank of lock location w.r.t. to communicator associated to
    /// MPI window
    uint32_t m_location;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIRMAUNLOCK_REP_H */
