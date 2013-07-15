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

#define HACK_elg_cntl_msg elg_cntl_msg

/* handling of events which modify callpath */

#if 0
XXXX might make callpath definitions on-the-fly or by traversal at finale
XXXX which accounts should be maintained by EPISODE (vs EPITOME)
     - path first entry time needed to ensure global calltree consistency
     - other metrics (such as count) could be basis for filtering
#endif

#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "epk_conf.h"
#include "esd_def.h"
#include "esd_paths.h"
#include "esd_event.h"
#include "elg_error.h"
#include "epk_vector.h"

/* runtime summarisation active */
EXTERN int esd_summary;

/* number of performance metrics (excluding time) */
EXTERN int esd_num_metrics;

/* reserved identifier for unknown/undefined regions */
EXTERN elg_ui4 esd_unknown_rid;

static double esd_clock_drift_factor = 1.0;

/* number of tracked frames */
static const unsigned EsdMinFrames = 1;
static unsigned EsdMaxFrames = 0; /* initialised by epk_get(ESD_FRAMES) */

static size_t esd_valv_size = 0;
static size_t esd_metrics_size = 0;

/* number of stored paths */
static const unsigned EsdMinPaths = 1;
static unsigned EsdMaxPaths = 0; /* initialised by epk_get(ESD_PATHS) */

/* fork path */
/*static*/ EsdPathIndex_t* esd_forkpath = NULL;

/* vector of fork region identifiers */
static EpkVector* esd_fork_ridv = NULL;

void esd_def_fork_rid (elg_ui4 rid)
{
    unsigned long tmp = rid;
    elg_ui4* ridp = (elg_ui4*)tmp;
    if (esd_fork_ridv == NULL) esd_fork_ridv = epk_vector_create();
    elg_cntl_msg("esd_fork_ridv[%d]:=%lu", epk_vector_size(esd_fork_ridv), ridp);
    epk_vector_push_back(esd_fork_ridv, ridp);
}

int rid_cmp (const void* value, const void* item)
{
    if (value == item) return 0;
    else if (value > item) return 1;
    else return -1;
}

int esd_is_fork_rid (elg_ui4 rid)
{
    unsigned long tmp = rid;
    elg_ui4* ridp = (elg_ui4*)tmp;
    if (esd_fork_ridv == NULL) return 0;
    return epk_vector_find(esd_fork_ridv, ridp, (epk_vec_cmp_f)&rid_cmp, NULL);
}


EsdPaths_t* esd_paths_init (void)
{
    int f;
    EsdPaths_t* paths = calloc(1, sizeof(EsdPaths_t));
    EsdMaxPaths = epk_str2int(epk_get(ESD_PATHS));
    EsdMaxFrames = epk_str2int(epk_get(ESD_FRAMES));
    if (EsdMaxPaths < EsdMinPaths) EsdMaxPaths = EsdMinPaths;
    if (EsdMaxFrames < EsdMinFrames) EsdMaxFrames = EsdMinFrames;
    elg_cntl_msg("esd_paths_init(EsdMaxFrames=%u, EsdMaxPaths=%u)",
                EsdMaxFrames, EsdMaxPaths);
    if (!paths)
        elg_error_msg("Failed to allocate path management object: %s",
                strerror(errno));

    esd_metrics_size = esd_num_metrics+1;
    esd_valv_size = sizeof(EsdMeasurement_t) * esd_metrics_size;

    paths->framev = calloc(EsdMaxFrames, sizeof(EsdMeasurement_t*));
    if (!paths->framev)
        elg_error_msg("Failed to allocate vector for %d frames: %s",
                EsdMaxFrames, strerror(errno));
    for (f=0; f<EsdMaxFrames; f++) {
        paths->framev[f] = calloc(esd_num_metrics+1, sizeof(EsdMeasurement_t));
        if (!paths->framev[f])
            elg_error_msg("Failed to allocate valv for frame[%d]: %s",
                    f, strerror(errno));
    }

    paths->pathv = calloc(EsdMaxPaths, sizeof(EsdPathIndex_t));
    if (!paths->pathv)
        elg_error_msg("Failed to allocate buffer for %d paths: %s",
                EsdMaxPaths, strerror(errno));
    else elg_cntl_msg("pathv@%p", paths->pathv);

    paths->masterv = NULL;           /* allocated during unification */

    paths->last_recv_enter = 0.0;
    paths->first_recv_time = 0.0;
    
    paths->stkdepth = 0;             /* maximum stack depth */
    paths->stkframe = (-1);          /* current stack frame */
    paths->pathid = (elg_ui4)(-1);   /* (last) path identifier */

    if (esd_summary) {
        paths->valv_sum = calloc(EsdMaxPaths, esd_valv_size);
        if (!paths->valv_sum)
            elg_error_msg("Failed to allocate buffer for %d path sum values: %s",
                          EsdMaxPaths, strerror(errno));
    }

    elg_cntl_msg("Management object for %d paths initialized", EsdMaxPaths);
    return paths;
}

