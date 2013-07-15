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
/**
 * \file cube_score.cpp
 * \brief Collect the data about every region in the cube and prints it out.
Does same like "cube_regioninfo.cpp", a little extended verions. 
 */


#ifndef CUBE_COMPRESSED
#include <fstream>
#else
#include "zfstream.h"
#endif
#include <iomanip>
#include <string>
#include <set>
#include <map>
#include <numeric>
#include <vector>
#include <unistd.h>

#include "Blacklist.h"
#include "AggrCubeMaps.h"
#include "Cnode.h"
#include "Thread.h"
#include "Metric.h"
#include "Region.h"

#include "CnodeInfo.h"
#include "RegionInfo.h"

#include "helper.h"
#include "services.h"
#include "cube_error.h"

using namespace std;
using namespace cube;
using namespace services;

string lowercase(const string& str);

size_t w0 = 14;
int w1 = 14;
int w2 = 8;

bool do_regions(false);
bool do_blacklist(false);
bool do_metriclist(false);
bool do_maxtbc(false);
bool do_totaltbc(false);
bool do_sort(false);

/**
 * A collection of all costs (??) for a region.
 */
struct tracefile_costs {
    vector<unsigned long long> acc_costs_by_region;
    vector<unsigned long long> max_costs_by_region;

    unsigned long long acc_all;
    unsigned long long acc_bl;
    unsigned long long acc_com;
    unsigned long long acc_mpi;
    unsigned long long acc_omp;
    unsigned long long acc_usr;

    unsigned long long acc_com_bl;
    unsigned long long acc_mpi_bl;
    unsigned long long acc_omp_bl;
    unsigned long long acc_usr_bl;

    vector<unsigned long long> pt_all;
    vector<unsigned long long> pt_bl;
    vector<unsigned long long> pt_wbl;
    vector<unsigned long long> pt_com;
    vector<unsigned long long> pt_mpi;
    vector<unsigned long long> pt_omp;
    vector<unsigned long long> pt_usr;
    vector<unsigned long long> pt_com_bl;
    vector<unsigned long long> pt_mpi_bl;
    vector<unsigned long long> pt_omp_bl;
    vector<unsigned long long> pt_usr_bl;

    unsigned long long max_all;
    unsigned long long max_bl;
    unsigned long long max_wbl;
    unsigned long long max_com;
    unsigned long long max_mpi;
    unsigned long long max_omp;
    unsigned long long max_usr;
    unsigned long long max_com_bl;
    unsigned long long max_mpi_bl;
    unsigned long long max_omp_bl;
    unsigned long long max_usr_bl;

    tracefile_costs(size_t rcnt, size_t tcnt)
    : acc_costs_by_region(rcnt, 0),
    max_costs_by_region(rcnt, 0),
    pt_all(tcnt, 0),
    pt_bl(tcnt, 0),
    pt_wbl(tcnt, 0),
    pt_com(tcnt, 0),
    pt_mpi(tcnt, 0),
    pt_omp(tcnt, 0),
    pt_usr(tcnt, 0),
    pt_com_bl(tcnt, 0),
    pt_mpi_bl(tcnt, 0),
    pt_omp_bl(tcnt, 0),
    pt_usr_bl(tcnt, 0) {
        acc_all = 0;
        acc_bl = 0;
        acc_com = 0;
        acc_mpi = 0;
        acc_omp = 0;
        acc_usr = 0;
        acc_com_bl = 0;
        acc_mpi_bl = 0;
        acc_omp_bl = 0;
        acc_usr_bl = 0;
        max_all = 0;
        max_bl = 0;
        max_wbl = 0;
        max_com = 0;
        max_mpi = 0;
        max_omp = 0;
        max_usr = 0;
        max_com_bl = 0;
        max_mpi_bl = 0;
        max_omp_bl = 0;
        max_usr_bl = 0;
    }

    /**
     * A method of a collection, allows to get the maximum values for all kinds of regions.
     */
    void calculate_maxima() {
        max_all = find_max(pt_all).first;
        max_bl = find_max(pt_bl).first;
        max_wbl = find_max(pt_wbl).first;
        max_com = find_max(pt_com).first;
        max_mpi = find_max(pt_mpi).first;
        max_omp = find_max(pt_omp).first;
        max_usr = find_max(pt_usr).first;
        max_com_bl = find_max(pt_com_bl).first;
        max_mpi_bl = find_max(pt_mpi_bl).first;
        max_omp_bl = find_max(pt_omp_bl).first;
        max_usr_bl = find_max(pt_usr_bl).first;
    }
};

