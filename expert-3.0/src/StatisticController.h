/****************************************************************************
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef STATISTIC_CONTROLLER_H
#define STATISTIC_CONTROLLER_H

#include <string>
#include <map>
#include <utility>
#include <set>
#include <vector>
#include <P2Statistic.h>

/*
 * --------------------------------------------------------------------------
 *                        class StatisticController
 *  
 *     collects and prints statistical information about found patterns
 * --------------------------------------------------------------------------
 */

class Report;

class StatisticController
{
   public:
     StatisticController(bool gatherStats);
     StatisticController(bool gatherStats, const std::string& filename);

     struct severePatternInstance
     {
         severePatternInstance(double enterTime, double exitTime,
                               double wastedTime, long cnode,
                               double markerTime, long markerLoc)
             : enterT(enterTime), exitT(exitTime), wastedT(wastedTime),
               cnode_id(cnode), markerT(markerTime), markerL(markerLoc)
         {}
         double enterT, exitT;
         double wastedT;
         long cnode_id;
         double markerT;
         long markerL;
     };

     bool IsActive(const std::string& name, double wastedTime) const;
     bool GatherStatistics() const {return gatherStatistics;}
     void AddSeverity(int id, const std::string &name,
                      severePatternInstance const& spi);
     void SetWaitAtBarrierWritten(bool written, int position);
     const std::pair<bool, int>& GetWaitAtBarrierWritten(void) const
     {return lastWaitAtBarrierStatus;}
     void PrintStatistics(const std::string& fileName) const;
     //pointer to the singleton-instance
     static StatisticController *theController;
     void AddCubeCNodeID(long earlCNodeID, long cubeCNodeID);

   private:
     stats::P2Statistic& GetStatistic(int id);
     bool StatisticExists(int id) const;
     void AddStatistic(int id, const std::string& name);
     std::map<int, std::pair<std::string, stats::P2Statistic> > statistics;
     std::map<std::string, double> importanceLimits;
     bool fileGiven;
     bool gatherStatistics;
     std::set<std::string> allowedPatternNames;
     //if a Barrier is written, this is set to the position of the WaitAtBarrier
     std::pair<bool, int> lastWaitAtBarrierStatus;
     std::map<int, std::vector<severePatternInstance> > maxSeverePatterns;
     static unsigned int const maxNumberOfSeverePatterns = 10;
     std::map<long, long> cNodeIDMap;
     static bool CompareSeverePatterns(severePatternInstance const &lhs,
                                       severePatternInstance const &rhs);
};

namespace earl
{
    class Event;
}

bool CompareEventsByTime(earl::Event const &lhs, earl::Event const &rhs);

#endif