void esd_path_dump (EsdPaths_t* paths, EsdPathIndex_t *path)
{
    int currpathid = path - paths->pathv;
#if 0 // unknown path record
    EsdPathIndex_t* ppath = 0;
    EsdPathRecord_t record = path->record;

    if (currpathid == 0) {
        printf("Path %3d: %6d#[%.6f,%.6f] : { UNKNOWN/TRUNCATED PATHS }\n",
                currpathid, record.count, record.first_time, record.final_time);
        return;
    }
#endif

    printf("%p ", path);
    printf("Path %3d: Node %3d Parent %3ld Next %3ld Child %3ld\n", currpathid,
        path->nodeid, path->parent ? (long) (path->parent - paths->pathv) : (-1L),
                      path->next   ? (long) (path->next   - paths->pathv) : (-1L),
                      path->child  ? (long) (path->child  - paths->pathv) : (-1L));
#if 0 // dump basic path record
    printf("Path %3d: %6d#[%.6f,%.6f] :",
            currpathid, record.count, record.first_time, record.final_time);
    printf(" { %3d", path->nodeid);
    ppath = path;
    while (ppath = ppath->parent) printf(" < %3d", ppath->nodeid);
    printf(" }\n");
#endif

    /* consistency checks! */
    if (path->parent && ((path->parent - paths->pathv) > currpathid))
        printf("Path %3d: ERROR inconsistent parent %p (%ld)\n",
                currpathid, path->parent, (long) (path->parent - paths->pathv));
    if (path->parent && ((path->parent - paths->pathv) < 0))
        printf("Path %3d: ERROR inconsistent parent %p (%ld)\n",
                currpathid, path->parent, (long) (path->parent - paths->pathv));
    if (path->next   && ((path->next - paths->pathv) <= currpathid))
        printf("Path %3d: ERROR inconsistent next %ld\n",
                currpathid, (long) (path->next - paths->pathv));
    if (path->child  && ((path->child - paths->pathv) <= currpathid))
        printf("Path %3d: ERROR inconsistent child %ld\n",
                currpathid, (long) (path->child - paths->pathv));

#if 0
    printf("   walltime: %.9f", GET_METX(record.sum));
    if (record.count > 1) {
        elg_d8 mean_time = GET_METX(record.sum) / record.count;
        Dprintf(" : %.9f", mean_time);
        elg_d8 sdev_time = sqrt((GET_METX(record.ssq) / record.count) - (mean_time*mean_time));
        Dprintf(" : (%.9f:%u,%.9f:%u) : %.2f%%%%", mean_time,
                GET_METX(record.min), GET_METX(record.min_index),
                GET_METX(record.max), GET_METX(record.max_index),
                100.0*sdev_time/mean_time);
    }
    Dprintf("\n");
    for (unsigned int m=0; m<esd_num_metrics; m++) {
        Dprintf("   metric_%d: %llu", m, GET_METV(record.sum, m));
        if (record.count > 1) {
            elg_d8 mean_metval = (elg_d8)(GET_METV(record.sum, m)) / record.count;
            Dprintf(" : %g", mean_metval);
#if 0
            elg_d8 sdev_metval = sqrt(((elg_d8)(GET_METV(record.ssq, m)) / record.count)
                                        - (mean_metval*mean_metval));
            Dprintf(" : (%llu:%u,%llu:%u) : %.2f%%%%", mean_metval,
                GET_METV(record.min, m), GET_METV(record.min_index, m),
                GET_METV(record.max, m), GET_METV(record.max_index, m),
                100.0*sdev_metval/mean_metval);
#endif
        }
        Dprintf("\n");
    }
#endif
}

void esd_pathv_dump (EsdPaths_t* paths)
{
    int i;
    printf("esd_pathv_dump(#paths=%d [%d])\n", paths->pathid, EsdMaxPaths);
    esd_path_dump(paths, &paths->pathv[0]); /* unknown paths */
    for (i=1; i<EsdMaxPaths; i++) {
        if (paths->pathv[i].nodeid) esd_path_dump(paths, &paths->pathv[i]);
    }
}

void esd_pathtree_dump (EsdPaths_t* paths, EsdPathIndex_t* path)
{
    int currpathid = path - paths->pathv;
    /*Dprintf("Path %d: DUMP\n", currpathid);*/
    if (currpathid == 0) return;
    esd_path_dump(paths, path);
    if (path->child) {
        /*Dprintf("Path %d: CHILD\n", currpathid);*/
        esd_pathtree_dump(paths, path->child);
    }
    if (path->next) {
        /*Dprintf("Path %d: NEXT\n", currpathid);*/
        esd_pathtree_dump(paths, path->next);
    }
    /*Dprintf("Path %d: DONE\n", currpathid);*/
}