/// @cond PROTOTYPES
tracefile_costs calculate_tracefile_costs(AggrCubeMaps*, const CRegionInfo&, CBlacklist*);
/// @endcond

/**
 * A collection about all data for a given metric.
 */
template<typename T>
struct metric_data {
    Metric* metric;
    std::string name;
    AggrCubeMaps* acube;
    T bl;
    T com;
    T com_bl;
    T mpi;
    T mpi_bl;
    T omp;
    T omp_bl;
    T total;
    T usr;
    T usr_bl;
    map<Region*, double> excl;
    map<Region*, double> incl;
    map<Region*, double> diff;

    metric_data() {
        bl = T(0);
        com = T(0);
        com_bl = T(0);
        mpi = T(0);
        mpi_bl = T(0);
        omp = T(0);
        omp_bl = T(0);
        total = T(0);
        usr = T(0);
        usr_bl = T(0);
        metric = 0;
        acube = 0;
    };

    /**
     * Returns a type of the metric value.
     */

    bool isfloat() {
        if (metric != 0)
            return (metric->get_dtype() == "FLOAT");
        else
            return false;
    }
};

/**
 *  Calculates a percent.
 */
template<class S, class T>
double percent(S total, T costs) {
    if (total != 0) {
        return 100.0 * (double) costs / (double) total;
    }
    else return 0.0;
}

/**
 * A STL conform way to sort tbc values.
 */
class SortByMaxtbc : public std::binary_function<uint32_t, uint32_t, bool> {
public:

    SortByMaxtbc(const tracefile_costs& costs)
    : m_costs(costs) {
    }

    bool operator()(const uint32_t& lhs, const uint32_t& rhs) const {
        return m_costs.max_costs_by_region[lhs] > m_costs.max_costs_by_region[rhs];
    }

private:
    const tracefile_costs& m_costs;
};

/**
 * Sort by TotalTBC 
 *
 * @param costs
 */
class SortByTotaltbc : public std::binary_function<uint32_t, uint32_t, bool> {
public:

    SortByTotaltbc(const tracefile_costs& costs)
    : m_costs(costs) {
    }

    bool operator()(const uint32_t& lhs, const uint32_t& rhs) const {
        return m_costs.acc_costs_by_region[lhs] > m_costs.acc_costs_by_region[rhs];
    }

private:
    const tracefile_costs& m_costs;
};

/**
 * Sort by regionname
 *
 * @param r
 */
class SortByRegionName : public std::binary_function<uint32_t, uint32_t, bool> {
public:

    SortByRegionName(const vector<Region*>& r)
    : m_regions(r) {
    }

    bool operator()(const uint32_t& lhs, const uint32_t& rhs) const {
        return m_regions[lhs]->get_name() > m_regions[rhs]->get_name();   //m_costs.acc_costs_by_region[lhs] > m_costs.acc_costs_by_region[rhs];
    }

private:
    const vector<Region*>& m_regions;
};


/**
 * sort by metric other than totaltbc / max tbc
 *
 * @param m 
 * @param r
 * @param anI index in m of metric used to sort ( only 0 used currently, to sort by left most metric )
 */
class SortByMetricI : public std::binary_function<uint32_t, uint32_t, bool> {
public:

    SortByMetricI(vector<metric_data<double> >& m, const vector<Region*>& r, int anI)
    : m_metrics(m),m_regions(r),i(anI) {

    }

    bool operator()(const uint32_t& lhs, const uint32_t& rhs) const {
        return m_metrics[i].excl[m_regions[lhs]] > m_metrics[i].excl[m_regions[rhs]];   //m_costs.acc_costs_by_region[lhs] > m_costs.acc_costs_by_region[rhs];
    }

private:
    vector<metric_data<double> >& m_metrics;
    const vector<Region*>& m_regions;
    int i;
};

/**
 * Prints a usage of this tool.
 */
