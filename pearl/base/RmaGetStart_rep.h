/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_RMAGETSTART_REP_H
#define PEARL_RMAGETSTART_REP_H


#include "Rma_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    RmaGetStart_rep.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class RmaGetStart_rep.
 *
 * This header file provides the declaration of the class RmaGetStart_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   RmaGetStart_rep
 * @ingroup PEARL_base
 * @brief   %Event representation for RMA_GET_START events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR RmaGetStart_rep : public Rma_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    RmaGetStart_rep(timestamp_t timestamp,
                    uint32_t    rma_id,
                    uint32_t    remote,
                    uint32_t    received);
    RmaGetStart_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual uint32_t get_remote() const;
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
    /// Rank of origin location (can be local of global, depending on the
    /// programming model)
    uint32_t m_remote;

    /// Length of data in bytes
    uint32_t m_received;
};


}   /* namespace pearl */


#endif   /* !PEARL_RMAGETSTART_REP_H */
