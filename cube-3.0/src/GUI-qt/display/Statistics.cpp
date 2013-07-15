/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "Statistics.h"

#if defined(WITH_VAMPIR)
#include "VampirConnecter.h"
#endif
#if defined(WITH_PARAVER)
#include "ParaverConnecter.h"
#endif
#include "Environment.h"
#include "Metric.h"
#include "Cnode.h"
#include <math.h>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <limits>

#include <iostream>

using namespace std;

namespace
{
   void ReadWhitespaces(istream &theFile)
   {
      char check = theFile.peek();
      while(check == ' ' || check == '\n')
      {
         theFile.ignore();
         check = theFile.peek();
      }
   }

   //if the opened file is called "expert.cube" this function returns the
   //position at which "expert.cube" begins in the string
   //otherwise it returns -1
   string::size_type getExpertFileNameIndex(string const &path)
   {
      string::size_type position = path.rfind("expert.cube");
      //check if "expert.cube" is in the path
      if(position != string::npos)
      {
         //check if the path ends with "expert.cube"
         if(path.length() == position + strlen("expert.cube"))
         {
            //check if there is a slash before "expert.cube"
            //or the sole path is only "expert.cube"
            if(position == 0 || path[position-1] == '/')
            {
               return position;
            }
         }
      }
      return string::npos;
   }
}

Statistics::SevereEvent::SevereEvent(ifstream &theFile)
{
   string buffer;
   //reads "cnode:"
   theFile >> buffer;

   theFile >> cnode_id;

   //reads "enter:"
   theFile >> buffer;

   theFile >> enter;

   //reads "exit:"
   theFile >> buffer;

   theFile >> exit;

   //reads "duration"
   theFile >> buffer;

   theFile >> wastedTime;

   //ignore rest of line
   theFile.ignore(std::numeric_limits<int>::max(), '\n');
}

Statistics::StatisticalInformation::StatisticalInformation(ifstream &theFile)
{
   mean = median = minimum = maximum = sum = variance = q1 = q3 = 0;
   theFile >> count;
   ReadWhitespaces(theFile);
   if(theFile.peek() != '-')
   {
      theFile >> mean;
      theFile >> median;
      theFile >> minimum;
      theFile >> maximum;
      theFile >> sum;
      if(count >= 2)
      {
         theFile >> variance;
      }
      if(count >= 5)
      {
         theFile >> q1;
         theFile >> q3;
      }
      statInformation = true;
   }
   else
   {
      statInformation = false;
   }
   
    valid_values = isStatisticalInformationValid();
}

Statistics::StatisticalInformation::StatisticalInformation(int theCount, double theSum, double theMean, double min,
                                                           double q25, double theMedian, double q75, double max, double theVariance)
    : count(theCount), statInformation(true), sum(theSum), mean(theMean), minimum(min), q1(q25),
      median(theMedian), q3(q75), maximum(max), variance(theVariance), valid_values( true ), data_status_description()
{
    valid_values = isStatisticalInformationValid();
}


/**
 * Performs a sequence of checks, whether statistical data looks meaningfull or not.
 */
bool
Statistics::StatisticalInformation::StatisticalInformation::isStatisticalInformationValid()
{
    data_status_description.clear();
    bool _return = true;
    if ( minimum > maximum )
    {
        data_status_description.append( "Min value is larger than Max." );
        _return = false;                   // the world is up side down
    }
    if ( mean > maximum && mean < minimum )
    {
        data_status_description.append( "Mean value is out of range [minimum, maximum]." );
        _return = false;
    } // mean is out of range
    if ( median > maximum && median < minimum )
    {
        data_status_description.append( "Median value is out of range [minimum, maximum]." );
        _return = false;
    } // median is out of range
    if ( maximum * count  < sum )
    {
        data_status_description.append( "Sum is larger than \"count * maximum\"." );
        _return = false;
    } // sum is bigger that count times maximum
    if ( ( minimum * count )  > sum )
    {
        data_status_description.append( "Sum is smaller than \"count * minimum\"." );
        _return = false;
    } // sum is lower that count times minimum

    if ( count >= 2)
    {
	if ( variance < 0 )
	{
	    data_status_description.append( "Variance is negative." );
	    _return = false;
	} // varianvve cannot be negativ
	if (count >= 5)
	{
	    if ( q1 > q3 )
	    {
		data_status_description.append( "25% quantile is larger than 75% quantile." );
		_return = false;
	    } // quantile 25% is bigger than 75%
	    if ( median > q3 )
	    {
		data_status_description.append( "Medium (50% quantile) is larger than 75% quantile." );
		_return = false;
	    } // definition of medium : 50% and it cannot be bigger than 75%
	    if ( median < q1 )
	    {
		data_status_description.append( "Medium (50% quantile) is smaller than 25% quantile." );
		_return = false;
	    } // definition of medium : 50% and it cannot be smaller  than 25%
	}
    }

    return _return;
}


