/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPISENDCOMPLETE_REP_H
#define PEARL_MPISENDCOMPLETE_REP_H


#include "MpiRequest_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiSendComplete_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiSendComplete_rep.
 *
 * This header file provides the declaration of the class MpiSendComplete_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiSendComplete_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for MPI_SEND_COMPLETE events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiSendComplete_rep : public MpiRequest_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiSendComplete_rep(timestamp_t timestamp,
                        ident_t     reqid);
    MpiSendComplete_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}


  protected:
    /// @name Find previous/next communication request entries
    /// @{

    virtual uint32_t get_prev_reqoffs() const;

    virtual void     set_prev_reqoffs(uint32_t);

    /// @}


  private:
    /// Offset to previous (test or request) event
    uint32_t m_prev_reqoffs;
};


}   /* namespace pearl */


#endif   /* !PEARL_MPISENDCOMPLETE_REP_H */
