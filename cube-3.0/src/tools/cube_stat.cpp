/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#include <iostream>
#include <iomanip>
#ifndef CUBE_COMPRESSED
#  include <fstream>
#else
#  include "zfstream.h"
#endif
#include <cstring>
#include <string>
#include <vector>

#include <unistd.h>
#include <stdlib.h>

#include <algorithm>

#include "AggrCube.h"
#include "Thread.h"
#include "Metric.h"
#include "Region.h"
#include "Cnode.h"

#include "P2Statistic.h"
#include "services.h"


/*-------------------------------------------------------------------------*/
/**
 * @file    cube_stat.cpp
 * @brief   Provides the tool cube3_stat which extracts statistical information
 *          from CUBE files
 *
 */
/*-------------------------------------------------------------------------*/

using namespace std;
using namespace cube;
using namespace services;
using stats::P2Statistic;

namespace {
    Metric* findMetric(AggrCube *cubeObject, const string& metname,
                       bool exact = true)
    {
        vector<Metric*> const &metVec = cubeObject->get_metv();
        for(vector<Metric*>::const_iterator it = metVec.begin();
            it != metVec.end(); ++it)
        {
            const string& uname = (*it)->get_uniq_name();
            size_t pos;
            if (exact)
            {
              if (uname == metname) return *it;
            }
            // uname ends with metname?
            else if ((pos = uname.rfind(metname)) != string::npos)
            { 
               if ((uname.length() - metname.length()) == pos) return *it;
            }
        }
        return 0;
    }
}

/*-------------------------------------------------------------------------*/
/**
 * @class   Printer
 * @brief   Abstract base class of all Printer classes.
 *          Used to extract the CUBE Metric and Cnode objects based on the
 *          names entered by the user.
 *
 * This is the common base class of all Printer classes. It's purpose is to
 * provide access to the CUBE Metric and Cnode objects based on given names.
 * It also prints warnings if there are no Metrics or Cnodes for a given name.
 */
/*-------------------------------------------------------------------------*/
class Printer
{
    public:
        /// @name Constructor & destructor
        /// @{
        Printer(AggrCube *cubeObject, vector<string> const &metNames,
                vector<string> const &cnodeNames);
        virtual ~Printer() {}
        /// @}
        /// @name Getters for cube objects
        /// @{
        vector<Metric*> const& GetRequestedMetrics() const
                        {return requestedMetricVec;}
        vector<Cnode*>  const& GetRequestedCnodes() const
                        {return requestedCnodeVec;}
        /// @}
    private:
        vector<Metric*> requestedMetricVec;
        vector<Cnode*> requestedCnodeVec;
};

/**
 * Creates a new Printer instance from the given CUBE object and the names of
 * the requested metrics and cnodes.
 *
 * @param cubeObject The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to be
 * gathered and printed.
 * @param cnodeNames A vector containing all region names for which data is to
 * be gathered and printed.
 */
Printer::Printer(AggrCube *cubeObject, vector<string> const &metNames,
                 vector<string> const &cnodeNames)
{
    for(vector<string>::const_iterator metNameIT = metNames.begin();
        metNameIT != metNames.end(); ++metNameIT)
    {
        if (Metric* m = findMetric(cubeObject, *metNameIT))
        {
            requestedMetricVec.push_back(m);
        }
        else
        {
            cerr << "WARNING: Metric " << *metNameIT << " doesn't exist"
                 << endl;
        }
    }

    vector<Cnode*> const &cnodeVec = cubeObject->get_cnodev();
    for(vector<string>::const_iterator cnodeNameIT = cnodeNames.begin();
        cnodeNameIT != cnodeNames.end(); ++cnodeNameIT)
    {
        bool cnodeFound = false;
        for(vector<Cnode*>::const_iterator cnodeIT = cnodeVec.begin();
            cnodeIT != cnodeVec.end(); ++cnodeIT)
        {
            if(*cnodeNameIT == (*cnodeIT)->get_callee()->get_name())
            {
                requestedCnodeVec.push_back(*cnodeIT);
                cnodeFound = true;
            }
        }
        if(!cnodeFound)
        {
            cerr << "WARNING: Cnode with name " << *cnodeNameIT
                 << " doesn't exist" << endl;
        }
    }
}

/*-------------------------------------------------------------------------*/
/**
 * @class   StatisticPrinter
 * @brief   Abstract base class for printers that print statistical information
 *          gathered about all threads.
 *
 * This is the abstract base class of all Printer classes used for output with
 * statistical information. It already implements the GatherAndPrint method
 * which is used by client classes. The subclasses only have to implement the
 * PrintLegend and PrintStatistic methods.
 */
/*-------------------------------------------------------------------------*/
class StatisticPrinter : public Printer
{
    public:
        /// @name Constructor & destructor
        /// @{
        StatisticPrinter(AggrCube *cubeObject, vector<string> const &metNames,
                         vector<string> const &cnodeNames);
        virtual ~StatisticPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void GatherAndPrint(Metric *met, Cnode *cnode,
                                    AggrCube *cubeObject) const;
        virtual void PrintLegend() const = 0;
        /// @}
        /// The captions used for printing the legend
        static char const *const captions[];
    private:
        /// PrintStatistic method to be overwritten by subclasses to modify
        /// the way the statistics are printed
        virtual void PrintStatistic(Cnode *currCnode, Metric *met,
                     P2Statistic const &stat, bool indented = false,
                     string const &name = "") const = 0;
};

char const *const StatisticPrinter::captions[] = {"Sum", "Mean", "Variance",
                  "Minimum", "Quartile 25", "Median", "Quartile 75", "Maximum"};

/**
 * Creates a new StatisticPrinter instance from the given CUBE object and the
 * names of the requested metrics and cnodes.
 *
 * @param cubeObject The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to be
 * gathered and printed.
 * @param cnodeNames A vector containing all region names for which data is to
 * be gathered and printed.
 */
StatisticPrinter::StatisticPrinter(AggrCube *cubeObject,
    vector<string> const &metNames, vector<string> const &cnodeNames)
    : Printer(cubeObject, metNames, cnodeNames)
{
}

/**
 * Method for gathering and printing statistical information for the
 * distribution of metrics upon all threads. This method creates and prints a
 * statistic about the distribution of the given metric met on all threads
 * for the given Cnode cnode and all cnodes called by cnode
 *
 * @param met The metric for which the distribution among all threads is
 * calculated
 * @param cnode The statistics is gathered for this cnode and all it's child
 * cnodes
 * @param cubeObject The CUBE object used to extract the severity information
 */
