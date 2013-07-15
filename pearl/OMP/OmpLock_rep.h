/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_OMPLOCK_REP_H
#define PEARL_OMPLOCK_REP_H


#include "Event_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    OmpLock_rep.h
 * @ingroup PEARL_omp
 * @brief   Declaration of the class OmpLock_rep.
 *
 * This header file provides the declaration of the class OmpLock_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   OmpLock_rep
 * @ingroup PEARL_omp
 * @brief   Base class for all OpenMP lock event representations (i.e.,
 *          OMP_ALOCK and OMP_RLOCK).
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR OmpLock_rep : public Event_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    OmpLock_rep(timestamp_t timestamp, ident_t lockid);
    OmpLock_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}

    /// @}
    /// @name Access event data
    /// @{

    virtual ident_t get_lockid() const;

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
    /// Identifier of the lock being acquired or released
    ident_t m_lockid;
};


}   /* namespace pearl */


#endif   /* !PEARL_OMPLOCK_REP_H */
