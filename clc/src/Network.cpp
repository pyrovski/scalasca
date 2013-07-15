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

#include <Machine.h>
#include <Node.h>
#include <Process.h>
#include <Thread.h>
#include <Latency.h>

#include <Network.h>

using namespace pearl;

Network::Network(clc_location loc, Latency *lat) {

  location.machine = loc.machine;
  location.node    = loc.node;
  location.process = loc.process;
  location.thread  = loc.thread;

  latency = new Latency();
  latency = lat;
}

Network::~Network() {

  delete latency;

}

timestamp_t Network::get_latency(clc_location &loc, clc_event_t com_type) {

  double lat = 0.0;
  
  if (location.machine != loc.machine)
    lat = latency->get_lat_machine();

  if (com_type == CLC_P2P) {
    if (location.node != loc.node)
      lat += latency->get_lat_p2p_inter();
    else
      lat += latency->get_lat_p2p_intra();
  } else if(com_type == CLC_COLL) {
    if (location.node != loc.node)
      lat += latency->get_lat_coll_inter();
    else
      lat += latency->get_lat_coll_intra();
  } else if (com_type ==   CLC_OMP) {
    lat += latency->get_lat_shared_mem();
  }

  return lat;

}
