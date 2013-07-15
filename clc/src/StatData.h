/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2013                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2013                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


#ifndef STATDATA_H
#define STATDATA_H

#include <clc_types.h>
#include <pearl_types.h>

struct statistic_data {
   pearl::timestamp_t rel; 
   pearl::timestamp_t abs;
   pearl::timestamp_t org;
   pearl::timestamp_t syn;
};

bool operator< (const statistic_data& a, const statistic_data& b);
void set_sort_att (stat_t att); 
#endif 

/* !STATDATA_H */