void esd_paths_exit (EsdPaths_t* paths)
{
    elg_cntl_msg("esd_paths_exit(stkframe=%d, currpath=%d)",
                paths->stkframe,
                paths->currpath ? (paths->currpath - paths->pathv) : (-1));

    /* some compilers mis-instrument "main" and result in excessive warnings */
    if ((paths->currpath - paths->pathv) == 1) {
        elg_cntl_msg("Forcing exit of initial path with region %d",   
                    paths->currpath->nodeid);
        esd_exit(paths->currpath->nodeid); /* mark as if exitted correctly */
    }

    /* generate esd_exit events for each frame left on stack */
    while (paths->stkframe >= 0) {
        if (paths->currpath) 
            elg_warning("Forcing exit of frame %d path %d region %d", paths->stkframe,
                paths->currpath - paths->pathv, paths->currpath->nodeid);
        else
            elg_warning("Forcing exit of frame %d", paths->stkframe);
        esd_exit(ELG_NO_ID); /* XXXX mark as fake exits */
    }
}

void esd_paths_dump (EsdPaths_t* paths)
{
    elg_cntl_msg("esd_paths_dump(#paths=%d)", paths->pathid);
#if DEBUG
    esd_path_dump(paths, &paths->pathv[0]); /* unknown paths */
    esd_pathtree_dump(paths, &paths->pathv[1]); /* start from root */
#endif
}

void esd_paths_free (EsdPaths_t* paths)
{
    if (!paths->pathv) {
        elg_warning("Path vector not allocated");
        return;
    }
    free(paths->pathv);
}

unsigned esd_paths_size (EsdPaths_t* paths)
{
    if (paths->pathid >= EsdMaxPaths) return EsdMaxPaths;
    else return paths->pathid + 1; /* last path + Unknown */
}

unsigned esd_paths_void (EsdPaths_t* paths)
{
    EsdPathIndex_t* r_path = &paths->pathv[1];
    while (r_path != NULL) {
        unsigned visited = r_path->record.count;
        if (visited) return 0; /* at least one non-void root path */   
        r_path = r_path->next; /* move to next root */
    }
    return 1; /* all root paths were void */
}

unsigned esd_paths_roots (EsdPaths_t* paths)
{
    EsdPathIndex_t* r_path = &paths->pathv[1];
    unsigned roots = 0;
    for (roots=0; r_path != NULL; roots++)
        r_path = r_path->next; /* move to next root */
    return roots;
}


/*
    Cube3 is currently unable to handle calltrees of inclusive metrics
    therefore the value vectors need to be converted to exclusive values.
*/
void esd_paths_exclusify (EsdPaths_t* paths)
{
    unsigned i;
    unsigned EsdNumPaths = esd_paths_size(paths);
    
    elg_cntl_msg("esd_paths_exclusify(%d)", EsdNumPaths);
    if (!paths->pathv) {
        elg_warning("Path vector not allocated");
        return;
    }
    if (!paths->valv_sum) {
        elg_warning("Path values vector not allocated");
        return;
    }

    EsdPathIndex_t* last_path = &paths->pathv[EsdNumPaths];
    /* path 0 for Unknown is already exclusive */
    for (i=1; i<EsdNumPaths; i++) {
        EsdPathIndex_t* path = &paths->pathv[i];
        if ((path->parent > paths->pathv) && (path->parent < last_path)) { /* set and from this thread */
            unsigned m;
            elg_d8 child_timing, parent_timing;
            elg_ui4 ppid = (path->parent - paths->pathv);
            /*EsdPathIndex_t* parent = &paths->pathv[ppid];*/
            PUT_VALX(paths->valv_sum[ppid*esd_metrics_size+esd_num_metrics], parent_timing);
            PUT_VALX(paths->valv_sum[i*esd_metrics_size+esd_num_metrics], child_timing);
            parent_timing -= child_timing;
            SET_VALX(paths->valv_sum[ppid*esd_metrics_size+esd_num_metrics], parent_timing);
            for (m=0; m<esd_num_metrics; m++) {
                elg_ui8 value = paths->valv_sum[i*esd_metrics_size+m];
                paths->valv_sum[ppid*esd_metrics_size+m] -= value;
            }
        } else {
          elg_cntl_msg("Skipping exclusify path %d with parent %p", i, path->parent);
        }
    }
}