void StatisticPrinter::GatherAndPrint(Metric *met, Cnode *cnode,
                                      AggrCube *cubeObject) const
{
    typedef Thread SysResType;

    //Gather and print the statistic for the given cnode inclusively
    //(that is with the severity caused by all its children)
    vector<SysResType*> const &processVec = cubeObject->get_thrdv();
    {
        P2Statistic stat;
        for(vector<SysResType*>::const_iterator it = processVec.begin();
            it != processVec.end(); ++it)
        {
            stat.add(cubeObject->get_vcsev(INCL, INCL, EXCL,  met, cnode, *it));
        }
        PrintStatistic(cnode, met, stat, false,
                       "INCL(" + cnode->get_callee()->get_name() + ")");
    }
    //Gather and print the statistic for the given cnode exclusively
    //(that is without the severity caused by its children)
    {
        P2Statistic stat;
        for(vector<SysResType*>::const_iterator it = processVec.begin();
            it != processVec.end(); ++it)
        {
            stat.add(cubeObject->get_vcsev(INCL, EXCL, EXCL,  met, cnode, *it));
        }
        PrintStatistic(cnode, met, stat, true,
                       "EXCL(" + cnode->get_callee()->get_name() + ")");
    }
    //Gather and print the statistics for all childs of the given cnode
    for(unsigned int i = 0; i < cnode->num_children(); ++i)
    {
        P2Statistic stat;
        Cnode *currCnode = cnode->get_child(i);
        for(vector<SysResType*>::const_iterator it = processVec.begin();
            it != processVec.end(); ++it)
        {
            stat.add(cubeObject->get_vcsev(INCL, INCL, EXCL,  met, currCnode,
                                           *it));
        }
        PrintStatistic(currCnode, met, stat, true);
    }
    cout << endl;
}

/*-------------------------------------------------------------------------*/
/**
 * @class   PrettyStatisticsPrinter
 * @brief   Class for printing pretty human readable tables with statistics
 */
/*-------------------------------------------------------------------------*/
class PrettyStatisticsPrinter : public StatisticPrinter
{
    public:
        /// @name Constructor & destructor
        /// @{
        PrettyStatisticsPrinter(AggrCube *cubeObj,
                                vector<string> const &metNames,
                                vector<string> const &cnodeNames);
        virtual ~PrettyStatisticsPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const;
        /// @}
    private:
        /// Virtual method overwrite of the abstract method used by
        /// StatisticPrinter::GatherAndPrint
        virtual void PrintStatistic(Cnode *currCnode, Metric *met,
                                    P2Statistic const &stat, bool indented,
                                    string const &name) const;
        /// Defines the widthes used for the columns of the statistics
        /// (e.g. the Sum, Min, Max,... columns) - compile time constant
        static int const widthes[];
        ///Widthes used for the cnode and metric name columns
        int maxCnodeNameLength;
        int maxMetricNameLength;
};

int const PrettyStatisticsPrinter::widthes[] = {15, 13, 14, 13, 15, 13, 15, 13};

/**
 * Creates a new PrettyStatisticsPrinter instance from the given CUBE object
 * and the names of the requested metrics and cnodes.
 *
 * @param cubeObj The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to
 * be gathered and printed.
 * @param cnodeNames A vector containing all region names for which data is to
 * be gathered and printed.
 */
PrettyStatisticsPrinter::PrettyStatisticsPrinter(AggrCube *cubeObj,
    vector<string> const &metNames, vector<string> const &cnodeNames)
    : StatisticPrinter(cubeObj, metNames, cnodeNames),
      maxCnodeNameLength(strlen("Routine")),
      maxMetricNameLength(0)
{
    //Calculate the width of the cnode name column
    vector<Cnode*> const &requestedCnodeVec = GetRequestedCnodes();
    for(vector<Cnode*>::const_iterator it = requestedCnodeVec.begin();
        it != requestedCnodeVec.end(); ++it)
    {
        maxCnodeNameLength = max<size_t>(maxCnodeNameLength,
                                      (*it)->get_callee()->get_name().length());
        for(unsigned int i = 0; i < (*it)->num_children(); ++i)
        {
            maxCnodeNameLength = max<size_t>(maxCnodeNameLength,
                    (*it)->get_child(i)->get_callee()->get_name().length() + 2);
        }
    }
    ++maxCnodeNameLength;

    //Calculate the width of the metric name column
    maxMetricNameLength = metNames[0].length();
    for(vector<string>::const_iterator it = metNames.begin();
        it != metNames.end(); ++it)
    {
        maxMetricNameLength = max<size_t>(maxMetricNameLength, it->length());
    }
    ++maxMetricNameLength;
}

/**
 * Prints the legend for the statistics using the widthes compile time constant
 * and the calculated widthes for the name columns
 */
void PrettyStatisticsPrinter::PrintLegend() const
{
    cout << setw(maxMetricNameLength) << left << "Metric"
         << setw(maxCnodeNameLength + 2) << left << "Routine"
         << setw(7) << right << "Count";
    for(size_t i = 0; i < sizeof(captions)/sizeof(char const*); ++i)
    {
        cout << setw(widthes[i]) << right << captions[i];
    }
    cout << endl;
}

/**
 * Virtual method overwrite of the abstract method used by
 * StatisticPrinter::GatherAndPrint. This method prints the given statistic
 * @p stat formatted in a nice way.
 *
 * @param currCnode Determines the current cnode. This is only used to obtain
 * the CUBE name of the region belonging to the cnode if no @p name is provided.
 * @param met The metric to which the statistic belongs. Only used to obtain
 * the metric name.
 * @param stat The statistic to be printed.
 * @param indented Determines whether the statistic is to be indented. This is
 * used for regions being called by another region, making it obvious that
 * it is a child of the first unindented region printed above.
 * @param name The name to be printed. Default is empty. If @p name is empty,
 * the CUBE name of the region is used as a default.
 */
