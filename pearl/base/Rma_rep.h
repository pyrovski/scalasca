/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_RMA_REP_H
#define PEARL_RMA_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    Rma_rep.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Rma_rep.
 *
 * This header file provides the declaration of the class Rma_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   Rma_rep
 * @ingroup PEARL_base
 * @brief   Base class for all remote memory access communication event
 *          representations
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR Rma_rep : public Event_rep
{
  public:
    /// @name Event type information
    /// @{

    virtual event_t  get_type() const;
    virtual bool     is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual uint32_t get_rma_id() const;

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    Rma_rep(timestamp_t timestamp, uint32_t rma_id);
    Rma_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Serialize event data
    /// @{

    virtual void pack(Buffer& buffer) const;

    /// @}
    /// @name Generate human-readable output of event data
    /// @{

    virtual std::ostream& output(std::ostream& stream) const;

    /// @}


  private:
    /// RMA ID
    uint32_t m_rma_id;
};


}   /* namespace pearl */


#endif   /* !PEARL_RMA_REP_H */