void esd_path_unify (EsdPaths_t* m_paths, EsdPathIndex_t* m_paths_last,
                     EsdPaths_t* paths, EsdPathIndex_t* path)
{
    int pathid = path - paths->pathv;
    elg_ui4 m_ppid = ELG_NO_ID;
    elg_ui4 rid = path->nodeid;
    if (!path->parent) {
        /* shouldn't happen if OpenMP fork event handled correctly, however,
           unify such non-fork paths under new roots when encountered */

        if (rid == 0) { /* UNKNOWN */
            paths->masterv[pathid] = 0;
            elg_cntl_msg("Path %d: Matched with UNKNOWN master cpathid %d", pathid, paths->masterv[pathid]);
            return;
        }

        /* start with master's first root */ 
        EsdPathIndex_t* m_path = &m_paths->pathv[1];
        while ((m_path != m_paths->pathv) && (m_path->nodeid != rid) && (m_path->next != NULL))
            m_path = m_path->next; /* move to next sibling */

        if (m_path->nodeid == rid) { /* got match */
            if (m_path->record.first_time > path->record.first_time)
                m_path->record.first_time = path->record.first_time;
            paths->masterv[pathid] = m_path - m_paths->pathv;
            elg_cntl_msg("Path %d: Matched with master cpathid %d", pathid, paths->masterv[pathid]);
        } else if (m_path == m_paths->pathv) { /* reached Unknown! */
            elg_warning("Reached Unknown root on master!");
        } else if (m_paths->pathid >= EsdMaxPaths) {
            elg_warning("Insufficient capacity to store worker callpath");
        } else {
            /* duplicate worker's root as new root in master's set of paths */
            m_paths->pathid++;
            elg_cntl_msg("Dup root path %d with rid %d next of path %d",
                            m_paths->pathid, rid, m_path - m_paths->pathv);
            m_path->next = &m_paths->pathv[m_paths->pathid];
            m_path = m_path->next;
            m_path->nodeid = rid;
            m_path->parent = NULL;
            m_path->record.first_time = path->record.first_time;
            paths->masterv[pathid] = m_path - m_paths->pathv;
            elg_cntl_msg("Path %d: New root created on master cpathid %d", pathid, paths->masterv[pathid]);
        }

        return;
    }

    if ((path->parent > m_paths->pathv) && (path->parent < m_paths_last)) {
        /* determine fork root path when parent path within master thread's pathv range */
        m_ppid = path->parent - m_paths->pathv;
        HACK_elg_cntl_msg("Path %d: Node %d Parent %p=%u FORKED", pathid, rid,
                    path->parent, m_ppid);
        HACK_elg_cntl_msg("Path %d: Matched with master fork cpathid %d", pathid, paths->masterv[pathid]);
        if (!path->child) {
            paths->masterv[pathid] = 0; /* XXXX Undefined */
            if (esd_summary) {
                elg_d8 ptime;
                PUT_VALX(paths->valv_sum[pathid*esd_metrics_size+esd_num_metrics], ptime);
                if (ptime>0.0) elg_warning("Path %d: No parallel region for fork with %.6fs!", pathid, ptime);
                SET_VALX(paths->valv_sum[pathid*esd_metrics_size+esd_num_metrics], 0.0);
            }
        } else {
            paths->masterv[pathid] = m_ppid;
            if (esd_summary) {
                /* augment parallel region metrics with those of parent fork callpath */
                int pregid = path->child - paths->pathv;
                elg_d8 ptime, pptime;
                PUT_VALX(paths->valv_sum[pathid*esd_metrics_size+esd_num_metrics], pptime);
                PUT_VALX(paths->valv_sum[pregid*esd_metrics_size+esd_num_metrics], ptime);
                ptime += pptime;
                SET_VALX(paths->valv_sum[pregid*esd_metrics_size+esd_num_metrics], ptime);
            }
        }
        return;
    } else if (paths->masterv[pathid]) {
        elg_cntl_msg("Path %d: Already matched with cpathid %d", pathid, paths->masterv[pathid]);
        return;
    } else { /* use already matched parent path */
        elg_ui4 ppid = path->parent - paths->pathv;
        m_ppid = paths->masterv[ppid];
        elg_cntl_msg("Path %d: Node %d Parent %d=%d", pathid, rid, ppid, m_ppid);
        if (m_ppid == 0) return; /* XXXX invalid! */
    }

    /* match rid with that of children of master m_ppid */
    EsdPathIndex_t* m_ppath = &m_paths->pathv[m_ppid];
    if (m_ppath->child == NULL) { /* duplicate path as master's (first) child */
        if (m_paths->pathid >= EsdMaxPaths) {
            elg_warning("Insufficient capacity to store worker callpath");
        } else {
            m_paths->pathid++;
            m_ppath->child = &m_paths->pathv[m_paths->pathid];
            EsdPathIndex_t* m_path = m_ppath->child;
            m_path->nodeid = rid;
            m_path->parent = m_ppath;
            m_path->record.first_time = path->record.first_time;
            elg_cntl_msg("Dup path %d with rid %d child of path %d",
                         m_paths->pathid, rid, m_path->parent - m_paths->pathv,
                         m_path - m_paths->pathv);
            paths->masterv[pathid] = m_path - m_paths->pathv;
            elg_cntl_msg("Path %d: Matched with new master cpathid %d", pathid, paths->masterv[pathid]);
        }
    } else {
        /* start with master's (first) child */
        EsdPathIndex_t* m_path = m_ppath->child;
        while ((m_path != m_paths->pathv) && (m_path->nodeid != rid) && (m_path->next != NULL))
            m_path = m_path->next; /* move to next sibling */

        if (m_path->nodeid == rid) { /* got match */
            if (m_path->record.first_time > path->record.first_time)
                m_path->record.first_time = path->record.first_time;
            paths->masterv[pathid] = m_path - m_paths->pathv;
            elg_cntl_msg("Path %d: Matched with master cpathid %d", pathid, paths->masterv[pathid]);
        } else if (m_path == m_paths->pathv) { /* reached Unknown! */
            elg_warning("Reached Unknown child on master!");
        } else if (m_paths->pathid >= EsdMaxPaths) {
            elg_warning("Insufficient capacity to store worker callpath");
        } else {
            /* duplicate worker's path as last sibling of master's child */
            /* XXXX appending doesn't respect order of first encounter! */
            m_paths->pathid++;
            elg_cntl_msg("Dup path %d with rid %d child of path %d next of path %d",
                         m_paths->pathid, rid, m_path->parent - m_paths->pathv,
                         m_path - m_paths->pathv);
            m_path->next = &m_paths->pathv[m_paths->pathid];
            m_path = m_path->next;
            m_path->nodeid = rid;
            m_path->parent = m_ppath;
            m_path->record.first_time = path->record.first_time;
            paths->masterv[pathid] = m_path - m_paths->pathv;
            elg_cntl_msg("Path %d: Matched with new master cpathid %d", pathid, paths->masterv[pathid]);
        }
    }
}

