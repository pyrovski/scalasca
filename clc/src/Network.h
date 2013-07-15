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

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <pearl_types.h>
#include <Latency.h>
#include <clc_types.h>

class Network
{
 public:

  // Constructors and Destructor
  Network(){};  
  Network(clc_location loc, Latency *lat);  
  ~Network();
  
  pearl::timestamp_t get_latency(clc_location &loc, clc_event_t com_type);
 
 private:
  clc_location location;
  Latency      *latency;
		     
};

#endif /* END OF NETWORK_H */

