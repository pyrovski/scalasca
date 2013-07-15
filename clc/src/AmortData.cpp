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

#include <AmortData.h>

using namespace pearl;

amortization_data pack_data(clc_location loc, timestamp_t m_time) {

  amortization_data data;

  data.machine_id = loc.machine; 
  data.node_id    = loc.node; 
  data.process_id = loc.process; 
  data.thread_id  = loc.thread; 
  data.timestamp  = m_time; 

  return data;

}

timestamp_t unpack_data(clc_location &loc, amortization_data data) {

  loc.machine = data.machine_id;
  loc.node    = data.node_id; 
  loc.process = data.process_id;
  loc.thread  = data.thread_id; 
  return data.timestamp; 

}