void PrettyStatisticsPrinter::PrintStatistic(Cnode *currCnode, Metric *met,
     P2Statistic const &stat, bool indented, string const &name) const
{
    cout << setw(maxMetricNameLength) << left << met->get_uniq_name();

    //Indented items have 2 spaces in front
    if(indented)
    {
        cout << "  ";
        cout << setw(maxCnodeNameLength) << left;
    }
    //Therefore unindented items have to be printed in a column which is
    //2 characters larger
    else
    {
        cout << setw(maxCnodeNameLength + 2) << left;
    }

    //If no name is given use the CUBE name as a default
    if(name.length() == 0)
    {
        cout << currCnode->get_callee()->get_name();
    }
    else
    {
        cout << name;
    }

    //values is an array of method pointers, allowing to access the statistic
    //information conveniently
    double (P2Statistic::*values[])() const = {
           &P2Statistic::sum, &P2Statistic::mean, &P2Statistic::var,
           &P2Statistic::min, &P2Statistic::q25, &P2Statistic::med,
           &P2Statistic::q75, &P2Statistic::max};
    //Array containing the minimal count a statistic object has to have in
    //order to provide a specific value
    //For example P2Statistic::q25 = values[4] requires a minimum count of 5
    //= minCount[4]
    int const minCount[] = {0, 0, 2, 0, 5, 0, 5, 0};
    //Array stating which statistic values are integers, if the metric is of
    //type integer (e.g. for hardware counters)
    bool const integerValues[] =
                          {true, false, false, true, false, false, false, true};
    //Print the statistic
    cout << setw(7) << right << stat.count();
    for(size_t i = 0; i < sizeof(values)/sizeof(double (P2Statistic::*)()); ++i)
    {
        cout << setw(widthes[i]) << right << fixed;
        if(stat.count() >= minCount[i])
        {
            if(integerValues[i] && met->get_dtype() == "INTEGER")
            {
                cout << noshowpoint << setprecision(0);
            }
            cout << (stat.*values[i])();
            cout << showpoint << setprecision(6);
        }
        else
        {
            cout << "";
        }
    }
    cout << endl;
}

/*-------------------------------------------------------------------------*/
/**
 * @class   CSVStatisticsPrinter
 * @brief   Class for printing comma seperated value files with statistics
 */
/*-------------------------------------------------------------------------*/
class CSVStatisticsPrinter : public StatisticPrinter
{
    public:
        /// @name Constructor & destructor
        /// @{
        CSVStatisticsPrinter(AggrCube *cubeObject,
                             vector<string> const &metNames,
                             vector<string> const &cnodeNames);
        virtual ~CSVStatisticsPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const;
        /// @}
    private:
        /// Virtual method overwrite of the abstract method used by
        /// StatisticPrinter::GatherAndPrint
        virtual void PrintStatistic(Cnode *currCnode, Metric *met,
                                    P2Statistic const &stat, bool indented,
                                    string const &name) const;
};

/**
 * Creates a new CSVStatisticsPrinter instance from the given CUBE object and
 * the names of the requested metrics and cnodes.
 *
 * @param cubeObject The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to be
 * gathered and printed.
 * @param cnodeNames A vector containing all region names for which data is to
 * be gathered and printed.
 */
CSVStatisticsPrinter::CSVStatisticsPrinter(AggrCube *cubeObject,
    vector<string> const &metNames, vector<string> const &cnodeNames)
    : StatisticPrinter(cubeObject, metNames, cnodeNames)
{
}

/**
 * Prints the legend for the statistics as a line of a comma seperated value
 * file
 */
void CSVStatisticsPrinter::PrintLegend() const
{
    cout << "Metric" << ',' << "Routine" << ',' << "Count";
    cout << ',' << captions[0];
    for(size_t i = 1; i < sizeof(captions)/sizeof(char const*); ++i)
    {
        cout << ',' << captions[i];
    }
    cout << endl;
}

/**
 * Virtual method overwrite of the abstract method used by
 * StatisticPrinter::GatherAndPrint.  This method prints the given statistic
 * @p stat formatted as a part of a comma seperated value file.
 *
 * @param currCnode Determines the current cnode. This is only used to obtain
 * the CUBE name of the region belonging to the cnode if no @p name is provided.
 * @param met The metric to which the statistic belongs. Only used to obtain
 * the metric name.
 * @param stat The statistic to be printed.
 * @param name The name to be printed. Default is empty. If @p name is empty,
 * the CUBE name of the region is used as a default.
 */
void CSVStatisticsPrinter::PrintStatistic(Cnode *currCnode, Metric *met,
     P2Statistic const &stat, bool, string const &name) const
{
    cout << met->get_uniq_name() << ',';
    //If no name is given use the CUBE name as a default
    if(name.length() == 0)
    {
        cout << currCnode->get_callee()->get_name();
    }
    else
    {
        cout << name;
    }

    //values is an array of method pointers, allowing to access the statistic
    //information conveniently
    double (P2Statistic::*values[])() const = {
           &P2Statistic::sum, &P2Statistic::mean, &P2Statistic::var,
           &P2Statistic::min, &P2Statistic::q25, &P2Statistic::med,
           &P2Statistic::q75, &P2Statistic::max};
    //Array containing the minimal count a statistic object has to have in
    //order to provide a specific value
    //For example P2Statistic::q25 = values[4] requires a minimum count of 5
    //= minCount[4]
    int const minCount[] = {0, 0, 2, 0, 5, 0, 5, 0};
    //Array stating which statistic values are integers, if the metric is of
    //type integer (e.g. for hardware counters)
    bool const integerValues[] =
                          {true, false, false, true, false, false, false, true};

    //Print the statistic
    cout << "," << stat.count();
    for(size_t i = 0; i < sizeof(values)/sizeof(double (P2Statistic::*)()); ++i)
    {
        if(stat.count() >= minCount[i])
        {
            if(integerValues[i] && met->get_dtype() == "INTEGER")
            {
                cout << fixed << noshowpoint << setprecision(0);
            }
            cout << "," << (stat.*values[i])();
            cout.setf(ios_base::fmtflags(0), ios_base::floatfield);
            cout << showpoint << setprecision(6);
        }
        else
        {
            cout << ",";
        }
    }
    cout << endl;
}

/*-------------------------------------------------------------------------*/
/**
 * @class   PlainPrinter
 * @brief   Abstract base class for printers that print severity data, but do
 * not print statistical information gathered about all threads.
 *
 * This is the abstract base class of all Printer classes used for output
 * without statistical information.
 */
/*-------------------------------------------------------------------------*/
class PlainPrinter : public Printer
{
    public:
        /// @name Constructor & destructor
        /// @{
        PlainPrinter(AggrCube *cubeObject, vector<string> const &metNames,
                     vector<string> const &cnodeNames);
        virtual ~PlainPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const = 0;
        virtual void GatherAndPrint(Cnode *cnode) const = 0;
        /// @}
};