void usage(const char* filename) {
    cout << "Usage: " << filename << " [-r] [-f <filename>] [-m <metric>[,<metric>...]] <cubefile>\n"
            << "  -r     Print metrics for each region\n"
#ifdef FILTER_FORMAT
            << "  -u     Mark all USR computation regions as filtered\n"
#endif
            << "  -f     File containing names of regions to mark as filtered\n"
            << "  -s     Sort by region names\n"
            << "  -m     List of metrics that should be displayed, default: max_tbc,time\n\n"
            << "Regions are classified into the categories ANY (aggregate of all regions),\n"
            << "MPI (pure MPI library functions), OMP (pure OpenMP functions/regions),\n"
            << "COM (\"combined\" user regions calling OpenMP/MPI, directly or indirectly), and\n"
            << "USR (user regions with purely local computation not containing MPI or OpenMP).\n\n"
            << "Metrics can be any of those defined in the cubefile, or two special metrics:\n"
            << "The total_tbc metric provides an estimate of total size of trace data (in bytes).\n"
            << "The max_tbc metric provides an estimate for the trace buffer capacity (in bytes)\n"
            << "that is required to store all events that would be generated by a single process.\n";
    exit(0);
}

/**
 * Accumulate usr, mpi, com and total values in md.
 */
template<class T>
void acc_with_type(metric_data<T>& md, CRegionInfo& reginfo, CBlacklist* blacklist) {
    /*
      accumulate usr, mpi, omp, com and total values in md
     */
    typename std::map<Region*, double>::const_iterator it;
    for (it = md.excl.begin(); it != md.excl.end(); it++) {
        T nv((T) it->second);
        const Region * region(it->first);

        md.total += nv;
        switch (reginfo[region->get_id()]) {
            case MPI:
                md.mpi += nv;
                break;

            case OMP:
                md.omp += nv;
                break;

            case COM:
                md.com += nv;
                break;

            case USR:
                md.usr += nv;
                break;
            default: // another types of regions are ignored
                break;
        }

        if (blacklist != 0) {
            if ((*blacklist)(region->get_id())) {
                md.bl += nv;
            }
            else
                switch (reginfo[region->get_id()]) {
                    case MPI:
                        md.mpi_bl += nv;
                        break;

                    case OMP:
                        md.omp_bl += nv;
                        break;

                    case COM:
                        md.com_bl += nv;
                        break;

                    case USR:
                        md.usr_bl += nv;
                        break;
                    default:// another types of regions are ignored
                        break;
                }
        }
    }
}

void setUpBlackList(string blackListName,
                    CBlacklist** blacklist,
                    AggrCubeMaps *input,
                    CRegionInfo& reginfo) {
    if (do_blacklist) {
        if (blackListName == "<USR>") {
            cerr << "Filtering all USR regions:" << endl;
            *blacklist = new CBlacklist(input, &reginfo);
        }
        else {
            cerr << "Applying filter \"" << blackListName << "\":" << endl;
            *blacklist = new CBlacklist(input, blackListName, &reginfo);
        }
    }
}

void setUpMetricData(string metriclist,
                     vector<metric_data<double> >& metricData,
                     AggrCubeMaps *input,
                     CBlacklist* blacklist,
                     CRegionInfo& reginfo) {

    if (do_metriclist) {
        size_t oldpos = 0;
        while (true) {
            const size_t f = metriclist.find_first_of(',', oldpos);
            string metricName;
            
            if (f == string::npos) {
                metricName = metriclist.substr(oldpos);
            }
            else {
                metricName = metriclist.substr(oldpos, f - oldpos);

            }

            oldpos = f + 1;

            if (metricName == "max_tbc") {

                do_maxtbc = true;
                
                if (f == string::npos)
                    break;
                else
                    continue;
            }
            else if (metricName == "total_tbc") {
                do_totaltbc = true;
                
                if (f == string::npos)
                    break;
                else
                    continue;
            }

            metricData.push_back(metric_data<double>());
            metricData[metricData.size() - 1].name = metricName;
            if (f == string::npos) break;
        }
    }
    else {
        metricData.push_back(metric_data<double>());
        metricData[0].name = string("time");
        do_maxtbc = true;
    }

    for (size_t i = 0; i < metricData.size(); i++) {
        metricData[i].metric = input->get_met(metricData[i].name);

        if (metricData[i].metric != 0) {
            input->get_reg_tree(metricData[i].excl,
                                metricData[i].incl,
                                metricData[i].diff,
                                INCL,
                                INCL,
                                metricData[i].metric,
                                0);
        }
        else {
            const vector<Metric*>& metrics = input->get_metv();

            cout << "unknown metric: " << metricData[i].name << endl;
            cout << "available metrics: " << endl;
            cout << "total_tbc   (bytes) \"Total trace event byte count for all processes\"" << endl;
            cout << "max_tbc     (bytes) \"Maximum trace event byte count for a process\"" << endl;

            for (size_t i = 0; i < metrics.size(); i++) {
                Metric* met = metrics[i];
                cout << left << setw(11) << met->get_uniq_name() << " (" << met->get_uom() << ") \"";
                cout << met->get_disp_name();
                while (NULL != (met = met->get_parent()))
                    cout << " - " << met->get_disp_name();
                cout << "\"" << endl;
            }
            exit(EXIT_FAILURE);
        }

        acc_with_type<double>(metricData[i], reginfo, blacklist);
    }
}