#define OUTPUT_PERCENT(P) (fabs(maximum) > 10e-6) ? (percentStream << " " << (int) ((P)*100/maximum+.5) << "%" << endl) : percentStream << endl;
QStringList Statistics::StatisticalInformation::Print(string const &patternName, PrecisionWidget *prec) const
{
   stringstream percentStream;
   stringstream theStream;
   string captionString;
   if (patternName.length() > 0) {
      captionString += "Pattern: \n";
      theStream << patternName << endl;
      percentStream << endl;
   }
    if ( valid_values )
    {
	if(statInformation)
	{
	    captionString += "Sum: \n";
	    captionString += "Count: \n";
	    captionString += "Mean: \n";
	    theStream << prec->numberToQString(sum, false, FORMAT1).toStdString() << endl
		    << prec->numberToQString(count, true, FORMAT1).toStdString() << endl
		    << prec->numberToQString(mean, false, FORMAT1).toStdString() << endl;
	    percentStream << endl << endl;
	    OUTPUT_PERCENT(mean);

	    if(count >= 2)
	    {
		captionString += "Standard deviation: \n";
		theStream << prec->numberToQString(sqrt(variance), false, FORMAT1).toStdString() << endl;
		OUTPUT_PERCENT(sqrt(variance));
	    }
	
	    captionString += "Maximum: \n";
	    theStream << prec->numberToQString(maximum, false, FORMAT1).toStdString() << endl;
	    OUTPUT_PERCENT(maximum);

	    if(count >= 5)
	    {
		captionString += "Upper quartile (Q3): \n";
		theStream << prec->numberToQString(q3, false, FORMAT1).toStdString() << endl;
		OUTPUT_PERCENT(q3);
	    }
	    captionString += "Median: \n";
	    theStream << prec->numberToQString(median, false, FORMAT1).toStdString() << endl;
	    OUTPUT_PERCENT(median);
	    if(count >= 5)
	    {
		captionString += "Lower quartile (Q1): \n";
		theStream << prec->numberToQString(q1, false, FORMAT1).toStdString() << endl;
		OUTPUT_PERCENT(q1);
	    }
	    captionString += "Minimum: ";
	    theStream << prec->numberToQString(minimum, false, FORMAT1).toStdString();
	    OUTPUT_PERCENT(minimum);
	}
	else
	{
	    captionString += "Count: \n";
	    theStream << prec->numberToQString(count, true, FORMAT1).toStdString() << endl;
	    percentStream << endl;
	}
    }
    else
    {
        theStream << "Statistical data seems to be bogus. Several errors are found (listed below): \n" << endl
                  << data_status_description.join( "\n" ).toUtf8().data() << endl;
    }
   QStringList list;
   list.append(QString::fromStdString(captionString));
   list.append(QString::fromStdString(theStream.str()));
   list.append(QString::fromStdString(percentStream.str()));
   return list;
}

Statistics::Statistics(string const &fileName, PrecisionWidget *thePrecisionWidget)
   : cubeFileName(fileName), prec(thePrecisionWidget), verbose(false)
{
   if(char * value = getenv("CUBE_VERBOSE"))
   {
      verbose = env_str2bool(value);
   }
   //calculate the path of the *.stat file
   string::size_type zipPos = cubeFileName.rfind(".gz");
   if((zipPos != string::npos) && ((cubeFileName.length() - zipPos) == 3))
   {
      cubeFileName.erase(zipPos, 3);
   }
   string statisticsFileName;
   string::size_type position = getExpertFileNameIndex(cubeFileName);
   //expert.cube -> epik.stat
   if(position != string::npos)
   {
      statisticsFileName =  cubeFileName.substr(0,position) + "epik.stat";
   }
   //*.cube -> *.stat
   else
   {
      position = cubeFileName.rfind(".cube");
      statisticsFileName = cubeFileName.substr(0,position) + ".stat";
   }
   ifstream theFile(statisticsFileName.c_str());
   if(!theFile.is_open())
   {
      fileExists = false;
   }
   else
   {
      if(verbose)
      {
         cout << "Found statistics file " << statisticsFileName << endl;
      }
      fileExists = true;
      string buffer;
      //read the title line
      getline(theFile, buffer);

      while(!theFile.eof() && !theFile.fail())
      {
         string name;
         theFile >> name;
         severeEventsType::iterator it = severeEvents.insert(make_pair(name,
                                                                       make_pair(StatisticalInformation(theFile),
                                                                                 vector<SevereEvent>()))).first;
         ReadWhitespaces(theFile);
         char check = theFile.peek();
         while(check == '-' && !theFile.eof())
         {
            theFile >> buffer;
            it->second.second.push_back(SevereEvent(theFile));
            ReadWhitespaces(theFile);
            check = theFile.peek();
         }
      }
   }
}

