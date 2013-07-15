/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRECEIVE_REP_H
#define PEARL_MPIRECEIVE_REP_H


#include "MpiP2P_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiReceive_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiReceive_rep.
 *
 * This header file provides the declaration of the class MpiReceive_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiReceive_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_RECV events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiReceive_rep : public MpiP2P_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiReceive_rep(timestamp_t timestamp,
                   MpiComm*    communicator,
                   uint32_t    source,
                   uint32_t    tag);
    MpiReceive_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual uint32_t get_source() const;

    /// @}


  protected:
    /// @name Serialize event data
    /// @{

    virtual void pack(Buffer& buffer) const;

    /// @}
    /// @name Generate human-readable output of event data
    /// @}

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}


  private:
    /// Local rank of sender location w.r.t. communicator
    uint32_t m_source;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPIRECEIVE_REP_H */