/**
 * Creates a new PlainPrinter instance from the given CUBE object and the names
 * of the requested metrics and cnodes.
 *
 * @param cubeObject The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to be
 * gathered and printed.
 * @param cnodeNames A vector containing all region names for which data is to
 * be gathered and printed.
 */
PlainPrinter::PlainPrinter(AggrCube *cubeObject, vector<string> const &metNames,
    vector<string> const &cnodeNames)
    : Printer(cubeObject, metNames, cnodeNames)
{
}

/*-------------------------------------------------------------------------*/
/**
 * @class   PrettyPlainPrinter
 * @brief   Class for printing pretty human readable tables
 */
/*-------------------------------------------------------------------------*/
class PrettyPlainPrinter : public PlainPrinter
{
    public:
        /// @name Constructor & destructor
        /// @{
        PrettyPlainPrinter(AggrCube *cubeObj, vector<string> const &metNames,
                           vector<string> const &cnodeNames);
        virtual ~PrettyPlainPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const;
        virtual void GatherAndPrint(Cnode *cnode) const;
        /// @}
    private:
        AggrCube *cubeObject;
        size_t maxCnodeNameLength;
        vector<int> columnWidthes;

        /// Private helper method for gathering and printing the data
        /// (without names)
        void PrintValues(Cnode *cnode, inclmode cnodeMode = INCL) const;
};

/**
 * Creates a new PrettyPlainPrinter instance from the given CUBE object and the
 * names of the requested metrics and cnodes.
 *
 * @param cubeObj The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to be
 * gathered and printed.
 * @param cnodeNames A vector containing all region names for which data is to
 * be gathered and printed.
 */
PrettyPlainPrinter::PrettyPlainPrinter(AggrCube *cubeObj,
    vector<string> const &metNames, vector<string> const &cnodeNames)
    : PlainPrinter(cubeObj, metNames, cnodeNames), cubeObject(cubeObj),
      maxCnodeNameLength(strlen("routine"))
{
    //Calculate the width of the cnode name column
    vector<Cnode*> const &requestedCnodeVec = GetRequestedCnodes();
    for(vector<Cnode*>::const_iterator it = requestedCnodeVec.begin();
        it != requestedCnodeVec.end(); ++it)
    {
        maxCnodeNameLength = max(maxCnodeNameLength,
                   (*it)->get_callee()->get_name().length() + strlen("INCL()"));
        for(unsigned int i = 0; i < (*it)->num_children(); ++i)
        {
            maxCnodeNameLength = max(maxCnodeNameLength,
                    (*it)->get_child(i)->get_callee()->get_name().length() + 2);
        }
    }
    ++maxCnodeNameLength;

    //Calculate column widthes for the different metric columns
    for(vector<string>::const_iterator it = metNames.begin();
        it != metNames.end(); ++it)
    {
        columnWidthes.push_back(max<size_t>(it->length()+1, 13));
    }
}

/**
 * Prints the legend for the data using the column widthes and the width for
 * the cnode name column calculated in the constructor 
 */
void PrettyPlainPrinter::PrintLegend() const
{
    cout << setw(maxCnodeNameLength) << left << "Routine";
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    for(size_t i = 0; i < requestedMetricVec.size(); ++i)
    {
        cout << setw(columnWidthes[i]) << right
             << requestedMetricVec[i]->get_uniq_name();
    }
    cout << endl;
    cout << fixed << showpoint << setprecision(6);
}

/**
 * Method for gathering and printing data about the severity of certain metrics
 * with respect to given cnodes and their children. This method gathers and
 * prints data about the requested metrics for the given Cnode cnode and all
 * cnodes called by cnode
 *
 * @param cnode The data is gathered for this cnode and all it's child cnodes.
 */
void PrettyPlainPrinter::GatherAndPrint(Cnode *cnode) const
{
    //First print data for the given cnode inclusively
    //and exclusively its children
    inclmode modes[] = {INCL, EXCL};
    for(size_t i = 0; i < sizeof(modes)/sizeof(inclmode); ++i)
    {
        cout << setw(maxCnodeNameLength) << left;
        if(modes[i] == INCL)
        {
            cout << ("INCL(" + cnode->get_callee()->get_name() + ")");
        }
        else
        {
            cout << ("  EXCL(" + cnode->get_callee()->get_name() + ")");
        }
        PrintValues(cnode, modes[i]);
    }

    //Print data for all children of the given cnode
    for(unsigned int i = 0; i < cnode->num_children(); ++i)
    {
        cout << setw(maxCnodeNameLength) << left
             << ("  " + cnode->get_child(i)->get_callee()->get_name());
        PrintValues(cnode->get_child(i), INCL);
    }
}

/**
 * Private helper method for gathering and printing the data (without names)
 * for a given cnode
 *
 * @param cnode The data is gathered for this cnode.
 * @param cnodeMode Determines the inclusion mode. If it is INCL, the
 * severities of the child cnodes are added. If it is EXCL, they are not.
 */
void PrettyPlainPrinter::PrintValues(Cnode *cnode, inclmode cnodeMode) const
{
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    vector<Thread*> const &threadVec = cubeObject->get_thrdv();
    for(size_t i = 0; i < requestedMetricVec.size(); ++i)
    {
        double severity = 0.0;
        for(vector<Thread*>::const_iterator threadIT = threadVec.begin();
            threadIT != threadVec.end(); ++threadIT)
        {
            severity += cubeObject->get_vcsev(INCL, cnodeMode, EXCL,
                                       requestedMetricVec[i], cnode, *threadIT);
        }
        cout << setw(columnWidthes[i]) << right;
        if(requestedMetricVec[i]->get_dtype() == "INTEGER")
        {
            cout << noshowpoint << setprecision(0) << severity;
        }
        else
        {
            cout << fixed << severity;
        }
        cout << showpoint << setprecision(6);
    }
    cout << endl;
}

/*-------------------------------------------------------------------------*/
/**
 * @class   CSVPlainPrinter
 * @brief   Class for printing data about severities of a metric on a chosen
 *          cnode as a comma seperated value file
 */
/*-------------------------------------------------------------------------*/
class CSVPlainPrinter : public PlainPrinter
{
    public:
        /// @name Constructor & destructor
        /// @{
        CSVPlainPrinter(AggrCube *cubeObj, vector<string> const &metNames,
                        vector<string> const &cnodeNames);
        virtual ~CSVPlainPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const;
        virtual void GatherAndPrint(Cnode *cnode) const;
        /// @}
    private:
        AggrCube *cubeObject;