bool Statistics::existsStatFile() const
{
   return fileExists;
}

Statistics::~Statistics()
{
#if defined(WITH_VAMPIR) || defined(WITH_PARAVER)
   for(list<VisualizationConnecter*>::const_iterator it
       = theVisualizations.begin(); it != theVisualizations.end(); ++it)
   {
      delete *it;
   }
#endif
}

PrecisionWidget *Statistics::getPrecisionWidget() const
{
   return prec;
}

bool Statistics::existsStatistics(cube::Metric const *metric) const
{
   if (metric == 0)
     return false;

   severeEventsType::const_iterator it = severeEvents.find(metric->get_uniq_name());
   if (it != severeEvents.end())
      return true;

   return false;
}

QStringList Statistics::getStatistics(cube::Metric const *metric) const
{
   if(!existsStatistics(metric))
   {
      return QStringList("") << "";
   }

   severeEventsType::const_iterator it = severeEvents.find(metric->get_uniq_name());
   if (it != severeEvents.end())
     return it->second.first.Print(it->first, prec);

   return QStringList("") << "";
}

Item Statistics::getItem(cube::Metric const *metric) const
{
    severeEventsType::const_iterator it = severeEvents.find(metric->get_uniq_name());
    if (it != severeEvents.end())
    {
        StatisticalInformation const &info = it->second.first;
        return Item(it->first, info.count, info.sum,
                    info.mean, info.minimum, info.q1, info.median, info.q3,
                    info.maximum, info.variance);
    }
    throw logic_error("No statistic information for the requested metric.");
}

#if defined(WITH_VAMPIR)
string Statistics::connectToVampir(string const &host, int port,
                                   string const &fileName)
{
   bool replaced = false;
   string message;
   for(unsigned i = 0; i < vampirVisualizationIterators.size(); ++i)
   {
      if(!VampirConnecter::ExistsVampirWithBusName(VampirConnecter::GetVampirBusName(i)))
      {
         if(verbose)
         {
            cout << "Replacing earlier connection to already closed vampir at bus name " << VampirConnecter::GetVampirBusName(i) << endl;
         }
         delete *(vampirVisualizationIterators[i]);
         VampirConnecter *theVisualization = new VampirConnecter(VampirConnecter::GetVampirBusName(i), host, port, fileName, verbose);
         message = theVisualization->InitiateAndOpenTrace();
         *(vampirVisualizationIterators[i]) = theVisualization;
         replaced = true;
      }
   }
   if(!replaced)
   {
      if(vampirVisualizationIterators.size() < (unsigned) VampirConnecter::GetMaxVampirNumber())
      {
         if(verbose)
         {
            cout << "Esablishing new connection (no old connection is replaced) to vampir on bus name "
                  << VampirConnecter::GetVampirBusName(vampirVisualizationIterators.size()) << endl;
         }
         VampirConnecter *theVisualization = new VampirConnecter(VampirConnecter::GetVampirBusName(vampirVisualizationIterators.size()), host, port, fileName, verbose);
         message = theVisualization->InitiateAndOpenTrace();
         if(message != "")
         {
            delete theVisualization;
         }
         else
         {
            theVisualizations.push_back(theVisualization);
            list<VisualizationConnecter*>::iterator it = theVisualizations.end();
            --it;
            vampirVisualizationIterators.push_back(it);
         }
      }
      else
      {
         return "Maximal number of vampir clients already reached.";
      }
   }
   return message;
}

