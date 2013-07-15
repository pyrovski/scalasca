/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_FLOW_REP_H
#define PEARL_FLOW_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    Flow_rep.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Flow_rep.
 *
 * This header file provides the declaration of the class Flow_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   Flow_rep
 * @ingroup PEARL_base
 * @brief   Base class for all flow event representations (i.e., ENTER,
 *          ENTER_CS and EXIT).
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR Flow_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~Flow_rep();

    /// @}
    /// @name Event type information
    /// @{

    virtual bool is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

#ifdef PEARL_ENABLE_METRICS
    virtual uint64_t get_metric(int index) const;
#endif

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    Flow_rep(timestamp_t timestamp, uint64_t* metrics);
    Flow_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}


  private:
#ifdef PEARL_ENABLE_METRICS
    /// Hardware counter metric values
    uint64_t* m_metrics;
#endif
};


}   /* namespace pearl */


#endif   /* !PEARL_FLOW_REP_H */