/**
 * base interface for all metric data output strategies
 *
 * returns name, short name, and description of aggregate metric
 *
 * and provides three functions to return the proper cost/metric value
 *
 * see function outMetric() below for usage
 *
 */
class IOutStrategy {
public:
    virtual double getValue(metric_data<double>& metric) = 0;
    virtual unsigned long long getMaxTbcValue(tracefile_costs& c) = 0;
    virtual unsigned long long getTotalTbcValue(tracefile_costs& c) = 0;
    virtual string getName() = 0;
    virtual string getShortName() = 0;
    virtual string getDescription() = 0;
    virtual ~IOutStrategy() {};
};

class MPIOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.mpi;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_mpi;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_mpi;
    }

    string getName() {
        return " (summary) MPI";
    }

    string getShortName() {
        return (do_blacklist ? " - " : "   ") + string("   MPI");
    }

    string getDescription() {
        return "# MPI # (MPI library routines)";
    }

    virtual ~MPIOutStrat(){};
};

class OMPOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.omp;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_omp;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_omp;
    }

    string getName() {
        return " (summary) OMP";
    }

    string getShortName() {
        return (do_blacklist ? " - " : "   ") + string("   OMP");
    }

    string getDescription() {
        return "# OMP # (OpenMP API + parallel regions)";
    }

    virtual ~OMPOutStrat(){};
};

class AllOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.total;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_all;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_all;
    }

    string getDescription() {
        return "# ANY # (aggregation of all types)";
    }

    string getShortName() {
        return (do_blacklist ? " - " : "   ") + string("   ANY");
    }

    string getName() {
        return " (summary) ALL";
    }

    virtual ~AllOutStrat() {};
};

class ComOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.com;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_com;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_com;
    }

    string getShortName() {
        return (do_blacklist ? " - " : "   ") + string("   COM");
    }

    string getName() {
        return " (summary) COM";
    }

    string getDescription() {
        return "# COM # (combined OMP/MPI & USR computation)";
    }

    virtual ~ComOutStrat() {}
};

class UsrOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.usr;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_usr;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_usr;
    }

    string getShortName() {
        return (do_blacklist ? " - " : "   ") + string("   USR");
    }

    string getName() {
        return " (summary) USR";
    }

    string getDescription() {
        return "# USR # (user computation routines)";
    }

    virtual ~UsrOutStrat(){};
};

/*
 Strategy to output all filtered(blacklisted) data
 */
class BlOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.bl;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_bl;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_bl;
    }

    string getName() {
        return " (summary) FLT";
    }

    string getShortName() {
        return " +    FLT";
    }

    string getDescription() {
        return "# FLT # (eliminated with filter)";
    }

    virtual ~BlOutStrat() {};
};

/**
 * Strategy to output difference filtered and total
 */
class BlDifOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.total - metric.bl;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_wbl;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_all - c.acc_bl;
    }

    string getShortName() {
        return " *    ANY";
    }

    string getDescription() {
        return "# ANY # (remaining after filter)";
    }

    string getName() {
        return " (summary) ALL-FLT";
    }

    virtual ~BlDifOutStrat(){};
};

/**
 * Strategy to output filtered MPI data
 */
class BlMpiOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.mpi_bl;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_mpi_bl;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_mpi_bl;
    }

    string getShortName() {
        return " -    MPI";
    }

    string getDescription() {
        return "# MPI # (remaining after filter)";
    }

    string getName() {
        return " (summary) MPI-FLT";
    }

    virtual ~BlMpiOutStrat(){};
};

/**
 * Strategy to output filtered OMP data
 */
class BlOmpOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.omp_bl;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_omp_bl;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_omp_bl;
    }

    string getShortName() {
        return " -    OMP";
    }

    string getDescription() {
        return "# OMP # (remaining after filter)";
    }

    string getName() {
        return " (summary) OMP-FLT";
    }

    virtual ~BlOmpOutStrat() {};
};

