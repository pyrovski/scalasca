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


#ifndef AMORTDATA_H
#define AMORTDATA_H

#include <clc_types.h>
#include <pearl_types.h>

struct  amortization_data { 
  pearl::ident_t     machine_id; 
  pearl::ident_t     node_id; 
  pearl::ident_t     process_id; 
  pearl::ident_t     thread_id; 
  pearl::timestamp_t timestamp; 
};

amortization_data  pack_data(clc_location loc, pearl::timestamp_t m_time);
pearl::timestamp_t unpack_data(clc_location &loc, amortization_data data);

#endif 

/* !AMORTDATA_H */
