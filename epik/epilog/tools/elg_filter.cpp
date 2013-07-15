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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

 /*STL*/
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <list>
#include <algorithm>

/*Epilog*/
#include "elg_error.h"
#include "elg_readcb.h"
#include "elg_rw.h"
#include "epk_archive.h"

using namespace std;

struct ElgLocation
{
    elg_ui4 lid;
    elg_ui4 mid;
    elg_ui4 nid;
    elg_ui4 pid;
    elg_ui4 tid;
};

struct ElgRegion
{
    elg_ui4 rid;
    elg_ui4 rnid;
    elg_ui4 fid;
    elg_ui4 begln;
    elg_ui4 endln;
    elg_ui4 rdid;
    elg_ui1 rtype;
};

struct ElgCallSite
{
    elg_ui4 csid;
    elg_ui4 fid;
    elg_ui4 lno;
    elg_ui4 erid;
    elg_ui4 lrid;
};

struct ElgCallPath
{
    elg_ui4 cpid;
    elg_ui4 rid;
    elg_ui4 ppid;
    elg_ui8 order;
};

struct ElgString
{
    elg_ui4 strid;
    elg_ui1 cntc;
    string str;
};

struct ElgMessage
{
    elg_ui4 spid;
    elg_ui4 dpid;
    elg_ui4 cid;
    elg_ui4 tag;
    bool filtered;
};

struct ElgTransfer
{
    elg_ui4 spid;
    elg_ui4 dpid;
    elg_ui4 wid;
    elg_ui4 rmaid;
    elg_ui1 type;
    bool filtered;
};

/* required for 'find' algorithm */
bool operator==(const ElgMessage& s1, const ElgMessage& s2)
{
    return s1.spid == s2.spid && s1.dpid == s2.dpid &&
        s1.cid == s2.cid && s1.tag == s2.tag;
}

bool operator!=(const ElgMessage& s1, const ElgMessage& s2)
{
    return s1.spid != s2.spid || s1.dpid != s2.dpid ||
        s1.cid != s2.cid || s1.tag != s2.tag;
}

bool operator==(const ElgTransfer & s1, const ElgTransfer & s2)
{
    return s1.type == s2.type && s1.spid == s2.spid && s1.dpid == s2.dpid &&
        s1.wid == s2.wid && s1.rmaid == s2.rmaid;
}

bool operator!=(const ElgTransfer & s1, const ElgTransfer & s2)
{
    return s1.type != s2.type || s1.spid != s2.spid || s1.dpid != s2.dpid ||
        s1.wid != s2.wid || s1.rmaid != s2.rmaid;
}

typedef vector < ElgString * >ElgNames;
typedef vector < ElgLocation * >ElgLocations;
typedef vector < ElgRegion * >ElgRegions;
typedef vector < ElgCallSite * >ElgCallSites;
typedef vector < ElgCallPath * >ElgCallPaths;
typedef map < string, elg_ui1 > ElgMessageTypes;
typedef list < ElgMessage > ElgMessageQueue;
typedef list < ElgTransfer > ElgTransferQueue;

static ElgMessageTypes msg_types;
static off_t   done = 0;

class ElgFlt
{
  public:
    ElgFlt() : cut_metric(false), native_bo(false), verbose(false),
               rgn2leave(ELG_NO_ID), zero_time(-1.0), filtering_done(false) {};

    void print();

    bool cut_metric;            // -m 
    bool native_bo;             // -n 
    bool verbose;               // -v 

    set < elg_ui4 > type2filter;        // -t
    set < string > typename2filter;     // -T

    set < elg_ui4 > rgn2filter; // -i
    map < string, bool > rgnname2filter;        // -I
    vector < elg_ui4 > rgn2filter_nesting;      // -I -i

    elg_ui4 rgn2leave;          // -o
    string rgnname2leave;       // -O
    vector < elg_ui4 >rgn2leave_nesting;    // -O -o
    elg_d8 zero_time;           // -O -o
    bool filtering_done;

    set < elg_ui4 > rgn2remove; // -r
    map < string, bool > rgnname2remove;        // -R 

    ElgMessageQueue mqueue;      // MPI_SEND <--> MPI_RECV
    ElgTransferQueue tqueue;      // MPI_*_1TS <--> MPI_*_1TE

    /* vector of stacks with region id's for each location */
    vector < stack < elg_ui4 > >rgn_stacks;     // ENTER <--> EXIT

    /* definitions records */
    ElgNames names;
    ElgLocations locations;
    ElgRegions regions;
    ElgCallSites call_sites;
    ElgCallPaths call_paths;

    /* I/O structures */
    ElgRCB *in;
    ElgOut *out;
};

void ElgFlt::print()
{
    set < elg_ui4 >::iterator itInt;
    set < string >::iterator itStr;
    map < string, bool >::iterator itStrMap;
    printf("filter parameters:");
    if (!type2filter.empty())
        printf("\n   record type ids to filter:");
    for (itInt = type2filter.begin(); itInt != type2filter.end(); itInt++)
        printf(" %u ", *itInt);

    if (!typename2filter.empty())
        printf("\n   record type names to filter:");
    for (itStr = typename2filter.begin(); itStr != typename2filter.end();
         itStr++)
        printf(" %s ", itStr->c_str());

    if (!rgn2filter.empty())
        printf("\n   region ids to inner filter:");
    for (itInt = rgn2filter.begin(); itInt != rgn2filter.end(); itInt++)
        printf(" %u ", *itInt);

    if (!rgnname2filter.empty())
        printf("\n   region names to inner filter:");
    for (itStrMap = rgnname2filter.begin(); itStrMap != rgnname2filter.end();
         itStrMap++)
        printf(" %s ", itStrMap->first.c_str());

    if (rgn2leave != ELG_NO_ID)
        printf("\n   region ids to leave: %u ", rgn2leave);

    if (rgnname2leave != "")
        printf("\n   region names to leave: %s ", rgnname2leave.c_str());

    if (!rgn2remove.empty())
        printf("\n   region ids to remove:");
    for (itInt = rgn2remove.begin(); itInt != rgn2remove.end(); itInt++)
        printf(" %u ", *itInt);

    if (!rgnname2remove.empty())
        printf("\n   region names to remove:");
    for (itStrMap = rgnname2remove.begin(); itStrMap != rgnname2remove.end();
         itStrMap++)
        printf(" %s ", itStrMap->first.c_str());

    if (cut_metric)
        printf("\n   metrics to filter: all");
    printf("\n");
}

struct ElgRidValidator
{
    ElgFlt *filter;
    ElgRidValidator(ElgFlt * flt):filter(flt)
    {
    }
    void operator() (elg_ui4 rid)
    {
        if (rid >= filter->regions.size() && rid != ELG_NO_ID)
            elg_warning("Invalid region id specified with -r or -i option: '%d'", rid);
    }
};

