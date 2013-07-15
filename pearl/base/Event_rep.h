/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_EVENT_REP_H
#define PEARL_EVENT_REP_H

#include <iosfwd>

#include "pearl_padding.h"
#include "pearl_types.h"
#include "SmallObject.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Event_rep.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Event_rep.
 *
 * This header file provides the declaration of the class Event_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class Buffer;
class Callsite;
class GlobalDefs;
class LocalTrace;
class MpiComm;
class MpiGroup;
class MpiWindow;
class Region;


/*-------------------------------------------------------------------------*/
/**
 * @class   Event_rep
 * @ingroup PEARL_base
 * @brief   Base class for all event representations.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR Event_rep : public SmallObject
{
  public:
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

            timestamp_t get_time() const;
    virtual Region*     get_region() const;
    virtual Callsite*   get_callsite() const;
    virtual MpiComm*    get_comm() const;
    virtual uint32_t    get_root() const;
    virtual uint32_t    get_sent() const;
    virtual uint32_t    get_received() const;
    virtual uint32_t    get_source() const;
    virtual uint32_t    get_dest() const;
    virtual uint32_t    get_tag() const;
    virtual ident_t     get_reqid() const;
    virtual uint32_t    get_lockid() const;
    virtual MpiWindow*  get_window() const;
    virtual uint32_t    get_remote() const;
    virtual uint32_t    get_rma_id() const;
    virtual MpiGroup*   get_group() const;
    virtual bool        is_sync() const;
    virtual bool        is_exclusive() const;
#ifdef PEARL_ENABLE_METRICS
    virtual uint64_t    get_metric(int index) const;
#endif

    /// @}
    /// @name Modify event data
    /// @{

    void set_time(timestamp_t timestamp);

    virtual void set_reqid(uint32_t reqid);

    /// @}


  protected:
    /// @name Constructors & destructor
    /// @{

    Event_rep(pearl::timestamp_t timestamp);
    Event_rep(const GlobalDefs& defs, Buffer& buffer);

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
    /// Timestamp when the event has occured
    timestamp_t m_timestamp;


    /* Declare friends */
    friend class Buffer;
    friend class Event;
    friend class LocalTrace;
    friend class RemoteEvent;
};


}   /* namespace pearl */


#endif   /* !PEARL_EVENT_REP_H */
