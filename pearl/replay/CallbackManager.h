/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_CALLBACKMANAGER_H
#define PEARL_CALLBACKMANAGER_H


#include <map>
#include <vector>

#include "Callback.h"
#include "pearl_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    CallbackManager.h
 * @ingroup PEARL_replay
 * @brief   Declaration of the class CallbackManager.
 *
 * This header file provides the declaration of the class CallbackManager.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CallbackData;


/*-------------------------------------------------------------------------*/
/**
 * @class   CallbackManager
 * @ingroup PEARL_replay
 * @brief   Manages a set of callback objects and provides a simple
 *          notification mechanism.
 */
/*-------------------------------------------------------------------------*/

class CallbackManager
{
  public:
    /// @name Registering callbacks
    /// @{

    void register_callback(event_t event_type, CallbackPtr callback);
    void register_callback(int     user_event, CallbackPtr callback);

    /// @{
    /// @name Notification routines
    /// @{

    void notify(                const Event& event, CallbackData* data) const;
    void notify(int user_event, const Event& event, CallbackData* data) const;

    /// @}


  private:
    /// Container type for callback objects
    typedef std::vector<CallbackPtr> callback_container;

    /// Container type for trace event |-@> callback mapping
    typedef std::map<event_t,callback_container> event_cb_map;

    /// Container type for user event |-@> callback mapping
    typedef std::map<int,callback_container> user_cb_map;


    /// Trace event callback mapping
    event_cb_map m_event_cbs;

    /// User event callback mapping
    user_cb_map m_user_cbs;
};


}   /* namespace pearl */


#endif   /* !PEARL_CALLBACKMANAGER_H */
