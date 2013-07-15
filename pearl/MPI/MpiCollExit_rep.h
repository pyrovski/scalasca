/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPICOLLEXIT_REP_H
#define PEARL_MPICOLLEXIT_REP_H


#include "Exit_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiCollExit_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiCollExit_rep.
 *
 * This header file provides the declaration of the class MpiCollExit_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiCollExit_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for EXIT events of MPI collective
 *          operations.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiCollExit_rep : public Exit_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiCollExit_rep(timestamp_t timestamp,
                    MpiComm*    communicator,
                    uint32_t    root,
                    uint32_t    sent,
                    uint32_t    received,
                    uint64_t*   metrics);
    MpiCollExit_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual MpiComm* get_comm() const;
    virtual uint32_t get_root() const;
    virtual uint32_t get_sent() const;
    virtual uint32_t get_received() const;

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
    /// MPI communicator used in collective operation
    MpiComm* m_communicator;

    /// Local rank of root location w.r.t. communicator
    uint32_t m_root;

    /// Number of bytes sent
    uint32_t m_sent;

    /// Number of bytes received
    uint32_t m_received;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPICOLLEXIT_REP_H */
