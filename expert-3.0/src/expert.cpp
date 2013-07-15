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

#include <cmath>
#include <cstdlib>
#include <earl.h>
#include <iostream>
#include <sys/time.h>
#include "PatternTrace.h"
#include "Analyzer.h"
#include "ExpertOpt.h"
#include "StatisticController.h"

using namespace earl;
using namespace std;

int main(int argc, char* argv[])
{
  try 
    {
      struct timeval start, end;
      long seconds;

      ExpertOpt opt(argc, argv);

      gettimeofday(&start, 0);

      // open trace file
      EventTrace trace(opt.get_filename());
      if ( opt.get_verbose() >= 0 ) 
      cerr << "Total number of events: " << trace.get_nevents() << endl;
      //PATTERN TRACE INIT
      if (opt.get_patterntrace())
      {
        PatternTrace::_pt_ptr = new PatternTrace(&trace,
                               opt.get_prefix() + "_" + trace.get_name(),
                               opt.get_patterntrace());
        PatternTrace::_pt_ptr->WriteHeader();
      }

      //Statistic Controller init
      if(opt.get_selectPatterns())
      {
        StatisticController::theController = new StatisticController(
                    opt.get_gatherStatistic(), opt.get_selectPatternFilename());
      }
      else
      {
        StatisticController::theController = new StatisticController(
                    opt.get_gatherStatistic());
      }
      // start analysis
      Analyzer analyzer(&trace, &opt);
      analyzer.run();

      // calculate time needed by analysis
      gettimeofday(&end, 0);
      seconds = long(ceil((end.tv_usec * 1.0e-6) + end.tv_sec) -
                       ((start.tv_usec * 1.0e-6) + start.tv_sec));
      if ( opt.get_verbose() >= 0 ) {
        cerr << "Elapsed time (h:m:s): ";
        cerr << seconds / 3600 << " : " 
             << (seconds % 3600) / 60 << " : "
             << seconds % 60
             << endl;      
      }

      // calculate event rate
      if ( seconds == 0 )
        seconds = 1;
      if ( opt.get_verbose() >=0 )
        cerr << "Events per second: " << trace.get_nevents() / seconds << endl;
      if (opt.get_patterntrace() ){
        delete PatternTrace::_pt_ptr;
        if ( opt.get_verbose() >=0 )
          cout << "EXPERT: Pattern Trace produced in "
               << opt.get_prefix() << "_" << trace.get_name() << endl ; 
      }
      if(StatisticController::theController->GatherStatistics())
      {
          //the following line changes the suffix of the file to .stat
          string statisticsFileName = opt.get_filename().substr(0,
                                opt.get_filename().find_last_of('.')) + ".stat";
          StatisticController::theController->PrintStatistics(statisticsFileName);
          cerr << "EXPERT: Pattern Statistics written to "
               << statisticsFileName << endl;
      }
      delete StatisticController::theController;
    }
  catch ( earl::Error error ) 
    {
      cerr << error.get_msg() << "\n";
      exit(EXIT_FAILURE);
    } 
} 


