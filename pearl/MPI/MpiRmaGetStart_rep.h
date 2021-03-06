/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_MPIRMAGETSTART_REP_H
#define PEARL_MPIRMAGETSTART_REP_H


#include "RmaGetStart_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    MpiRmaGetStart_rep.h
 * @ingroup PEARL_mpi
 * @brief   Declaration of the class MpiRmaGetStart_rep.
 *
 * This header file provides the declaration of the class MpiRmaGetStart_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   MpiRmaGetStart_rep
 * @ingroup PEARL_mpi
 * @brief   %Event representation for RMA_GET_START events.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR MpiRmaGetStart_rep : public RmaGetStart_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    MpiRmaGetStart_rep(timestamp_t timestamp,
                       uint32_t    rma_id,
                       uint32_t    remote,
                       uint32_t    length,
                       MpiWindow*  window);
    MpiRmaGetStart_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual MpiWindow* get_window() const;

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
    /// Memory window
    MpiWindow* m_window;

};


}   /* namespace pearl */


#endif   /* !PEARL_RMAGETSTART_REP_H */
