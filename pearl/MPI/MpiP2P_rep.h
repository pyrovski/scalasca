/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIP2P_REP_H
#define PEARL_MPIP2P_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiP2P_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiP2P_rep.
 *
 * This header file provides the declaration of the class MpiP2P_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiP2P_rep
 * @ingroup PEARL_mpi
 * @brief   Base class for all point-to-point communication event
 *          representations (i.e., MPI_SEND and MPI_RECV).
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiP2P_rep : public Event_rep
{
  public:
    /// @name Event type information
    /// @{

    virtual bool is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual MpiComm* get_comm() const;
    virtual uint32_t get_tag() const;
    virtual ident_t  get_reqid() const;

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    MpiP2P_rep(timestamp_t timestamp, MpiComm* communicator, uint32_t tag);
    MpiP2P_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Serialize event data
    /// @{

    virtual void pack(Buffer& buffer) const;

    /// @}
    /// @name Generate human-readable output of event data
    /// @{

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
    /// MPI communicator used
    MpiComm* m_communicator;

    /// Message tag
    uint32_t m_tag;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIP2P_REP_H */
