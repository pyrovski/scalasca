/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_ENTER_REP_H
#define PEARL_ENTER_REP_H


#include "Flow_rep.h"


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @file    Enter_rep.h
 * @ingroup PEARL_base
 * @brief   Declaration of the class Enter_rep.
 *
 * This header file provides the declaration of the class Enter_rep.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CNode;


/*-------------------------------------------------------------------------*/
/**
 * @internal
 *
 * @class   Enter_rep
 * @ingroup PEARL_base
 * @brief   %Event representation for entering a source code region.
 */
/*-------------------------------------------------------------------------*/

class PEARL_NOPAD_ATTR Enter_rep : public Flow_rep
{
  public:
    /// @name Constructors & destructor
    /// @{

    Enter_rep(timestamp_t timestamp, Region* region, uint64_t* metrics);
    Enter_rep(const GlobalDefs& defs, Buffer& buffer);

    /// @}
    /// @name Event type information
    /// @{

    virtual event_t get_type() const;
    virtual bool    is_typeof(event_t type) const;

    /// @}
    /// @name Access event data
    /// @{

    virtual Region*   get_region() const;
    virtual Callsite* get_callsite() const;

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
    union {
      /// Region being entered (valid before preprocessing)
      Region* m_region;

      /// Corresponding call tree node (valid after preprocessing)
      CNode* m_cnode;
    };


    /* Private methods */
    Region* get_regionEntered() const;
    CNode*  get_cnode() const;
    void    set_cnode(CNode* cnode);


    /* Declare friends */
    friend class Event;
    friend class LocalTrace;
    friend void PEARL_verify_calltree     (      GlobalDefs& defs,
                                           const LocalTrace& trace);
    friend void PEARL_omp_verify_calltree (      GlobalDefs& defs,
                                           const LocalTrace& trace);
    friend void PEARL_preprocess_trace    (const GlobalDefs& defs,
                                           const LocalTrace& trace);
    friend void PEARL_omp_preprocess_trace(const GlobalDefs& defs,
                                           const LocalTrace& trace);
};


}   /* namespace pearl */


#endif   /* !PEARL_ENTER_REP_H */