struct ElgTypeValidator
{
    void operator() (elg_ui4 type_id )
    {
        ElgMessageTypes::iterator it,itEnd;
        it=msg_types.begin();
        itEnd=msg_types.end();
        for (;it!=itEnd;it++) 
        {
           if (it->second == type_id)
               return;/* region Ok */
        }
        elg_warning("Invalid message type id specified with -t option: '%d'", type_id);
    }
};

struct ElgErrorCollector
{
    string msg;
    void operator() (const pair < const string, bool > &map_item)
    {
        if (!map_item.second)
            msg += map_item.first + " ";
    }
    
    
};



void ElgFlt_resolve(ElgFlt * filter)
{
    set < string >::iterator itStr;
    map < string, bool >::iterator itStrMap;
    ElgRegions::iterator itRgn;
    ElgMessageTypes::iterator itMsgType;

    /* resolve record type names */
    for (itStr = filter->typename2filter.begin();
         itStr != filter->typename2filter.end(); itStr++)
    {
        itMsgType = msg_types.find(*itStr);
        if (itMsgType != msg_types.end())
            filter->type2filter.insert(itMsgType->second);
        else if ( *itStr != "MPI" && *itStr != "OMP" )
            elg_warning("Invalid record type specified with -T option: '%s'",
                        itStr->c_str());
    }

    /* handle record type wildcards */
    if (filter->typename2filter.find("MPI") != filter->typename2filter.end())
    {
        filter->type2filter.insert(ELG_MPI_SEND);
        filter->type2filter.insert(ELG_MPI_RECV);
    }

    if (filter->typename2filter.find("MPIPUT") != filter->typename2filter.end())
    {
        filter->type2filter.insert(ELG_MPI_PUT_1TS);
        filter->type2filter.insert(ELG_MPI_PUT_1TE);
    }

    if (filter->typename2filter.find("MPIGET") != filter->typename2filter.end())
    {
        filter->type2filter.insert(ELG_MPI_GET_1TO);
        filter->type2filter.insert(ELG_MPI_GET_1TS);
        filter->type2filter.insert(ELG_MPI_GET_1TE);
    }

    if (filter->typename2filter.find("PUT") != filter->typename2filter.end())
    {
        filter->type2filter.insert(ELG_PUT_1TS);
        filter->type2filter.insert(ELG_PUT_1TE);
    }

    if (filter->typename2filter.find("GET") != filter->typename2filter.end())
    {
        filter->type2filter.insert(ELG_GET_1TS);
        filter->type2filter.insert(ELG_GET_1TE);
    }

    if (filter->typename2filter.find("OMP") != filter->typename2filter.end())
    {
        filter->type2filter.insert(ELG_OMP_JOIN);
        filter->type2filter.insert(ELG_OMP_FORK);
        filter->type2filter.insert(ELG_OMP_ALOCK);
        filter->type2filter.insert(ELG_OMP_RLOCK);
    }

    /* resolve region names */
    for (itRgn = filter->regions.begin();
         itRgn != filter->regions.end(); itRgn++)
    {
        string & str = filter->names[(*itRgn)->rnid]->str;
        //elg_cntl_msg("  <REGION>    rid: %d  rnid: %d  name : %s",
        //             (*itRgn)->rid, (*itRgn)->rnid, str.c_str());

        /* -R regions */
        itStrMap = filter->rgnname2filter.find(str);
        if (itStrMap != filter->rgnname2filter.end())
        {
            filter->rgn2filter.insert((*itRgn)->rid);
            itStrMap->second = true;
        }

        /* -O region */
        if (filter->rgnname2leave == str)
            filter->rgn2leave = (*itRgn)->rid;

        /* -I regions */
        itStrMap = filter->rgnname2remove.find(str);
        if (itStrMap != filter->rgnname2remove.end())
        {
            filter->rgn2remove.insert((*itRgn)->rid);
            itStrMap->second = true;
        }
    }

    /* region name validator */
    ElgErrorCollector err;
    err = for_each(filter->rgnname2filter.begin(),
                   filter->rgnname2filter.end(), ElgErrorCollector());
    if (err.msg != "")
        elg_warning("Invalid region names specified with -I option: '%s'",
                    err.msg.c_str());

    err = for_each(filter->rgnname2remove.begin(),
                   filter->rgnname2remove.end(), ElgErrorCollector());
    if (err.msg != "")
        elg_warning("Invalid region names specified with -R option: '%s'",
                    err.msg.c_str());

    if (filter->rgn2leave == ELG_NO_ID && filter->rgnname2leave != "")
        elg_warning("Invalid region names specified with -O option: '%s'",
                    filter->rgnname2leave.c_str());

    /* region id validator */
    ElgRidValidator vrid(filter);
    vrid(filter->rgn2leave);
    for_each(filter->rgn2remove.begin(), filter->rgn2remove.end(), vrid);
    for_each(filter->rgn2filter.begin(), filter->rgn2filter.end(), vrid);

    /* Type id validator */
    for_each(filter->type2filter.begin(), filter->type2filter.end(),
             ElgTypeValidator() );
}


void elg_readcb_ALL(elg_ui1 type, elg_ui1 length, void *userdata)
{
    //ElgFlt *filter = (ElgFlt *) userdata;
    done += (length + 2);
}

void adjust_timestamp(ElgFlt * filter, elg_d8 * time)
{
    /* initialize time correction if -z was specified */
    if (filter->zero_time == -2.0)
        filter->zero_time = *time;

    /* time correction if -O specified */
    if (filter->zero_time > 0.0)
        *time -= filter->zero_time;

}

/* return false if event must be filtered */
bool handle_event(ElgFlt * filter, elg_ui1 type, elg_ui4 lid, elg_d8 * time)
{
    if (filter->filtering_done && filter->rgn2leave_nesting[lid] == 0)
        return false;

    /* exit function if event 'type' must be filtered */
    if (filter->type2filter.find(type) != filter->type2filter.end())
        return false;

    /* exit function if -O 'region' is sepecified and we are out of region */
    if (filter->rgn2leave != ELG_NO_ID && filter->rgn2leave_nesting[lid] <= 0)
        return false;

    /* exit function if -I 'regions' are sepecified and we are in a  region */
    if (filter->rgn2filter_nesting[lid] > 0)
        return false;

    adjust_timestamp(filter, time);

    return true;
}