/**
 * Strategy to output filtered COM data
 */
class BlComOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.com_bl;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_com_bl;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_com_bl;
    }

    string getShortName() {
        return " *    COM";
    }

    string getDescription() {
        return "# COM # (remaining after filter)";
    }

    string getName() {
        return " (summary) COM-FLT";
    }

    virtual ~BlComOutStrat() {};
};

/**
 * strategy to output filtered USR data
 */
class BlUsrOutStrat : public IOutStrategy {
public:
    double getValue(metric_data<double>& metric) {
        return metric.usr_bl;
    }

    unsigned long long getMaxTbcValue(tracefile_costs& c) {
        return c.max_usr_bl;
    }

    unsigned long long getTotalTbcValue(tracefile_costs& c) {
        return c.acc_usr_bl;
    }

    string getShortName() {
        return " *    USR";
    }

    string getDescription() {
        return "# USR # (remaining after filter)";
    }

    string getName() {
        return " (summary) USR-FLT";
    }

    virtual ~BlUsrOutStrat() {};
};

/**
 * output particular metric
 *
 * @param str strategy to access metric/cost values and provide descriptions
 * @param metricData
 * @param c
 * @param printFooter whether to print footer ( default true )
 * @param printHeader whether to print header ( default true )
 */
void outMetric(IOutStrategy* str, vector<metric_data<double> >& metricData, tracefile_costs& c, bool printFooter = true, bool printHeader = true) {
    if (printHeader) {
#ifndef FILTER_FORMAT
        cout << right << str->getShortName();
#else
        cout << left << setw(48) << str->getDescription() << "#" << right;
#endif
    }

    if (do_maxtbc)
        cout << setw(w1) << str->getMaxTbcValue(c);
    if (do_totaltbc)
        cout << setw(w1) << str->getTotalTbcValue(c);

    for (size_t i = 0; i < metricData.size(); i++) {
        int prec = 0;
        
        if (metricData[i].isfloat()) {
            prec = 2;
        }

        cout << setw(w1) << setprecision(prec) << fixed << str->getValue(metricData[i])
             << setw(w2) << setprecision(2)    << fixed << percent(metricData[i].total, str->getValue(metricData[i]));
    }

    if (printFooter) {
#ifndef FILTER_FORMAT
        cout << left << str->getName();
#endif
        cout << endl;
    }
}

/**
 * Main program.
 * - Check calling arguments
 * - Read the .cube input file.
 * - Create blacklist for the cube and regions in the cube.
 * - Run over metrics (comma separated values in command parameter.) and for every metric do.
 * - Calculate a values for every region for given metric.
 * - Create a trace cost.
 * - Prints the result in human readable form.
 * - end.
 */
