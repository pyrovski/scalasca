/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#ifndef STATISTICS_H
#define STATISTICS_H

#include "Cube.h"
#if defined(WITH_VAMPIR) || defined(WITH_PARAVER)
#  include "VisualizationConnecter.h"
#endif
#include "PrecisionWidget.h"
#include "BoxPlot.h"

#include <utility>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <inttypes.h>

class Statistics
{
    public:
        struct StatisticalInformation
        {
            StatisticalInformation(std::ifstream &theFile);
            StatisticalInformation(int theCount, double theSum,
                                   double theMean, double min, double q25,
                                   double theMedian, double q75, double max,
                                   double theVariance);
            QStringList Print(std::string const &patternName, PrecisionWidget *prec) const;
	    bool isStatisticalInformationValid();
            int count;
            bool statInformation;
            double sum, mean, minimum, q1, median, q3, maximum, variance;
	    bool         valid_values;
	    QStringList  data_status_description;
        };
        Statistics(std::string const &fileName,
                   PrecisionWidget *thePrecisionWidget = 0);
        PrecisionWidget *getPrecisionWidget() const;
        bool existsStatFile() const;
        bool existsStatistics(cube::Metric const *metric) const;
        QStringList getStatistics(cube::Metric const *metric) const;
        Item getItem(cube::Metric const *metric) const;

        bool existsMaxSeverity(cube::Metric const *metric,
                               cube::Cnode const *cnode = 0) const;

#if defined(WITH_VAMPIR) || defined(WITH_PARAVER)
        bool existsMaxSeverities() const;
        bool isConnected() const;
        std::string showMaxSeverity(cube::Metric const *metric,
                                    cube::Cnode const *cnode = 0) const;

        #if defined(WITH_VAMPIR)
        std::string connectToVampir(std::string const &host, int port,
                                    std::string const &fileName);
        std::string getDefaultVampirFileName() const;
        #endif

        #if defined(WITH_PARAVER)
        std::string connectToParaver(std::string const &configFileName,
                                     std::string const &fileName);
        std::string getDefaultParaverFileName() const;
        #endif
#endif

        ~Statistics();

    private:
        struct SevereEvent
        {
            SevereEvent(std::ifstream &theFile);
            double enter, exit;
            double wastedTime;
            uint32_t cnode_id;
        };
        typedef std::map<std::string,
                         std::pair<StatisticalInformation,
                                   std::vector<SevereEvent>
                                  >
                       > severeEventsType;
        std::map<std::string,
                 std::pair<StatisticalInformation, std::vector<SevereEvent> >
                > severeEvents;

        SevereEvent const* findMaxSeverity(cube::Metric const *metric,
                                           cube::Cnode const *cnode) const;

#if defined(WITH_VAMPIR) || defined(WITH_PARAVER)
        std::list<VisualizationConnecter*> theVisualizations;
        std::vector<std::list<VisualizationConnecter*>::iterator>
                                                  vampirVisualizationIterators;
#endif

        std::string cubeFileName;
        PrecisionWidget *prec;
        bool fileExists;
        bool verbose;
};

#endif
