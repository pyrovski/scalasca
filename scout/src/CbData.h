/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_CBDATA_H
#define SCOUT_CBDATA_H


#include <stack>

#include <CallbackData.h>
#include <Event.h>

#include "scout_types.h"


/*-------------------------------------------------------------------------*/
/**
 * @file  CbData.h
 * @brief Declaration of the CbData struct.
 *
 * This header file provides the declaration of the CbData struct which
 * stores the data required for the pattern search replay.
 */
/*-------------------------------------------------------------------------*/


//--- Forward declarations --------------------------------------------------

namespace pearl
{

class EventSet;
class GlobalDefs;
class LocalTrace;
class RemoteEventSet;

}   // namespace pearl


namespace scout
{

/*-------------------------------------------------------------------------*/
/**
 * @struct CbData
 * @brief  Datatype aggregating all data which is required for the pattern
 *         search replay.
 *
 * The CbData struct provides a datatype which aggregates a number of values
 * which are required during the pattern search replay. In most cases, this
 * is data which has to be passed between callbacks (i.e., from a base pattern
 * to a more specific sub-pattern).
 *
 * @todo Review, potentially rename and document all undocumented attributes.
 */
/*-------------------------------------------------------------------------*/

struct CbData : public pearl::CallbackData
{
  /// @brief   Local event set
  /// @details Collects local pattern constituents of point-to-point
  ///          communication patterns before sending them to the peer.
  pearl::EventSet* m_local;

  /// @brief   Remote event set
  /// @details Stores remote pattern constituents of point-to-point
  ///          communication patterns after receiving them from the peer.
  pearl::RemoteEventSet* m_remote;

  /// Global definitions object
  pearl::GlobalDefs* m_defs;

  /// Local trace object
  pearl::LocalTrace* m_trace;

  /// @brief   Detected waiting time
  /// @details Used to transfer the already calculated waiting time to
  ///          sub-patterns.
  double m_idle;

  double m_tmptime;
  double m_tmptime2;
  TimeVec2 m_timevec;

  /// @brief Detected count;
  int    m_count;
  
  /// Current call stack
  std::stack<pearl::Event> m_callstack;


  /// @name Constructors & destructor
  /// @{

  CbData();
  virtual ~CbData();

  /// @}
  /// @name Pre- and postprocessing
  /// @{

  virtual void postprocess(const pearl::Event& event);

  /// @}
};


}   // namespace scout


#endif   // !SCOUT_CBDATA_H