int main(int argc, char* argv[]) {
    string cubefile;
    string blacklistname;
    string metriclist;

    int c;

    if (argc == 1)
        usage(argv[0]);

    opterr = 0;
    int mna(1);

    while ((c = getopt(argc, argv, "m:hruf:s")) != -1) {
        switch (c) {
            case 'h':
                usage(argv[0]);
                break;
            case 'r':
                do_regions = true;
                mna++;
                break;
            case 'u':
                do_regions = true;
                do_blacklist = true;
                if (!blacklistname.empty())
                    cerr << "Warning: filter \"" << blacklistname
                        << "\" will be ignored!" << endl;
                blacklistname = "<USR>";
                mna++;
                break;
            case 'f':
                blacklistname = string(optarg);
                do_blacklist = true;
                mna += 2;
                break;
            case 'm':
                metriclist = string(optarg);
                do_metriclist = true;
                mna += 2;
                break;
            case 's':
                do_sort = true;
                mna += 2;
                break;
        }
    }

    if (mna == (argc)) {
        usage(argv[0]);
    }
    else if (mna == (argc - 1)) {
        cubefile = string(argv[argc - 1]);
    }

    if (check_file(cubefile.c_str()) != 0) {
        exit(-1);
    }

    AggrCubeMaps *input = NULL;
    try
    {
        input = new AggrCubeMaps();
    
    #ifndef CUBE_COMPRESSED
        ifstream in(cubefile.c_str());
    #else
        gzifstream in(cubefile.c_str(), ios_base::in | ios_base::binary);
    #endif
        
        // Read input file
        cerr << "Reading " << cubefile << "...";
        if (!in) {
            cerr << "Error: open " << cubefile << endl;
            exit(1);
        }
        in >> *input;
        cerr << " done." << endl;
    
        const vector<Region*>&  regions = input->get_regv();
    
        CRegionInfo reginfo = input;
    
        CBlacklist* blacklist = 0;
            
        setUpBlackList(blacklistname, &blacklist,input, reginfo);
    
        vector<metric_data<double> > metricdata;
    
        setUpMetricData(metriclist, metricdata, input, blacklist, reginfo);
    
        tracefile_costs tbcosts(calculate_tracefile_costs(input, reginfo, blacklist));
    
        /*
        BEGIN: Print results
        */
    
        cerr << "Estimated aggregate size of event trace (total_tbc): "
                << tbcosts.acc_all - tbcosts.acc_bl << " bytes" << endl;
        cerr << "Estimated size of largest ";
        if (tbcosts.acc_omp > 0)
            cerr << "thread trace (max_tbc):    ";
        else
            cerr << "process trace (max_tbc):   ";
        if (do_blacklist)
            cerr << tbcosts.max_wbl << " bytes" << endl;
        else
            cerr << tbcosts.max_all << " bytes" << endl;
    
        cerr << "(Hint: When tracing set ELG_BUFFER_SIZE > max_tbc to avoid intermediate flushes";
        if (!do_blacklist) {
            cerr << endl << " or reduce requirements using file listing names of USR regions to be filtered";
        }
        cerr << ".)" << endl << endl;
    
        if (do_regions) {
            /* calculate layout information */
            for (size_t i = 0; i < regions.size(); i++) {
                size_t l = regions[i]->get_name().length();
                
                if (l > w0) {
                    w0 = l;
                }
            }
        }
    
        w0 += 2;
    
    #ifndef FILTER_FORMAT
        cout << right
                << setw(3) << "flt"
                << setw(6) << "type";
    #else
        cout << "# " << left << setw(6) << "type" << setw(41) << "region" << right;
    #endif
    
        if (do_maxtbc)
            cout << setw(w1) << "max_tbc";
        if (do_totaltbc)
            cout << setw(w1) << "total_tbc";
    
        for (size_t i = 0; i < metricdata.size(); i++) {
            string::size_type n = string::npos;
    
            int sz = metricdata[i].name.length();
            if (sz > w1) {
                n = w2 - 3;
            }
    
            cout << setw(w1) << metricdata[i].name.substr(0, n)
                << setw(w2) << "%";
        }
    
    #ifndef FILTER_FORMAT
        cout << left
                << " "
                << "region";
    #endif
        cout << endl;
    
    
        outMetric(new AllOutStrat(), metricdata, tbcosts);
    
        if (tbcosts.acc_mpi > 0) {
            outMetric(new MPIOutStrat(), metricdata, tbcosts);
        }
    
        if (tbcosts.acc_omp > 0) {
    #ifdef FILTER_FORMAT
            cout << "#" << setw(44) << right << "(summary) # OMP";
    #endif
            outMetric(new OMPOutStrat(), metricdata, tbcosts);
        }

        if (tbcosts.acc_mpi > 0 || tbcosts.acc_omp > 0) {
            outMetric(new ComOutStrat(), metricdata, tbcosts);
        }

        outMetric(new UsrOutStrat(), metricdata, tbcosts);
    
        cout << endl;
    
        if (do_blacklist) {
    #ifdef FILTER_FORMAT
            cout << "# ";
            for (size_t i = 2; i <= 80; i++) cout << '-';
            cout << endl;
    #endif
    
            outMetric(new BlOutStrat(), metricdata, tbcosts);
    
            outMetric(new BlDifOutStrat(), metricdata, tbcosts);
    
            if (tbcosts.acc_mpi > 0) {
                outMetric(new BlMpiOutStrat(), metricdata, tbcosts);
            }
    
            if (tbcosts.acc_omp > 0) {
                outMetric(new BlOmpOutStrat(), metricdata, tbcosts);
            }
    
            if (tbcosts.acc_mpi > 0 || tbcosts.acc_omp > 0) {
                outMetric(new BlComOutStrat(), metricdata, tbcosts);
            }
    
            outMetric(new BlUsrOutStrat(), metricdata, tbcosts);
    
            cout << endl;
        }
    
        if (do_regions) {
    #ifdef FILTER_FORMAT
            cout << "# ";
            for (size_t i = 2; i <= 80; i++) cout << '-';
            cout << endl;
    #endif
    
            // Sort regions by trace buffer costs
            vector<uint32_t> region_ids;
            region_ids.reserve(regions.size());
    
            for (size_t i = 0; i < regions.size(); i++) {
                region_ids.push_back(i);
            }
    
            if(do_sort) {
                //if(sortCol == "region") { // TODO allow specification of sort metric some time
                    sort(region_ids.begin(), region_ids.end(), SortByRegionName(regions));
                //}
            }
            else {
                if(do_maxtbc) {
                    sort(region_ids.begin(), region_ids.end(), SortByMaxtbc(tbcosts));
                }
                else if ( do_totaltbc ) {
                    sort(region_ids.begin(), region_ids.end(), SortByTotaltbc(tbcosts));
                }
                else { // sort by first metric, the one on the left
                    sort(region_ids.begin(), region_ids.end(),SortByMetricI(metricdata,regions,0));
                }
            }
    
            for (size_t k = 0; k < regions.size(); k++) {
                int i = region_ids[k];
    
                if (reginfo[i] == NUL) continue;
                if (tbcosts.acc_costs_by_region[i] == 0) continue;
    
    #ifndef FILTER_FORMAT
                if (do_blacklist) {
                    if ((*blacklist)((Region *) regions[i]))
                        cout << " + ";
                    else
                        cout << " - ";
                }
                else
                    cout << "   ";
    
                cout << right
                        << setw(6) << Callpathtype2String(reginfo[i]);
                if (do_maxtbc)
                    cout << setw(w1) << tbcosts.max_costs_by_region[i];
                if (do_totaltbc)
                    cout << setw(w1) << tbcosts.acc_costs_by_region[i];
    
                for (size_t j = 0; j < metricdata.size(); j++) {
                    int prec = 0;
                    if (metricdata[j].isfloat()) prec = 2;
                    cout << setprecision(prec) << fixed
                            << setw(w1) << metricdata[j].excl[regions[i]]
                            << setprecision(2) << fixed
                            << setw(w2) << percent(metricdata[j].total, metricdata[j].excl[regions[i]]);
                }
    
                cout << left
                        << " "
                        << regions[i]->get_name() << endl;
    #else // filter-format
                if (!do_blacklist || !(*blacklist)((Region *) regions[i])) {
                    cout << "# " << Callpathtype2String(reginfo[i]);
                    // distinguish non-filterable regions
                    if ((reginfo[i] == EPK) || (reginfo[i] == MPI) || (reginfo[i] == OMP)) cout << " #";
                }
                cout << '\t';
    
                cout << setw(39) << left << regions[i]->get_name();
    
                cout << "\t#" << right;
                if (do_maxtbc)
                    cout << setw(w1) << tbcosts.max_costs_by_region[i];
                if (do_totaltbc)
                    cout << setw(w1) << tbcosts.acc_costs_by_region[i];
    
                for (size_t j = 0; j < metricdata.size(); j++) {
                    int prec = metricdata[j].isfloat() ? 2 : 0;
                    cout << setprecision(prec) << fixed
                            << setw(w1) << metricdata[j].excl[regions[i]]
                            << setprecision(2) << fixed
                            << setw(w2) << percent(metricdata[j].total, metricdata[j].excl[regions[i]]);
                }
    
                cout << endl;
    #endif
            }
        }

    /* END: Print results */
    }
    catch (RuntimeError& err)
    {
        cerr << err.get_msg() << endl;
    }

    if (input != NULL) delete input;
}