void esd_paths_unify (EsdPaths_t* m_paths, EsdPaths_t* paths)
{
    int i;
    EsdPathIndex_t* m_paths_last = &m_paths->pathv[EsdMaxPaths];
    unsigned EsdNumPaths = esd_paths_size(paths);
    HACK_elg_cntl_msg("esd_paths_unify(%d paths)", EsdNumPaths);
    paths->masterv = calloc(EsdNumPaths, sizeof(elg_ui4));
    if (!paths->masterv) {
        elg_warning("Failed to allocate masterv for %d paths", EsdNumPaths);
        return;
    }

    /* (re)set 1st path to UNKNOWN */
    paths->pathv[0].nodeid = esd_unknown_rid;
    paths->pathv[0].parent = NULL;

    /* XXXX drift adjustment presumably required for hybrid worker threads */
    /* XXXX at least those that have paths not executed by the master threads */

    if (esd_summary) esd_paths_exclusify(paths);

    /* Skip already defined unknown path 0 */
    for (i=1; i<EsdNumPaths; i++) {
        /* match local (worker) call-path to that of master */
        esd_path_unify(m_paths, m_paths_last, paths, &paths->pathv[i]);
    }
}

void esd_paths_def (EsdPaths_t* paths, double ltime, double offset, double drift)
{
    int i;
    unsigned EsdNumPaths = esd_paths_size(paths);
    HACK_elg_cntl_msg("esd_paths_def(%d paths)", EsdNumPaths);
    if (!paths->pathv) {
        elg_warning("Path vector not allocated");
        return;
    }

    /* (re)set 1st path to UNKNOWN */
    paths->pathv[0].nodeid = esd_unknown_rid;
    paths->pathv[0].parent = NULL;

/* XXXX drift should only be applied once (for master thread only) */

    esd_clock_drift_factor += drift; /* correction for time durations */
    
    if (esd_summary) esd_paths_exclusify(paths);

    if (esd_fork_ridv != NULL) for (i=0; i<epk_vector_size(esd_fork_ridv); i++)
        elg_cntl_msg("esd_fork_ridv[%d]=%lu", i, epk_vector_at(esd_fork_ridv,i));

    for (i=0; i<EsdNumPaths; i++) {
        EsdPathIndex_t* path = &paths->pathv[i];
        elg_ui4 ppid = path->parent ? (path->parent - paths->pathv) : ELG_NO_ID;
        elg_d8 first_time = path->record.first_time + offset +
                           (path->record.first_time - ltime) * drift;
        elg_ui8 order;
        elg_ui4 id;
        SET_VALX(order, first_time); /* adjusted for offset */
        /* OpenMP fork paths stored as duplicates of their parallel regions,
           distinguished from normal recursion by their OpenMP fork rids */
        if (path->parent && (path->nodeid == path->parent->nodeid)
                         && esd_is_fork_rid(path->nodeid)) {
            elg_cntl_msg("Path %d: OMP fork rid %d matches parent!", i, path->nodeid);
            if (esd_summary) {
                /* augment metrics with those acquired from fork callpath */
                elg_d8 ptime, pptime;
                PUT_VALX(paths->valv_sum[ppid*esd_metrics_size+esd_num_metrics], pptime);
                PUT_VALX(paths->valv_sum[i*esd_metrics_size+esd_num_metrics], ptime);
                ptime += pptime;
                SET_VALX(paths->valv_sum[i*esd_metrics_size+esd_num_metrics], ptime);
            }
            /* duplicate parent path so unification skips intermediate path */
            ppid = path->parent->parent - paths->pathv;
        }
        id = esd_def_callpath(path->nodeid, ppid, order);
        if (id != i) elg_warning("Callpath %d != %d", id, i);
        HACK_elg_cntl_msg("Path %d: Node %d Parent %d", i, path->nodeid,
                path->parent ? path->parent - paths->pathv : -1);
    }
}

/*
    Return the specified metric for the requested callpath.
*/

elg_ui4 esd_path_visits (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.count : 0;
}

#ifdef MSG_HACK /* message statistics */
elg_ui8 esd_path_coll_bytes (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
/*          paths->pathv[pathid].record.coll_bytes : 0; */
            paths->pathv[pathid].record.coll_bsent + paths->pathv[pathid].record.coll_brcvd : 0;
}

elg_ui8 esd_path_coll_bsent (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.coll_bsent : 0;
}

elg_ui8 esd_path_coll_brcvd (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.coll_brcvd : 0;
}

