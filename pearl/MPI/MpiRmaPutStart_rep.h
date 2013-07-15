/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRMAPUTSTART_REP_H
#define PEARL_MPIRMAPUTSTART_REP_H


#include "RmaPutStart_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRmaPutStart_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRmaPutStart_rep.
 *
 * This header file provides the declaration of the class MpiRmaPutStart_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRmaPutStart_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_RMA_PUT_START events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaPutStart_rep : public RmaPutStart_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiRmaPutStart_rep(timestamp_t timestamp,
                       uint32_t    rma_id,
                       uint32_t    remote,
                       uint32_t    length,
                       MpiWindow*  window);
    MpiRmaPutStart_rep(const GlobalDefs& defs, Buffer& buffer);

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


#endif   /* !PEARL_MPIRMAPUTSTART_REP_H */