        /// Private helper method for gathering and printing the data
        /// (without names)
        void PrintValues(Cnode *cnode, inclmode cnodeMode = INCL) const;
};

/**
 * Creates a new CSVPlainPrinter instance from the given CUBE object and the
 * names of the requested metrics and cnodes.
 *
 * @param cubeObj The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to
 * be gathered and printed.
 * @param cnodeNames A vector containing all region names for which data is to
 * be gathered and printed.
 */
CSVPlainPrinter::CSVPlainPrinter(AggrCube *cubeObj,
    vector<string> const &metNames, vector<string> const &cnodeNames)
    : PlainPrinter(cubeObj, metNames, cnodeNames), cubeObject(cubeObj)
{
}

/**
 * Prints the legend for the statistics as a line of a comma seperated value
 * file
 */
void CSVPlainPrinter::PrintLegend() const
{
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    cout << "Routine";
    for(vector<Metric*>::const_iterator it = requestedMetricVec.begin();
        it != requestedMetricVec.end(); ++it)
    {
        cout << ',' << (*it)->get_uniq_name();
    }
    cout << endl;
}

/**
 * Method for gathering and printing data about the severity of certain metrics
 * with respect to given cnodes and their children. This method gathers and
 * prints data about the requested metrics for the given Cnode cnode and all
 * cnodes called by cnode.
 *
 * @param cnode The data is gathered for this cnode and all it's child cnodes.
 */
void CSVPlainPrinter::GatherAndPrint(Cnode *cnode) const
{
    //First print data for the given cnode inclusively and
    //exclusively its children
    inclmode modes[] = {INCL, EXCL};
    for(size_t i = 0; i < sizeof(modes)/sizeof(inclmode); ++i)
    {
        if(modes[i] == INCL)
        {
            cout << ("INCL(" + cnode->get_callee()->get_name() + ")");
        }
        else
        {
            cout << ("EXCL(" + cnode->get_callee()->get_name() + ")");
        }
        PrintValues(cnode, modes[i]);
    }

    //Print data for all children of the given cnode
    for(unsigned int i = 0; i < cnode->num_children(); ++i)
    {
        cout << cnode->get_child(i)->get_callee()->get_name();
        PrintValues(cnode->get_child(i), INCL);
    }
}

/**
 * Private helper method for gathering and printing the data (without names)
 * for a given cnode
 *
 * @param cnode The data is gathered for this cnode.
 * @param cnodeMode Determines the inclusion mode. If it is INCL, the
 * severities of the child cnodes are added. If it is EXCL, they are not.
 */
void CSVPlainPrinter::PrintValues(Cnode *cnode, inclmode cnodeMode) const
{
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    vector<Thread*> const &threadVec = cubeObject->get_thrdv();
    for(vector<Metric*>::const_iterator reqMetIT = requestedMetricVec.begin();
        reqMetIT != requestedMetricVec.end(); ++reqMetIT)
    {
        double severity = 0.0;
        for(vector<Thread*>::const_iterator threadIT = threadVec.begin();
            threadIT != threadVec.end(); ++threadIT)
        {
            severity += cubeObject->get_vcsev(INCL, cnodeMode, EXCL, *reqMetIT,
                                              cnode, *threadIT);
        }
        cout << ',';
        if((*reqMetIT)->get_dtype() == "INTEGER")
        {
            cout << fixed << noshowpoint << setprecision(0);
        }
        cout << severity;
        cout.setf(ios_base::fmtflags(0), ios_base::floatfield);
        cout << showpoint << setprecision(6);
    }
    cout << endl;
}

/*-------------------------------------------------------------------------*/
/**
 * @class   TopNPrinter
 * @brief   Abstract base class for printers that print flat profiles.
 *
 * This is the abstract base class of all Printer classes used for printing
 * of topN flat profiles.
 */
/*-------------------------------------------------------------------------*/
class TopNPrinter : public Printer
{
    public:
        /// @name Constructor & destructor
        /// @{
        TopNPrinter(AggrCube *cubeObject, vector<string> const &metNames);
        virtual ~TopNPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const = 0;
        virtual void Print(int n) const = 0;
        /// @}

    protected:
        /// Vector of requested metroc value vectors
        vector< vector<double> > values;
        /// Index vector represending sort order (per exclusive time)
        vector<int> indexVec;

    private:
        static vector<string> const dummyEmptyCnodenames;
};

vector<string> const TopNPrinter::dummyEmptyCnodenames;

namespace {
/*-------------------------------------------------------------------------*/
/**
 * @class   indirectCmp
 * @brief   Greater functor for index sorting
 *
 * Functor calculating "Greater" used for generating index vector.
 * Constructor argument "vals" is reference to base vector which values
 * determine the sort order.
 */
/*-------------------------------------------------------------------------*/
    class indirectCmp
    {
        public:
            indirectCmp(vector<double> &vals) : myVals(vals) {}
            bool operator()(int l, int r) const
            {
                return myVals[l] > myVals[r];
            }
        private:
            vector<double> &myVals;
    };
}

/**
 * Creates a new TopNPrinter instance from the given CUBE object and the names
 * of the requested metrics. Gathers requested metic value vectors and
 * determines sort order.
 *
 * @param cubeObject The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is to
 * be gathered and printed.
 */
TopNPrinter::TopNPrinter(AggrCube *cubeObject, vector<string> const &metNames)
    : Printer(cubeObject, metNames, dummyEmptyCnodenames)
{
    // Gather values
    const int inclTimeMetricID = 2;
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    vector<Region*> const &regionsVec = cubeObject->get_regv();
    int num_metrics = requestedMetricVec.size();
    int num_regions = regionsVec.size();
    values.resize(requestedMetricVec.size());

    for(int m = 0; m < num_metrics; ++m)
    {
        vector<double>& curr_met_values = values[m];
        curr_met_values.resize(num_regions);
#ifdef _OPENMP
        #pragma omp parallel for
#endif
        for (int r = 0; r < num_regions; ++r)
        {
            curr_met_values[r] = cubeObject->get_vrsev(INCL,
                                       (m==inclTimeMetricID?INCL:EXCL), INCL,
                                       requestedMetricVec[m], regionsVec[r],
                                       0 /* = machine */);
        }
    }

    // Create index vector, i.e. sorting order for exclusive time
    const int exclTimeMetricID = 1;
    indexVec.resize(num_regions);
    for (int r = 0; r < num_regions; ++r) indexVec[r] = r;
    sort(indexVec.begin(), indexVec.end(), indirectCmp(values[exclTimeMetricID]));
}