elg_ui4 esd_path_coll_syncs (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.coll_syncs : 0;
}

elg_ui4 esd_path_coll_c_src (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.coll_c_src : 0;
}

elg_ui4 esd_path_coll_c_dst (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.coll_c_dst : 0;
}

elg_ui4 esd_path_coll_c_xch (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.coll_c_xch : 0;
}

elg_ui4 esd_path_send_comms (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.send_comms : 0;
}

elg_ui4 esd_path_send_syncs (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.send_syncs : 0;
}

elg_ui8 esd_path_send_bytes (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.send_bytes : 0;
}

elg_ui4 esd_path_recv_comms (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.recv_comms : 0;
}

elg_ui4 esd_path_recv_syncs (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.recv_syncs : 0;
}

elg_ui8 esd_path_recv_bytes (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.recv_bytes : 0;
}

elg_ui8 esd_path_file_bytes (EsdPaths_t* paths, unsigned pathid)
{
    return (paths->pathv && pathid <= paths->pathid) ?
            paths->pathv[pathid].record.file_bytes : 0;
}
#endif /* message statistics */

/* XXXX OMP_HACK */
elg_d8 esd_path_tmgmt_time (EsdPaths_t* paths, unsigned pathid)
{
    if (!paths->pathv || (pathid > paths->pathid)) return 0.0;
    else {
        //EsdPathIndex_t* path = &paths->pathv[pathid];
        elg_d8 time = esd_path_timing (paths, pathid-1);
        HACK_elg_cntl_msg("Path %d: Mgmt time %.6f", pathid, time);
        return time;
    }
}

elg_d8 esd_path_tfork_time (EsdPaths_t* paths, unsigned pathid)
{
    if (!paths->pathv || (pathid > paths->pathid)) return 0.0;
    else {
        elg_d8 fork_time = 0.0;
#ifdef MSG_HACK /* XXXX fork_time hacked into message statistics record */
        /* threads that don't execute parallel region have undefined paths! */
        if (pathid>0) PUT_VALX(paths->pathv[pathid-1].record.coll_bsent, fork_time);
        HACK_elg_cntl_msg("Path %d: Fork time %.6f", pathid, fork_time);
#endif
        return fork_time;
    }
}

elg_d8 esd_path_timing (EsdPaths_t* paths, unsigned pathid)
{
    elg_d8 timing = 0.0;
    if (paths->pathv && paths->valv_sum && (pathid <= paths->pathid))
        PUT_VALX(paths->valv_sum[pathid*esd_metrics_size+esd_num_metrics], timing);
    return timing * esd_clock_drift_factor;
}

elg_ui8 esd_path_metric (EsdPaths_t* paths, unsigned pathid, unsigned m)
{
     return (paths->pathv && paths->valv_sum && (pathid <= paths->pathid) 
                                            && (m < esd_metrics_size)) ?
             paths->valv_sum[pathid*esd_metrics_size+m] : 0;
}


/*
    Where current path is defined, increment count and reset to parent path.
    If current path is truncated, only reset when stack frame returns
    to level of truncation.
    Returns rid of new stack frame.
*/
elg_ui4 esd_path_pop (EsdPaths_t* paths, elg_ui4 rid, elg_d8 exit_time)
{
    if (paths->currpath == NULL) { /* don't have a path defined! */
        elg_error_msg("No current path to pop! (rid=%d)", rid);
    } else if (paths->stkframe < 0) { /* no frame on stack */
        elg_warning("No stack frame to pop! (rid=%d)", rid);
    } else {
        int currpathid = paths->currpath - paths->pathv;
        elg_ui4 curr_rid = paths->currpath->nodeid;

        /* update statistics for this path */
        paths->currpath->record.count++;
        paths->currpath->record.final_time = exit_time;

        if ((currpathid == 0) && ((int)curr_rid < paths->stkframe)) {
            /* skip truncated path */
            elg_cntl_msg(/*"%.6f: "*/"[%d]EXIT  rid=%3d TRUNCATED @%d",
                        /*exit_time,*/ paths->stkframe, rid, curr_rid);
        } else {
            if ((currpathid == 0) && ((int)curr_rid == paths->stkframe)) 
                /* returning from truncated paths to known path */
                elg_cntl_msg(/*"%.6f: "*/"[%d]EXIT  rid=%3d (CLIPPED -> rid=%3d) count %d",
                        /*exit_time,*/ paths->stkframe, rid,
                        paths->currpath->parent ? paths->currpath->parent->nodeid : (-1),
                        paths->currpath->record.count);
            else if ((rid != curr_rid) && (rid != ELG_NO_ID))
                elg_warning(/*"%.6f: "*/"[%d]EXIT  rid=%3d ****MISMATCH**** %d",
                        /*exit_time,*/ paths->stkframe, rid, curr_rid);
#if DEBUG
            else /* normal exit */
                elg_cntl_msg(/*"%.6f: "*/"[%d]EXIT  rid=%3d (path %2d -> rid=%3d) count %d",
                        /*exit_time,*/ paths->stkframe, rid, currpathid,
                        paths->currpath->parent ? paths->currpath->parent->nodeid : (-1),
                        paths->currpath->record.count);
#endif
#if 1
            /* XXXX this additional conditional executed for every valid callpath
                    could be handled as a special case by esd_omp_join */
            if (esd_forkpath && (paths->stkframe == 0) && (paths->currpath->parent == esd_forkpath)) {
                elg_cntl_msg("[%d]exit parallel region %p", paths->stkframe, esd_forkpath);
                paths->currpath = NULL;
            } else 
#endif
                paths->currpath = paths->currpath->parent; /* pop current step from path */
        }
        paths->stkframe--;
        return (paths->currpath ? paths->currpath->nodeid : ELG_NO_ID);
    }
    return ELG_NO_ID;
}