/* return false if events in region must be filtered */
bool handle_enter(ElgFlt * filter, elg_ui1 type, elg_ui4 lid, elg_d8 * time,
                  elg_ui4 rid, elg_ui1 * metc)
{
    set < elg_ui4 >::iterator it;
    int nesting_level;

    if (filter->filtering_done && filter->rgn2leave_nesting[lid] == 0)
    {
        /* warning */
        if (rid == filter->rgn2leave)
        {
            printf("\n");
            const string & name =
                filter->names[filter->regions[rid]->rnid]->str;
            elg_warning
                ("Multiplete instances of region '%s' found at location: %d",
                 name.c_str(), lid);
        }
        return false;
    }

    /* put region on location stack */
    filter->rgn_stacks[lid].push(rid);

    /* cut metric if requested */
    if (filter->cut_metric)
        *metc = 0;

    /* we are entering region of interest (-i Option) */
    nesting_level = filter->rgn2filter_nesting[lid];
    it = filter->rgn2filter.find(rid);
    if (it != filter->rgn2filter.end())
        filter->rgn2filter_nesting[lid]++;
    /* exit function if region must be filtered (-i Option) */
    if (nesting_level > 0)
        return false;

    /* check if all messages in the region 'rid' must be filtered (-R or -r option) */
    it = filter->rgn2remove.find(rid);
    if (it != filter->rgn2remove.end())
        return false;

    /* we are entering region of interest (-o option) */
    if (rid == filter->rgn2leave)
    {
        filter->rgn2leave_nesting[lid]++;
        filter->zero_time = *time;
    }
    adjust_timestamp(filter, time);

    /* exit function if -o sepecified and we are out of region */
    if (filter->rgn2leave != ELG_NO_ID && filter->rgn2leave_nesting[lid] <= 0)
        return false;

    /* exit function if region 'type' must be filtered */
    if (filter->type2filter.find(type) != filter->type2filter.end())
        return false;

    return true;
}

/* return false if event must be filtered */
bool handle_exit(ElgFlt * filter, elg_ui1 type, elg_ui4 lid, elg_d8 * time,
                 elg_ui1 * metc)
{
    set < elg_ui4 >::iterator it;
    elg_ui4 rid;
    int nesting_level;

    if (filter->filtering_done && filter->rgn2leave_nesting[lid] == 0)
        return false;

    rid = filter->rgn_stacks[lid].top();
    filter->rgn_stacks[lid].pop();

    /* cut metric if requested */
    if (filter->cut_metric)
        *metc = 0;

    adjust_timestamp(filter, time);

    /* exiting -o  region */
    nesting_level = filter->rgn2leave_nesting[lid];
    if (rid == filter->rgn2leave)
    {
        filter->rgn2leave_nesting[lid]--;
        if (nesting_level == 1)
            filter->filtering_done = true;
        return true;
    }

    /* exit function if -O 'region' is sepecified and we are out of region */
    if (filter->rgn2leave != ELG_NO_ID && nesting_level <= 0)
        return false;

    /* exiting region of interest (-i Option) */
    it = filter->rgn2filter.find(rid);
    if (it != filter->rgn2filter.end())
        filter->rgn2filter_nesting[lid]--;

    /* check if we are in a -i region */
    if (filter->rgn2filter_nesting[lid] > 0)
        return false;

    /* check if all messages out region 'rid' must be writen (-R or -r option ) */
    it = filter->rgn2remove.find(rid);
    if (it != filter->rgn2remove.end())
        return false;

    /* exit function if region 'type' must be filtered */
    if (filter->type2filter.find(type) != filter->type2filter.end())
        return false;

    return true;
}

/* return false if event must be filtered */
bool handle_1ts(ElgFlt * filter, elg_ui1 type, elg_ui4 lid, elg_d8* time,
                elg_ui4 dlid, elg_ui4 wid, elg_ui4 rmaid)
{
    ElgTransfer start;

    /* add 1TS to transfer queue */
    start.type  = type;
    start.spid  = lid;
    start.dpid  = dlid;
    start.wid   = wid;
    start.rmaid = rmaid;
    start.filtered = true;       /* default value */
    filter->tqueue.push_back(start);

    /* do common stuff for events */
    if (!handle_event(filter, type, lid, time))
        return false;

    filter->tqueue.back().filtered = false;
    return true;
}

/* return false if event must be filtered */
bool handle_1te(ElgFlt * filter, elg_ui1 type, elg_ui1 stype,
                elg_ui4 lid, elg_d8* time,
                elg_ui4 slid, elg_ui4 wid, elg_ui4 rmaid)
{
    ElgTransferQueue::iterator itStart;
    bool start_filtered;
    ElgTransfer start;

    /* find coresponding 1TS  */
    start.type  = stype;
    start.spid  = slid;
    start.dpid  = lid;
    start.wid   = wid;
    start.rmaid = rmaid;

    itStart = find(filter->tqueue.begin(), filter->tqueue.end(), start);
    if (itStart != filter->tqueue.end())
    {
        start_filtered = itStart->filtered;
        filter->tqueue.erase(itStart);
        if (start_filtered)
            return false;
    }
    else
    {
        elg_warning("1TE before 1TS");
    }

    /* exit function if event type must be filtered */
    if (filter->type2filter.find(type) != filter->type2filter.end())
        return false;

    adjust_timestamp(filter, time);

    return true;
}

void elg_readcb_STRING(elg_ui4 strid,
                       elg_ui1 cntc, const char *str, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    char buffer[251];
    ElgString *elg_str = new ElgString;
    elg_str->strid = strid;
    elg_str->cntc = cntc;

    if (cntc == 0)
    {
        /* short strings */
        elg_str->str = str;
    }
    else
    {
        /* continued strings */
        memcpy(buffer, str, 250);
        buffer[250] = '\0';     /* enforce string termination */
        elg_str->str = buffer;
    }
    filter->names.push_back(elg_str);
    ElgOut_write_STRING(filter->out, strid, cntc, str);
}

void elg_readcb_STRING_CNT(const char *str, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgString *elg_str;
    char buffer[256];

    strncpy(buffer, str, 255);
    buffer[255] = '\0';         /* enforce string termination */

    elg_str = filter->names.back();
    elg_str->str += buffer;

    ElgOut_write_STRING_CNT(filter->out, str);
}

void elg_readcb_MACHINE(elg_ui4 mid,
                        elg_ui4 nodec, elg_ui4 mnid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_MACHINE(filter->out, mid, nodec, mnid);
}

void elg_readcb_NODE(elg_ui4 nid,
                     elg_ui4 mid,
                     elg_ui4 cpuc, elg_ui4 nnid, elg_d8 cr, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_NODE(filter->out, nid, mid, cpuc, nnid, cr);
}

void elg_readcb_PROCESS(elg_ui4 pid, elg_ui4 pnid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_PROCESS(filter->out, pid, pnid);
}

void elg_readcb_THREAD(elg_ui4 tid, elg_ui4 pid, elg_ui4 tnid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_THREAD(filter->out, tid, pid, tnid);
}

