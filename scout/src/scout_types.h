/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef SCOUT_TYPES_H
#define SCOUT_TYPES_H


#include <map>

#include <Location.h>
#include <pearl_types.h>


/*-------------------------------------------------------------------------*/
/**
 * @file  scout_types.h
 * @brief Declaration of SCOUT-specific datatypes.
 *
 * This header file provides the declaration of some SCOUT-specific
 * datatypes.
 */
/*-------------------------------------------------------------------------*/


//--- Preprocessor defines --------------------------------------------------

/// Maximum number of most-severe instances to be tracked
#define TOP_SEVERE_INSTANCES   5

//#define MOST_SEVERE_MAX //if one needs to find the maximal time among top severe instances


//--- Type definitions ------------------------------------------------------

namespace scout
{

/// Remote severity map. Stores a location @f$\times@f$ cnode @f$\times@f$
/// severity value matrix.
typedef std::map<pearl::Location*, std::map<pearl::ident_t, double> > 
  rem_sev_container_t;


/**
 * @struct TimeVec2
 * @brief  Simple datatype storing two timestamp values.
 *
 * This struct provides a simple datatype to store two timestamp values.
 * The associated MPI datatype handle is provided by TIMEVEC2.
 */

struct TimeVec2
{
  /// Timestamp values
  pearl::timestamp_t value[2];
};


struct TopMostSevere
{
  pearl::timestamp_t idletime;
  pearl::timestamp_t entertime;
  pearl::timestamp_t exittime;
  pearl::ident_t     cnode;
  pearl::ident_t     rank;

  TopMostSevere() : idletime(0.0), entertime(0.0), exittime(0.0), cnode(0), rank(0)
  {
  }

  TopMostSevere(pearl::timestamp_t idletime_, pearl::timestamp_t entertime_, pearl::timestamp_t exittime_, pearl::ident_t cnode_, pearl::ident_t rank_) :
  idletime(idletime_), entertime(entertime_), exittime(exittime_), cnode(cnode_), rank(rank_)
  {
  }
};

#ifdef MOST_SEVERE_MAX
  struct TopMostSevereMaxSum : TopMostSevere
  {
    // idletime will be used for summation of waiting time
    // idletime_max will be used for storage of maximal value of the waiting time
    pearl::timestamp_t idletime_max;

    TopMostSevereMaxSum() : idletime_max(0.0)
    {
      idletime  = 0.0;
      entertime = 0.0;
      exittime  = 0.0;
      cnode     = 0;
      rank      = 0;
    }

    TopMostSevereMaxSum(pearl::timestamp_t idletimemax_, pearl::timestamp_t idletimesum_, pearl::timestamp_t entertime_, pearl::timestamp_t exittime_, pearl::ident_t cnode_, pearl::ident_t rank_) :
    idletime_max(idletimemax_)
    {
      idletime  = idletimesum_;
      entertime = entertime_;
      exittime  = exittime_;
      cnode     = cnode_;
      rank      = rank_;
    }

  };
#endif

}   // namespace scout


#endif   // !SCOUT_TYPES_H
