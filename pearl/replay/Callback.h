/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef PEARL_CALLBACK_H
#define PEARL_CALLBACK_H


#include "CountedPtr.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    Callback.h
 * @ingroup PEARL_replay
 * @brief   Declaration of the class Callback and related functions.
 *
 * This header file provides the declaration of the class Callback and
 * related functions.
 */
/*-------------------------------------------------------------------------*/


namespace pearl
{

//--- Forward declarations --------------------------------------------------

class CallbackData;
class CallbackManager;
class Event;


/*-------------------------------------------------------------------------*/
/**
 * @class   Callback
 * @ingroup PEARL_replay
 * @brief   Abstract base class for callback objects.
 */
/*-------------------------------------------------------------------------*/

class Callback
{
  public:
    /// @name Constructors & destructor
    /// @{

    virtual ~Callback() {};

    /// @}
    /// @name Callback method
    /// @{

    virtual void execute(const CallbackManager& cbmanager, int user_event,
                         const Event& event, CallbackData* data) = 0;

    /// @}
};


//--- Type definitions ------------------------------------------------------

/// Type name for reference-counted callback objects
typedef CountedPtr<Callback> CallbackPtr;


//--- Related functions -----------------------------------------------------

template <class classT, typename funcT>
CallbackPtr PEARL_create_callback(classT* instance, funcT function)
{
  class Adapter : public Callback
  {
    public:
      Adapter(classT* instance, funcT function)
        : m_instance(instance), m_function(function)
      {}

      virtual void execute(const CallbackManager& cbmanager, int user_event,
                           const Event& event, CallbackData* data)
      {
        (m_instance->*m_function)(cbmanager, user_event, event, data);
      }

    private:
      classT* m_instance;
      funcT   m_function;
  };

  return CallbackPtr(new Adapter(instance, function));
}


}   /* namespace pearl */


#endif   /* !PEARL_CALLBACK_H */
