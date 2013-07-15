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

#include "StatisticController.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <assert.h>

#include "earl.h"

using namespace std;
using namespace earl;
using stats::P2Statistic;

// ************************************************
// *****              Statistics              *****
// *****                                      *****
// ************************************************


void StatisticController::SetWaitAtBarrierWritten(bool written, int position)
{
    lastWaitAtBarrierStatus.first = written;
    lastWaitAtBarrierStatus.second = position;
}

StatisticController *StatisticController::theController = 0;

StatisticController::StatisticController(bool gatherStats) 
    : fileGiven(false), gatherStatistics(gatherStats),
      lastWaitAtBarrierStatus(false, -1)
{
}

StatisticController::StatisticController(bool gatherStats,
                                         const std::string &filename)
    : fileGiven(true), gatherStatistics(gatherStats),
      lastWaitAtBarrierStatus(false, -1)
{
    allowedPatternNames.insert("mpi_barrier_completion");
    allowedPatternNames.insert("mpi_earlyreduce");
    allowedPatternNames.insert("mpi_latebroadcast");
    allowedPatternNames.insert("shmem_latebroadcast");
    allowedPatternNames.insert("mpi_latesender");
    allowedPatternNames.insert("mpi_latesender_wo");
    allowedPatternNames.insert("mpi_latereceiver");
    allowedPatternNames.insert("mpi_latereceiver_wo");
    allowedPatternNames.insert("mpi_barrier_wait");
    allowedPatternNames.insert("mpi_wait_nxn");
    allowedPatternNames.insert("mpi_rma_fence_wait");
    allowedPatternNames.insert("mpi_rma_wait_window_create");
    allowedPatternNames.insert("mpi_rma_wait_window_free");
    allowedPatternNames.insert("omp_ebarrier_wait");
    allowedPatternNames.insert("omp_ibarrier_wait");
    allowedPatternNames.insert("shmem_wait_nxn");
    allowedPatternNames.insert("shmem_barrier_wait");
    allowedPatternNames.insert("omp_lock_api");
    allowedPatternNames.insert("omp_lock_critical");
    ifstream theFile(filename.c_str());
    assert(theFile.is_open());
    string theLine = "";
    string myString = "";
    double myDouble = 0.0;
    while(getline(theFile, theLine))
    {
        size_t pos = theLine.find_first_not_of(" \t"); 
        if(theLine.size() > 0 && (pos == string::npos || theLine[pos] != '#'))
        {
            istringstream Str(theLine);
            Str >> myString;
            if(allowedPatternNames.find(myString) == allowedPatternNames.end())
            {
                cerr << "EXPERT: Warning, unknown patternname "
                     << myString << " has been ignored." << endl;
            }
            if(Str >> myDouble)
            {
                importanceLimits.insert(make_pair(myString, myDouble));
            }
            else
            {
                importanceLimits.insert(make_pair(myString, -100.0));
            }
        }
    }
}

bool StatisticController::StatisticExists(int id) const
{
  return statistics.find(id) != statistics.end();
}

void StatisticController::AddStatistic(int id, const std::string &name)
{
  statistics.insert(
       make_pair(id, std::pair<std::string, P2Statistic>(name, P2Statistic())));
  maxSeverePatterns.insert(make_pair(id, vector<severePatternInstance>()));
}

P2Statistic& StatisticController::GetStatistic(int id)
{
  std::map<int, std::pair<std::string, P2Statistic> >::iterator it =
                                                            statistics.find(id);
  if(it != statistics.end())
  {
     return it->second.second;
  }
  assert(false);
  return it->second.second; /* stupid never reached statement
                               to shutup compiler warning */
}

bool
StatisticController::IsActive(const std::string& name, double wastedTime) const
{
   if(!fileGiven)
   {
      return true;
   }
   map<string, double>::const_iterator it = importanceLimits.find(name);
   if(it != importanceLimits.end())
   {
      return wastedTime >= it->second;
   }
   else
   {
      return false;
   }
}