/*
    Determine new path based on given node region as extension of current path.
    If not defined, create and append to vector of paths, unless capacity already
    reached (in which case current path is truncated and new path unknown).
*/
void esd_path_push (EsdPaths_t* paths, elg_ui4 rid, elg_d8 enter_time)
{
    EsdPathIndex_t* path = &paths->pathv[0]; /* default: unknown/truncated path */
    if (paths->currpath == paths->pathv) { /* on a truncated path */
        /* current path remains unchanged with unknown/truncated path */
    } else if (paths->currpath == NULL) { /* initialise root of new paths */
        if (paths->pathid == (-1)) {
            paths->pathid = 1; /* first valid path */
            path = &paths->pathv[1];
            path->nodeid = (paths->pathid < EsdMaxPaths) ? rid: (paths->stkframe+1);
            path->parent = esd_forkpath; /* NULL for master thread */
            path->record.first_time = enter_time;
            if (esd_forkpath != NULL)
                elg_cntl_msg("1st fork path %d with rid %d child of %p",
                             paths->pathid, rid, esd_forkpath);
            else
                elg_cntl_msg("1st root path %d with rid %d defined", paths->pathid, rid);
        } else { /* check existing root(s) */
            elg_cntl_msg("New root check rid=%d esd_forkpath=%p", rid, esd_forkpath);
            path = &paths->pathv[1]; /* start with primary root */
            if (esd_forkpath != NULL) {
                while ((path != paths->pathv) && (path->next != NULL)
                                              && (path->parent != esd_forkpath))
                    path = path->next; /* move to next fork root */

                if ((path != paths->pathv) && (path->parent != esd_forkpath)) { /* new fork path */
                    paths->pathid++;
                    elg_cntl_msg("New fork path %d with rid %d child of %p",
                                 paths->pathid, rid, esd_forkpath);
                    path->next = &paths->pathv[(paths->pathid < EsdMaxPaths) ? paths->pathid : 0];
                    path = path->next;
                    path->nodeid = (paths->pathid < EsdMaxPaths) ? rid: (paths->stkframe+1);
                    path->parent = esd_forkpath;
                    if (path->record.first_time == 0.0) path->record.first_time = enter_time;
                } else { /* otherwise have a known fork root path */
                    elg_cntl_msg("Known fork root path %d with rid %d revisited",
                                 path - paths->pathv, rid);
                }
            } else { 
                while ((path != paths->pathv) && (path->next != NULL)
                                              && (path->nodeid != rid))
                    path = path->next; /* move to next root */

                if ((path != paths->pathv) && (path->nodeid != rid)) { /* new root path */
                    paths->pathid++;
                    elg_cntl_msg("New root path %d with rid %d next of path %d",
                                  paths->pathid, rid, path - paths->pathv);
                    path->next = &paths->pathv[(paths->pathid < EsdMaxPaths) ? paths->pathid : 0];
                    path = path->next;
                    path->nodeid = (paths->pathid < EsdMaxPaths) ? rid: (paths->stkframe+1);
                    path->parent = NULL;
                    if (path->record.first_time == 0.0) path->record.first_time = enter_time;
                } else { /* otherwise have a known root path */
                    elg_cntl_msg("Known root path %d with rid %d revisited",
                                 path - paths->pathv, rid);
                }
            }
        }
    } else if (paths->currpath->child == NULL) { /* new first callee path */
        paths->pathid++;
        elg_cntl_msg("New path %d with rid %d child of path %d",
                     paths->pathid, rid, paths->currpath - paths->pathv);
        path = &paths->pathv[(paths->pathid < EsdMaxPaths) ? paths->pathid : 0];
        path->nodeid = (paths->pathid < EsdMaxPaths) ? rid: (paths->stkframe+1);
        path->parent = paths->currpath;
        if (path->record.first_time == 0.0) path->record.first_time = enter_time;
        paths->currpath->child = path;         /* link first child path */
    } else { /* check known steps from current path */
        path = paths->currpath->child; /* start with (first) child */
        while ((path != paths->pathv) && (path->nodeid != rid) && (path->next != NULL))
            path = path->next; /* move to next sibling */

        if ((path != paths->pathv) && (path->nodeid != rid)) { /* new sibling path */
            paths->pathid++;
            elg_cntl_msg("New path %d with rid %d child of path %d next of path %d",
                         paths->pathid, rid, path->parent - paths->pathv,
                         path - paths->pathv);
            path->next = &paths->pathv[(paths->pathid < EsdMaxPaths) ? paths->pathid : 0];
            path = path->next;
            path->nodeid = (paths->pathid < EsdMaxPaths) ? rid: (paths->stkframe+1) ;
            path->parent = paths->currpath;
            if (path->record.first_time == 0.0) path->record.first_time = enter_time;
        } else if (path == paths->pathv) { /* truncated path */
            path->nodeid = paths->stkframe+1;
            path->parent = paths->currpath;
        } /* otherwise have a known path */
    }
    paths->currpath = path;    /* push new step on current callpath */
    paths->stkframe++;

    /* initialise new path? */
    {
    static int clipping = 0;
    int currpathid = paths->currpath - paths->pathv;
    if (currpathid == 0 && !clipping) {
        clipping = 1;
        elg_warning("Clipping call-paths in excess of ESD_PATHS=%d!", EsdMaxPaths);
    }
    if (currpathid == 0)
        elg_cntl_msg(/*"%.6f: "*/"[%d]ENTER rid=%3d (CLIPPED <- rid=%3d) count %d",
                /*enter_time,*/ paths->stkframe, rid,
                paths->currpath->parent ? paths->currpath->parent->nodeid : (-1),
                paths->currpath->record.count);
#if DEBUG
    else /* normal enter */
        elg_cntl_msg(/*"%.6f: "*/"[%d]ENTER rid=%3d (path %2d <- rid=%3d) count %d",
                /*enter_time,*/ paths->stkframe, rid, currpathid,
                paths->currpath->parent ? paths->currpath->parent->nodeid : (-1),
                paths->currpath->record.count);
#endif
    }
}