void elg_readcb_LOCATION(elg_ui4 lid,
                         elg_ui4 mid,
                         elg_ui4 nid,
                         elg_ui4 pid, elg_ui4 tid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    static stack < elg_ui4 > rgn_stack;

    ElgLocation *elg_location = new ElgLocation;
    elg_location->lid = lid;
    elg_location->mid = mid;
    elg_location->nid = nid;
    elg_location->pid = pid;
    elg_location->tid = tid;
    filter->locations.push_back(elg_location);


    /* create stack for each location */
    filter->rgn_stacks.push_back(rgn_stack);
    /* create nesting counter for each location */
    filter->rgn2leave_nesting.push_back(0);
    /* create nesting counter for each location */
    filter->rgn2filter_nesting.push_back(0);



    ElgOut_write_LOCATION(filter->out, lid, mid, nid, pid, tid);
}

void elg_readcb_FILE(elg_ui4 fid, elg_ui4 fnid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_FILE(filter->out, fid, fnid);
}

void elg_readcb_REGION(elg_ui4 rid,
                       elg_ui4 rnid,
                       elg_ui4 fid,
                       elg_ui4 begln,
                       elg_ui4 endln,
                       elg_ui4 rdid, elg_ui1 rtype, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    ElgRegion *elg_region = new ElgRegion;
    elg_region->rid = rid;
    elg_region->rnid = rnid;
    elg_region->fid = fid;
    elg_region->begln = begln;
    elg_region->endln = endln;
    elg_region->rdid = rdid;
    elg_region->rtype = rtype;
    filter->regions.push_back(elg_region);

    ElgOut_write_REGION(filter->out, rid, rnid, fid, begln, endln,
                        rdid, rtype);
}

void elg_readcb_CALL_PATH(elg_ui4 cpid,
                          elg_ui4 rid,
                          elg_ui4 ppid, elg_ui8 order, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    /* save call-path info to allow resolving 'cpid' */
    ElgCallPath *elg_cp = new ElgCallPath;
    elg_cp->cpid = cpid;
    elg_cp->rid = rid;
    elg_cp->ppid = ppid;
    elg_cp->order = order;
    filter->call_paths.push_back(elg_cp);

    ElgOut_write_CALL_PATH(filter->out, cpid, rid, ppid, order);
}

void elg_readcb_CALL_SITE(elg_ui4 csid,
                          elg_ui4 fid,
                          elg_ui4 lno,
                          elg_ui4 erid, elg_ui4 lrid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    /* save call site info to allow resolving 'csid' */
    ElgCallSite *elg_cs = new ElgCallSite;
    elg_cs->csid = csid;
    elg_cs->fid = fid;
    elg_cs->lno = lno;
    elg_cs->erid = erid;
    elg_cs->lrid = lrid;
    filter->call_sites.push_back(elg_cs);

    ElgOut_write_CALL_SITE(filter->out, csid, fid, lno, erid, lrid);
}

void elg_readcb_METRIC(elg_ui4 metid,
                       elg_ui4 metnid,
                       elg_ui4 metdid,
                       elg_ui1 metdtype,
                       elg_ui1 metmode, elg_ui1 metiv, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    if (filter->cut_metric)
        return;

    ElgOut_write_METRIC(filter->out, metid, metnid, metdid, metdtype,
                        metmode, metiv);
}

void elg_readcb_MPI_GROUP(elg_ui4 gid, elg_ui1 mode, elg_ui4 grpc,
                          elg_ui4 grpv[], void* userdata) 
{
    fprintf(stderr, "Unexpected ELG_MPI_GROUP record!");
    exit(EXIT_FAILURE);
}

void elg_readcb_MPI_COMM_DIST(elg_ui4 cid, elg_ui4 root, elg_ui4 lcid,
                              elg_ui4 lrank, elg_ui4 size, void* userdata ) 
{
    fprintf(stderr, "Unexpected ELG_MPI_COMM_DIST record!");
    exit(EXIT_FAILURE);
}

void elg_readcb_MPI_COMM_REF(elg_ui4 cid, elg_ui4 gid, void* userdata) 
{
    fprintf(stderr, "Unexpected ELG_MPI_COMM_REF record!");
    exit(EXIT_FAILURE);
}

void elg_readcb_MPI_COMM(elg_ui4 cid, elg_ui1 mode,
                         elg_ui4 grpc, elg_ui1 grpv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_MPI_COMM(filter->out, cid, mode, grpc, grpv);
}

void elg_readcb_CART_TOPOLOGY(elg_ui4 topid,
                              elg_ui4 tnid,
                              elg_ui4 cid,
                              elg_ui1 ndims,
                              elg_ui4 dimv[],
                              elg_ui1 periodv[],
                              elg_ui4 dimids[],
                              void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_CART_TOPOLOGY(filter->out, topid, tnid, cid, ndims, dimv, periodv, dimids);
}

void elg_readcb_CART_COORDS(elg_ui4 topid,
                            elg_ui4 lid,
                            elg_ui1 ndims, elg_ui4 coordv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_CART_COORDS(filter->out, topid, lid, ndims, coordv);
}

void elg_readcb_OFFSET(elg_d8 ltime, elg_d8 offset, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_OFFSET(filter->out, ltime, offset);
}

void elg_readcb_NUM_EVENTS(elg_ui4 eventc, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_NUM_EVENTS(filter->out, eventc);
}

void elg_readcb_EVENT_TYPES(elg_ui4 ntypes, elg_ui1 typev[], void *userdata)
{
/*  ElgFlt *filter = (ElgFlt *) userdata;                 */
/*  XXXX should be updated as a result of filtering! XXXX */
/*  ElgOut_write_EVENT_TYPES(filter->out, ntypes, typev); */
}

void elg_readcb_EVENT_COUNTS(elg_ui4 ntypes, elg_ui4 countv[], void *userdata)
{
/*  ElgFlt *filter = (ElgFlt *) userdata;                   */
/*  XXXX should be updated as a result of filtering! XXXX   */
/*  ElgOut_write_EVENT_COUNTS(filter->out, ntypes, countv); */
}

void elg_readcb_MAP_SECTION(elg_ui4 rank, void* userdata )
{
  /* NOT USED YET */
}

void elg_readcb_MAP_OFFSET(elg_ui4 rank, elg_ui4 offset, void* userdata )
{
  /* NOT USED YET */
}

void elg_readcb_IDMAP(elg_ui1 type, elg_ui1 mode, elg_ui4 count,
                      elg_ui4 mapv[], void* userdata )
{
  /* NOT USED YET */
}

void elg_readcb_LAST_DEF(void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_LAST_DEF(filter->out);
    ElgFlt_resolve(filter);

    if ( filter->verbose )    
        filter->print();
}

/* -- Attributes -- */

void elg_readcb_ATTR_UI1(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui1 val,       /* value                       */
                      void* userdata)
{ 
  /* ignore for now */
}