/**
 * Calculate a cost ( given as a severity value mulpilied wih a factor ) for every
region for every thread. Calculate a cost for every kind of regions 
(usr, mpi, omp, and so on) over all threads. And get a maximum value at the end.
 */
tracefile_costs calculate_tracefile_costs(AggrCubeMaps* input, const CRegionInfo& reginfo, CBlacklist* blacklist) {
    const vector<Region*>& regions = input->get_regv();
    const vector<Thread*>& threads = input->get_thrdv();

    const size_t rcnt = regions.size();
    const size_t tcnt = threads.size();

    tracefile_costs tbcosts = tracefile_costs(rcnt, tcnt);

    Metric* visits = input->get_met("visits");

    /* accumulated costs on a per thread basis */
    for (size_t i = 0; i < rcnt; i++) {
        const Region* region = regions[i];
        const unsigned long long d = TypeFactor(region->get_name());
        const vector<Cnode*>& cnodev = region->get_cnodev();
        const size_t ccnt = cnodev.size();
        vector<unsigned long long> buffer(tcnt, 0);

        for (size_t j = 0; j < ccnt; j++) {
            Cnode* cnode = cnodev[j];
            for (size_t ThreadId = 0; ThreadId < tcnt; ThreadId++) {
                Thread* thread = threads[ThreadId];

                const unsigned long long newval = static_cast<unsigned long long> (input->get_sev(visits, cnode, thread));
                const unsigned long long costs = newval*d;
                buffer[ThreadId] += costs;
                tbcosts.pt_all[ThreadId] += costs;
                tbcosts.acc_costs_by_region[i] += costs;
                if (blacklist != 0) {
                    if ((*blacklist)((Region*) region)) {
                        tbcosts.pt_bl[ThreadId] += costs;
                    }
                    else {
                        switch (reginfo[i]) {
                            case MPI:
                                tbcosts.pt_mpi_bl[ThreadId] += costs;
                                break;

                            case OMP:
                                tbcosts.pt_omp_bl[ThreadId] += costs;
                                break;

                            case USR:
                                tbcosts.pt_usr_bl[ThreadId] += costs;
                                break;

                            case COM:
                                tbcosts.pt_com_bl[ThreadId] += costs;
                                break;
                            default:// another types of regions are ignored
                                break;
                        }
                        tbcosts.pt_wbl[ThreadId] += costs;
                    }
                }

                switch (reginfo[i]) {
                    case MPI:
                        tbcosts.pt_mpi[ThreadId] += costs;
                        break;

                    case OMP:
                        tbcosts.pt_omp[ThreadId] += costs;
                        break;

                    case USR:
                        tbcosts.pt_usr[ThreadId] += costs;
                        break;

                    case COM:
                        tbcosts.pt_com[ThreadId] += costs;
                        break;
                    default:// another types of regions are ignored
                        break;
                }
            }

        }

        unsigned long long max_costs = find_max(buffer).first;
        if (tbcosts.max_costs_by_region[i] < max_costs)
            tbcosts.max_costs_by_region[i] = max_costs;
    }

    /*
      BEGIN: calculate total costs split by category (mpi, omp, com, usr, blacklist) using get_met_tree(...)
     */

    for (size_t regionId = 0; regionId < regions.size(); regionId++) {
        Region* region = regions[regionId];
        const vector<Cnode*>& cnodev(region->get_cnodev());
        unsigned long long d = TypeFactor(region->get_name());
        for (size_t cnodeId = 0; cnodeId < cnodev.size(); cnodeId++) {
            map<Metric*, double> excl_metrics;
            map<Metric*, double> incl_metrics;
            Cnode* cnode = cnodev[cnodeId];
            input->get_met_tree(excl_metrics, incl_metrics, EXCL, INCL, cnode, 0);
            Metric* metric = input->get_met("visits");

            unsigned long long visits((unsigned long long) excl_metrics[metric]);
            unsigned long long nc = d*visits;

            tbcosts.acc_all += nc;

            if (blacklist != 0) {
                if ((*blacklist)((Region*) region)) {
                    tbcosts.acc_bl += nc;
                }
                else {
                    switch (reginfo[regionId]) {
                        case MPI:
                            tbcosts.acc_mpi_bl += nc;
                            break;
                        case OMP:
                            tbcosts.acc_omp_bl += nc;
                            break;
                        case USR:
                            tbcosts.acc_usr_bl += nc;
                            break;
                        case COM:
                            tbcosts.acc_com_bl += nc;
                            break;
                        default:// another types of regions are ignored
                            break;
                    }
                }
            }

            switch (reginfo[regionId]) {
                case MPI:
                    tbcosts.acc_mpi += nc;
                    break;
                case OMP:
                    tbcosts.acc_omp += nc;
                    break;
                case USR:
                    tbcosts.acc_usr += nc;
                    break;
                case COM:
                    tbcosts.acc_com += nc;
                    break;
                default:// another types of regions are ignored
                    break;
            }
        }
    }

    /*
      DONE: calculate total costs split by category (mpi, omp, com, usr, blacklist) using get_met_tree(...)
     */

    /*
      BEGIN: accumulute per thread data items
     */

    tbcosts.calculate_maxima();

    /*
      END: accumulute per thread data items
     */

    return tbcosts;
}