/*-------------------------------------------------------------------------*/
/**
 * @class   CSVTopNPrinter
 * @brief   Class for printing topN flat profile data as a comma seperated
 *          value file
 */
/*-------------------------------------------------------------------------*/
class CSVTopNPrinter : public TopNPrinter
{
    public:
        /// @name Constructor & destructor
        /// @{
        CSVTopNPrinter(AggrCube *cubeObj, vector<string> const &metNames);
        virtual ~CSVTopNPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const;
        virtual void Print(int n) const;
        /// @}
    private:
        AggrCube *cubeObject;
};

/**
 * Creates a new CSVTopNPrinter instance from the given CUBE object and
 * the names of the requested metrics and cnodes.
 *
 * @param cubeObj The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is
 * to be gathered and printed.
 */
CSVTopNPrinter::CSVTopNPrinter(AggrCube *cubeObj,
    vector<string> const &metNames)
    :TopNPrinter(cubeObj, metNames), cubeObject(cubeObj)
{
}

/**
 * Prints the legend for topN flat profile as a line of a comma seperated
 * value file
 */
void CSVTopNPrinter::PrintLegend() const
{
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    int num_metrics = requestedMetricVec.size();
    const int inclTimeMetricID = 2;
    
    cout << "Region,Number of Calls,Exclusive Time,Inclusive Time";
    for(int m = inclTimeMetricID+1; m < num_metrics; ++m)
    {
        cout << ',' << requestedMetricVec[m]->get_uniq_name();
    }
    cout << endl;
}

/**
 * Prints the values for the @p n top regions as a topN flat profile
 * as a line of a comma seperated value file
 *
 * @param n Number of regions to print
 */
void CSVTopNPrinter::Print(int n) const
{
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    vector<Region*> const &regionsVec = cubeObject->get_regv();
    int num_metrics = values.size();
    int num_regions = regionsVec.size();
    const int visitsMetricID = 0;

    for (int r = 0; r < num_regions && r < n; ++r)
    {
        int rr = indexVec[r];
        if (values[visitsMetricID][rr] == 0)
        {
            //abort loop when functions got never called
            break;
        }
        cout << regionsVec[rr]->get_name();
        for(int m = 0; m < num_metrics; ++m)
        {
            vector<double> const &curr_met_values = values[m];
            cout << ',';
            if(requestedMetricVec[m]->get_dtype() == "INTEGER")
            {
                cout << fixed << noshowpoint << setprecision(0);
            }
            cout << curr_met_values[rr];
            cout.setf(ios_base::fmtflags(0), ios_base::floatfield);
            cout << showpoint << setprecision(6);
        }
        cout << endl;
    }
}

/*-------------------------------------------------------------------------*/
/**
 * @class   PrettyTopNPrinter
 * @brief   Class for printing topN flat profile data as a comma seperated
 *          value file
 */
/*-------------------------------------------------------------------------*/
class PrettyTopNPrinter : public TopNPrinter
{
    public:
        /// @name Constructor & destructor
        /// @{
        PrettyTopNPrinter(AggrCube *cubeObj, vector<string> const &metNames,
                          int n);
        virtual ~PrettyTopNPrinter() {}
        /// @}
        /// @name Print methods used by clients
        /// @{
        virtual void PrintLegend() const;
        virtual void Print(int n) const;
        /// @}
    private:
        AggrCube *cubeObject;
        size_t maxRegionNameLength;
        vector<int> columnWidthes;
};

/**
 * Creates a new PrettyTopNPrinter instance from the given CUBE object and
 * the names of the requested metrics and cnodes.
 *
 * @param cubeObj The CUBE object used to parse the CUBE file to be analyzed.
 * @param metNames A vector containing all metric names for which data is
 * to be gathered and printed.
 * @param n Maximum number of top @p n functions to print
 */
PrettyTopNPrinter::PrettyTopNPrinter(AggrCube *cubeObj,
    vector<string> const &metNames, int n)
    :TopNPrinter(cubeObj, metNames), cubeObject(cubeObj),
     maxRegionNameLength(strlen("Region"))
{
    //Calculate the width of the region name column
    vector<Region*> const &regionsVec = cubeObject->get_regv();
    int num_regions = regionsVec.size();
    const int visitsMetricID = 0;

    for (int r = 0; r < num_regions && r < n; ++r)
    {
        int rr = indexVec[r];
        if (values[visitsMetricID][rr] == 0)
        {
            //abort loop when functions got never called
            break;
        }
        maxRegionNameLength = max(maxRegionNameLength,
                                  regionsVec[rr]->get_name().length());
    }
    ++maxRegionNameLength;

    //Calculate column widthes for the different metric columns
    for(vector<string>::const_iterator it = metNames.begin();
        it != metNames.end(); ++it)
    {
        columnWidthes.push_back(max<size_t>(it->length()+1, 14));
    }
}

/**
 * Prints the legend for topN flat profile as a line of a comma seperated
 * value file
 */
void PrettyTopNPrinter::PrintLegend() const
{
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    int num_metrics = requestedMetricVec.size();
    const int inclTimeMetricID = 2;

    cout << setw(maxRegionNameLength) << left << "Region";
    cout << setw(14) << right << "NumberOfCalls";
    cout << setw(14) << right << "ExclusiveTime";
    cout << setw(14) << right << "InclusiveTime";

    for(int m = inclTimeMetricID+1; m < num_metrics; ++m)
    {
        cout << setw(columnWidthes[m]) << right
             << requestedMetricVec[m]->get_uniq_name();
    }
    cout << endl;
    cout << fixed << showpoint << setprecision(6);
}

/**
 * Prints the values for the @p n top regions as a topN flat profile
 * as a line of a comma seperated value file
 *
 * @param n Number of regions to print
 */
void PrettyTopNPrinter::Print(int n) const
{
    vector<Metric*> const &requestedMetricVec = GetRequestedMetrics();
    vector<Region*> const &regionsVec = cubeObject->get_regv();
    int num_metrics = values.size();
    int num_regions = regionsVec.size();
    const int visitsMetricID = 0;

    for (int r = 0; r < num_regions && r < n; ++r)
    {
        int rr = indexVec[r];
        if (values[visitsMetricID][rr] == 0)
        {
            //abort loop when functions got never called
            break;
        }
        cout << setw(maxRegionNameLength) << left << regionsVec[rr]->get_name();
        for(int m = 0; m < num_metrics; ++m)
        {
            vector<double> const &curr_met_values = values[m];
            cout << setw(columnWidthes[m]) << right;
            if(requestedMetricVec[m]->get_dtype() == "INTEGER")
            {
                cout << noshowpoint << setprecision(0);
            }
            else
            {
                cout << fixed;
            }
            cout << curr_met_values[rr];
            cout << showpoint << setprecision(6);
        }
        cout << endl;
    }
}