void elg_readcb_ATTR_UI4(
                      elg_ui1 attrtype,  /* attribute type              */
                      elg_ui4 val,       /* value                       */
                      void* userdata)
{ 
  /* ignore for now */
}

/* Events */

void elg_readcb_ENTER(elg_ui4 lid,
                      elg_d8 time,
                      elg_ui4 rid,
                      elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for an ENTERs events */
    if (!handle_enter(filter, ELG_ENTER, lid, &time, rid, &metc))
        return;

    ElgOut_write_ENTER(filter->out, lid, time, rid, metc, metv);
}

void elg_readcb_ENTER_CS(elg_ui4 lid,
                         elg_d8 time,
                         elg_ui4 csid,
                         elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    elg_ui4 rid;

    /* find id of entered region */
    rid = filter->call_sites[csid]->erid;

    /* do common stuff for an ENTERs events */
    if (!handle_enter(filter, ELG_ENTER_CS, lid, &time, rid, &metc))
        return;

    ElgOut_write_ENTER_CS(filter->out, lid, time, csid, metc, metv);
}

void elg_readcb_EXIT(elg_ui4 lid,
                     elg_d8 time,
                     elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for an EXITs events */
    if (!handle_exit(filter, ELG_EXIT, lid, &time, &metc))
        return;

    ElgOut_write_EXIT(filter->out, lid, time, metc, metv);
}

void elg_readcb_MPI_SEND(elg_ui4 lid,
                         elg_d8 time,
                         elg_ui4 dlid,
                         elg_ui4 cid,
                         elg_ui4 tag, elg_ui4 sent, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgMessage send;

    /* save all MPI_SENDs, find corresponding MPI_RECVs later */
    send.spid = lid;
    send.dpid = dlid;
    send.cid = cid;
    send.tag = tag;
    send.filtered = true;       /* default value */
    filter->mqueue.push_back(send);

    /* do common stuff for events */
    if (!handle_event(filter, ELG_MPI_SEND, lid, &time))
        return;

    filter->mqueue.back().filtered = false;
    ElgOut_write_MPI_SEND(filter->out, lid, time, dlid, cid, tag, sent);
}

void elg_readcb_MPI_RECV(elg_ui4 lid,
                         elg_d8 time,
                         elg_ui4 slid,
                         elg_ui4 cid, elg_ui4 tag, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    ElgMessageQueue::iterator itSend;
    bool send_filtered;
    ElgMessage send;

    /* find coresponding MPI_SEND  */
    send.spid = slid;
    send.dpid = lid;
    send.cid = cid;
    send.tag = tag;

    itSend = find(filter->mqueue.begin(), filter->mqueue.end(), send);
    if (itSend != filter->mqueue.end())
    {
        send_filtered = itSend->filtered;
        filter->mqueue.erase(itSend);
        if (send_filtered)
            return;
    }
    else
    {
        elg_warning("MPI_RECV before MPI_SEND");
    }

    /* exit function if event 'MPI_RECV' must be filtered */
    if (filter->type2filter.find(ELG_MPI_RECV) != filter->type2filter.end())
        return;

    adjust_timestamp(filter, &time);

    ElgOut_write_MPI_RECV(filter->out, lid, time, slid, cid, tag);
}

void elg_readcb_MPI_COLLEXIT(elg_ui4 lid, elg_d8 time, elg_ui1 metc,
                             elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid,
                             elg_ui4 sent, elg_ui4 recvd, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for an EXITs events */
    if (!handle_exit(filter, ELG_MPI_COLLEXIT, lid, &time, &metc))
        return;

    ElgOut_write_MPI_COLLEXIT(filter->out, lid, time, metc,
                              metv, rlid, cid, sent, recvd);
}

/* Non-blocking */

void elg_readcb_MPI_SEND_COMPLETE(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
  /* ignore for now */
}

void elg_readcb_MPI_RECV_REQUEST(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
  /* ignore for now */
}

void elg_readcb_MPI_REQUEST_TESTED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
  /* ignore for now */
}

void elg_readcb_MPI_CANCELLED(elg_ui4 lid, elg_d8 time, elg_ui4 reqid,
       void* userdata) 
{
  /* ignore for now */
}

/* RMA */

void elg_readcb_MPI_WIN(elg_ui4 wid, elg_ui4 cid, void* userdata) 
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgOut_write_MPI_WIN(filter->out, wid, cid);
}


void elg_readcb_MPI_WINEXIT(elg_ui4 lid, elg_d8 time,
                            elg_ui1 metc, elg_ui8 metv[],
                            elg_ui4 wid, elg_ui4 cid, elg_ui1 synex,
                            void* userdata) 
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for an EXITs events */
    if (!handle_exit(filter, ELG_MPI_WINEXIT, lid, &time, &metc))
        return;

    ElgOut_write_MPI_WINEXIT(filter->out, lid, time, metc,
                             metv, wid, cid, synex);
}

void elg_readcb_MPI_WINCOLLEXIT(elg_ui4 lid, elg_d8 time,
                                elg_ui1 metc, elg_ui8 metv[],
                                elg_ui4 wid, void* userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for an EXITs events */
    if (!handle_exit(filter, ELG_MPI_WINCOLLEXIT, lid, &time, &metc))
        return;

    ElgOut_write_MPI_WINCOLLEXIT(filter->out, lid, time, metc,
                                 metv, wid);
}

void elg_readcb_MPI_WIN_LOCK(elg_ui4 lid, elg_d8  time, elg_ui4 llid,
                             elg_ui4 wid, elg_ui1 ltype, void* userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_MPI_WIN_LOCK, lid, &time))
        return;

    ElgOut_write_MPI_WIN_LOCK(filter->out, lid, time, llid, wid, ltype);
}

void elg_readcb_MPI_WIN_UNLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 llid,
                               elg_ui4 wid, void* userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_MPI_WIN_UNLOCK, lid, &time))
        return;

    ElgOut_write_MPI_WIN_UNLOCK(filter->out, lid, time, llid, wid);
}

void elg_readcb_MPI_PUT_1TS(elg_ui4 lid, elg_d8  time, elg_ui4 dlid,
			    elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes,
                            void* userdata) 
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1ts(filter, ELG_MPI_PUT_1TS, lid, &time, dlid, wid, rmaid))
        return;

    ElgOut_write_MPI_PUT_1TS(filter->out, lid, time, dlid, wid, rmaid, nbytes);
}


void elg_readcb_MPI_PUT_1TE(elg_ui4 lid, elg_d8  time, elg_ui4 slid,
			    elg_ui4 wid, elg_ui4 rmaid, void* userdata) 
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1te(filter, ELG_MPI_PUT_1TE, ELG_MPI_PUT_1TS, lid, &time,
                    slid, wid, rmaid))
        return;

    ElgOut_write_MPI_PUT_1TE(filter->out, lid, time, slid, wid, rmaid);
}

