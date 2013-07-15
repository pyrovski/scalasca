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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "epk_conf.h"
#include "elg_error.h"
#include "epk_metric.h"

/* metricmap operations */

epk_metricmap_t* epk_metricmap_append(epk_metricmap_t* map, epk_metmap_t type,
                                      char* event, char* alias)
{
    elg_cntl_msg("Def 0x%X %s = <%s>", type, event, alias);

    if (map == NULL) {
        map = calloc(1, sizeof(epk_metricmap_t));
        if (map == NULL) {
            elg_warning("Metricmap creation failed!");
            return NULL;
        }
        /*printf("Created new metricmap head @0x%p\n", map);*/
    } else {
        while (map->next != NULL) map = map->next;
        map->next = calloc(1, sizeof(epk_metricmap_t));
        if (map->next == NULL) {
            elg_warning("Metricmap append failed!");
            return NULL;
        }
        map = map->next;
        /*printf("Created new metricmap node @0x%p\n", map);*/
    }

    map->type = type;
    map->event_name = strdup(event);
    map->alias_name = strdup(alias);
    map->next = NULL;
    
    return map;
}

void epk_metricmap_dump(epk_metricmap_t* map)
{
    unsigned j=0;

    if (map == NULL) {
        printf("Can't dump empty metricmap!\n");
        return;
    }

    printf("Metricmap dump (head=0x%p):\n", map);
    while (map != NULL) {
        printf("m[%3u] 0x%X %s = %s\n", j, map->type,
                map->event_name, map->alias_name);
        j++;
        map = map->next;
    }
    printf("Metricmap dumped %u maps\n", j);
}

void epk_metricmap_free(epk_metricmap_t* map)
{
    if (map == NULL) {
        /*printf("Can't free empty metricmap!\n");*/
        return;
    }

    /*printf("Metricmap free (head=0x%p):\n", map);*/
    while (map != NULL) {
        epk_metricmap_t* next = map->next;
        if (map->event_name != NULL) free(map->event_name);
        if (map->alias_name != NULL) free(map->alias_name);
        free(map);
        map = next;
    }
}

#if 0
// METRICS.SPEC entry format:
// measure MEASURE_DEF = NAME                // no operators
// aggroup AGGROUP_DEF = NAME1 NAME2 ...     // no operators
// compose COMPOSE_DEF = NAME1 + NAME2 + ... // one or more "+"
// compute COMPUTE_DEF = NAME1 & NAME2 & ... // "&" is "+-*/"
#endif

/* initialize metric map */
epk_metricmap_t* epk_metricmap_init(int match)
{
    epk_metricmap_t* mapv = NULL, *map = NULL;
    const char* specfile = epk_get(EPK_METRICS_SPEC);
    unsigned lineno=0, defs=0;
    unsigned invalid_defs=0, unknown_defs=0;
    unsigned measure_defs=0, aggroup_defs=0, compose_defs=0, compute_defs=0;
    char line[2048];
    FILE *fp;

    if (!specfile) return NULL;

    if (access(specfile, F_OK)) {
        elg_warning("Metric specification file not found: %s", specfile);
        return NULL;
    }

    fp = fopen(specfile, "r"); 
    if (fp == NULL) {
        elg_warning("Failed to open metric specification: %s", specfile);
        return NULL;
    }

    /*printf("specfile=%s match=0x%X\n", specfile, match);*/

    while (fgets(line, sizeof(line), fp)) {
        epk_metmap_t type=EPK_METMAP_UNKNOWN;
        char* def_name, *def_args;
        int len = strcspn(line, "#\n"); /* length of non-comment string */
        while (len && ((line[len-1] == ' ') || (line[len-1] == '\t'))) len--;
        line[len] = '\0'; /* chop comment and return */
        lineno++;
        if (len <= 1) continue; 
        defs++;
        if      (!strncmp("measure", line, 7)) type=EPK_METMAP_MEASURE;
        else if (!strncmp("compose", line, 7)) type=EPK_METMAP_COMPOSE;
        else if (!strncmp("compute", line, 7)) type=EPK_METMAP_COMPUTE;
        else if (!strncmp("aggroup", line, 7)) type=EPK_METMAP_AGGROUP;
        /*printf("%3d:%2d %d-[%2d] %s\n", lineno, defs, type, len, line);*/
        if (type == EPK_METMAP_UNKNOWN) {
            unknown_defs++;
            elg_warning("Failed to parse metric definition line %d: %s", lineno, line);
            continue;
        }
        line[7] = '\0'; /* terminate definition type */
        def_name = line + 8;
        def_name += strspn(def_name, " \t"); /* get start of definition name */
        len = strcspn(def_name, "= \t"); /* length of definition name */
        *(def_name+len)='\0'; /* terminate definition name */
        def_args = line + 8 + len + 1;
        def_args += strspn(def_args, "= \t"); /* get start of def argument */
        /*printf("Def %2d:<%s> %s <%s>\n", defs, def_name, line, def_args);*/
        len = strlen(def_args); /* length of definition arguments */
        if (((type == EPK_METMAP_MEASURE) && (match & EPK_METMAP_MEASURE)) ||
            ((type == EPK_METMAP_AGGROUP) && (match & EPK_METMAP_AGGROUP))) {
            if ((strcspn(def_args, "=+") != len) ||
                      ((strcspn(def_args, "=+-*/ \t") != len)
                          && (type == EPK_METMAP_MEASURE))) {
                type = EPK_METMAP_INVALID;
                invalid_defs++;
                elg_warning("Metric def %d:%s <%s> invalid!", lineno, line, def_name);
            } else {
                map = epk_metricmap_append(map, type, def_name, def_args);
                measure_defs++;
            }
        } else if ((type == EPK_METMAP_COMPOSE) && (match & EPK_METMAP_COMPOSE)) {
                map = epk_metricmap_append(map, type, def_name, def_args);
                compose_defs++;
        } else if ((type == EPK_METMAP_COMPUTE) && (match & EPK_METMAP_COMPUTE)) {
                map = epk_metricmap_append(map, type, def_name, def_args);
                compute_defs++;
        }
        if (mapv == NULL) mapv = map; /* initialise head of vector */
    }
    elg_cntl_msg("Mapped %d/%d defs from \"%s\"",
        measure_defs+aggroup_defs+compose_defs+compute_defs, defs, specfile);
#if 0
    printf("measure %d aggroup %d compose %d compute %d unknown %d invalid %d\n", 
            measure_defs, aggroup_defs, compose_defs, compute_defs,
            unknown_defs, invalid_defs);
#endif
    fclose(fp);
    return mapv;
}