/*-------------------------------------------------------------------------*/
/**
 * @class   CubeStatistics
 * @brief   Main class providing the functionality for cube3_stat
 *
 * The CubeStatistics class provides the main functinality for the
 * cube3_stat tool. It parses the given command line parameters and
 * can than be used to print the statistical information accordingly.
 */
/*-------------------------------------------------------------------------*/

class CubeStatistics
{
    public:
        /// @name Constructor & destructor
        /// @{
        CubeStatistics(int argc, char *argv[]);
        ~CubeStatistics();
        /// @}
        /// @name Printing the selected data
        /// @{
        void Print() const;
        /// @}
    private:
        ///Metric names chosen by the user to be printed
        vector<string> metricNames;
        ///Cnode names chosen by the user to be printed
        vector<string> cnodeNames;
        ///Cube object used to read the selected cube file
        AggrCube *cubeObject;
        ///Printer object used for output with statistics
        StatisticPrinter *myStatPrinter;
        ///Printer object used for output without statistics
        PlainPrinter *myPlainPrinter;
        ///Printer object used for topN output
        TopNPrinter *myTopNPrinter;
        ///Flag indicating wether to print statistics
        bool printStatistics;
        ///Number of regions for topN profile
        int top;
        /// @name Internal parsing and printing routines
        /// @{
        static void split(string const &str, vector<string> &vec,
                          char seperator = ',');
        void PrintWithStatistics() const;
        void PrintWithoutStatistics() const;
        void PrintUsage() const;
        /// @}
};

/**
 * Helper method for command line parsing.
 * This method splits the string @p str at all occurences of the given
 * character @p seperator and saves all resulting parts into the passed
 * vector @p vec.
 *
 * @param str The string to be split
 * @param vec Vector of strings in which the resulting parts are stored
 * @param seperator The character at which str is to be split. Default: ','
 */
void CubeStatistics::split(string const &str, vector<string> &vec,
                           char seperator)
{
    // offset is the current reading position
    string::size_type offset = 0;
    // commaPos is the position of the next found comma
    string::size_type commaPos = 0;
    while(offset != string::npos && offset < str.length())
    {
        commaPos = str.find(seperator, offset);
        if(commaPos != string::npos)
        {
            vec.push_back(str.substr(offset, commaPos - offset));
            offset = commaPos + 1;
        }
        else
        {
            vec.push_back(str.substr(offset));
            offset = commaPos;
        }
    }
}

/**
 * Creates a new CubeStatistics instance from the given command line options.
 * Available options:
 *
 *    -p: Pretty print. This will switch all the output to nice human readable
 *        tables. If this is not provided the output is just a comma seperated
 *        value file.
 *    -m: Metrics. This option has to be followed by a (comma seperated) list
 *        of (unique) names of metrics. If this option is not provided, "time"
 *        is used as the default. If "time" can't be found, the metric
 *        with metric id of zero (with respect to the cube file) is chosen.
 *
 * Either call tree profile (-r [-%]) or flat profiles (-t) can be
 * generated and printed
 *
 *    -r: Regions for which 1-level call tree profiles are requested.
 *        This option has to be followed by a (comma seperated) list
 *        of region names. If this option is not provided, "main" is used as
 *        the default. If "main" can't be found, the region of the cnode with
 *        cnode id of zero (with respect to the cube file) is chosen.
 *    -%: Print statistics. This is used to print statistical information for
 *        the given metrics. The statistics describe the distribution of the
 *        metrics onto the different threads.
 *
 *    -t: Number of regions for which a topN flat profile is requested.
 *
 * @param argc Argument count (just pass the argument of main)
 * @param argv Array of command line arguments (just pass the argument of main)
 */