void elg_readcb_MPI_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TO(elg_ui4 lid, elg_d8  time, elg_ui4 rmaid,
                            void* userdata) 
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_MPI_GET_1TO, lid, &time))
        return;

    ElgOut_write_MPI_GET_1TO(filter->out, lid, time, rmaid);
}

void elg_readcb_MPI_GET_1TS(elg_ui4 lid, elg_d8  time, elg_ui4 dlid,
			    elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes,
                            void* userdata) 
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1ts(filter, ELG_MPI_GET_1TS, lid, &time, dlid, wid, rmaid))
        return;

    ElgOut_write_MPI_GET_1TS(filter->out, lid, time, dlid, wid, rmaid, nbytes);
}

void elg_readcb_MPI_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 wid, elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_MPI_GET_1TE(elg_ui4 lid, elg_d8  time, elg_ui4 slid,
			    elg_ui4 wid, elg_ui4 rmaid, void* userdata) 
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1te(filter, ELG_MPI_GET_1TE, ELG_MPI_GET_1TS, lid, &time,
                    slid, wid, rmaid))
        return;

    ElgOut_write_MPI_GET_1TE(filter->out, lid, time, slid, wid, rmaid);
}

void elg_readcb_PUT_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1ts(filter, ELG_PUT_1TS, lid, &time, dlid, ELG_NO_ID, rmaid))
        return;

    ElgOut_write_PUT_1TS(filter->out, lid, time, dlid, rmaid, nbytes);
}

void elg_readcb_PUT_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1te(filter, ELG_PUT_1TE, ELG_PUT_1TS, lid, &time,
                    slid, ELG_NO_ID, rmaid))
        return;

    ElgOut_write_PUT_1TE(filter->out, lid, time, slid, rmaid);
}

void elg_readcb_PUT_1TE_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TS(elg_ui4 lid, elg_d8 time, elg_ui4 dlid, elg_ui4 rmaid,
       elg_ui4 nbytes, void* userdata) {
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1ts(filter, ELG_GET_1TS, lid, &time, dlid, ELG_NO_ID, rmaid))
        return;

    ElgOut_write_GET_1TS(filter->out, lid, time, dlid, rmaid, nbytes);
}

void elg_readcb_GET_1TS_REMOTE(elg_ui4 lid, elg_d8 time, elg_ui4 dlid,
       elg_ui4 rmaid, elg_ui4 nbytes, void* userdata) {
  /* INTERNAL EVENT - IGNORED */
}

void elg_readcb_GET_1TE(elg_ui4 lid, elg_d8 time, elg_ui4 slid, elg_ui4 rmaid,
       void* userdata) {
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_1te(filter, ELG_GET_1TE, ELG_GET_1TS, lid, &time,
                    slid, ELG_NO_ID, rmaid))
        return;

    ElgOut_write_GET_1TE(filter->out, lid, time, slid, rmaid);
}

void elg_readcb_COLLEXIT(elg_ui4 lid, elg_d8 time,
       elg_ui1 metc, elg_ui8 metv[], elg_ui4 rlid, elg_ui4 cid, elg_ui4 sent,
       elg_ui4 recvd, void* userdata) {
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for an EXITs events */
    if (!handle_exit(filter, ELG_COLLEXIT, lid, &time, &metc))
        return;

    ElgOut_write_COLLEXIT(filter->out, lid, time, metc,
                          metv, rlid, cid, sent, recvd);
}

void elg_readcb_ALOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid, void* userdata) {
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_ALOCK, lid, &time))
        return;

    ElgOut_write_ALOCK(filter->out, lid, time, lkid);

}

void elg_readcb_RLOCK(elg_ui4 lid, elg_d8 time, elg_ui4 lkid, void* userdata) {
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_RLOCK, lid, &time))
        return;

    ElgOut_write_RLOCK(filter->out, lid, time, lkid);
}

struct ElgSlaveCheck
{
    ElgFlt *filter;
    ElgLocation *master;
    ElgSlaveCheck(ElgFlt * flt, ElgLocation * mst):filter(flt), master(mst)
    {
    }
    ElgSlaveCheck(ElgFlt * flt, elg_ui4 mst_lid):filter(flt),
        master(flt->locations[mst_lid])
    {
    }
    bool operator() (elg_ui4 lid)
    {
        return operator()(filter->locations[lid]);
    }
    bool operator() (ElgLocation * slv)
    {
        return master->mid == slv->mid &&
            master->nid == slv->nid &&
            master->pid == slv->pid && master->tid != slv->tid;
    }
};

void elg_readcb_OMP_FORK(elg_ui4 lid, elg_d8 time, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgSlaveCheck is_slave(filter, lid);

    /* if -I 'regions' are sepecified and OMP_FORK occures in one of specified regions */
    if (filter->rgn2filter_nesting[lid] > 0)
    {
        elg_ui4 l, n = filter->locations.size();
        /* only for slaves */
        for (l = 0; l < n; l++)
            if (is_slave(l))
                filter->rgn2filter_nesting[l]++;
    }

    /* if -O 'region' is sepecified and OMP_FORK occures in specified region */
    if (filter->rgn2leave_nesting[lid] > 0)
    {
        elg_ui4 l, n = filter->locations.size();
        /* only for slaves */
        for (l = 0; l < n; l++)
            if (is_slave(l))
                filter->rgn2leave_nesting[l]++;
    }

    /* do common stuff for events */
    if (!handle_event(filter, ELG_OMP_FORK, lid, &time))
        return;

    ElgOut_write_OMP_FORK(filter->out, lid, time);
}

void elg_readcb_OMP_JOIN(elg_ui4 lid, elg_d8 time, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;
    ElgSlaveCheck is_slave(filter, lid);

    /* if -I 'regions' are sepecified and OMP_FORK occures in one of specified regions */
    if (filter->rgn2filter_nesting[lid] > 0)
    {
        elg_ui4 l, n = filter->locations.size();
        /* only for slaves */
        for (l = 0; l < n; l++)
            if (is_slave(l))
                filter->rgn2filter_nesting[l]--;
    }

    /* if -O 'region' is sepecified and OMP_JOIN occures in specified region */
    if (filter->rgn2leave_nesting[lid] > 0)
    {
        elg_ui4 l, n = filter->locations.size();
        /* only for slaves */
        for (l = 0; l < n; l++)
            if (is_slave(l))
                filter->rgn2leave_nesting[l]--;
    }


    /* do common stuff for events */
    if (!handle_event(filter, ELG_OMP_JOIN, lid, &time))
        return;

    ElgOut_write_OMP_JOIN(filter->out, lid, time);
}

void elg_readcb_OMP_ALOCK(elg_ui4 lid,
                          elg_d8 time, elg_ui4 lkid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_OMP_ALOCK, lid, &time))
        return;

    ElgOut_write_OMP_ALOCK(filter->out, lid, time, lkid);
}

