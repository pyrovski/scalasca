/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIREQUEST_REP_H
#define PEARL_MPIREQUEST_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRequest_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRequest_rep.
 *
 * This header file provides the declaration of the class MpiRequest_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRequest_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_SEND_COMPLETE events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRequest_rep : public Event_rep
{
  public:

    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual ident_t get_reqid() const;

    /// @}
    /// @name Modify event data
    /// @{

    virtual void set_reqid(uint32_t);

    /// @}


  protected:

    /// @name Constructors & destructor
    /// @{

    MpiRequest_rep(timestamp_t timestamp, ident_t reqid);
    MpiRequest_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Generate human-readable output of event data
    /// @}

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}
    /// @name Find previous/next communication request entries
    /// @{

    virtual uint32_t get_next_reqoffs() const;
    virtual uint32_t get_prev_reqoffs() const;

    virtual void     set_prev_reqoffs(uint32_t);
    virtual void     set_next_reqoffs(uint32_t);

    /// @}


  private:
    /// Nonblocking request id
    ident_t m_reqid;


    /* Declare friends */
    friend class Event;
    friend class LocalTrace;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIREQUEST_REP_H */