/*    
   Stash current measurements in current stack frame
   (unless stack capacity exceeded or frame is undefined).
*/
void esd_frame_stash (EsdPaths_t* paths, elg_ui8 valv[])
{
    int frame = paths->stkframe;
    elg_d8 entry_time;
    PUT_VALX(valv[esd_num_metrics], entry_time);
    if ((frame >= 0) && (frame < EsdMaxFrames)) {
#if DEBUG
        int currpathid = paths->currpath - paths->pathv;
#endif
        memcpy(paths->framev[frame], valv, esd_valv_size);
#if DEBUG
        elg_cntl_msg("%2d:ENTER[%03d] %.6f", frame, currpathid, entry_time);
#endif
    } else
        elg_cntl_msg("%2d:ENTER[---] %.6f", frame, entry_time);
}

/*    
   Evaluate current measurements with respect to stashed current stack frame
   entry measurements (where available).
*/
void esd_frame_leave (EsdPaths_t* paths, elg_ui8 valv[])
{
    int frame = paths->stkframe;
    elg_d8 exit_time;
    PUT_VALX(valv[esd_num_metrics], exit_time);
    if (frame > paths->stkdepth) paths->stkdepth = frame; /* max stack depth */
    if ((frame >= 0) && (frame < EsdMaxFrames)) {
        int currpathid = paths->currpath - paths->pathv;
        elg_d8 delta_time, entry_time, old_sum=0.0, new_sum=0.0;
        PUT_VALX(paths->framev[frame][esd_num_metrics], entry_time);
        delta_time = exit_time - entry_time;
        if (esd_summary) {
            unsigned m;
            for (m=0; m<esd_num_metrics; m++) {
                elg_ui8 delta_value = valv[m] - paths->framev[frame][m];
                paths->valv_sum[currpathid*esd_metrics_size+m] += delta_value;
            }  
            PUT_VALX(paths->valv_sum[currpathid*esd_metrics_size+esd_num_metrics], old_sum);
            new_sum = old_sum + delta_time;
            SET_VALX(paths->valv_sum[currpathid*esd_metrics_size+esd_num_metrics], new_sum);
        }
#if DEBUG // if (delta_time < 0)
        HACK_elg_cntl_msg("%2d:LEAVE[%03d] %.6f = %.6f + %.6f -> %.6f",
                frame, currpathid,
                exit_time, delta_time, old_sum, new_sum);
#endif
    } else
        elg_cntl_msg("%2d:LEAVE[---] %.6f", frame, exit_time);
}

elg_d8 esd_frame_time (EsdPaths_t* paths, unsigned frame)
{
    elg_d8 entry_time;
    if (frame>=EsdMaxFrames) return 0.0;
    PUT_VALX(paths->framev[frame][esd_num_metrics], entry_time);
    return entry_time;
}
    

/* Dump current framev contents */
void esd_framev_dump (EsdPaths_t* paths)
{
    unsigned curr_frame = paths->stkframe;
    unsigned frame;
    for (frame=0; frame<EsdMaxFrames; frame++) {
        elg_d8 entry_time = esd_frame_time(paths, frame);
        if (entry_time == 0) break;
        elg_warning("%2d:ENTER %.6f %d", frame, entry_time, frame==curr_frame);
    }
}

/*
    Maximum frame depth encountered during measurement.
*/
unsigned esd_frame_depth(EsdPaths_t* paths)
{
    return paths->stkdepth;
}