void elg_readcb_OMP_RLOCK(elg_ui4 lid,
                          elg_d8 time, elg_ui4 lkid, void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_OMP_RLOCK, lid, &time))
        return;

    ElgOut_write_OMP_RLOCK(filter->out, lid, time, lkid);
}

void elg_readcb_OMP_COLLEXIT(elg_ui4 lid,
                             elg_d8 time,
                             elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for an EXITs events */
    if (!handle_exit(filter, ELG_OMP_COLLEXIT, lid, &time, &metc))
        return;

    ElgOut_write_OMP_COLLEXIT(filter->out, lid, time, metc, metv);
}

void elg_readcb_ENTER_TRACING(elg_ui4 lid,
                           elg_d8 time,
                           elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_ENTER_TRACING, lid, &time))
        return;

    ElgOut_write_ENTER_TRACING(filter->out, lid, time, metc, metv);
}

void elg_readcb_EXIT_TRACING(elg_ui4 lid,
                          elg_d8 time,
                          elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_EXIT_TRACING, lid, &time))
        return;

    ElgOut_write_EXIT_TRACING(filter->out, lid, time, metc, metv);
}

void elg_readcb_LOG_OFF(elg_ui4 lid,
                        elg_d8 time,
                        elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_LOG_OFF, lid, &time))
        return;

    ElgOut_write_LOG_OFF(filter->out, lid, time, metc, metv);
}

void elg_readcb_LOG_ON(elg_ui4 lid,
                       elg_d8 time,
                       elg_ui1 metc, elg_ui8 metv[], void *userdata)
{
    ElgFlt *filter = (ElgFlt *) userdata;

    /* do common stuff for events */
    if (!handle_event(filter, ELG_LOG_ON, lid, &time))
        return;

    ElgOut_write_LOG_ON(filter->out, lid, time, metc, metv);
}



int parse_opt(int argc, char *argv[], ElgFlt * filter)
{
    int ch;
    while ((ch = getopt(argc, argv, "t:T:r:R:mnI:i:O:o:h?vz")) != EOF)
    {
        switch (ch)
        {
        case 't':
            filter->type2filter.insert(atoi(optarg));
            break;
        case 'T':
            filter->typename2filter.insert(optarg);
            break;
        case 'i':
            if (filter->rgnname2leave != "")
                elg_error_msg("Options -O and -i are mutual exclusive");
            if (filter->rgn2leave != ELG_NO_ID)
                elg_error_msg("Options -o and -i are mutual exclusive");
            /* -i Ok */
            filter->rgn2filter.insert(atoi(optarg));
            break;
        case 'I':
            if (filter->rgnname2leave != "")
                elg_error_msg("Options -O and -I are mutual exclusive");
            if (filter->rgn2leave != ELG_NO_ID)
                elg_error_msg("Options -o and -I are mutual exclusive");
            /* -I Ok */
            filter->rgnname2filter[optarg] = false;
            break;
        case 'o':
            if (filter->rgnname2leave != "" || filter->rgn2leave != ELG_NO_ID)
                elg_error_msg("multiple -O/-o options not allowed");
            if (!filter->rgnname2filter.empty())
                elg_error_msg("Options -I and -o are mutual exclusive");
            if (!filter->rgn2filter.empty())
                elg_error_msg("Options -i and -o are mutual exclusive");
            /* -o Ok */
            filter->rgn2leave = atoi(optarg);
            break;
        case 'O':
            if (filter->rgnname2leave != "" || filter->rgn2leave != ELG_NO_ID)
                elg_error_msg("multiple -O/-o options not allowed");
            if (!filter->rgnname2filter.empty())
                elg_error_msg("Options -I and -O are mutual exclusive");
            if (!filter->rgn2filter.empty())
                elg_error_msg("Options -i and -O are mutual exclusive");
            /* -O Ok */
            filter->rgnname2leave = optarg;
            break;
        case 'r':
            filter->rgn2remove.insert(atoi(optarg));
            break;
        case 'R':
            filter->rgnname2remove[optarg] = false;
            break;
        case 'm':
            filter->cut_metric = true;
            break;
        case 'n':
            filter->native_bo = true;
            break;
        case 'h':
        case '?':
            return argc;
        case 'v':
	    filter->verbose = true;
	    break;
        case 'z':
	    filter->zero_time = -2.0;
	    break;
        }
    }
    return optind;
}

