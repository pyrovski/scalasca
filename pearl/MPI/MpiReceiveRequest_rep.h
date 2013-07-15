/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRECEIVEREQUEST_REP_H
#define PEARL_MPIRECEIVEREQUEST_REP_H


#include "MpiRequest_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiReceiveRequest_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiReceiveRequest_rep.
 *
 * This header file provides the declaration of the class MpiReceiveRequest_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiReceiveRequest_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_RECV_REQUEST events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiReceiveRequest_rep : public MpiRequest_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiReceiveRequest_rep(timestamp_t timestamp, ident_t reqid);
    MpiReceiveRequest_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}

  protected:
    /// @name Find previous/next communication request entries
    /// @{

    virtual uint32_t get_next_reqoffs() const;

    virtual void     set_next_reqoffs(uint32_t);

    /// @}


  private:
    /// Offset to next (test or completion) request event
    uint32_t m_next_reqoffs;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIRECEIVEREQUEST_REP_H */