void StatisticController::PrintStatistics(const string& fileName) const
{
   const int widthes[] = {26, 10, 12, 10, 10, 10, 10, 10, 12, 12};
   double (P2Statistic::*valueFuncs[])() const = {
     &P2Statistic::mean, &P2Statistic::med, &P2Statistic::min,
     &P2Statistic::max, &P2Statistic::sum, &P2Statistic::var,
     &P2Statistic::q25, &P2Statistic::q75
   };
   ofstream statisticsFile(fileName.c_str());
   statisticsFile << setw(widthes[0]) << left << "Pattern Name"
                  << setw(widthes[1]) << "Count"
                  << setw(widthes[2]) << "Mean Value"
                  << setw(widthes[3]) << "Median"
                  << setw(widthes[4]) << "Minimum"
                  << setw(widthes[5]) << "Maximum"
                  << setw(widthes[6]) << "Sum"
                  << setw(widthes[7]) << "Variance"
                  << setw(widthes[8]) << "Quartil 25"
                  << setw(widthes[9]) << "Quartil 75"
                  << endl;
   map<int, pair<string, P2Statistic> >::const_iterator it = statistics.begin();
   for(; it !=  statistics.end(); ++it)
   {
      statisticsFile << setw(widthes[0]) << left << it->second.first
                     << setw(widthes[1]) << it->second.second.count();
      size_t maxIndex = sizeof(valueFuncs)
                      / sizeof(double (P2Statistic::*)() const); 
      if(it->second.second.count() < 5)
      {
          if(it->second.second.count() < 2)
          {
              maxIndex -= 3;
          }
          else
          {
              maxIndex -= 2;
          }
      }
      for(size_t i = 0; i < maxIndex; ++i)
      {
         statisticsFile << setw(widthes[i + 2]) << fixed
                        << (it->second.second.*valueFuncs[i])();
      }
      statisticsFile << endl;
      if(maxSeverePatterns.find(it->first) != maxSeverePatterns.end())
      {
          vector<severePatternInstance> const &severeVector =
                                      maxSeverePatterns.find(it->first)->second;
          for(vector<severePatternInstance>::const_iterator
              severe_it = severeVector.begin();
              severe_it != severeVector.end();
              ++severe_it)
          {
              long cubeCNodeID = -1;
              if(cNodeIDMap.find(severe_it->cnode_id) == cNodeIDMap.end())
              {
                  cerr << "WARNING: Illegal cnode_id" << endl;
              }
              else
              {
                  cubeCNodeID = cNodeIDMap.find(severe_it->cnode_id)->second;
              }
              statisticsFile << "-" << " cnode: " << cubeCNodeID
                             << " enter: " << severe_it->enterT
                             << " exit: " << severe_it->exitT
                             << " duration: " << severe_it->wastedT
                             << " markerTime: " << severe_it->markerT
                             << " markerLoc: " << severe_it->markerL << endl;
          }
      }
      statisticsFile << endl;
   }
   statisticsFile << endl;
}

void StatisticController::AddSeverity(int id, const string &Name,
                                      severePatternInstance const& spi)
{  
   if(gatherStatistics)
   {
       if(!StatisticExists(id))
       {
          AddStatistic(id, Name);
       }
       GetStatistic(id).add(spi.wastedT);
       vector<severePatternInstance> &eventVector =
                                             maxSeverePatterns.find(id)->second;
       bool found = false;
       for(vector<severePatternInstance>::iterator it = eventVector.begin();
           it != eventVector.end(); ++it)
       {
           if(it->cnode_id == spi.cnode_id)
           {
               found = true;
               if(it->wastedT < spi.wastedT)
               {
                   *it = spi;
               }
           }
       }
       if(!found)
       {
           if(eventVector.size() <= maxNumberOfSeverePatterns)
           {
               eventVector.push_back(spi);
           }
           else
           {
               eventVector[0] = spi;
           }
           sort(eventVector.begin(), eventVector.end(), CompareSeverePatterns);
       }
   }
}

void StatisticController::AddCubeCNodeID(long earlCNodeID, long cubeCNodeID)
{
    cNodeIDMap[earlCNodeID] = cubeCNodeID;
}

bool StatisticController::CompareSeverePatterns(
     severePatternInstance const &lhs, severePatternInstance const &rhs)
{
    return lhs.wastedT < rhs.wastedT;
}

bool CompareEventsByTime(Event const &lhs, Event const &rhs)
{
    return lhs.get_time() < rhs.get_time();
}

