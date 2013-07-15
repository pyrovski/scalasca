/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRECEIVECOMPLETE_REP_H
#define PEARL_MPIRECEIVECOMPLETE_REP_H


#include "MpiReceive_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiReceiveComplete_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiReceiveComplete_rep.
 *
 * This header file provides the declaration of the class MpiReceiveComplete_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiReceiveComplete_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_RECV events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiReceiveComplete_rep : public MpiReceive_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiReceiveComplete_rep(timestamp_t timestamp,
                           MpiComm*    communicator,
                           uint32_t    source,
                           uint32_t    tag,
                           ident_t     reqid);
    MpiReceiveComplete_rep(const GlobalDefs& defs, Buffer& buffer);

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
    /// @}

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}
    /// @name Find previous/next communication request entries
    /// @{

    virtual uint32_t get_prev_reqoffs() const;

    virtual void     set_prev_reqoffs(uint32_t);

    /// @}


  private:
    /// Nonblocking request id
    ident_t  m_reqid;

    /// Offset to previous test or receive start
    uint32_t m_prev_reqoffs;
};


}   /* namespace pearl */


#endif