void ElgFlt_init()
{
    msg_types["ENTER"] = ELG_ENTER;
    msg_types["EXIT"] = ELG_EXIT;
    msg_types["MPI_SEND"] = ELG_MPI_SEND;
    msg_types["MPI_RECV"] = ELG_MPI_RECV;
    msg_types["MPI_COLLEXIT"] = ELG_MPI_COLLEXIT;
    msg_types["OMP_FORK"] = ELG_OMP_FORK;
    msg_types["OMP_JOIN"] = ELG_OMP_JOIN;
    msg_types["OMP_ALOCK"] = ELG_OMP_ALOCK;
    msg_types["OMP_RLOCK"] = ELG_OMP_RLOCK;
    msg_types["OMP_COLLEXIT"] = ELG_OMP_COLLEXIT;
    msg_types["ENTER_CS"] = ELG_ENTER_CS;
    msg_types["MPI_PUT_1TS"] = ELG_MPI_PUT_1TS;
    msg_types["MPI_PUT_1TE"] = ELG_MPI_PUT_1TE;
    msg_types["MPI_GET_1TS"] = ELG_MPI_GET_1TS;
    msg_types["MPI_GET_1TE"] = ELG_MPI_GET_1TE;
    msg_types["MPI_GET_1TO"] = ELG_MPI_GET_1TO;
    msg_types["MPI_WINEXIT"] = ELG_MPI_WINEXIT;
    msg_types["MPI_WINCOLLEXIT"] = ELG_MPI_WINCOLLEXIT;
    msg_types["MPI_WIN_LOCK"] = ELG_MPI_WIN_LOCK;
    msg_types["MPI_WIN_UNLOCK"] = ELG_MPI_WIN_UNLOCK;
    msg_types["PUT_1TS"] = ELG_PUT_1TS;
    msg_types["PUT_1TE"] = ELG_PUT_1TE;
    msg_types["GET_1TS"] = ELG_GET_1TS;
    msg_types["GET_1TE"] = ELG_GET_1TE;
    msg_types["COLLEXIT"] = ELG_COLLEXIT;
    msg_types["ALOCK"] = ELG_ALOCK;
    msg_types["RLOCK"] = ELG_RLOCK;
    msg_types["LOG_OFF"] = ELG_LOG_OFF;
    msg_types["LOG_ON"] = ELG_LOG_ON;
    msg_types["ENTER_TRACING"] = ELG_ENTER_TRACING;
    msg_types["EXIT_TRACING"] = ELG_EXIT_TRACING;

    msg_types["ELG_ENTER"] = ELG_ENTER;
    msg_types["ELG_ENTER"] = ELG_ENTER;
    msg_types["ELG_EXIT"] = ELG_EXIT;
    msg_types["ELG_MPI_SEND"] = ELG_MPI_SEND;
    msg_types["ELG_MPI_RECV"] = ELG_MPI_RECV;
    msg_types["ELG_MPI_COLLEXIT"] = ELG_MPI_COLLEXIT;
    msg_types["ELG_OMP_FORK"] = ELG_OMP_FORK;
    msg_types["ELG_OMP_JOIN"] = ELG_OMP_JOIN;
    msg_types["ELG_OMP_ALOCK"] = ELG_OMP_ALOCK;
    msg_types["ELG_OMP_RLOCK"] = ELG_OMP_RLOCK;
    msg_types["ELG_OMP_COLLEXIT"] = ELG_OMP_COLLEXIT;
    msg_types["ELG_ENTER_CS"] = ELG_ENTER_CS;
    msg_types["ELG_MPI_PUT_1TS"] = ELG_MPI_PUT_1TS;
    msg_types["ELG_MPI_PUT_1TE"] = ELG_MPI_PUT_1TE;
    msg_types["ELG_MPI_GET_1TS"] = ELG_MPI_GET_1TS;
    msg_types["ELG_MPI_GET_1TE"] = ELG_MPI_GET_1TE;
    msg_types["ELG_MPI_GET_1TO"] = ELG_MPI_GET_1TO;
    msg_types["ELG_MPI_WINEXIT"] = ELG_MPI_WINEXIT;
    msg_types["ELG_MPI_WINCOLLEXIT"] = ELG_MPI_WINCOLLEXIT;
    msg_types["ELG_MPI_WIN_LOCK"] = ELG_MPI_WIN_LOCK;
    msg_types["ELG_MPI_WIN_UNLOCK"] = ELG_MPI_WIN_UNLOCK;
    msg_types["ELG_PUT_1TS"] = ELG_PUT_1TS;
    msg_types["ELG_PUT_1TE"] = ELG_PUT_1TE;
    msg_types["ELG_GET_1TS"] = ELG_GET_1TS;
    msg_types["ELG_GET_1TE"] = ELG_GET_1TE;
    msg_types["ELG_COLLEXIT"] = ELG_COLLEXIT;
    msg_types["ELG_ALOCK"] = ELG_ALOCK;
    msg_types["ELG_RLOCK"] = ELG_RLOCK;
    msg_types["ELG_LOG_OFF"] = ELG_LOG_OFF;
    msg_types["ELG_LOG_ON"] = ELG_LOG_ON;
    msg_types["ELG_ENTER_TRACING"] = ELG_ENTER_TRACING;
    msg_types["ELG_EXIT_TRACING"] = ELG_EXIT_TRACING;

}

void print_usage(const char *file)
{
    fprintf(stderr,
            "\nUsage:    %s [OPTIONS]...  (<infile>.elg | <experiment_archive>) <outfile>.elg\n\n"
            "[OPTIONS] are:\n"
            "  -m                 - filter all metrics\n"
            "  -n                 - transform to native byteorder\n"
            "  -t <msg type ID>   - filter all records of this type \n"
            "  -T <msg type name> - filter all records of this type \n"
            "  -r <region ID>     - filter <ENTER> and <EXIT> records of region  \n"
            "  -R <region name>   - filter <ENTER> and <EXIT> records of region  \n"
            "  -o <region ID>     - filter all events outside of region\n"
            "  -O <region name>   - filter all events outside of region\n"
            "  -i <region ID>     - filter all events in region\n"
            "  -I <region name>   - filter all events in region\n\n"
            "  -T MPI    is the same as   -T MPI_SEND -T MPI_RECV\n"
            "  -T MPIPUT is the same as   -T MPI_PUT_1TS -T MPI_PUT_1TE\n"
            "  -T MPIGET is the same as   -T MPI_GET_1TS -T MPI_GET_1TE -T MPI_GET_1TO\n"
            "  -T PUT    is the same as   -T PUT_1TS -T PUT_1TE\n"
            "  -T GET    is the same as   -T GET_1TS -T GET_1TE\n"
            "  -T OMP is the same as   -T OMP_FORK -T OMP_JOIN "
            "-T OMP_ALOCK -T OMP_RLOCK\n"
            "  -v                 - verbose output\n"
            "  -z                 - fix non-zero begin of timeline\n"
            "\nExamples:\n"
            "  %s -R MPI_Init -R MPI_Finalize in.elg out.elg\n"
            "  %s -O my_mat_mult in.elg out.elg\n"
            "  %s -I my_send -I my_recv -I MPI_Isend -I MPI_Irecv in.elg out.elg\n"
            "  %s -I my_send -I my_recv -R my_send   -R my_recv   in.elg out.elg\n",
            file, file, file, file, file);

}

int main(int argc, char **argv)
{
    char *infile;
    char *outfile;
    int opt_ind;
    ElgFlt filter;
    struct stat statbuf;
    elg_ui4 numrec=0;
    elg_ui1 byteorder;

    opt_ind = parse_opt(argc, argv, &filter);

    if ((argc - opt_ind) != 2)
    {
        print_usage(argv[0]);
        return 1;
    }

    /* init filter */
    ElgFlt_init();

    /* init IO */
    infile = epk_get_elgfilename(argv[optind]);
    outfile = argv[optind + 1];

    filter.in = elg_read_open(infile);
    if (filter.in == NULL)
        elg_error_msg("Open intput file failed.\n");
    stat(infile, &statbuf);

    if (filter.native_bo)
      byteorder = ELG_BYTE_ORDER;
    else
      byteorder = elg_read_byteorder(filter.in);
      

    filter.out = ElgOut_open(outfile, byteorder, ELG_UNCOMPRESSED);
    if (filter.out == NULL)
        elg_error_msg("Open output file failed.\n");

    /* main loop */
    while (elg_read_next(filter.in, &filter))
    {
       numrec++;
       if ( numrec % 10000 == 0 ) {
             printf("filtering progress: %3.0f%%\r",
                   ((double) done / statbuf.st_size) * 100.0);
             fflush(stdout);
       }
    }
    
    printf("\nfiltering done\n");

    /* check message queue */
    if (filter.mqueue.size() > 0)
    {
        elg_warning("%d MPI_SEND still in message queue",
                    filter.mqueue.size());
    }

    /* check transfer queue */
    if (filter.tqueue.size() > 0)
    {
        elg_warning("%d PUT/GET still in transfer queue",
                    filter.tqueue.size());
    }

    /* finalize IO */
    elg_read_close(filter.in);
    ElgOut_close(filter.out);

    return 0;
}
