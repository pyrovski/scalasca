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

#ifndef __LATENCY_H__
#define __LATENCY_H__

#include <pearl_types.h>

class Latency
{
 public:

  // Constructors and Destructor
  Latency();
  Latency(pearl::timestamp_t lat_machine, pearl::timestamp_t lat_p2p_intra,
	  pearl::timestamp_t lat_p2p_inter, pearl::timestamp_t lat_coll_intra,
	  pearl::timestamp_t lat_coll_inter, pearl::timestamp_t lat_shared_mem);
    
  ~Latency(){};
  pearl::timestamp_t get_lat_machine()    { return lat_machine; }
  pearl::timestamp_t get_lat_p2p_intra()  { return lat_p2p_intra; }
  pearl::timestamp_t get_lat_p2p_inter()  { return lat_p2p_inter; }
  pearl::timestamp_t get_lat_coll_intra() { return lat_coll_intra; }
  pearl::timestamp_t get_lat_coll_inter() { return lat_coll_inter; }
  pearl::timestamp_t get_lat_shared_mem() { return lat_shared_mem; }

 private:
  pearl:: timestamp_t lat_machine;
  pearl:: timestamp_t lat_p2p_intra;
  pearl:: timestamp_t lat_p2p_inter;
  pearl:: timestamp_t lat_coll_intra;
  pearl:: timestamp_t lat_coll_inter;
  pearl:: timestamp_t lat_shared_mem;
		
};

#endif /* END OF LATENCY_H */