string Statistics::getDefaultVampirFileName() const
{
    string::size_type position = getExpertFileNameIndex(cubeFileName);
    //expert.cube -> epik.esd
    if(position != string::npos)
    {
        return cubeFileName.substr(0,position) + "epik.elg";
    }
    //*.cube -> epik.esd
    else
    {
        position = cubeFileName.rfind("/");
        if (position == string::npos)
          return "epik.esd";

        return cubeFileName.substr(0,position) + "/epik.esd";

#if 0
        position = cubeFileName.rfind(".cube");
        //should never happen
        if(position == string::npos)
        {
            return "";
        }
        else
        {
            return cubeFileName.substr(0,position) + ".esd";
        }
#endif
    }
}

#endif

#if defined(WITH_PARAVER)
string Statistics::connectToParaver(string const &configFileName,
                                    string const &fileName)
{
   if(verbose)
   {
      cout << "Esablishing new paraver connection" << endl;
   }
   VisualizationConnecter *theVisualization = new ParaverConnecter(fileName, configFileName, verbose);
   string message = theVisualization->InitiateAndOpenTrace();
   if(message != "")
   {
      delete theVisualization;
   }
   else
   {
      theVisualizations.push_back(theVisualization);
   }
   return message;
}

string Statistics::getDefaultParaverFileName() const
{
   string::size_type position = getExpertFileNameIndex(cubeFileName);
   //expert.cube -> epik.prv
   if(position != string::npos)
   {
      return cubeFileName.substr(0,position) + "epik.prv";
   }
   //*.cube -> *.prv
   else
   {
      position = cubeFileName.rfind(".cube");
      //should never happen
      if(position == string::npos)
      {
         return "";
      }
      else
      {
         return cubeFileName.substr(0,position) + ".prv";
      }
   }
}
#endif

#if defined(WITH_VAMPIR) || defined(WITH_PARAVER)
bool Statistics::existsMaxSeverities() const
{
   for(severeEventsType::const_iterator it
       = severeEvents.begin(); it != severeEvents.end(); ++it)
   {
      if(it->second.second.size() > 0)
      {
         return true;
      }
   }
   return false;
}

bool Statistics::isConnected() const
{
   return !theVisualizations.empty();
}


string Statistics::showMaxSeverity(cube::Metric const *metric,
                                   cube::Cnode const *cnode) const
{
   if(!isConnected())
   {
      return "Not connected to a trace brwoser.";
   }
   else
   {
      assert(existsMaxSeverity(metric, cnode));
      SevereEvent const *theEvent = findMaxSeverity(metric, cnode);
      if(theEvent == 0)
      {
         return "No event with maximal severity at this cnode.";
      }
      else
      {
         string message = "";
         const int zoomSteps = 3;
         const double offsetMultiplier = 0.1;
         double duration = theEvent->exit - theEvent->enter;
         double exitTime = theEvent->exit + offsetMultiplier * duration;

         for (int step=1; step<=zoomSteps; step++)
         {
            double enterTime = max(theEvent->enter
                                   - ((zoomSteps-step)*5) * duration
                                   - offsetMultiplier * duration, 0.0);
            for(list<VisualizationConnecter*>::const_iterator it =
                theVisualizations.begin(); it != theVisualizations.end(); ++it)
            {
               if((*it)->IsActive())
               {
                  message += (*it)->ZoomIntervall(enterTime, exitTime, step);
               }
            }
         }
         return message;
      }
   }
}
#endif

bool Statistics::existsMaxSeverity(cube::Metric const *metric,
                                   cube::Cnode const *cnode) const
{
    return findMaxSeverity(metric, cnode) != 0;
}

Statistics::SevereEvent const*
Statistics::findMaxSeverity(cube::Metric const *metric,
                            cube::Cnode const *cnode) const
{
    severeEventsType::const_iterator it = severeEvents.find(metric->get_uniq_name());
    if (it != severeEvents.end())
    {
        const vector<SevereEvent> &vec = it->second.second;
        if(vec.size() == 0)
        {
            return 0;
        }
        if(cnode == 0)
        {
            SevereEvent const *max_elem = &(*vec.begin());
            for(vector<SevereEvent>::const_iterator it2
                = vec.begin(); it2 != vec.end(); ++it2)
            {
                if(it2->wastedTime > max_elem->wastedTime)
                {
                    max_elem = &(*it2);
                }
            }
            return max_elem;
        }
        else
        {
            for(vector<SevereEvent>::const_iterator it2
                = vec.begin(); it2 != vec.end(); ++it2)
            {
                if(it2->cnode_id == cnode->get_id())
                {
                    return &(*it2);
                }
            }
            return 0;
        }
    }

    return 0;
}
