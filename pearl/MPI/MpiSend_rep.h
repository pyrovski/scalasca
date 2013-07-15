/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPISEND_REP_H
#define PEARL_MPISEND_REP_H


#include "MpiP2P_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiSend_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiSend_rep.
 *
 * This header file provides the declaration of the class MpiSend_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiSend_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_SEND events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiSend_rep : public MpiP2P_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiSend_rep(timestamp_t timestamp,
                MpiComm*    communicator,
                uint32_t    destination,
                uint32_t    tag,
                uint32_t    sent);
    MpiSend_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual uint32_t get_sent() const;
    virtual uint32_t get_dest() const;

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
    /// Local rank of receiver location w.r.t. communicator
    uint32_t m_destination;

    /// Message length
    uint32_t m_sent;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPISEND_REP_H */
