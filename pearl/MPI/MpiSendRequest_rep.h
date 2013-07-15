/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPISENDREQUEST_REP_H
#define PEARL_MPISENDREQUEST_REP_H


#include "MpiSend_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiSendRequest_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiSendRequest_rep.
 *
 * This header file provides the declaration of the class MpiSendRequest_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiSendRequest_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_SEND events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiSendRequest_rep : public MpiSend_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiSendRequest_rep(timestamp_t timestamp,
                       MpiComm*    communicator,
                       uint32_t    destination,
                       uint32_t    tag,
                       uint32_t    sent,
                       ident_t     reqid);
    MpiSendRequest_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
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
    /// @name Generate human-readable output of event data
    /// @{

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}
    /// @name Find previous/next communication request entries
    /// @{

    virtual uint32_t get_next_reqoffs() const;

    virtual void     set_next_reqoffs(uint32_t);

    /// @}


  private:
    /// Nonblocking request id
    ident_t  m_reqid;

    /// Offset to next (test or completion) request event
    uint32_t m_next_reqoffs;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPINBSEND_REP_H */