CubeStatistics::CubeStatistics(int argc, char *argv[])
    : cubeObject(0), myStatPrinter(0), myPlainPrinter(0), myTopNPrinter(0),
      printStatistics(false), top(0)
{
    // Parse the command line arguments
    int c = -1;
    bool pretty = false;
    while((c = getopt(argc, argv, "h?m:pr:t:%")) != -1)
    {
        switch(c)
        {
            case 'h':
            case '?':
                PrintUsage();
                break;
            case 'p':
                pretty = true;
                break;
            case '%':
                if(top)
                {
                  cerr << "ERROR: Options -% and -t cannot be used together."
                       << endl;
                  exit(1);
                }
                printStatistics = true;
                break;
            case 'm':
                split(optarg, metricNames);
                break;
            case 'r':
                if(top)
                {
                  cerr << "ERROR: Options -r and -t cannot be used together."
                       << endl;
                  exit(1);
                }
                split(optarg, cnodeNames);
                break;
            case 't':
                if(!cnodeNames.empty())
                {
                  cerr << "ERROR: Options -r and -t cannot be used together."
                       << endl;
                  exit(1);
                }
                else if(printStatistics)
                {
                  cerr << "ERROR: Options -% and -t cannot be used together."
                       << endl;
                  exit(1);
                }
                top = atoi(optarg);
                break;
        }
    }
    if((argc-optind) != 1)
    {
        PrintUsage();
    }
    else if((argc-optind) == 1)
    {
      char * filename = argv[optind];
      if (check_file(filename) != 0 ) exit(-1);


#ifndef CUBE_COMPRESSED
        ifstream cubeFile(filename);
#else
        gzifstream cubeFile(filename, ios_base::in|ios_base::binary);
#endif
        if(!cubeFile.is_open())
        {
            cerr << "ERROR: Could not open file '" << argv[optind]
                 << "'." << endl;
            exit(1);
        }
        cubeObject = new AggrCube();
        cubeFile >> *cubeObject;
    }

    // If topN flat profile requested, get default metrics
    // (visits, excl time, incl time)
    if(top)
    {
         // Insert metric "time" twice (for incl + excl)
         if (Metric* m = findMetric(cubeObject, "time"))
         {
             metricNames.insert(metricNames.begin(), 2, m->get_uniq_name());
         }
         else if (Metric* m = findMetric(cubeObject, "Time", false))
         {
             metricNames.insert(metricNames.begin(), 2, m->get_uniq_name());
         }
         else
         {
            cerr << "ERROR: Cannot find metric 'Time'." << endl;
            exit(1);
         }

         // Insert metric "visits"
         if (Metric* m = findMetric(cubeObject, "visits"))
         {
             metricNames.insert(metricNames.begin(), m->get_uniq_name());
         }
         else if (Metric* m = findMetric(cubeObject, "Visits", false))
         {
             metricNames.insert(metricNames.begin(), m->get_uniq_name());
         }
         else
         {
            cerr << "ERROR: Cannot find metric 'Visits'." << endl;
            exit(1);
         }
    }

    // If no metrics are provided (for non flat profiles) use the default
    // ("time" or the metric with an id of zero)
    if(metricNames.empty() && top == 0)
    {
        vector<Metric*> const &metVec = cubeObject->get_metv();
        vector<Metric*>::const_iterator zeroMetricIDIt = metVec.begin();
        bool timeFound = false;
        for(vector<Metric*>::const_iterator it = metVec.begin();
            it != metVec.end(); ++it)
        {
            if((*it)->get_uniq_name() == "time")
            {
                timeFound = true;
            }
            if((*it)->get_id() == 0)
            {
                zeroMetricIDIt = it;
            }
        }
        if(timeFound)
        {
            metricNames.push_back("time");
        }
        else
        {
            metricNames.push_back((*zeroMetricIDIt)->get_uniq_name());
        }
    }

    if(top)
    {
        //FLAT PROFILE (TOP top)
        //Create the appropiate Printer objects
        if(pretty)
        {
            myTopNPrinter =
                new PrettyTopNPrinter(cubeObject, metricNames, top);
        }
        else
        {
            myTopNPrinter =
                new CSVTopNPrinter(cubeObject, metricNames);
        }
    }
    else
    {
        //1-LEVEL CALLTREE PROFILE
        //If no region names are provided use the default ("main" or the
        //region name which belongs to the cnode with an id of zero)
        if(cnodeNames.empty())
        {
            vector<Cnode*> const &cnodeVec = cubeObject->get_cnodev();
            vector<Cnode*>::const_iterator zeroCnodeIDIt = cnodeVec.begin();
            bool mainFound = false;
            for(vector<Cnode*>::const_iterator it = cnodeVec.begin();
                it != cnodeVec.end(); ++it)
            {
                if((*it)->get_callee()->get_name() == "main")
                {
                    mainFound = true;
                }
                if((*it)->get_id() == 0)
                {
                    zeroCnodeIDIt = it;
                }
            }
            if(mainFound)
            {
                cnodeNames.push_back("main");
            }
            else
            {
                cnodeNames.push_back((*zeroCnodeIDIt)->get_callee()->get_name());
            }
        }

        //Create the appropiate Printer objects
        if(pretty)
        {
            if(printStatistics)
                myStatPrinter  = new PrettyStatisticsPrinter(cubeObject,
                                     metricNames, cnodeNames);
            else
                myPlainPrinter = new PrettyPlainPrinter(cubeObject,
                                     metricNames, cnodeNames);
        }
        else
        {
            if(printStatistics)
                myStatPrinter = new CSVStatisticsPrinter(cubeObject,
                                    metricNames, cnodeNames);
            else
               myPlainPrinter = new CSVPlainPrinter(cubeObject,
                                    metricNames, cnodeNames);
        }
    }
}

/**
 * Prints the selected data according to the command line arguments used for
 * initialization of the CubeStatistics object.
 */
void CubeStatistics::Print() const
{
    if(printStatistics)
    {
        PrintWithStatistics();
    }
    else
    {
        PrintWithoutStatistics();
    }
}

/**
 * Prints the selected data with statistic information describing the
 * distribution of the selected metrics upon the different threads.
 */
void CubeStatistics::PrintWithStatistics() const
{
    typedef vector<Metric*>::const_iterator MetricIT;
    typedef vector<Cnode*>::const_iterator CnodeIT;

    myStatPrinter->PrintLegend();
    vector<Metric*> const &requestedMetricVec =
                                           myStatPrinter->GetRequestedMetrics();
    vector<Cnode*> const &requestedCnodeVec =
                                            myStatPrinter->GetRequestedCnodes();
    for(MetricIT requestedMetricIT = requestedMetricVec.begin();
        requestedMetricIT != requestedMetricVec.end(); ++requestedMetricIT)
    {
        for(CnodeIT requestedCnodeIT = requestedCnodeVec.begin();
            requestedCnodeIT != requestedCnodeVec.end(); ++requestedCnodeIT)
        {
            myStatPrinter->GatherAndPrint(*requestedMetricIT, *requestedCnodeIT,
                                          cubeObject);
        }
    }
}

/**
 * Prints the selected data without statistic information,
 * thus only calculating the sum for the selected metrics over all threads.
 */
void CubeStatistics::PrintWithoutStatistics() const
{
    if(top)
    {
        myTopNPrinter->PrintLegend();
        myTopNPrinter->Print(top);
    }
    else
    {
        typedef vector<Cnode*>::const_iterator CnodeIT;

        myPlainPrinter->PrintLegend();
        vector<Cnode*> const &requestedCnodeVec =
                                           myPlainPrinter->GetRequestedCnodes();
        for(CnodeIT requestedCnodeIT = requestedCnodeVec.begin();
            requestedCnodeIT != requestedCnodeVec.end(); ++requestedCnodeIT)
        {
            myPlainPrinter->GatherAndPrint(*requestedCnodeIT);
            cout << endl;
        }
    }
}

CubeStatistics::~CubeStatistics()
{
    delete cubeObject;
    delete myStatPrinter;
    delete myPlainPrinter;
    delete myTopNPrinter;
}

void CubeStatistics::PrintUsage() const
{
    cerr << "Usage: cube3_stat [-h] [-p] [-m <metric>[,<metric>...]]" << endl;
    cerr << "                  [-%] [-r <routine>[,<routine>...]] <cubefile>" << endl;
    cerr << "                  OR" << endl;
    cerr << "                  [-t <topN>] <cubefile>" << endl;
    cerr << "  -h     Display this help message" << endl;
    cerr << "  -p     Pretty-print statistics (instead of CSV output)" << endl;
    cerr << "  -%     Provide statistics about process/thread metric values" << endl;
    cerr << "  -m     List of metrics (default: time)" << endl;
    cerr << "  -r     List of routines (default: main)" << endl;
    cerr << "  -t     Number for topN regions flat profile" << endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    CubeStatistics myCubeStat(argc, argv);
    myCubeStat.Print();
    return 0;
}

